#include "tensorflow/contrib/velox/velox_server_lib.h"

#include <memory>

#include "grpc/support/alloc.h"

#include "tensorflow/contrib/verbs/rdma_mgr.h"
#include "tensorflow/contrib/verbs/rdma_rendezvous_mgr.h"
#include "tensorflow/core/distributed_runtime/server_lib.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

namespace {

// static utility function
RendezvousMgrInterface* NewRdmaRendezvousMgr(const WorkerEnv* env) {
  return new RdmaRendezvousMgr(env);
}

std::once_flag reg_mem_visitors_call;

}  // namespace

VeloxServer::VeloxServer(const ServerDef& server_def, Env* env)
    : GrpcServer(server_def, env), velox_state_(DISCONNECTED) {}

Status VeloxServer::Start() {
  Status s = GrpcServer::Start();
  {
    mutex_lock l(mu_);
    if (velox_state_ == DISCONNECTED) {
      // velox_thread needs to be initiated
      // before rdma_mgr sets up the rdma channels.
      velox_thread_.reset(worker_env()->env->StartThread(
          ThreadOptions(), "TF_velox_service",
          [this] { velox_service_->HandleRPCsLoop(); }));
      rdma_mgr_->SetupChannels();
      CHECK(rdma_mgr_->ConnectivityCheck()) << "Connectivity check failed!";
      rdma_mgr_->InitAllocators();
      velox_state_ = CONNECTED;
    }
  }
  return s;
}

Status VeloxServer::Join() {
  Status s = GrpcServer::Join();
  {
    mutex_lock l(mu_);
    if (velox_state_ == CONNECTED) {
      velox_state_ = DISCONNECTED;
      velox_thread_.reset();
    }
  }
  return s;
}

Status VeloxServer::Create(const ServerDef& server_def, Env* env,
                           std::unique_ptr<ServerInterface>* out_server) {
  std::unique_ptr<VeloxServer> ret(new VeloxServer(server_def, Env::Default()));
  ServiceInitFunction service_func = [&ret](const WorkerEnv* worker_env,
                                            ::grpc::ServerBuilder* builder) {
    return SetNewVeloxService(&ret->velox_service_, worker_env, builder);
  };
  TF_RETURN_IF_ERROR(ret->Init(service_func, NewRdmaRendezvousMgr));
  *out_server = std::move(ret);
  return Status::OK();
}

void VeloxServer::AddParamTensor(const Tensor& param_tensor) {
  params_.emplace_back(param_tensor);
}

Tensor VeloxServer::GetParamTensorByIdx(const uint32_t idx) {
  return params_[idx];
}

VeloxServer::~VeloxServer() {
  TF_CHECK_OK(Stop());
  TF_CHECK_OK(Join());
  delete rdma_mgr_;
  delete velox_service_;
  delete channel_cache_;
}

Status VeloxServer::ChannelCacheFactory(const ServerDef& server_def,
                                        GrpcChannelCache** channel_cache) {
  string name_prefix =
      strings::StrCat("/job:", server_def.job_name(), "/replica:0",
                      "/task:", server_def.task_index());

  GrpcChannelSpec channel_spec;
  TF_RETURN_IF_ERROR(ParseChannelSpec(server_def, &channel_spec));

  *channel_cache =
      NewGrpcChannelCache(channel_spec, GetChannelCreationFunction());

  const string host_port = (*channel_cache)->TranslateTask(name_prefix);
  int requested_port;

  if (!strings::safe_strto32(str_util::Split(host_port, ':')[1],
                             &requested_port)) {
    return errors::Internal("Could not parse port for local server from \"",
                            (*channel_cache)->TranslateTask(name_prefix),
                            "\".");
  }
  if (requested_port != bound_port()) {
    return errors::InvalidArgument("Requested port ", requested_port,
                                   " differs from expected port ",
                                   bound_port());
  }

  return Status::OK();
}

Status VeloxServer::Init(ServiceInitFunction service_func,
                         RendezvousMgrCreationFunction rendezvous_mgr_func) {
  std::call_once(reg_mem_visitors_call, []() { RdmaMgr::RegMemVisitors(); });
  Status s = GrpcServer::Init(service_func, rendezvous_mgr_func);
  {
    mutex_lock l(mu_);
    CHECK_EQ(velox_state_, DISCONNECTED);
    CHECK(ChannelCacheFactory(server_def(), &channel_cache_).ok());
    rdma_mgr_ = new RdmaMgr(worker_env(), channel_cache_);
    // set rdma_mgr for velox_service and rdma_rendezvous_mgr
    velox_service_->SetRdmaMgr(rdma_mgr_);
    dynamic_cast<RdmaRendezvousMgr*>(worker_env()->rendezvous_mgr)
        ->SetRdmaMgr(rdma_mgr_);
  }
  return s;
}

namespace {

class VeloxServerFactory : public ServerFactory {
 public:
  bool AcceptsOptions(const ServerDef& server_def) override {
    return server_def.protocol() == "velox";
  }

  Status NewServer(const ServerDef& server_def,
                   std::unique_ptr<ServerInterface>* out_server) override {
    return VeloxServer::Create(server_def, Env::Default(), out_server);
  }
};

// Registers a `ServerFactory` for `VeloxServer` instances.
class VeloxServerRegistrar {
 public:
  VeloxServerRegistrar() {
    gpr_allocation_functions alloc_fns;
    alloc_fns.malloc_fn = port::Malloc;
    alloc_fns.realloc_fn = port::Realloc;
    alloc_fns.free_fn = port::Free;
    gpr_set_allocation_functions(alloc_fns);
    ServerFactory::Register("VELOX_SERVER", new VeloxServerFactory());
  }
};
static VeloxServerRegistrar registrar;

}  // namespace

}  // namespace tensorflow
