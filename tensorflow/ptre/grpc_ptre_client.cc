#include "tensorflow/ptre/grpc_ptre_client.h"

#include "tensorflow/core/distributed_runtime/rpc/grpc_util.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"

namespace tensorflow {

Status GrpcPtreClient::GetRemoteAddress(CallOptions* call_options,
                                         const GetRemoteAddressRequest* request,
                                         GetRemoteAddressResponse* response) {
  ::grpc::ClientContext ctx;
  ctx.set_fail_fast(false);
  SetDeadline(&ctx, call_options->GetTimeout());
  return FromGrpcStatus(stub_->GetRemoteAddress(&ctx, *request, response));
}

Status GrpcPtreClient::GetRemoteAddress(const GetRemoteAddressRequest* request,
                                         GetRemoteAddressResponse* response) {
  CallOptions call_options;
  call_options.SetTimeout(-1);  // no time out
  return GetRemoteAddress(&call_options, request, response);
}

void GrpcPtreClient::SetDeadline(::grpc::ClientContext* ctx,
                                  int64 time_in_ms) {
  if (time_in_ms > 0) {
    ctx->set_deadline(gpr_time_from_millis(time_in_ms, GPR_TIMESPAN));
  }
}

}  // namespace tensorflow
