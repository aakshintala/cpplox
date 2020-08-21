#include "gtest/gtest.h"

#include "cpplox/AST/NodeTypes.h"

TEST(LiteralExprPrintTest, emptyscript) {
  cpplox::Types::Literal literal = {"Testing LiteralExpr Printing"};
  cpplox::AST::LiteralExpr literalExpr(literal);
  EXPECT_EQ("Testing LiteralExpr Printing",
            cpplox::Types::getLiteralString(literalExpr.literalVal.value()));
}