#ifndef TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_CLIENT_H_
#define TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_CLIENT_H_

#include "tensorflow/contrib/velox/grpc_velox_service_impl.h"
#include "tensorflow/contrib/velox/velox_service.pb.h"
#include "tensorflow/core/distributed_runtime/call_options.h"
#include "tensorflow/core/distributed_runtime/rpc/grpc_util.h"
#include "tensorflow/core/lib/core/status.h"

namespace tensorflow {

// GrpcVeloxClient is a client that uses gRPC to talk to the Velox service.
class GrpcVeloxClient {
 public:
  explicit GrpcVeloxClient(SharedGrpcChannelPtr client_channel)
      : stub_(grpc::VeloxService::NewStub(client_channel)) {}
  ~GrpcVeloxClient() {}

  Status GetRemoteAddress(CallOptions* call_options,
                          const GetRemoteAddressRequest* request,
                          GetRemoteAddressResponse* response);
  Status GetRemoteAddress(const GetRemoteAddressRequest* request,
                          GetRemoteAddressResponse* response);

 private:
  std::unique_ptr<grpc::VeloxService::Stub> stub_;

  void SetDeadline(::grpc::ClientContext* ctx, int64 time_in_ms);

  TF_DISALLOW_COPY_AND_ASSIGN(GrpcVeloxClient);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_CONTRIB_VELOX_GRPC_VELOX_CLIENT_H_
