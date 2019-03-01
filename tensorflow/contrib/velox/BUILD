package(default_visibility = ["//tensorflow:__subpackages__"])

load(
    "//tensorflow:tensorflow.bzl",
    "tf_cc_test",
    "tf_cc_tests",
    "tf_cc_binary",
    "tf_cuda_library",
)

filegroup(
    name = "c_srcs",
    data = glob([
        "**/*.cc",
        "**/*.h",
    ]),
)

load(
    "//tensorflow/core:platform/default/build_config.bzl",
    "tf_proto_library_cc",
)

tf_proto_library_cc(
    name = "velox_service_proto",
    srcs = ["velox_service.proto"],
    has_services = 1,
    cc_api_version = 2,
    visibility = [
        "//tensorflow:__subpackages__",
    ],
)

cc_library(
    name = "grpc_velox_service",
    srcs = ["grpc_velox_service.cc"],
    hdrs = ["grpc_velox_service.h"],
    deps = [
        ":grpc_velox_service_impl",
        "//tensorflow/contrib/verbs:rdma_mgr",
        ":velox_service_proto_cc",
        "//tensorflow:grpc++",
        "//tensorflow/core:lib_internal",
        "//tensorflow/core/distributed_runtime:session_mgr",
        "//tensorflow/core/distributed_runtime/rpc:async_service_interface",
        "//tensorflow/core/distributed_runtime/rpc:grpc_call",
        "//tensorflow/core/distributed_runtime/rpc:grpc_util",
    ],
    alwayslink = 1,
)

cc_library(
    name = "grpc_velox_service_impl",
    srcs = ["grpc_velox_service_impl.cc"],
    hdrs = ["grpc_velox_service_impl.h"],
    deps = [
        ":velox_service_proto_cc",
        "//tensorflow:grpc++",
    ],
)

cc_library(
    name = "grpc_velox_client",
    srcs = ["grpc_velox_client.cc"],
    hdrs = ["grpc_velox_client.h"],
    deps = [
        ":grpc_velox_service_impl",
        ":velox_service_proto_cc",
        "//tensorflow/core:lib",
        "//tensorflow/core/distributed_runtime:call_options",
        "//tensorflow/core/distributed_runtime/rpc:grpc_util",
    ],
    alwayslink = 1,
)

cc_library(
    name = "velox_server_lib",
    srcs = ["velox_server_lib.cc"],
    hdrs = ["velox_server_lib.h"],
    linkstatic = 1,
    deps = [
        ":grpc_velox_service",
        "//tensorflow/contrib/verbs:rdma_mgr",
        "//tensorflow/contrib/verbs:rdma_rendezvous_mgr",
        "//tensorflow/core/distributed_runtime/rpc:grpc_server_lib",
    ],
    alwayslink = 1,
)

tf_cc_test(
    name = "velox_server_lib_test",
    srcs = ["velox_server_lib_test.cc"],
    deps = [
        ":velox_server_lib",
        "//tensorflow/core:testlib",
        "//tensorflow/core:test_main",
    ],
)

tf_cc_test(
    name = "server_lib_test",
    srcs = ["server_lib_test.cc"],
    deps = [
        ":velox_server_lib",
        "//tensorflow/core:lib",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
    ],
)

tf_cc_binary(
    name = "velox_testlib_server",
    testonly = 1,
    srcs = [
        "velox_testlib_server.cc",
    ],
    deps = [
        ":velox_server_lib",
        "//tensorflow:grpc++",
        "//tensorflow/core/distributed_runtime/rpc:grpc_server_lib",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:testlib",
        "//tensorflow/core/distributed_runtime:server_lib",
        "//tensorflow/core/kernels:constant_op",
        "//tensorflow/core/kernels:cwise_op",
        "//tensorflow/core/kernels:dense_update_ops",
        "//tensorflow/core/kernels:identity_op",
        "//tensorflow/core/kernels:matmul_op",
        "//tensorflow/core/kernels:reduction_ops",
        "//tensorflow/core/kernels:variable_ops",
    ],
)


#filegroup(
#    name = "c_srcs",
#    data = glob([
#        "**/*.cc",
#        "**/*.h",
#    ]),
#)
#load(
#    "//tensorflow/core:platform/default/build_config.bzl",
#    "tf_kernel_tests_linkstatic",
#)
#cc_library(
#    name = "p2ptf_server_lib",
#    srcs = ["core/p2ptf_server_lib.cc"],
#    hdrs = ["core/p2ptf_server_lib.h"],
#    linkstatic = 1,  # Seems to be needed since alwayslink is broken in bazel
#    deps = [
#        "//tensorflow/contrib/verbs:grpc_verbs_service",
#        "//tensorflow/contrib/verbs:rdma_mgr",
#        "//tensorflow/contrib/verbs:rdma_rendezvous_mgr",
#        "//tensorflow/core:lib",
#        "//tensorflow/core/distributed_runtime:server_lib",
#        "//tensorflow/core/distributed_runtime/rpc:grpc_server_lib",
#    ],
#    alwayslink = 1,
#)
#
#tf_cc_binary(
#    name = "p2ptf_testlib_server",
#    testonly = 1,
#    srcs = [
#        "p2ptf_testlib_server.cc",
#    ],
#    deps = [
#        "//tensorflow/contrib/verbs:verbs_server_lib",
#        "//tensorflow:grpc++",
#        "//tensorflow/core:core_cpu",
#        "//tensorflow/core:framework_internal",
#        "//tensorflow/core:lib",
#        "//tensorflow/core:protos_all_cc",
#        "//tensorflow/core:testlib",
#        "//tensorflow/core/distributed_runtime:server_lib",
#        "//tensorflow/core/kernels:constant_op",
#        "//tensorflow/core/kernels:cwise_op",
#        "//tensorflow/core/kernels:dense_update_ops",
#        "//tensorflow/core/kernels:identity_op",
#        "//tensorflow/core/kernels:matmul_op",
#        "//tensorflow/core/kernels:reduction_ops",
#        "//tensorflow/core/kernels:variable_ops",
#    ],
#)
#
#tf_cuda_library(
#    name = "p2ptf_testlib",
#    testonly = 1,
#    srcs = ["p2ptf_testlib.cc"],
#    hdrs = ["p2ptf_testlib.h"],
#    data = [
#        ":p2ptf_testlib_server",
#    ],
#    visibility = ["//tensorflow:__subpackages__"],
#    deps = [
#        ":grpc_session",
#        "//tensorflow/core:core_cpu",
#        "//tensorflow/core:framework",
#        "//tensorflow/core:lib",
#        "//tensorflow/core:protos_all_cc",
#        "//tensorflow/core:test",
#        "//tensorflow/core:testlib",
#    ],
#    alwayslink = 1,
#)
#
#tf_cc_test(
#    name = "verb_server_lib_test",
#    srcs = ["core/verb_server_lib_test.cc"],
#    deps = [
#        "//tensorflow/contrib/verbs:verbs_server_lib",
#        "//tensorflow/core/distributed_runtime:server_lib",
#        "//tensorflow/core:lib",
#        "//tensorflow/core:test",
#        "//tensorflow/core:test_main",
#    ],
#)
#
#tf_cc_test(
#    name = "rdma_test",
#    size = "small",
#    srcs = ["core/rendez_test.cc"],
#    #linkstatic = tf_kernel_tests_linkstatic(),
#    deps = [
#        "//tensorflow/c:c_api",
#        #"//tensorflow/c:c_api_internal",
#        #"//tensorflow/c:env",
#        #"//tensorflow/core:framework",
#        #"//tensorflow/core:framework_internal",
#        "//tensorflow/core:lib",
#        #"//tensorflow/core:lib_internal",
#        "//tensorflow/core:proto_text",
#        #"//tensorflow/core/kernels:ops_testutil",
#        #"//tensorflow/core:protos_all_cc",
#        "//tensorflow/core:test",
#        "//tensorflow/core:test_main",
#        #"//tensorflow/core:testlib",
#        #"//tensorflow/core/debug",
#        #"//tensorflow/cc:cc_ops",
#        #"//tensorflow/cc:scope",
#    ],
#)
