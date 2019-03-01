//#include "tensorflow/core/distributed_runtime/server_lib.h"
#include "tensorflow/contrib/velox/velox_server_lib.h"
#include "tensorflow/core/lib/core/status_test_util.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/platform/test.h"

namespace tensorflow {

class TestServerFactory : public ServerFactory {
 public:
  bool AcceptsOptions(const ServerDef& server_def) override {
    return server_def.protocol() == "test_protocol";
  }

  Status NewServer(const ServerDef& server_def,
                   std::unique_ptr<ServerInterface>* out_server) override {
    return Status::OK();
  }
};

TEST(ServerLibTest, NewServerFactoryAccepts) {
  ServerFactory::Register("TEST_SERVER", new TestServerFactory());
  ServerDef server_def;
  server_def.set_protocol("test_protocol");
  std::unique_ptr<ServerInterface> server;
  TF_EXPECT_OK(NewServer(server_def, &server));
}

TEST(ServerLibTest, NewServerNoFactoriesAccept) {
  ServerDef server_def;
  server_def.set_protocol("fake_protocol");
  std::unique_ptr<ServerInterface> server;
  Status s = NewServer(server_def, &server);
  ASSERT_NE(s, Status::OK());
  EXPECT_TRUE(str_util::StrContains(
      s.error_message(),
      "No server factory registered for the given ServerDef"));
  EXPECT_TRUE(str_util::StrContains(s.error_message(),
                                    "The available server factories are: ["));
}

}  // namespace tensorflow
