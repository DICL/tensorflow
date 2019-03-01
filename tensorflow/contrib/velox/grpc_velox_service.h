#ifndef TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_SERVICE_H_
#define TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_SERVICE_H_

#include "tensorflow/contrib/velox/grpc_velox_service_impl.h"
#include "tensorflow/contrib/verbs/rdma_mgr.h"
#include "tensorflow/contrib/velox/velox_service.pb.h"
#include "tensorflow/core/distributed_runtime/rpc/async_service_interface.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_call.h"
#include "tensorflow/core/lib/core/refcount.h"

namespace grpc {
class ServerBuilder;
class ServerCompletionQueue;
class Alarm;
}  // namespace grpc

namespace tensorflow {

class GrpcVeloxService : public AsyncServiceInterface {
 public:
  GrpcVeloxService(const WorkerEnv* worker_env, ::grpc::ServerBuilder* builder);
  ~GrpcVeloxService();
  void HandleRPCsLoop() override;
  void Shutdown() override;
  void SetRdmaMgr(RdmaMgr* rdma_mgr) { rdma_mgr_ = rdma_mgr; }

 private:
  template <class RequestMessage, class ResponseMessage>
  using WorkerCall = Call<GrpcVeloxService, grpc::VeloxService::AsyncService,
                          RequestMessage, ResponseMessage>;
  void GetRemoteAddressHandler(
      WorkerCall<GetRemoteAddressRequest, GetRemoteAddressResponse>* call);
  Status GetRemoteAddressSync(const GetRemoteAddressRequest* request,
                              GetRemoteAddressResponse* response);

  ::grpc::ServerCompletionQueue* cq_;
  grpc::VeloxService::AsyncService velox_service_;
  mutex shutdown_mu_;
  bool is_shutdown_ GUARDED_BY(shutdown_mu_);
  ::grpc::Alarm* shutdown_alarm_;
  // not owned
  RdmaMgr* rdma_mgr_;
  const WorkerEnv* const worker_env_;

  TF_DISALLOW_COPY_AND_ASSIGN(GrpcVeloxService);
};

// Create a GrpcVeloxService, then assign it to a given handle.
void SetNewVeloxService(GrpcVeloxService** handle, const WorkerEnv* worker_env,
                        ::grpc::ServerBuilder* builder);

}  // namespace tensorflow

#endif  // TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_SERVICE_H_
