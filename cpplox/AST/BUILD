load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "expressions",
    srcs = ["Expr.cpp"],
    hdrs = ["Expr.h"],
    deps = ["//cpplox/Types:types"],
)

cc_test(
    name = "expressions_test",
    srcs = ["ExprTest.cpp"],
    deps = [
        ":expressions",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "pretty-printer",
    srcs = ["PrettyPrinter.cpp"],
    hdrs = ["PrettyPrinter.h"],
    deps = [
        ":expressions",
        "//cpplox/Types:types",
    ],
)

cc_test(
    name = "pretty-printer_test",
    srcs = ["PrettyPrinterTest.cpp"],
    deps = [
        ":expressions",
        ":pretty-printer",
        "@googletest//:gtest_main",
    ],
)
