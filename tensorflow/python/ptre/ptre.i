%include "tensorflow/python/platform/base.i"

%typemap(in) const ServerDef& (tensorflow::ServerDef temp) {
  char* c_string;
  Py_ssize_t py_size;
  if (PyBytes_AsStringAndSize($input, &c_string, &py_size) == -1) {
    // Python has raised an error (likely TypeError or UnicodeEncodeError).
    SWIG_fail;
  }

  if (!temp.ParseFromString(string(c_string, py_size))) {
    PyErr_SetString(
        PyExc_TypeError,
        "The ServerDef could not be parsed as a valid protocol buffer");
    SWIG_fail;
  }
  $1 = &temp;
}

%typemap(in, numinputs=0)
    std::unique_ptr<tensorflow::PtreServer>* out_server (
        std::unique_ptr<tensorflow::PtreServer> temp) {
  $1 = &temp;
}

%typemap(argout) std::unique_ptr<tensorflow::PtreServer>* out_server {
  // TODO(mrry): Convert this to SWIG_POINTER_OWN when the issues with freeing
  // a server are fixed.
  $result = SWIG_NewPointerObj($1->release(),
                               $descriptor(tensorflow::PtreServer*),
                               0);
}

%{
#include "tensorflow/ptre/ptre_server_lib.h"
//#include "tensorflow/ptre/grpc_ptre_server.h"
//#include "tensorflow/ptre/grpc_ptre_service.h"

using tensorflow::ServerDef;
//using tensorflow::PtreServiceImpl;
//class PtreServiceImpl;

static void PtreServer_New(const ServerDef& server_def,
                           const int& rank,
                           std::unique_ptr<tensorflow::PtreServer>* out_server) {
  tensorflow::PtreServer::Create(server_def, rank, out_server);
}

%}

// Wrap this function.
void PtreServer_New(const ServerDef& server_def,
                    const int& rank,
                    std::unique_ptr<tensorflow::PtreServer>* out_server);

%ignoreall

%unignore tensorflow;
%unignore tensorflow::ServerDef;
//%unignore tensorflow::PtreServiceImpl;
//%unignore PtreServiceImpl;
%unignore PtreServer_New;

%include "tensorflow/ptre/ptre_server_lib.h"
//%include "tensorflow/ptre/grpc_ptre_server.h"
//%include "tensorflow/ptre/grpc_ptre_service.h"

%unignoreall
