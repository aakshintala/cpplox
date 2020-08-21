#include "gtest/gtest.h"

#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

TEST(PrettyPrinterTest, simple_expression_stmt) {
  using namespace cpplox::AST;
  using namespace cpplox::Types;

  // clang-format off
  std::vector<StmtPtrVariant> stmtVec;
  stmtVec.push_back({
    createExprSPV(
      createBinaryEPV(
        createUnaryEPV(
          Token(TokenType::MINUS, "-"),
          createLiteralEPV(makeOptionalLiteral(123.0))
        ),
        Token(TokenType::STAR, "*"),
        createGroupingEPV(
          createLiteralEPV(makeOptionalLiteral(45.67))
        )
      )
    )
  });
  // clang-format on
  PrettyPrinter printer(stmtVec);
  auto strs = printer.toString();
  EXPECT_EQ(1, strs.size());
  EXPECT_EQ("( (* (- 123) (group 45.67)));", strs[0]);
}