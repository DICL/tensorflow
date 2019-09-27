#include "tensorflow/ptre/ptre_server_lib.h"

namespace tensorflow {

class PtreServiceImpl;

std::once_flag reg_mem_visitors_call;

PtreServer::~PtreServer() {
  delete ptre_service_;
  delete rdma_mgr_;
  delete grpc_channel_cache_;
}

/* static */
void PtreServer::Create(const ServerDef& server_def,
                        const int& rank,
                        std::unique_ptr<PtreServer>* out_server) {
  std::unique_ptr<PtreServer> ret(new PtreServer(server_def, rank));
  LOG(INFO) << "Created PtreServer for rank " << rank << std::endl;
  ret->Init();
  //TF_RETURN_IF_ERROR(ret->Init(service_func, NewRdmaRendezvousMgr));
  *out_server = std::move(ret);
}

PtreServer::PtreServer(const ServerDef& server_def,
                       const int& rank)
    : rank_(rank), server_def_(server_def) {
  local_worker_ = PtreUtil::GetWorkerNameByRank(rank_);
}


void PtreServer::GrpcStart() {
  LOG(INFO) << "Starting GrpcPtreServer for rank " << rank_ << std::endl;
  string worker(local_worker_);
  std::string server_address(PtreUtil::GetGrpcServerAddress());
  //TODO: Complete PtreServiceImpl
  ::grpc::ServerBuilder builder;
  ptre_service_ = new PtreServiceImpl(worker, &builder, rdma_mgr_);
  builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
  builder.RegisterService(ptre_service_);
  builder.AddCompletionQueue();
  Env::Default()->StartThread(ThreadOptions(), "TF_grpc_ptre_service",
                              [this] { ptre_service_->HandleRPCsLoop(); });
  LOG(INFO) << "GrpcPtreServer listening on " << server_address << std::endl;
}

//void PtreServer::GrpcChannelTableFactory(GrpcChannelTable &channel_table) {
//  std::vector<string> workers;
//  PtreUtil::ListWorkers(workers);
//  for (size_t i = 0; i < workers.size(); i++) {
//    if (local_worker_.compare(workers[i]) != 0) {
//      grpc_channel_table_.insert(
//          {workers[i],
//           new RdmaChannel(rdma_adapter_, local_worker_, workers[i])});
//    }
//  }
//
//}

Status PtreServer::ChannelCacheFactory(const ServerDef& server_def,
                                        GrpcChannelCache** grpc_channel_cache) {
LOG(INFO) << "Creating channel cache. job_name=" << server_def.job_name()
  << ", task_index=" << server_def.task_index() << std::endl;  // WKIM_DEBUG
  string name_prefix =
      strings::StrCat("/job:", server_def.job_name(), "/replica:0",
                      "/task:", server_def.task_index());
LOG(INFO) << "name_prefix=" << name_prefix << std::endl;  // WKIM_DEBUG

  GrpcChannelSpec channel_spec;
  TF_RETURN_IF_ERROR(ParseChannelSpec(server_def, &channel_spec));
LOG(INFO) << "Parsed GrpcChannelSpec." << std::endl;  // WKIM_DEBUG

  *grpc_channel_cache =
      NewGrpcChannelCache(channel_spec, GetChannelCreationFunction());
LOG(INFO) << "Created GrpcChannelCache." << std::endl;  // WKIM_DEBUG

  const string host_port = (*grpc_channel_cache)->TranslateTask(name_prefix);
LOG(INFO) << "got host_port=" << host_port << std::endl;  // WKIM_DEBUG
  int requested_port;

  if (!strings::safe_strto32(str_util::Split(host_port, ':')[1],
                             &requested_port)) {
    return errors::Internal("Could not parse port for local server from \"",
                            (*grpc_channel_cache)->TranslateTask(name_prefix),
                            "\".");
  }
  if (requested_port != bound_port_) {
LOG(INFO) << "InvalidArgument:" << "Requested port " << requested_port <<  " differs from expected port " << bound_port_ << std::endl;  // WKIM_DEBUG
  int requested_port;
    return errors::InvalidArgument("Requested port ", requested_port,
                                   " differs from expected port ",
                                   bound_port_);
  }

  return Status::OK();
}

ChannelCreationFunction PtreServer::GetChannelCreationFunction() const {
  // We can do this because SparseGrpcChannelCache is robust to nullptr being
  // returned by the channel creation function
  return ConvertToChannelCreationFunction(NewHostPortGrpcChannel);
}

Status PtreServer::ParseChannelSpec(const WorkerCacheFactoryOptions& options,
                                    GrpcChannelSpec* channel_spec) {
LOG(INFO) << "Parsing ChannelSpec" << std::endl;  // WKIM_DEBUG
  for (const auto& job : options.cluster_def->job()) {
LOG(INFO) << "Inside for (const auto& job : options.cluster_def->job())" << std::endl;  // WKIM_DEBUG
    std::map<int, string> host_ports;
    for (const auto& task : job.tasks()) {
LOG(INFO) << "Inside for (const auto& task : job.tasks())" << std::endl;  // WKIM_DEBUG
      string& host_port = host_ports[task.first];
      if (!host_port.empty()) {
        return errors::InvalidArgument("JobDef for job \"", job.name(),
                                       "\" specified two addresses for task \"",
                                       task.first, "\": ", host_port, " and ",
                                       task.second);
      }
      if (job.name() == *options.job_name && task.first == options.task_index) {
        host_port = strings::StrCat("localhost:", bound_port_);
      } else {
        host_port = task.second;
      }
    }
    TF_RETURN_IF_ERROR(channel_spec->AddHostPortsJob(job.name(), host_ports));
  }
  return Status::OK();
}

void PtreServer::Init() {
  LOG(INFO) << "Initializing PtreServer for rank " << rank_ << std::endl;
  CHECK(ChannelCacheFactory(server_def_, &grpc_channel_cache_).ok());
LOG(INFO) << "PtreServer::Init(): Creating RdmaMgr." << std::endl;
  rdma_mgr_ = new RdmaMgr(local_worker_, grpc_channel_cache_);
  // Build and Run grpc server
LOG(INFO) << "PtreServer::Init(): Calling GrpcStart()." << std::endl;
  GrpcStart();

  std::call_once(reg_mem_visitors_call, []() { RdmaMgr::RegMemVisitors(); });
  {
    mutex_lock l(mu_);
    // 1. Init grpc channel table

    // 2. Init RDMA Manager
  }
  //TODO: pywrap_tensorflow.PtreServer_Start()
  Start();
  while (true) {
  }
}

void PtreServer::Start() {
  LOG(INFO) << "Starting PtreServer for rank " << rank_ << std::endl;

  // rdma_mgr client 
  {
    mutex_lock l(mu_);
    LOG(INFO) << "Setting up rdma channels." << std::endl;
    rdma_mgr_->SetupChannels();
      //grpc_thread_.reset(ptre_worker_env()->env->StartThread(
      //      ThreadOptions(), "TF_ptre_service",
      //      [this] { ptre_service_->HandleRPCsLoop(); }));
      // Check connectivity by pinging every channel
      /*
      polling_thread_.reset(Env::Default()->StartThread(
            ThreadOptions(), "RdmaCQThread",
            [this] { Process_CQ(); }));
      */
  }
}

int PtreServer::get_rank() {
  return rank_;
}



}  // namespace tensorflow

/*
  tensorflow::PtreServer* PtreServer_New(
      const tensorflow::ServerDef& server_def,
      tensorflow::Env* env, const int rank) {
    return new tensorflow::PtreServer(server_def, env, rank);
  }

static void PyServer_New(const ServerDef& server_def,
                         std::unique_ptr<tensorflow::ServerInterface>* out_server,
                         TF_Status* out_status) {
  tensorflow::Status status =
      tensorflow::NewServer(server_def, out_server);
  tensorflow::Set_TF_Status_from_Status(out_status, status);
}

static void PyServer_Start(
    tensorflow::ServerInterface* in_server,
    TF_Status* out_status) {
  tensorflow::Set_TF_Status_from_Status(out_status, in_server->Start());
}

static void PyServer_Stop(
    tensorflow::ServerInterface* in_server,
    TF_Status* out_status) {
  tensorflow::Set_TF_Status_from_Status(out_status, in_server->Stop());
}

static void PyServer_Join(
    tensorflow::ServerInterface* in_server,
    TF_Status* out_status) {
  tensorflow::Set_TF_Status_from_Status(out_status, in_server->Join());
}
*/
