#include "gtest/gtest.h"

#include "TreewalkInterpreter.h"

TEST(unimplemented_test, emptyscript) {
  cpplox::TreewalkInterpreter interpreter;
  EXPECT_EQ(70, interpreter.runScript("./empty.lox"));
}
