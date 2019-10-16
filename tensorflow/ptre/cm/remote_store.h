#ifndef TENSORFLOW_PTRE_CM_REMOTE_STORE_H_
#define TENSORFLOW_PTRE_CM_REMOTE_STORE_H_

#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

class Tensor;

class RemoteStore {
  friend class Tensor;
  friend class TensorBuffer;

 public:
  RemoteStore(const Tensor& tensor);
  ~RemoteStore();
  //void Write(const Tensor& tensor);
  //void Read(Tensor& tensor);

 private:
  Tensor* tensor_;
};

}  // namespace tensorflow

#endif  // TENSORFLOW_PTRE_CM_REMOTE_STORE_H_
