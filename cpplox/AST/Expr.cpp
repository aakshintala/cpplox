#include "cpplox/AST/Expr.h"

#include <initializer_list>
#include <memory>
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

VariableExpr::VariableExpr(Token varName) : varName(std::move(varName)) {}

AssignmentExpr::AssignmentExpr(Token varName, ExprPtrVariant right)
    : varName(std::move(varName)), right(std::move(right)) {}

LogicalExpr::LogicalExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

// ==============================//
// EPV creation helper functions //
// ==============================//
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<UnaryExpr>(op, std::move(right));
}

auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<GroupingExpr>(std::move(right));
}

auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant {
  return std::make_unique<LiteralExpr>(std::move(literal));
}

auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant {
  return std::make_unique<ConditionalExpr>(
      std::move(condition), std::move(then), std::move(elseBranch));
}

auto createPostfixEPV(ExprPtrVariant left, Token op) -> ExprPtrVariant {
  return std::make_unique<PostfixExpr>(std::move(left), op);
}

auto createVariableEPV(Token varName) -> ExprPtrVariant {
  return std::make_unique<VariableExpr>(varName);
}

auto createAssignmentEPV(Token varName, ExprPtrVariant expr) -> ExprPtrVariant {
  return std::make_unique<AssignmentExpr>(varName, std::move(expr));
}

auto createLogicalEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<LogicalExpr>(std::move(left), op, std::move(right));
}

}  // namespace cpplox::AST