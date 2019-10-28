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
def PTRE_InitTrainableVariables(server, tvars, cvars):
  """
  Args:
    server: PTRE_Server.
    tvars: List of tensorflow.python.ops.resource_variable_ops.ResourceVariable
           representing trainable variables of Worker.
    cvars: List of tensorflow.python.ops.resource_variable_ops.ResourceVariable
           for ConsensusManager.
  """
  print("python api PTRE_InitTrainableVariables")
  names = [ v.name for v in tvars ]
  ndarrs_t = [ v.value()._numpy() for v in tvars ]
  ndarrs_c = [ v.value()._numpy() for v in cvars ]
  PTRE_InitTrainableVariables_wrapper(server, names, ndarrs_t, ndarrs_c)
%}

//%unignore tensorflow::PTRE_LogDebugString_wrapper;
//%insert("python") %{
//def PTRE_LogDebugString(server, name, max_entries):
//  PTRE_LogDebugString_wrapper(server, name, max_entries)
//%}

%include "tensorflow/python/ptre/ptre_helper.h"

%unignoreall
