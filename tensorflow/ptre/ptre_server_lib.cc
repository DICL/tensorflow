#include "tensorflow/ptre/ptre_server_lib.h"

#include <string>
#include <memory>

#include "tensorflow/ptre/ptre_util.h"

//#include "tensorflow/core/lib/strings/strcat.h"

namespace tensorflow {

class PtreServiceImpl;

std::once_flag reg_mem_visitors_call;

PtreServer::~PtreServer() {
  delete ptre_service_;
  //delete rdma_mgr_;
}

/* static */
void PtreServer::Create(const int& rank,
                        std::unique_ptr<PtreServer>* out_server) {
  std::unique_ptr<PtreServer> ret(new PtreServer(rank));
  LOG(INFO) << "Created PtreServer for rank " << rank << std::endl;
  ret->Init();
  //TF_RETURN_IF_ERROR(ret->Init(service_func, NewRdmaRendezvousMgr));
  *out_server = std::move(ret);
}

PtreServer::PtreServer(const int& rank) : rank_(rank) {
  local_worker_ = PtreUtil::GetWorkerNameByRank(rank_);
  //rdma_mgr_ = new RdmaMgr(local_worker_);
}


void PtreServer::GrpcStart() {
  LOG(INFO) << "Starting GrpcPtreServer for rank " << rank_ << std::endl;
  string worker(local_worker_);
  std::string server_address(PtreUtil::GetGrpcServerAddress());
  //TODO: Complete PtreServiceImpl
  ::grpc::ServerBuilder builder;
  ptre_service_ = new PtreServiceImpl(worker, &builder);
  builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
  builder.RegisterService(ptre_service_);
  builder.AddCompletionQueue();
  Env::Default()->StartThread(ThreadOptions(), "TF_grpc_ptre_service",
                              [this] { ptre_service_->HandleRPCsLoop(); });
  LOG(INFO) << "GrpcPtreServer listening on " << server_address << std::endl;
}

void PtreServer::GrpcChannelTableFactory(GrpcChannelTable &channel_table) {

}

void PtreServer::Init() {
  LOG(INFO) << "Initializing PtreServer for rank " << rank_ << std::endl;
  // 1. Build and Run grpc server
  GrpcStart();

  //std::call_once(reg_mem_visitors_call, []() { RdmaMgr::RegMemVisitors(); });
  //{
  //  mutex_lock l(mu_);
  //  // 1. Init grpc channel cache
  //  // 2. Init RDMA Manager
  //}
  //TODO: pywrap_tensorflow.PtreServer_Start()
  //Start();
  while (true) {
  }
}

void PtreServer::Start() {
  LOG(INFO) << "Starting PtreServer for rank " << rank_ << std::endl;

  // rdma_mgr client 
  
  mutex_lock l(mu_);
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

int PtreServer::get_rank() {
  return rank_;
}

/* static */
//void PtreServer::RunGrpcServer(const string& worker) {
//  std::string server_address(PtreUtil::GetGrpcServerAddress());
//LOG(INFO) << "RunGrpcServer(): got server_address" << server_address << std::endl;
//  //TODO: Complete PtreServiceImpl
//  PtreServiceImpl service(worker)//;
//LOG(INFO) << "RunGrpcServer(): initialized a PtreServiceImpl instance." << std::endl;
//  ::grpc::ServerBuilder builder;
//  builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
//LOG(INFO) << "RunGrpcServer(): AddedListeningPort" << std::endl;
//  builder.RegisterService(&service);
//LOG(INFO) << "RunGrpcServer(): registered service(PtreServiceImpl)" << std::endl;
//  //cq_ = builder->AddCompletionQueue().release();
//  builder.AddCompletionQueue();
//LOG(INFO) << "RunGrpcServer(): added CompletionQueue" << std::endl;
//  std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
//  LOG(INFO) << "GrpcPtreServer listening on " << server_address << std::endl;
//  server->Wait();
//}


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
