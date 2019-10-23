#include "tensorflow/python/ptre/ptre_helper.h"
#include "tensorflow/c/tf_tensor.h"
#include "tensorflow/python/lib/core/ndarray_tensor.h"

namespace tensorflow {

void PTRE_InitTrainableVariables_wrapper(PTRE_Server* server,
                                         const NameVector& var_names,
                                         PyObject* vars) {
  LOG(INFO) << "Begining PTRE_InitTrainableVariables_wrapper" << std::endl;
  int nvars = PyList_Size(vars);
  LOG(INFO) << "PyList_Size(vars)=" << nvars << std::endl;
  TF_TensorVector input_vals;
  std::vector<Safe_TF_TensorPtr> input_vals_safe;
  for (int i = 0; i < nvars; i++) {
    auto ndarray = PyList_GetItem(vars, i);
    input_vals_safe.emplace_back(make_safe(static_cast<TF_Tensor*>(nullptr)));
    PyArrayToTF_Tensor(ndarray, &input_vals_safe.back());
    input_vals.push_back(input_vals_safe.back().get());
  }

  //s = PyArrayToTF_Tensor(value, &inputs_safe.back());
  LOG(INFO) << "Calling PTRE_InitTrainableVariables in wrapper" << std::endl;
  PTRE_InitTrainableVariables(server,
                              const_cast<const char**>(var_names.data()),
                              input_vals.data(),
                              var_names.size());
}

//void PTRE_LogDebugString_wrapper(PTRE_Server* server,
//                                 const char* name,
//                                 int max_entries) {
//  server->server->LogDebugString(name, max_entries);
//}

}  // namespace tensorflow
