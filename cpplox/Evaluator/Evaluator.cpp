#include "cpplox/Evaluator/Evaluator.h"

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "cpplox/ErrorsAndDebug/RuntimeError.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"

namespace cpplox::Evaluator {

// throws RuntimeError if right isn't a double
auto Evaluator::getDouble(const Token& token, Value right) -> double {
  if (!std::holds_alternative<double>(right))
    throw reportRuntimeError(
        eReporter, token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + Types::getValueString(right));
  return std::get<double>(right);
}

//===============================//
// Expression Evaluation Methods //
//===============================//
auto Evaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value {
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

auto Evaluator::evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value {
  return evaluate(expr->expression);
}

namespace {
auto getValuefromStringLiteral(const Literal& strLiteral) -> Value {
  const auto& str = std::get<std::string>(strLiteral);
  if (str == "true") return Value(true);
  if (str == "false") return Value(false);
  if (str == "nil") return Value(nullptr);
  return Value(str);
};
}  // namespace

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

auto Evaluator::evaluateConditionalExpr(const ConditionalExprPtr& expr)
    -> Value {
  Value condition = evaluate(expr->condition);
  if (Types::isTrue(condition)) return evaluate(expr->thenBranch);
  return evaluate(expr->elseBranch);
}

auto Evaluator::evaluateVariableExpr(const VariableExprPtr& expr) -> Value {
  return environManager.get(expr->varName);
}

auto Evaluator::evaluateAssignmentExpr(const AssignmentExprPtr& expr) -> Value {
  Value rightVal = evaluate(expr->right);
  environManager.assign(expr->varName, rightVal);
  return rightVal;
}

namespace {
auto match(const Token& token, Types::TokenType tType) -> bool {
  return token.getType() == tType;
}

auto doPostfixOp(const Token& op, const Value& val) -> Value {
  if (std::holds_alternative<double>(val)) {
    double dVal = std::get<double>(val);
    if (match(op, TokenType::PLUS_PLUS)) return Value(++dVal);
    if (match(op, TokenType::MINUS_MINUS)) return Value(--dVal);
  }
  throw ErrorsAndDebug::RuntimeError();
}
}  // namespace

auto Evaluator::evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value {
  Value leftVal = evaluate(expr->left);
  if (std::holds_alternative<VariableExprPtr>(expr->left)) {
    environManager.assign((std::get<VariableExprPtr>(expr->left))->varName,
                          doPostfixOp(expr->op, leftVal));
  }
  return leftVal;
}

auto Evaluator::evaluate(const ExprPtrVariant& expr) -> Value {
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
    case 6:  // VariableExprPtr
      return evaluateVariableExpr(std::get<6>(expr));
    case 7:  // AssignmentExprPtr
      return evaluateAssignmentExpr(std::get<7>(expr));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 8,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return "";
  }
}

//==============================//
// Statement Evaluation Methods //
//==============================//
void Evaluator::evaluateExprStmtPtr(const ExprStmtPtr& stmt) {
  evaluate(stmt->expression);
}

void Evaluator::evaluatePrintStmtPtr(const PrintStmtPtr& stmt) {
  Value value = evaluate(stmt->expression);
  std::cout << Types::getValueString(value) << std::endl;
}

void Evaluator::evaluateBlockStmtPtr(const BlockStmtPtr& stmt) {
  environManager.createNewEnviron();
  for (auto& statement : stmt->statements) {
    try {
      evaluate(statement);
    } catch (const ErrorsAndDebug::RuntimeError& e) {
      if (++numRunTimeErr > maxRuntimeErr) {
        std::cerr << "Too many errors occurred. Exiting evaluation."
                  << std::endl;
        throw e;
      }
    }
  }
  environManager.discardCurrentEnviron();
}

void Evaluator::evaluateVarStmtPtr(const VarStmtPtr& stmt) {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName, evaluate(stmt->initializer.value()));
  } else {
    environManager.define(stmt->varName, Value(nullptr));
  }
}

void Evaluator::evaluate(const AST::StmtPtrVariant& stmt) {
  switch (stmt.index()) {
    case 0:  // ExprStmtPtr
      evaluateExprStmtPtr(std::get<0>(stmt));
      break;
    case 1:  // PrintStmtPtr
      evaluatePrintStmtPtr(std::get<1>(stmt));
      break;
    case 2:  // BlockStmtPtr
      evaluateBlockStmtPtr(std::get<2>(stmt));
      break;
    case 3:  // VarStmtPtr
      evaluateVarStmtPtr(std::get<3>(stmt));
      break;
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 4,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
  }
}

void Evaluator::evaluate(const std::vector<AST::StmtPtrVariant>& stmts) {
  for (const auto& stmt : stmts) {
    try {
      evaluate(stmt);
    } catch (const ErrorsAndDebug::RuntimeError& e) {
      if (++numRunTimeErr > maxRuntimeErr) {
        std::cerr << "Too many errors occurred. Exiting evaluation."
                  << std::endl;
        throw e;
      }
    }
  }
}

Evaluator::Evaluator(ErrorReporter& eReporter)
    : eReporter(eReporter), environManager(eReporter) {}

}  // namespace cpplox::Evaluator
