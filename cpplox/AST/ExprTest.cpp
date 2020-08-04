#include "cpplox/AST/Expr.h"

#include "gtest/gtest"

TEST(LiteralExprPrintTest, emptyscript) {
  cpplox::Types::Literal literal = {"Testing LiteralExpr Printing"};
  cpplox::AST::LiteralExpr literalExpr(literal);
  EXPECT_EQ("Testing LiteralExpr Printing", literalExpr.printLiteral()));
}