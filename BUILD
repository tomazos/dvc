package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opts",
    srcs = [
        "opts.cc",
    ],
    hdrs = [
        "opts.h",
    ],
    deps = [
        ":log",
        ":parser",
        ":string",
    ],
)

cc_binary(
    name = "opts_test",
    srcs = [
        "opts_test.cc",
    ],
    deps = [
        ":opts",
    ],
)

cc_library(
    name = "log",
    hdrs = [
        "log.h",
    ],
    deps = [
        ":string",
        ":time",
    ],
)

cc_test(
    name = "log_test",
    srcs = [
        "log_test.cc",
    ],
    deps = [
        ":log",
        ":program",
    ],
)

cc_library(
    name = "program",
    srcs = [
        "program.cc",
    ],
    hdrs = [
        "program.h",
    ],
    linkopts = [
        "-lgflags",
        "-ldl",
    ],
    deps = [
        ":log",
        ":opts",
        ":terminate",
    ],
)

cc_library(
    name = "file",
    hdrs = [
        "file.h",
    ],
    linkopts = [
        "-lstdc++fs",
    ],
)

cc_library(
    name = "scanner",
    hdrs = [
        "scanner.h",
    ],
    deps = [
        ":log",
    ],
)

cc_library(
    name = "parser",
    hdrs = [
        "parser.h",
    ],
    deps = [
        ":log",
    ],
)

cc_library(
    name = "json",
    hdrs = [
        "json.h",
    ],
    deps = [
        ":log",
    ],
)

cc_library(
    name = "container",
    hdrs = [
        "container.h",
    ],
)

cc_library(
    name = "string",
    hdrs = [
        "string.h",
    ],
)

cc_test(
    name = "string_test",
    srcs = [
        "string_test.cc",
    ],
    deps = [
        ":log",
        ":string",
    ],
)

cc_library(
    name = "time",
    hdrs = [
        "time.h",
    ],
)

cc_library(
    name = "math",
    hdrs = [
        "math.h",
    ],
)

cc_library(
    name = "terminate",
    srcs = [
        "terminate.cc",
    ],
    hdrs = [
        "terminate.h",
    ],
    linkopts = [
        "-lbacktrace",
    ],
    deps = [
        ":log",
    ],
)

cc_library(
    name = "sha3",
    hdrs = [
        "hex.h",
        "sha3.h",
    ],
    deps = [
        ":log",
    ],
)

cc_test(
    name = "sha3_test",
    srcs = [
        "sha3_test.cc",
    ],
    data = [
        "testdata/ShortMsgKAT_SHA3-224.txt",
        "testdata/ShortMsgKAT_SHA3-256.txt",
        "testdata/ShortMsgKAT_SHA3-384.txt",
        "testdata/ShortMsgKAT_SHA3-512.txt",
        "testdata/ShortMsgKAT_SHAKE128.txt",
        "testdata/ShortMsgKAT_SHAKE256.txt",
    ],
    deps = [
        ":sha3",
        "//dvc:file",
    ],
)

cc_library(
    name = "sampler",
    hdrs = [
        "sampler.h",
    ],
    deps = [
        ":log",
    ],
)

cc_test(
    name = "sampler_test",
    srcs = [
        "sampler_test.cc",
    ],
    deps = [
        ":log",
        ":math",
        ":program",
        ":sampler",
    ],
)
