#ifndef TENSORFLOW_PTRE_CORE_PTRE_SERVER_H_
#define TENSORFLOW_PTRE_CORE_PTRE_SERVER_H_

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/platform/mutex.h"
#include "tensorflow/ptre/rpc/ptre_service_impl.h"

namespace tensorflow {

class PtreServer {
 public:
  PtreServer(int rank);
  ~PtreServer();

  void Init();
  void Start();
  void Stop();
  void Join();

  bool CheckIncoming();
  void InitTrainableVariables(const std::vector<std::string>& names,
                              const std::vector<DataType>& dtypes,
                              const std::vector<TensorShape>& shapes,
                              int nvars);
  //void NewPtreServer(int rank, std::unique_ptr<PtreServer>* out_server);
  const std::string target() const;

 private:
  void GrpcStart();

  mutex mu_;
  enum State { DISCONNECTED, CONNECTED };
  PtreServiceImpl* ptre_service_ = nullptr;
  int rank_;
};

void NewPtreServer(int rank, std::unique_ptr<PtreServer>* out_server);

}

#endif  // TENSORFLOW_PTRE_CORE_PTRE_SERVER_H_
