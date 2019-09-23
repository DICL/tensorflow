#ifndef DICL_PTRE_PTRE_SERVER_LIB_H_
#define DICL_PTRE_PTRE_SERVER_LIB_H_

//#include "tensorflow/core/distributed_runtime/server_lib.h"
//#include "tensorflow/core/platform/env.h"
#include "tensorflow/ptre/rdma_mgr.h"
#include "tensorflow/ptre/grpc_ptre_server.h"
//#include "tensorflow/core/distributed_runtime/rpc/grpc_server_lib.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <thread>

namespace tensorflow {

class PtreServiceImpl;

//struct PtreWorkerEnv {
//  Env* env = nullptr;
//  SessionMgr* session_mgr = nullptr;
//  thread::ThreadPool* compute_pool = nullptr;
//};

class PtreServer {
 public:
  static void Create(const int& rank,
                     std::unique_ptr<PtreServer>* out_server);
  PtreServer(const int& rank);
  // Destruction is only supported in the factory method. Clean
  // shutdown is not currently implemented for this server type.
  ~PtreServer();

  void Init();
  void Start();
  void GrpcStart();
  // Implementations of ServerInterface methods.
  //Status Start() override;
  //Status Stop() override;
  //Status Join() override;
  //const string target() const override;

  //void set_rank();
  int get_rank();

 private:
  typedef std::unordered_map<string, ::grpc::Channel*> GrpcChannelTable;

  //static void RunGrpcServer(const string& worker);
  void GrpcChannelTableFactory(GrpcChannelTable &channel_table);

  int rank_;
  string local_worker_;
  RdmaMgr* rdma_mgr_;
  //std::unique_ptr<Thread> ptre_thread_ GUARDED_BY(mu_);
  PtreServiceImpl* ptre_service_ = nullptr;
  GrpcChannelTable grpc_channel_table_;
  // Guards state transitions.
  mutex mu_;
  
};

}  // namespace tensorflow

#endif  // DICL_PTRE_PTRE_SERVER_LIB_H_
