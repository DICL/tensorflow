#include "tensorflow/contrib/velox/grpc_velox_service_impl.h"

#include "grpcpp/impl/codegen/async_stream.h"
#include "grpcpp/impl/codegen/async_unary_call.h"
#include "grpcpp/impl/codegen/channel_interface.h"
#include "grpcpp/impl/codegen/client_unary_call.h"
#include "grpcpp/impl/codegen/method_handler_impl.h"
#include "grpcpp/impl/codegen/rpc_service_method.h"
#include "grpcpp/impl/codegen/service_type.h"
#include "grpcpp/impl/codegen/sync_stream.h"

namespace tensorflow {

namespace grpc {

static const char* grpcVeloxService_method_names[] = {
    "/tensorflow.VeloxService/GetRemoteAddress",
};

std::unique_ptr<VeloxService::Stub> VeloxService::NewStub(
    const std::shared_ptr< ::grpc::ChannelInterface>& channel,
    const ::grpc::StubOptions& options) {
  std::unique_ptr<VeloxService::Stub> stub(new VeloxService::Stub(channel));
  return stub;
}

VeloxService::Stub::Stub(
    const std::shared_ptr< ::grpc::ChannelInterface>& channel)
    : channel_(channel),
      rpcmethod_GetRemoteAddress_(grpcVeloxService_method_names[0],
                                  ::grpc::internal::RpcMethod::NORMAL_RPC,
                                  channel) {}

::grpc::Status VeloxService::Stub::GetRemoteAddress(
    ::grpc::ClientContext* context, const GetRemoteAddressRequest& request,
    GetRemoteAddressResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(
      channel_.get(), rpcmethod_GetRemoteAddress_, context, request, response);
}

VeloxService::AsyncService::AsyncService() {
  for (int i = 0; i < 1; ++i) {
    AddMethod(new ::grpc::internal::RpcServiceMethod(
        grpcVeloxService_method_names[i],
        ::grpc::internal::RpcMethod::NORMAL_RPC, nullptr));
    ::grpc::Service::MarkMethodAsync(i);
  }
}

VeloxService::AsyncService::~AsyncService() {}

}  // namespace grpc

}  // namespace tensorflow
