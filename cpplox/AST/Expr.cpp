#include "cpplox/AST/Expr.h"

#include <initializer_list>
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

ConditionalExpr::ConditionalExpr(ExprPtrVariant condition,
                                 ExprPtrVariant thenBranch,
                                 ExprPtrVariant elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

PostfixExpr::PostfixExpr(ExprPtrVariant left, Token op)
    : left(std::move(left)), op(std::move(op)) {}

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

auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant {
  return std::make_shared<ConditionalExpr>(condition, then, elseBranch);
}

auto createPostfixEPV(ExprPtrVariant left, Token op) -> ExprPtrVariant {
  return std::make_shared<PostfixExpr>(left, op);
}

}  // namespace cpplox::AST