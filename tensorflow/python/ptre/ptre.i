%include "tensorflow/python/platform/base.i"

%{
#include "tensorflow/python/ptre/ptre_helper.h"
#include "tensorflow/c/tf_tensor.h"
%}

%include "tensorflow/c/c_api.h"

//%typemap(in) (std::vector<PyObject*>& inputs)
//    (std::vector<TF_Tensor*> inputs) {
//  inputs.
//}

//%ignore PTRE_InitTrainableVariables;
//%rename(PTRE_InitTrainableVariables) tensorflow::PTRE_InitTrainableVariables_wrapper;
//%unignore tensorflow;
//%unignore PTRE_InitTrainableVariables;

%unignore tensorflow::PTRE_InitTrainableVariables_wrapper;
%insert("python") %{
def PTRE_InitTrainableVariables(server, vars):
  print("python api PTRE_InitTrainableVariables")
  var_names = [ v.name for v in vars ]
  ndarrays = [ v.numpy() for v in vars ]
  PTRE_InitTrainableVariables_wrapper(server, var_names, ndarrays)
%}

%include "tensorflow/python/ptre/ptre_helper.h"

%unignoreall
