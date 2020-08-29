load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "evaluator",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    copts = [
        # "-DENVIRON_DEBUG",
        # "-DEVAL_DEBUG",
    ],
    deps = [
        "//cpplox/AST:ASTNodes",
        "//cpplox/AST:pretty-printer",
        "//cpplox/ErrorsAndDebug:debug-print",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/ErrorsAndDebug:runtime-error",
        "//cpplox/Types:types",
    ],
)
