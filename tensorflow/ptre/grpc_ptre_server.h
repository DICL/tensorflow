#ifndef TENSORFLOW_PTRE_GRPC_PTRE_SERVER_H_
#define TENSORFLOW_PTRE_GRPC_PTRE_SERVER_H_

#include "grpcpp/grpcpp.h"
#include "tensorflow/ptre/grpc_ptre_service.h"
#include "tensorflow/ptre/ptre_service.pb.h"
#include "tensorflow/ptre/rdma_mgr.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_call.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_util.h"

namespace tensorflow {


class PtreServiceImpl : public grpc::PtreService::AsyncService {
 public:
  PtreServiceImpl(const string& worker,
                  ::grpc::ServerBuilder* builder,
                  RdmaMgr* rdma_mgr);
  ~PtreServiceImpl();
  void HandleRPCsLoop();
  //::grpc::Status GetRemoteAddress(::grpc::ServerContext* context,
  //                                const GetRemoteAddressRequest& request,
  //                                GetRemoteAddressResponse* response);
  Status GetRemoteAddressSync(const GetRemoteAddressRequest* request,
                              GetRemoteAddressResponse* response);

 private:
  template <class RequestMessage, class ResponseMessage>
  using WorkerCall = Call<PtreServiceImpl, grpc::PtreService::AsyncService,
                          RequestMessage, ResponseMessage>;
  void GetRemoteAddressHandler(
      WorkerCall<GetRemoteAddressRequest, GetRemoteAddressResponse>* call);

  RdmaMgr* rdma_mgr_;
  std::string local_worker_;
  mutex shutdown_mu_;
  bool is_shutdown_ GUARDED_BY(shutdown_mu_);
  ::grpc::ServerCompletionQueue* cq_;
};

}  // namespace tensorflow

#endif // TENSORFLOW_PTRE_GRPC_PTRE_SERVER_H_
