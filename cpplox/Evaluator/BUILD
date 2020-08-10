load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "evaluator",
    srcs = ["Evaluator.cpp"],
    hdrs = ["Evaluator.h"],
    deps = [
        "//cpplox/AST:expressions",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Types:types",
    ],
)
