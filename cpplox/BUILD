load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "error-debug",
    srcs = glob(["src/ErrorsAndDebug/*.cpp"]),
    hdrs = glob(["src/ErrorsAndDebug/*.h"]),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "types",
    srcs = glob(["src/Types/*.cpp"]),
    hdrs = glob(["src/Types/*.h"]),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "cpplox-twi",
    srcs = glob([
        "src/*.cpp",
        "src/*.h",
    ]),
    hdrs = glob(["include/*.h"]),
    includes = ["include/"] + glob(["src/**"]),
    visibility = ["//visibility:public"],
    deps = [
        ":error-debug",
        ":types",
    ],
)

cc_binary(
    name = "cpplox",
    srcs = ["main.cpp"],
    includes = ["include/"],
    deps = [":cpplox-twi"],
)

# TODO: #2 Break tests down into a tester per library
cc_test(
    name = "cpplox-utest",
    srcs = glob(["unittest/**/*.cpp"]),
    deps = [
        ":cpplox-twi",
        "@googletest//:gtest_main",
    ],
)
