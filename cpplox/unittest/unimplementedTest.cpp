#include "gtest/gtest.h"

#include "TreewalkInterpreter.h"

TEST(unimplementedTest, emptyscript) {
  cpplox::TreewalkInterpreter interpreter;
  EXPECT_EQ(65, interpreter.runScript("./empty.lox"));
}