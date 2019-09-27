#ifndef TENSORFLOW_PTRE_GRPC_PTRE_CLIENT_H_
#define TENSORFLOW_PTRE_GRPC_PTRE_CLIENT_H_

#include "tensorflow/ptre/grpc_ptre_service.h"
#include "tensorflow/ptre/ptre_service.pb.h"
#include "tensorflow/core/distributed_runtime/call_options.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_util.h"

namespace tensorflow {

class GrpcPtreClient {
 public:
  explicit GrpcPtreClient(SharedGrpcChannelPtr client_channel)
      : stub_(grpc::PtreService::NewStub(client_channel)) {}
  ~GrpcPtreClient() {}

  Status GetRemoteAddress(CallOptions* call_options,
                          const GetRemoteAddressRequest* request,
                          GetRemoteAddressResponse* response);
  Status GetRemoteAddress(const GetRemoteAddressRequest* request,
                          GetRemoteAddressResponse* response);

 private:
  std::unique_ptr<grpc::PtreService::Stub> stub_;

  void SetDeadline(::grpc::ClientContext* ctx, int64 time_in_ms);

  TF_DISALLOW_COPY_AND_ASSIGN(GrpcPtreClient);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_PTRE_GRPC_PTRE_CLIENT_H_
