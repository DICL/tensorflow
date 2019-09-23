package(default_visibility = ["//visibility:public"])

load("//tensorflow:tensorflow.bzl", "tf_cc_binary")
load("//tensorflow:tensorflow.bzl", "tf_cuda_library")
load(
    "//tensorflow/core:platform/default/build_config.bzl",
    "tf_proto_library_cc",
)

#tf_cc_binary(
#    name = "tensor_test",
#    srcs = ["tensor_test.cc"],
#    #hdrs = ["framework/tensor.h"],
#    deps = [
#        "//tensorflow/core:framework",
#    ],
#)

tf_proto_library_cc(
    name = "ptre_service_proto",
    srcs = ["ptre_service.proto"],
    has_services = 1,
    cc_api_version = 2,
)

cc_library(
    name = "grpc_ptre_server",
    srcs = ["grpc_ptre_server.cc"],
    hdrs = [
      "grpc_ptre_server.h",
		  "ptre_util.h",
    ],
    deps = [
        ":rdma_mgr",
        ":ptre_service_proto_cc",
        ":grpc_ptre_service",
        "//tensorflow:grpc++",
        "//tensorflow/core:lib_internal",
        #"//tensorflow/core/distributed_runtime:session_mgr",
        #"//tensorflow/core/distributed_runtime/rpc:async_service_interface",
        "//tensorflow/core/distributed_runtime/rpc:grpc_call",
        "//tensorflow/core/distributed_runtime/rpc:grpc_util",
    ],
)


#cc_library(
#    name = "grpc_ptre_service",
#    srcs = ["grpc_ptre_service.cc"],
#    hdrs = ["grpc_ptre_service.h"],
#    deps = [
#        ":rdma_mgr",
#        ":ptre_service_proto_cc",
#        "//tensorflow:grpc++",
#        "//tensorflow/core:lib_internal",
#        #"//tensorflow/core/distributed_runtime:session_mgr",
#        #"//tensorflow/core/distributed_runtime/rpc:async_service_interface",
#        #"//tensorflow/core/distributed_runtime/rpc:grpc_call",
#        #"//tensorflow/core/distributed_runtime/rpc:grpc_util",
#    ],
#)

cc_library(
    name = "grpc_ptre_service",
    srcs = ["grpc_ptre_service.cc"],
    hdrs = ["grpc_ptre_service.h"],
    linkstatic = 1,
    deps = [
        ":ptre_service_proto_cc",
        "//tensorflow:grpc++",
    ],
)

cc_library(
    name = "ptre_server_lib",
    srcs = ["ptre_server_lib.cc"],
    hdrs = ["ptre_server_lib.h"],
    linkstatic = 1,
    deps = [
				":rdma_mgr",
        ":grpc_ptre_server",
        ":grpc_ptre_service",
        ":ptre_service_proto_cc",
        #"//tensorflow/core:lib",
        #"//tensorflow/core/distributed_runtime:server_lib",
        #"//tensorflow/core/distributed_runtime/rpc:grpc_server_lib",
    ],
)

tf_cuda_library(
    name = "rdma_mgr",
    srcs = ["rdma_mgr.cc"],
    hdrs = [
			"rdma_mgr.h",
			"ptre_util.h",
		],
    deps = [
        ":ptre_service_proto_cc",
        ":rdma",
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
				"//tensorflow/core:stream_executor",
        "//tensorflow/core:gpu_runtime",
    ],
)

tf_cuda_library(
    name = "rdma",
    srcs = ["rdma.cc"],
    hdrs = ["rdma.h"],
    linkopts = ["-libverbs"],
    deps = [
        "//tensorflow/core:core_cpu_internal",
        "//tensorflow/core:framework",
        "//tensorflow/core:framework_internal",
        "//tensorflow/core:gpu_runtime",
        "//tensorflow/core:lib",
        "//tensorflow/core:lib_internal",
    ],
)
