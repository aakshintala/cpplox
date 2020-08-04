load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

cc_binary(
    name = "cpplox",
    srcs = ["main.cpp"],
    deps = ["//cpplox/InterpreterDriver:interpreter-driver"],
)

# TODO: #2 Break tests down into a tester per library
cc_test(
    name = "cpplox-utest",
    srcs = ["TestMain.cpp"],
    deps = [
        "//cpplox/InterpreterDriver:interpreter-driver",
        "@googletest//:gtest_main",
    ],
)
