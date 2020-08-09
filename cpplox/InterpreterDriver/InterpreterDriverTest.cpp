#include "gtest/gtest.h"

#include <exception>

#include "cpplox/InterpreterDriver/InterpreterDriver.h"

TEST(DriverTest, emptyscript) {
  cpplox::InterpreterDriver interpreter;
  EXPECT_EQ(65, interpreter.runScript("sample-lox-programs/empty_file.lox"));
}

TEST(DriverTest, unexpectedCharFile) {
  cpplox::InterpreterDriver interpreter;
  EXPECT_EQ(65, interpreter.runScript(
                    "sample-lox-programs/unexpected_character.lox"));
}

TEST(DriverFileTest, correctInputFile) {
  cpplox::InterpreterDriver interpreter;
  EXPECT_EQ(
      0, interpreter.runScript("sample-lox-programs/expressions/evaluate.lox"));
}