load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interpreter-driver",
    srcs = ["InterpreterDriver.cpp"],
    hdrs = ["InterpreterDriver.h"],
    copts = [
        # "-DPARSER_DEBUG",
        # "-DSCANNER_DEBUG",
        #"-DPERF_DEBUG",
    ],
    deps = [
        "//cpplox/AST:ASTNodes",
        "//cpplox/AST:pretty-printer",
        "//cpplox/ErrorsAndDebug:debug-print",
        "//cpplox/ErrorsAndDebug:error-reporter",
        "//cpplox/Evaluator:evaluator",
        "//cpplox/Parser:parser",
        "//cpplox/Scanner:scanner",
        "//cpplox/Types:types",
    ],
)

cc_test(
    name = "interpreter-driver_test",
    size = "small",
    srcs = ["InterpreterDriverTest.cpp"],
    data = [
        "//sample-lox-programs:empty_file.lox",
        "//sample-lox-programs:expressions/evaluate.lox",
        "//sample-lox-programs:unexpected_character.lox",
    ],
    deps = [
        ":interpreter-driver",
        "@googletest//:gtest_main",
    ],
)
