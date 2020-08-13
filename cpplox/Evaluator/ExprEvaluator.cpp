#include "cpplox/Evaluator/ExprEvaluator.h"

#include <cstddef>
#include <string>
#include <variant>

#include "cpplox/ErrorsAndDebug/RuntimeError.h"
#include "cpplox/Types/Literal.h"

namespace cpplox::Evaluator {

namespace {  // helper functions
auto getValuefromStringLiteral(const Literal& strLiteral) -> Value {
  const auto& str = std::get<std::string>(strLiteral);
  if (str == "true") return Value(true);
  if (str == "false") return Value(false);
  if (str == "nil") return Value(nullptr);
  return Value(str);
};
}  // namespace

// throws RuntimeError if right isn't a double
auto ExprEvaluator::getDouble(const Token& token, Value right) -> double {
  if (!std::holds_alternative<double>(right))
    throw reportRuntimeError(
        eReporter, token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + Types::getValueString(right));
  return std::get<double>(right);
}

// Evaluation functions.

auto ExprEvaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value {
  Value left = evaluate(expr->left);
  Value right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::COMMA: return right;
    case TokenType::BANG_EQUAL: return !Types::areEqual(left, right);
    case TokenType::EQUAL_EQUAL: return Types::areEqual(left, right);
    case TokenType::MINUS:
      return getDouble(expr->op, left) - getDouble(expr->op, right);
    case TokenType::SLASH: {
      double denominator = getDouble(expr->op, right);
      if (denominator == 0.0)
        throw reportRuntimeError(eReporter, expr->op,
                                 "Division by zero is illegal");
      return getDouble(expr->op, left) / denominator;
    }
    case TokenType::STAR:
      return getDouble(expr->op, left) * getDouble(expr->op, right);
    case TokenType::LESS:
      return getDouble(expr->op, left) < getDouble(expr->op, right);
    case TokenType::LESS_EQUAL:
      return getDouble(expr->op, left) <= getDouble(expr->op, right);
    case TokenType::GREATER:
      return getDouble(expr->op, left) > getDouble(expr->op, right);
    case TokenType::GREATER_EQUAL:
      return getDouble(expr->op, left) >= getDouble(expr->op, right);
    case TokenType::PLUS: {
      if (std::holds_alternative<double>(left)
          && std::holds_alternative<double>(right)) {
        return std::get<double>(left) + std::get<double>(right);
      }
      if (std::holds_alternative<std::string>(left)
          || std::holds_alternative<std::string>(right)) {
        return Types::getValueString(left) + Types::getValueString(right);
      }
      throw reportRuntimeError(
          eReporter, expr->op,
          "Operands to 'plus' must be numbers or strings; This is invalid: "
              + Types::getValueString(left) + " + "
              + Types::getValueString(right));
    }
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Attempted to apply invalid operator to binary expr: "
              + expr->op.getTypeString());
  }
}

auto ExprEvaluator::evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value {
  return evaluate(expr->expression);
}

auto ExprEvaluator::evaluateLiteralExpr(const LiteralExprPtr& expr) -> Value {
  return expr->literalVal.has_value()
             ? std::holds_alternative<std::string>(expr->literalVal.value())
                   ? getValuefromStringLiteral(expr->literalVal.value())
                   : Value(std::get<double>(expr->literalVal.value()))
             : Value(std::nullptr_t());
}

auto ExprEvaluator::evaluateUnaryExpr(const UnaryExprPtr& expr) -> Value {
  Value right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::BANG: return !Types::isTrue(right);
    case TokenType::MINUS: return -getDouble(expr->op, right);
    case TokenType::PLUS_PLUS: return getDouble(expr->op, right) + 1;
    case TokenType::MINUS_MINUS: return getDouble(expr->op, right) - 1;
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Illegal unary expression: " + expr->op.getLexeme()
              + Types::getValueString(right));
  }
}

auto ExprEvaluator::evaluateConditionalExpr(const ConditionalExprPtr& expr)
    -> Value {
  Value condition = evaluate(expr->condition);
  if (Types::isTrue(condition)) return evaluate(expr->thenBranch);
  return evaluate(expr->elseBranch);
}

// !The postfix operator evaluation does nothing. This evaluator parses a single
// !expression at a time, and doesn't support variable definition or lookup.
// !A side-effect based operator like postfix is meaningless under these
// !circumstances;
auto ExprEvaluator::evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value {
  return evaluate(expr->left);
}

auto ExprEvaluator::evaluate(const ExprPtrVariant& expr) -> Value {
  switch (expr.index()) {
    case 0:  // BinaryExprPtr
      return evaluateBinaryExpr(std::get<0>(expr));
    case 1:  // GroupingExprPtr
      return evaluateGroupingExpr(std::get<1>(expr));
    case 2:  // LiteralExprPtr
      return evaluateLiteralExpr(std::get<2>(expr));
    case 3:  // UnaryExprPtr
      return evaluateUnaryExpr(std::get<3>(expr));
    case 4:  // ConditionalExprPtr
      return evaluateConditionalExpr(std::get<4>(expr));
    case 5:  // PostfixExprPtr
      return evaluatePostfixExpr(std::get<5>(expr));
    // These cannot be supported without an Environment
    // They are implemented in StmtExprEvaluator instead.
    // case 6:  // VariableExprPtr
    //    return evaluateVariableExpr(std::get<6>(expr));
    // case 7:  // AssignmentExprPtr
    //   return evaluateAssignmentExpr(std::get<7>(expr));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 8,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return Value();
  }
}

ExprEvaluator::ExprEvaluator(ErrorReporter& eReporter) : eReporter(eReporter) {}

}  // namespace cpplox::Evaluator