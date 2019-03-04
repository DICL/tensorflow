#ifndef TENSORFLOW_CONTRIB_VELOX_VELOX_UTIL_H_
#define TENSORFLOW_CONTRIB_VELOX_VELOX_UTIL_H_

namespace tensorflow {

// Tensor meta-data
class TensorMetaData {
 public:
  TensorShape tensor_shape_;
  DataType data_type_;
  size_t proto_size_;
  bool is_dead_;

  std::ostream& print(std::ostream& out) const {
    out << "Dtype = " << DataTypeString(data_type_)
        << ", Shape = " << tensor_shape_.DebugString() << ", Proto size = 0x"
        << std::hex << proto_size_ << ", Is dead = " << is_dead_;
    return out;
  }
};





}


#endif  // TENSORFLOW_CONTRIB_VELOX_VELOX_UTIL_H_
