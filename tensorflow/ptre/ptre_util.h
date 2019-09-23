#ifndef TENSORFLOW_PTRE_PTRE_UTIL_H_
#define TENSORFLOW_PTRE_PTRE_UTIL_H_

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

namespace tensorflow {

class PtreUtil {
 public:
  static void ListWorkers(std::vector<string>& workers) {
    workers.emplace_back("dumbo001");
    workers.emplace_back("dumbo002");
  }

  static std::string GetWorkerNameByRank(const int rank) {
    std::stringstream suffix;
    suffix << std::setw(3) << std::setfill('0') << rank;
    std::string worker("dumbo" + suffix.str());
    return worker;
  }

  static std::string GetGrpcServerAddress() {
    return "0.0.0.0:50051";
  }

};


}


#endif  // TENSORFLOW_PTRE_PTRE_UTIL_H_
