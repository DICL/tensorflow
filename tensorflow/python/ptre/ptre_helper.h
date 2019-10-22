#ifndef TENSORFLOW_PYTHON_PTRE_PTRE_HELPER_H_
#define TENSORFLOW_PYTHON_PTRE_PTRE_HELPER_H_

#include <Python.h>

#include "tensorflow/c/c_api.h"
#include "tensorflow/core/lib/gtl/inlined_vector.h"
#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

typedef tensorflow::gtl::InlinedVector<const char*, 8> NameVector;
typedef gtl::InlinedVector<TF_Tensor*, 8> TF_TensorVector;

void PTRE_InitTrainableVariables_wrapper(PTRE_Server* server,
                                         const NameVector& var_names,
                                         PyObject* vars);

}  // namespace tensorflow

#endif  // TENSORFLOW_PYTHON_PTRE_PTRE_HELPER_H_
