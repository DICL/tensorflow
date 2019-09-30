#include "tensorflow/core/framework/op_kernel.h"

namespace tensorflow {

class VeloxPushOp : public AsyncOpKernel {
 public:
  explicit VeloxPushOp(OpKernelConstruction* ctx) : AsyncOpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("ea_mgr", ea_mgr_));
  }
  void ComputeAsync(OpKernelContext* ctx, DoneCallback done) override {
    auto tensor = ctx->input(0);
    /*
    NEED:
      RdmaMessageBuffer
      RdmaChannel
      dst
      RdmaMemoryRegion


    RdmaMemoryRegionRequest
    RdmaChannel
    worker_env
    MR

    RdmaPushRequest(
    Push(input
    input_tensor,
         push_attempt_id,
         
    */
  }
 private:
  RdmaEaMgr* ea_mgr_;
};

//class P2PTfPullOp : public AsyncOpKernel {
// public:
//  explicit P2PTfPullOp(OpKernelConstruction* ctx) : AsyncOpKernel(ctx) {
//    OP_REQUIRES(
//        ctx, ctx->rendezvous() != nullptr,
//        errors::Internal("Op kernel ctx needs to provide a rendezvous."));
//    auto rendezvous = ctx->rendezvous();
//  }
//  void ComputeAsync
//    OP_REQUIRES_ASYNC(
//        ctx, ctx->rendezvous() != nullptr,
//        errors::Internal("Op kernel ctx needs to provide a rendezvous."),
//        done);
//    Rendezvous::Args args;
//    args.device_context = ctx->op_device_context();
//    args.alloc_attrs = ctx->output_alloc_attr(0);
//    rendezvous->RecvAsync(parsed_key_, args,
//                          make_recv_callback(ctx, std::move(done)));
//};

//REGISTER_KERNEL_BUILDER(Name("P2PTfPull").Device(DEVICE_CPU),
//                        P2PTfPullOp);
}  // namespace tensorflow
