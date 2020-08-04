#include "cpplox/AST/Expr.h"

#include <utility>

namespace cpplox::AST {

namespace {
auto getLiteralString(Literal &value) -> std::string {
  // Literal = std::variant<std::string, DoubleLiteral>;
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
      break;
    case 1:  // DoubleLiteral
      return std::get<1>(value).stringLiteral;
      break;
    default:
      static_assert(
          std::variant_size_v<Literal> == 2,
          "Looks like you forgot to update the cases in getLiteralString()!");
  }
  return "";
}
}  // namespace

BinaryExpr::BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

GroupingExpr::GroupingExpr(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

LiteralExpr::LiteralExpr(Literal value) : value(std::move(value)) {}

auto LiteralExpr::printLiteral() -> std::string {
  return getLiteralString(this->value);
}

UnaryExpr::UnaryExpr(Token op, ExprPtrVariant right)
    : op(std::move(op)), right(std::move(right)) {}

}  // namespace cpplox::AST