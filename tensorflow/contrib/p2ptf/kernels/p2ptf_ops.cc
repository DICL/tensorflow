#include "tensorflow/core/framework/op_kernel.h"

namespace dddl {
namespace tensorflow {

class DddlPushOp : public AsyncOpKernel {
public:
  explicit DddlPushOp(OpKernelConstruction* ctx)
      : AsyncOpKernel(ctx) {}
  void ComputeAsync(OpKernelContext* ctx, DoneCallback done) override {
    const Tensor& input_tensor = ctx->input(0);
  }
};

class DddlRecvOp : public AsyncOpKernel {
};

class DddlPullOp : public AsyncOpKernel {
 public:
  explicit DddlPullOp(OpKernelConstruction* ctx) : AsyncOpKernel(ctx) {
    OP_REQUIRES(
        ctx, ctx->rendezvous() != nullptr,
        errors::Internal("Op kernel ctx needs to provide a rendezvous."));
    auto rendezvous = ctx->rendezvous();
  }
  void ComputeAsync
    OP_REQUIRES_ASYNC(
        ctx, ctx->rendezvous() != nullptr,
        errors::Internal("Op kernel ctx needs to provide a rendezvous."),
        done);
    Rendezvous::Args args;
    args.device_context = ctx->op_device_context();
    args.alloc_attrs = ctx->output_alloc_attr(0);
    rendezvous->RecvAsync(parsed_key_, args,
                          make_recv_callback(ctx, std::move(done)));
};

REGISTER_KERNEL_BUILDER(Name("DddlPush").Device(DEVICE_CPU),
                        DddlPushOp);
REGISTER_OP("DddlPush")
    .Input("input: T")
    .Output("data: T")
    .Attr("T: {float, float16, float64, int32, int64}")
    .Attr("group_size: int")
    .Attr("group_key: int")
    .Attr("instance_key: int")
    .Attr("shape: shape")
    .SetIsStateful()
    .SetShapeFn(shape_inference::ExplicitShape);

}  // namespace tensorflow
}  // namespace dddl
