#include "cpplox/Evaluator/Evaluator.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace cpplox::Evaluator {

// helper functions
auto Evaluator::getDoubleElseError(const Token& token, Value right) -> double {
  if (!std::holds_alternative<double>(right))
    throw error(
        token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + Types::getValueString(right));
  return std::get<double>(right);
}

auto Evaluator::isTrue(const Value& value) -> bool {
  if (std::holds_alternative<std::nullptr_t>(value)) return false;
  if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
  return true;  // for all else we go to true
}
auto Evaluator::isEqual(const Value& left, const Value& right) -> bool {
  if (left.index() == right.index()) {
    switch (left.index()) {
      case 0:  // string
        return std::get<std::string>(left) == std::get<std::string>(right);
      case 1:  // double
        return std::get<double>(left) == std::get<double>(right);
      case 2:  // bool
        return std::get<bool>(left) == std::get<bool>(right);
      case 3:  // std::nullptr_t
        // The case where one is null and the other isn't is handled by the
        // outer condition;
        return true;
      default:
        static_assert(std::variant_size_v<Value> == 4,
                      "Looks like you forgot to update the cases in "
                      "Evaluator::isEqual(const Value&, const Value&)!");
    }
  }
  return false;
}

// Evaluation functions.

auto Evaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value {
  Value left = evaluate(expr->left);
  Value right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::COMMA: return right;
    case TokenType::BANG_EQUAL: return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL: return isEqual(left, right);
    case TokenType::MINUS:
      return getDoubleElseError(expr->op, left)
             - getDoubleElseError(expr->op, right);
    case TokenType::SLASH:
      return getDoubleElseError(expr->op, left)
             / getDoubleElseError(expr->op, right);
    case TokenType::STAR:
      return getDoubleElseError(expr->op, left)
             * getDoubleElseError(expr->op, right);
    case TokenType::LESS:
      return getDoubleElseError(expr->op, left)
             < getDoubleElseError(expr->op, right);
    case TokenType::LESS_EQUAL:
      return getDoubleElseError(expr->op, left)
             <= getDoubleElseError(expr->op, right);
    case TokenType::GREATER:
      return getDoubleElseError(expr->op, left)
             > getDoubleElseError(expr->op, right);
    case TokenType::GREATER_EQUAL:
      return getDoubleElseError(expr->op, left)
             >= getDoubleElseError(expr->op, right);
    case TokenType::PLUS: {
      if (std::holds_alternative<double>(left)
          && std::holds_alternative<double>(right)) {
        return std::get<double>(left) + std::get<double>(right);
      }
      if (std::holds_alternative<std::string>(left)
          || std::holds_alternative<std::string>(right)) {
        return Types::getValueString(left) + Types::getValueString(right);
      }
      throw error(
          expr->op,
          "Operands to 'plus' must be numbers or strings; This is invalid: "
              + Types::getValueString(left) + " + "
              + Types::getValueString(right));
    }
    default:
      throw error(expr->op,
                  "Attempted to apply invalid operator to binary expr: "
                      + expr->op.getTypeString());
  }
}

auto Evaluator::evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value {
  return evaluate(expr->expression);
}

auto getValuefromStringLiteral(const Literal& strLiteral) -> Value {
  const auto& str = std::get<std::string>(strLiteral);
  if (str == "true") return Value(true);
  if (str == "false") return Value(false);
  if (str == "nil") return Value(nullptr);
  return Value(str);
};

auto Evaluator::evaluateLiteralExpr(const LiteralExprPtr& expr) -> Value {
  return expr->literalVal.has_value()
             ? std::holds_alternative<std::string>(expr->literalVal.value())
                   ? getValuefromStringLiteral(expr->literalVal.value())
                   : Value(std::get<double>(expr->literalVal.value()))
             : Value(std::nullptr_t());
}

auto Evaluator::evaluateUnaryExpr(const UnaryExprPtr& expr) -> Value {
  Value right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::BANG: return !isTrue(right);
    case TokenType::MINUS: return -getDoubleElseError(expr->op, right);
    case TokenType::PLUS_PLUS: return getDoubleElseError(expr->op, right) + 1;
    case TokenType::MINUS_MINUS: return getDoubleElseError(expr->op, right) - 1;
    default:
      throw error(expr->op, "Illegal unary expression: " + expr->op.getLexeme()
                                + Types::getValueString(right));
  }
}

auto Evaluator::evaluateConditionalExpr(const ConditionalExprPtr& expr)
    -> Value {
  Value condition = evaluate(expr->condition);
  if (isTrue(condition)) return evaluate(expr->thenBranch);
  return evaluate(expr->elseBranch);
}

// !For now the postfix operator does nothing. This is because we currently
// !don't have statements and name lookup. Once that is implemented, this
// needs !to update the value in place and return the previous val
auto Evaluator::evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value {
  return evaluate(expr->left);
}

Evaluator::Evaluator(ExprPtrVariant expression, ErrorReporter& eReporter)
    : expression(std::move(expression)), eReporter(eReporter) {}

auto Evaluator::error(const Token& token, const std::string& message)
    -> RuntimeError {
  eReporter.setError(token.getLine(), message);
  return RuntimeError();
}

auto Evaluator::evaluate() -> Value { return evaluate(expression); }

auto Evaluator::evaluate(const ExprPtrVariant& expr) -> Value {
  // std::variant<BinaryExprPtr, GroupingExprPtr, LiteralExprPtr,
  // UnaryExprPtr,
  //  ConditionalExprPtr, PostfixExprPtr>;
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
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 6,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return "";
  }
}

}  // namespace cpplox::Evaluator