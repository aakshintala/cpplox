load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "parser",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//cpplox/AST:ASTNodes",
        "//cpplox/ErrorsAndDebug:debug-print",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Types:types",
    ],
)
