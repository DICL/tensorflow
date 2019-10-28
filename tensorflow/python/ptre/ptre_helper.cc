#include "tensorflow/python/ptre/ptre_helper.h"
#include "tensorflow/c/tf_tensor.h"
#include "tensorflow/python/lib/core/ndarray_tensor.h"

namespace tensorflow {

void PTRE_InitTrainableVariables_wrapper(PTRE_Server* server,
                                         const NameVector& var_names,
                                         PyObject* ndarrs_t,
                                         PyObject* ndarrs_c) {
  LOG(INFO) << "Begining PTRE_InitTrainableVariables_wrapper" << std::endl;
  TF_TensorVector tvec_t;
  TF_TensorVector tvec_c;
  std::vector<Safe_TF_TensorPtr> tvec_t_safe;
  std::vector<Safe_TF_TensorPtr> tvec_c_safe;
  int nvars = PyList_Size(ndarrs_t);
  LOG(INFO) << "PyList_Size(ndarrs_t)=" << nvars << std::endl;
  for (int i = 0; i < nvars; i++) {
    auto ndarray = PyList_GetItem(ndarrs_t, i);
    tvec_t_safe.emplace_back(make_safe(static_cast<TF_Tensor*>(nullptr)));
    PyArrayToTF_Tensor(ndarray, &tvec_t_safe.back());
    tvec_t.push_back(tvec_t_safe.back().get());
  }
  nvars = PyList_Size(ndarrs_c);
  LOG(INFO) << "PyList_Size(ndarrs_c)=" << nvars << std::endl;
  for (int i = 0; i < nvars; i++) {
    auto ndarray = PyList_GetItem(ndarrs_c, i);
    tvec_c_safe.emplace_back(make_safe(static_cast<TF_Tensor*>(nullptr)));
    PyArrayToTF_Tensor(ndarray, &tvec_c_safe.back());
    tvec_c.push_back(tvec_c_safe.back().get());
  }

  //s = PyArrayToTF_Tensor(value, &inputs_safe.back());
  LOG(INFO) << "Calling PTRE_InitTrainableVariables in wrapper" << std::endl;
  PTRE_InitTrainableVariables(server,
                              const_cast<const char**>(var_names.data()),
                              tvec_t.data(),
                              tvec_c.data(),
                              var_names.size());
}

//void PTRE_CmTensor_wrapper(PTRE_Server* server,

//void PTRE_LogDebugString_wrapper(PTRE_Server* server,
//                                 const char* name,
//                                 int max_entries) {
//  server->server->LogDebugString(name, max_entries);
//}

}  // namespace tensorflow
