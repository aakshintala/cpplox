cc_library(
  name = "error-debug",
  srcs = glob(["src/ErrorsAndDebug/*.cpp", "src/ErrorsAndDebug/*.h"]),
  hdrs = glob(["src/ErrorsAndDebug/*.h"]),

  visibility = ["//visibility:private"],
)

cc_library(
  name = "types",
  srcs = glob(["src/Types/*.cpp", "src/Types/*.h"]),
  hdrs = glob(["src/Types/*.h"]),

  visibility = ["//visibility:private"],
)


cc_library(
	name = "cpplox-twi",
	includes = ["include/"] + glob(["src/**"]),
	hdrs = glob(["include/*.h"]),
	srcs = glob(["src/*.cpp", "src/*.h"]),
	visibility = ["//visibility:public"],
	deps = [":types", ":error-debug"]
)

cc_binary(
	name = "cpplox",
	srcs = ["main.cpp"],
	includes = ["include/"],
	deps = [":cpplox-twi"],
)

# Break tests down into a tester per library
cc_test(
	name = "cpplox-utest",
	srcs = glob(["unittest/**/*.cpp"]),
	deps = [":cpplox-twi",
			"@googletest//:gtest_main"],
)
