#include "gtest/gtest.h"

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <string>

TEST(PrettyPrinterTest, simple_expression) {
  using namespace cpplox::AST;
  using namespace cpplox::Types;

  // clang-format off
  ExprPtrVariant expr =
    createBinaryEPV(
      createUnaryEPV(
        Token(TokenType::MINUS, "-"),
        createLiteralEPV(makeOptionalDoubleLiteral(123, "123"))
      ),
      Token(TokenType::STAR, "*"),
      createGroupingEPV(
        createLiteralEPV(makeOptionalDoubleLiteral(45.67, "45.67"))
      )
    );
  // clang-format on
  PrettyPrinter printer(expr);
  EXPECT_EQ("(* (- 123) (group 45.67))", printer.toString());

  PrettyPrinterRPN printerRPN(expr);
  EXPECT_EQ("123 ~ 45.67 *", printerRPN.toString());
}