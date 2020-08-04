load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "Expr",
    srcs = ["Expr.cpp"],
    hdrs = ["Expr.h"],
    deps = ["//cpplox/Types:types"],
)

# cc_test(
#     name = "AST_test",
#     srcs = ["ExprTest.cpp"],
#     deps = [
#         ":Expr",
#         "@googletest//:gtest_main",
#     ],
# )
