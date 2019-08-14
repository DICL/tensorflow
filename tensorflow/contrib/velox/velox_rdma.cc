#include "tensorflow/contrib/velox/velox_rdma.h"

namespace tensorflow {

/*  static */
string create_key(const string& name, const string& training_id) {
  // NOTE: ';' is not used in the device name's job name.
  //
  // We include both sender and receiver in the key to facilitate
  // debugging. For correctness, we only need to encode the receiver.
  //
  // "src_incarnation" is used to distinguish a worker when it
  // restarts.
  char buf[strings::kFastToBufferSize];
  return strings::StrCat(name, ";", job);
}

static StringPiece consume_next_part(StringPiece* s, char delim) {
  for (size_t offset = 0; offset < s->size(); offset++) {
    if ((*s)[offset] == delim) {
      StringPiece result(s->data(), offset);
      s->remove_prefix(offset + 1);  // +1: remove delim, as well
      return result;
    }
  }
  // No delimiter found: return rest of string
  StringPiece result(s->data(), s->size());
  s->remove_prefix(s->size());
  return result;
}

/* static */
Status parse_key(StringPiece key, ParsedKey* out) {
  if (key.data() == out->buf_.data()) {
    DCHECK_EQ(key.size(), out->buf_.size());
  } else {
    // Make a copy that our StringPieces can point at a copy that will persist
    // for the lifetime of the ParsedKey object.
    out->buf_.assign(key.data(), key.size());
  }
  StringPiece s(out->buf_);
  StringPiece parts[2];
  for (int i = 0; i < 2; i++) {
    parts[i] = consume_next_part(&s, ';');
  }
  if (s.empty() &&          // Consumed the whole string
      !parts[1].empty() &&  // Exactly five parts
      !parts[0].empty()) {
    out->tensor_name = StringPiece(parts[0].data(), parts[0].size());
    out->training_id = StringPiece(parts[1].data(), parts[1].size());
    return Status::OK();
  }
  return errors::InvalidArgument("Invalid  tensor key: ", key);
}

// Function to set device. If RDMA_DEVICE not set, search for device with active
// port.
// Fails if more than one device with active port was found.
// Returns:
//   device to use
ibv_device* set_device() {
  ibv_device** dev_list;
  int dev_num, device_index, device_to_open = 0;
  int num_devs_with_active_port = 0;
  string env_p_rdma_device, str_port_num;

  dev_list = ibv_get_device_list(&dev_num);
  CHECK(dev_list) << "No InfiniBand device found";

  env_p_rdma_device = get_env_var("RDMA_DEVICE");
  if (!env_p_rdma_device.empty()) {
    for (device_index = 0; device_index < dev_num; device_index++) {
      if (!env_p_rdma_device.compare(
              ibv_get_device_name(dev_list[device_index]))) {
        CHECK(get_dev_active_port_count(dev_list[device_index]) != 0)
            << "Device " << ibv_get_device_name(dev_list[device_index])
            << " has no active ports";
        return dev_list[device_index];
      }
    }
    // check validity of input device
    CHECK(false) << "The device " << env_p_rdma_device << " wasn't found";
  } else {
    // set default device
    str_port_num = get_env_var("RDMA_DEVICE_PORT");
    CHECK(str_port_num.empty())
        << "RDMA_DEVICE should be provided if RDMA_DEVICE_PORT is set by user";
    for (device_index = 0; device_index < dev_num; device_index++) {
      // get port_num
      if (get_dev_active_port_count(dev_list[device_index]) > 0) {
        num_devs_with_active_port++;
        CHECK(num_devs_with_active_port <= 1) << ". More than one device with "
                                                 "active port in the system. "
                                                 "Please enter RDMA_DEVICE";
        // found device with at least 1 active port
        device_to_open = device_index;
      }
    }
    CHECK(num_devs_with_active_port > 0)
        << "There is no active port in the system";
    return dev_list[device_to_open];
  }
  CHECK(false) << "No device was set!";
  return NULL;  // never happens
}

// Function to open device
// Args:
//   ibv_dev device to open
// Returns:
//   context of the opened device
ibv_context* open_device(ibv_device* ibv_dev) {
  ibv_context* context = ibv_open_device(ibv_dev);

  CHECK(context) << "Open context failed for " << ibv_get_device_name(ibv_dev);
  return context;
}

ibv_pd* alloc_protection_domain(ibv_context* context) {
  ibv_pd* pd = ibv_alloc_pd(context);
  CHECK(pd) << "Failed to allocate protection domain";
  return pd;
}

void reg_mem_visitors() {
  SubAllocator::Visitor alloc_visitor = [](void* ptr, int numa_node,
                                           size_t num_bytes) {
    RdmaMemoryMgr::Singleton().InsertMemoryRegion(
        ptr, num_bytes, strings::StrCat("CPU:", numa_node));
  };
  SubAllocator::Visitor free_visitor = [](void* ptr, int numa_node,
                                          size_t num_bytes) {
    RdmaMemoryMgr::Singleton().EvictMemoryRegion(ptr, num_bytes);
  };

  ProcessState::singleton()->AddCPUAllocVisitor(alloc_visitor);
  ProcessState::singleton()->AddCPUFreeVisitor(free_visitor);
}

ibv_mr* create_tensor_cpu_buffer(const Tensor& t, ibv_pd* pd, int numa_node) {
  DataType data_type = t.dtype();
  TensorShape tensor_shape = t.shape();
  auto cpu_allocator = ProcessState::singleton()->GetCPUAllocator(numa_node);
  CASES(data_type, buf = new Buffer<T>(cpu_allocator, tensor_shape.num_elements()));
  void* addr = buf->data();
  size_t length = buf->size();
  if (length == 0) {
    return nullptr;
  }
  //bool can_memcpy = DataTypeCanUseMemcpy(data_type);
  //if (can_memcpy) {
  //  //rdma_addr = DMAHelper::base(result_tensor_);
  //  void* rdma_addr = addr;
  //} else {

  //}
  ibv_mr* mr = ibv_reg_mr(pd, addr, length,
                          IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
  return mr;
}

// Args:
//   name: peer name, e.g. worker1
LocalMRTable* create_local_mr_table(
          const std::vector<std::pair<string, Tensor>>& tensors,
          const string& training_id,
          ibv_pd* pd, int numa_node) {
  auto ltable = std::make_shared<LocalMRTable>();
  for (const auto& nt : tensors) {
    string key = create_key(nt.first, training_id);
    auto mr = create_tensor_cpu_buffer(nt.second, pd, numa_node);
    ltable->insert({key, mr});
  }
  return ltable;
}


class RdmaTensorPush {


};


}  // namespace tensorflow
