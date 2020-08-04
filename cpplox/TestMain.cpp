#include "cpplox/InterpreterDriver/InterpreterDriver.h"

#include "gtest/gtest.h"

TEST(unimplementedTest, emptyscript) {
  cpplox::InterpreterDriver interpreter;
  EXPECT_EQ(65, interpreter.runScript("../lox-tests/empty_file.lox"));
}