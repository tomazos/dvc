package(default_visibility = ["//visibility:public"])

cc_library(
    name = "log",
    hdrs = [
        "log.h",
    ],
    deps = [
        ":string",
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

cc_library(
    name = "time",
    hdrs = [
        "time.h",
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
