%include "tensorflow/python/platform/base.i"

%typemap(in, numinputs=0)
    std::unique_ptr<tensorflow::PtreServer>* out_server (
        std::unique_ptr<tensorflow::PtreServer> temp) {
  $1 = &temp;
}

%typemap(argout) std::unique_ptr<tensorflow::Ptre>* out_server {
  // TODO(mrry): Convert this to SWIG_POINTER_OWN when the issues with freeing
  // a server are fixed.
  $result = SWIG_NewPointerObj($1->release(),
                               $descriptor(tensorflow::PtreServer*));
}

%{
#include "tensorflow/ptre/ptre_server_lib.h"
//#include "tensorflow/ptre/grpc_ptre_server.h"
//#include "tensorflow/ptre/grpc_ptre_service.h"

//using tensorflow::PtreServiceImpl;
//class PtreServiceImpl;

static void PtreServer_New(const int& rank,
    std::unique_ptr<tensorflow::PtreServer>* out_server) {
  tensorflow::PtreServer::Create(rank, out_server);
}

%}

// Wrap this function.
void PtreServer_New(const int& rank,
    std::unique_ptr<tensorflow::PtreServer>* out_server);

%ignoreall

%unignore tensorflow;
//%unignore tensorflow::PtreServiceImpl;
//%unignore PtreServiceImpl;
%unignore PtreServer_New;

%include "tensorflow/ptre/ptre_server_lib.h"
//%include "tensorflow/ptre/grpc_ptre_server.h"
//%include "tensorflow/ptre/grpc_ptre_service.h"

%unignoreall
