load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "scanner",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Types:types",
    ],
)
# cc_test(
#     name = "scanner_test",
#     srcs = "ScannerTest.cpp",
#     deps = [
#         ":scanner",
#         "@googletest//:gtest_main",
#     ],
# )
