#include "cpplox/AST/Expr.h"

#include <utility>

namespace cpplox::AST {

BinaryExpr::BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

GroupingExpr::GroupingExpr(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

LiteralExpr::LiteralExpr(OptionalLiteral value)
    : literalVal(std::move(value)) {}

UnaryExpr::UnaryExpr(Token op, ExprPtrVariant right)
    : op(std::move(op)), right(std::move(right)) {}

auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_shared<BinaryExpr>(left, op, right);
}

auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_shared<UnaryExpr>(op, right);
}

auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_shared<GroupingExpr>(right);
}

auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant {
  return std::make_shared<LiteralExpr>(literal);
}

}  // namespace cpplox::AST