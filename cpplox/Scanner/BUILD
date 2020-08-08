load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "scanner",
    srcs = ["Scanner.cpp"],
    hdrs = ["Scanner.h"],
    deps = [
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Types:types",
    ],
)

cc_test(
    name = "scanner_test",
    size = "small",
    srcs = ["ScannerTest.cpp"],
    deps = [
        ":scanner",
        "@googletest//:gtest_main",
    ],
)
