cc_library(
	name = "cpplox",
	includes = ["include/"],
	hdrs = glob(["include/*.h"]),
	srcs = glob(["src/**/*.cpp", "src/**/*.h"]),
	copts = ["-std=c++17"],
	visibility = ["//visibility:public"],
)

cc_binary(
	name = "lox",
	srcs = ["main.cpp"],
	includes = ["include/"],
	copts = ["-std=c++17"],
	deps = [":cpplox"],
)

cc_test(
	name = "cpplox-utest",
	srcs = glob(["unittest/**/*.cpp"]),
	copts = ["-std=c++17"],
	deps = [":cpplox",
			"@googletest//:gtest_main"],
)
