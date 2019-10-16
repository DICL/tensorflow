#include "tensorflow/ptre/cm/remote_store.h"
//#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

static Allocator* get_default_cpu_allocator() {
  static Allocator* default_cpu_allocator =
      cpu_allocator(port::kNUMANoAffinity);
  return default_cpu_allocator;
}

RemoteStore::RemoteStore(const Tensor& other) {
  tensor_ = new Tensor(get_default_cpu_allocator(),
                       other.dtype(),
                       other.shape());
}

RemoteStore::~RemoteStore() {
  delete tensor_;
}

//void RemoteStore::Write(const Tensor& other) {
//  CHECK_EQ(tensor_->shape(), other.shape());
//  TensorBuffer* buf = tensor_->buf_;
//  TensorBuffer* other_buf = other.buf_;
//  size_t size = other_buf->size();
//  for (size_t i = 0; i < size; i++) {
//    buf[i] = other_buf[i];
//  }
//}

}  // namespace tensorflow
