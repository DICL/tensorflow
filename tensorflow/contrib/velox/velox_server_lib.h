#ifndef TENSORFLOW_CONTRIB_VELOX_VELOX_SERVER_LIB_H_
#define TENSORFLOW_CONTRIB_VELOX_VELOX_SERVER_LIB_H_

#include <vector>
#include <memory>
#include "tensorflow/core/framework/tensor.h"
//#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/contrib/velox/grpc_velox_service.h"
#include "tensorflow/contrib/verbs/rdma_mgr.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_server_lib.h"

namespace tensorflow {

class VeloxServer : public GrpcServer {
 protected:
  VeloxServer(const ServerDef&, Env*);
 public:
  static Status Create(const ServerDef& server_def, Env* env,
                       std::unique_ptr<ServerInterface>* out_server);
  virtual ~VeloxServer() override;
  Status Start() override;
  Status Join() override;
  void AddParamTensor(const Tensor&);
  Tensor GetParamTensorByIdx(const uint32_t idx);
  void Init();
 protected:
  Status Init(ServiceInitFunction service_func,
              RendezvousMgrCreationFunction rendezvous_mgr_func);
  Status ChannelCacheFactory(const ServerDef& server_def,
                             GrpcChannelCache** channel_cache);
 private:
  RdmaMgr* rdma_mgr_;
  //std::vector<std::pair<string, Tensor>> params_;
  std::vector<Tensor> params_;
  mutex mu_;
  enum State { DISCONNECTED, CONNECTED };
  State velox_state_ GUARDED_BY(mu_);

  GrpcVeloxService* velox_service_ = nullptr;
  std::unique_ptr<Thread> velox_thread_ GUARDED_BY(mu_);
  GrpcChannelCache* channel_cache_ = nullptr;
};

}  // namespace tensorflow

#endif  // TENSORFLOW_CONTRIB_VELOX_VELOX_SERVER_LIB_H_
