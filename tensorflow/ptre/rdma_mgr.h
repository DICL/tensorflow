#ifndef TENSORFLOW_PTRE_RDMA_MGR_H_
#define TENSORFLOW_PTRE_RDMA_MGR_H_


#include <string>
#include <unordered_map>

#include "tensorflow/ptre/rdma.h"
#include "tensorflow/ptre/grpc_ptre_client.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_channel.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_util.h"

namespace tensorflow {

class RdmaMgr {
  friend class RdmaChannel;
  friend class RdmaAdapter;

 public:
  explicit RdmaMgr(const string& worker,
                   GrpcChannelCache* const grpc_channel_cache);
  ~RdmaMgr();
  RdmaChannel* FindChannel(const string& key);
  void SetupChannels();
  bool ConnectivityCheck();
  void InitAllocators();
  static void RegMemVisitors();
  const string& local_worker() { return local_worker_; }

 private:
  string local_worker_;
  size_t num_remote_workers_;
  RdmaAdapter* rdma_adapter_;
  GrpcChannelCache* const grpc_channel_cache_;
  typedef std::unordered_map<string, RdmaChannel*> ChannelTable;
  ChannelTable channel_table_;
  TF_DISALLOW_COPY_AND_ASSIGN(RdmaMgr);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_PTRE_RDMA_MGR_H_
