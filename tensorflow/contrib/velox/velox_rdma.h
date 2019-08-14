#ifndef TENSORFLOW_CONTRIB_VELOX_VELOX_RDMA_H_
#define TENSORFLOW_CONTRIB_VELOX_VELOX_RDMA_H_

#include <infiniband/verbs.h>

namespace tensorflow {
#define PKEY_DEFAULT 0
#define QUEUE_DEPTH_DEFAULT 1024
#define TIMEOUT_DEFAULT 14
#define RETRY_CNT_DEFAULT 7
#define SL_DEFAULT 0
#define TRAFFIC_CLASS 0

#define RDMA_LOG_0 LOG(INFO)
#define RDMA_LOG_1 VLOG(1)
#define RDMA_LOG_2 VLOG(2)
#define RDMA_LOG(LEVEL) RDMA_LOG_##LEVEL

struct RdmaParams {
  uint8_t port_num;
  uint8_t sgid_index;
  uint8_t pkey_index;
  uint32_t queue_depth;
  uint8_t timeout;
  uint8_t retry_cnt;
  uint8_t sl;
  enum ibv_mtu mtu;
  uint8_t traffic_class;
};
// structure to save the address of remote channels.
struct RdmaAddress {
  uint32_t lid;
  uint32_t qpn;
  uint32_t psn;
  uint64_t snp;
  uint64_t iid;
};
// structure to save information for remote memory regions.
struct RemoteMR {
  uint64_t remote_addr;
  uint32_t rkey;
};
enum BufferStatus { none, idle, busy };
enum Location { local, remote };

enum RdmaMessageType {
  RDMA_MESSAGE_META_DATA_UPDATE,
  RDMA_MESSAGE_ERROR_STATUS,
};

typedef std::unordered_map<string, RemoteMR> RemoteMRTable;
typedef std::unordered_map<string, ibv_mr*> LocalMRTable;

class RdmaTensorPush {
 public:
  RdmaTensorPush()
};

static string create_key(const string& name, const string& training_id);

struct TensorKey {
  StringPiece tensor_name;
  StringPiece training_id;

  TensorKey() {}
  TensorKey(const TensorKey& b) { *this = b; }

  TensorKey& operator=(const TensorKey& b);
  StringPiece FullKey() const { return buf_; }

 private:
  string buf_;
};
static Status parse_key(StringPiece key, ParsedKey* out);

class RdmaTensorPush {
};

class RdmaMessageBuffer;
// Class that represents the Rdma Adapter.
// Responsible for creation of the completion queue, and handling
// of work completions.
class RdmaAdapter {
  friend class RdmaChannel;
  friend class RdmaMessageBuffer;
  friend class RdmaMgr;

 public:
  RdmaAdapter(const WorkerEnv* worker_env);
  ~RdmaAdapter();
  // Adapter name, e.g. mlx5_0.
  string name() const;
  void StartPolling();
  void Process_CQ();

 protected:
  static const int MAX_CONCURRENT_WRITES = 1000;
  ibv_context* context_;
  // RDMA configuration parameters
  RdmaParams params_;
  // ibverbs protection domain
  ibv_pd* pd_;
  // Completion event channel, to wait for work completions
  ibv_comp_channel* event_channel_;
  // Completion queue, to poll on work completions
  ibv_cq* cq_;
  // Pre-allocated work completions array used for polling
  ibv_wc wc_[MAX_CONCURRENT_WRITES * 2];
  // worker env for thread
  const WorkerEnv* worker_env_;
  // thread for cq.
  std::unique_ptr<Thread> polling_thread_;
};

// Class that represents a buffer for Rdma message sending.
class RdmaMessageBuffer {
  friend class RdmaChannel;
  friend class RdmaAdapter;
  friend class RdmaMgr;

 public:
  explicit RdmaMessageBuffer(RdmaChannel* channel, string name);
  ~RdmaMessageBuffer();

  inline void* buffer() const { return buffer_; }
  inline ibv_mr* self() const { return self_; }
  inline void SetBufferStatus(Location loc, BufferStatus status) {
    mu_.lock();
    if (loc == local) {
      local_status_ = status;
    } else {
      remote_status_ = status;
    }
    mu_.unlock();
  }
  void FreeBuffer();
  void EnqueueItem(string Item);
  void SendNextItem();
  void CreateCPUBuffer(size_t size, bool lock = true);
  void SetRemoteMR(RemoteMR rmi, bool override);
  void Write(uint32_t imm_data, size_t buffer_size);
  static void Write(const RdmaChannel* channel, uint32_t imm_data,
                    size_t buffer_size, uint64_t src_addr, uint32_t lkey,
                    uint64_t remote_addr, uint32_t rkey,
                    RdmaWriteIDType write_type, void* write_context);
  static void SendAck(const RdmaChannel* channel);

 protected:
  const RdmaChannel* channel_;
  void* buffer_ = nullptr;
  bool buffer_on_host_ = true;
  size_t size_ = 0;
  const string name_;
  ibv_mr* self_ = nullptr;
  mutex mu_;
  RemoteMR remote_;
  std::queue<string> queue_ GUARDED_BY(mu_);
  BufferStatus local_status_ GUARDED_BY(mu_) = none;
  BufferStatus remote_status_ GUARDED_BY(mu_) = none;
};

}  // namespace tensorflow


#endif  // TENSORFLOW_CONTRIB_VELOX_VELOX_RDMA_H_
