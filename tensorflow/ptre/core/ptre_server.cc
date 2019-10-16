#include "tensorflow/ptre/core/ptre_server.h"

#include <string>
#include "tensorflow/ptre/rpc/ptre_service_impl.h"

namespace tensorflow {

PtreServer::PtreServer(int rank) : rank_(rank) {}

PtreServer::~PtreServer() {
  delete ptre_service_;
}

void PtreServer::Init() {
}
void PtreServer::Start() {
}
void PtreServer::Stop() {
}
void PtreServer::Join() {
}

bool PtreServer::CheckIncoming() {
  //TODO: imp.
  //auto ret = cm_->CheckIncoming();
  //return ret;
  return true;
}

void PtreServer::InitTrainableVariables(const std::vector<std::string>& names,
                                        const std::vector<DataType>& dtypes,
                                        const std::vector<TensorShape>& shapes,
                                        int nvars) {
  LOG(INFO) << "PtreServer got " << nvars << " tensors:" << std::endl;
  for (int i = 0; i < nvars; i++) {
    LOG(INFO) << i << ": " << names[i] << std::endl;
  }
}

const std::string PtreServer::target() const {
  return "grpc://localhost:50051";
}

void PtreServer::GrpcStart() {
  std::string server_address("0.0.0.0:50051");

}

void NewPtreServer(int rank, std::unique_ptr<PtreServer>* out_server) {
  std::unique_ptr<PtreServer> ret(new PtreServer(rank));
  *out_server = std::move(ret);
}

}  // namespace tensorflow
