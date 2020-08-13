#include "cpplox/Evaluator/StmtExprEvaluator.h"

#include <iostream>
#include <memory>
#include <variant>

#include "cpplox/ErrorsAndDebug/RuntimeError.h"
#include "cpplox/Types/Token.h"

namespace cpplox::Evaluator {

auto StmtExprEvaluator::evaluateVariableExpr(const VariableExprPtr& expr)
    -> Value {
  return environManager.get(expr->varName);
}

auto StmtExprEvaluator::evaluateAssignmentExpr(const AssignmentExprPtr& expr)
    -> Value {
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

auto StmtExprEvaluator::evaluatePostfixExpr(const PostfixExprPtr& expr)
    -> Value {
  Value leftVal = evaluate(expr->left);
  if (std::holds_alternative<VariableExprPtr>(expr->left)) {
    environManager.assign((std::get<VariableExprPtr>(expr->left))->varName,
                          doPostfixOp(expr->op, leftVal));
  }
  return leftVal;
}

auto StmtExprEvaluator::evaluate(const ExprPtrVariant& expr) -> Value {
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

void StmtExprEvaluator::evaluateExprStmtPtr(const ExprStmtPtr& stmt) {
  evaluate(stmt->expression);
}

void StmtExprEvaluator::evaluatePrintStmtPtr(const PrintStmtPtr& stmt) {
  Value value = evaluate(stmt->expression);
  std::cout << Types::getValueString(value) << std::endl;
}

void StmtExprEvaluator::evaluateBlockStmtPtr(const BlockStmtPtr& stmt) {
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

void StmtExprEvaluator::evaluateVarStmtPtr(const VarStmtPtr& stmt) {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName, evaluate(stmt->initializer.value()));
  } else {
    environManager.define(stmt->varName, Value(nullptr));
  }
}

void StmtExprEvaluator::evaluate(const AST::StmtPtrVariant& stmt) {
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

void StmtExprEvaluator::evaluate() {
  for (auto& stmt : statements) {
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

StmtExprEvaluator::StmtExprEvaluator(std::vector<AST::StmtPtrVariant> stmts,
                                     ErrorReporter& eReporter)
    : ExprEvaluator(eReporter),
      environManager(eReporter),
      statements(std::move(stmts)) {}

}  // namespace cpplox::Evaluator