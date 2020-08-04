load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "types",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
)

# cc_test(
#     name = "types_test",
#     srcs = glob(["tests/*.cpp"]),
#     deps = [
#         ":types",
#         "@googletest//:gtest_main",
#     ],
# )
