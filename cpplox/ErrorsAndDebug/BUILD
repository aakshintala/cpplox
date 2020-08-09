load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "error-reporter",
    srcs = ["ErrorReporter.cpp"],
    hdrs = ["ErrorReporter.h"],
)

cc_library(
    name = "debug-print",
    srcs = ["DebugPrint.cpp"],
    hdrs = ["DebugPrint.h"],
)

# cc_test(
#     name = "error-reporter_test",
#     srcs = "ErrorReporterTest.cpp",
#     deps = [
#         ":error-reporter",
#         "@googletest//:gtest_main",
#     ],
# )
