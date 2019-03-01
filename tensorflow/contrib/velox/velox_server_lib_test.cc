#include "tensorflow/contrib/velox/velox_server_lib.h"

//#include "tensorflow/core/lib/core/status_test_util.h"
#include "tensorflow/core/platform/test.h"

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_testutil.h"

namespace tensorflow {
namespace {

TEST(VeloxServerLibTest, AddParamTensor) {

Tensor a_tensor(DT_FLOAT, TensorShape({2, 2}));
test::FillValues<float>(&a_tensor, {3, 2, -1, 0});
Tensor x_tensor(DT_FLOAT, TensorShape({2, 1}));
test::FillValues<float>(&x_tensor, {1, 1});

VeloxServer vs;
vs.AddParamTensor(a_tensor);
vs.AddParamTensor(x_tensor);
Tensor a_from_vs = vs.GetParamTensorByIdx(0);
Tensor x_from_vs = vs.GetParamTensorByIdx(1);
test::ExpectTensorEqual<float>(a_tensor, a_from_vs);
test::ExpectTensorEqual<float>(x_tensor, x_from_vs);
vs.Init();
}

}  // namespace
}  // namespace tensorflow
