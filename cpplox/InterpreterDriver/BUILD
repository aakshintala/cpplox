load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interpreter-driver",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//cpplox/AST:Expr",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Scanner:scanner",
        "//cpplox/Types:types",
    ],
)
# cc_test(
#     name = "interpreter-driver_test",
#     srcs = "InterpreterDriverTest.cpp",
#     deps = [
#         ":interpreter-driver",
#         "@googletest//:gtest_main",
#     ],
# )
