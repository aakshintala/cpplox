load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ASTNodes",
    srcs = ["NodeTypes.cpp"],
    hdrs = ["NodeTypes.h"],
    deps = ["//cpplox/Types:types"],
)

cc_test(
    name = "ASTNodes_test",
    size = "small",
    srcs = ["NodeTypesTest.cpp"],
    deps = [
        ":ASTNodes",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "pretty-printer",
    srcs = ["PrettyPrinter.cpp"],
    hdrs = ["PrettyPrinter.h"],
    deps = [
        ":ASTNodes",
        "//cpplox/Types:types",
    ],
)

cc_test(
    name = "pretty-printer_test",
    size = "small",
    srcs = ["PrettyPrinterTest.cpp"],
    deps = [
        ":ASTNodes",
        ":pretty-printer",
        "@googletest//:gtest_main",
    ],
)
