load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "evaluator",
    srcs = [
        "Environment.cpp",
        "ExprEvaluator.cpp",
        "StmtExprEvaluator.cpp",
    ],
    hdrs = [
        "Environment.h",
        "ExprEvaluator.h",
        "StmtExprEvaluator.h",
    ],
    deps = [
        "//cpplox/AST:ASTNodes",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/ErrorsAndDebug:runtime-error",
        "//cpplox/Types:types",
    ],
)
