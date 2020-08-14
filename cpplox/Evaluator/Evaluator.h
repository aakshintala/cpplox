#ifndef CPPLOX_EVALUATOR_EVALUATOR__H
#define CPPLOX_EVALUATOR_EVALUATOR__H
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/Stmt.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Evaluator/Environment.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"
#include "cpplox/Types/Value.h"

namespace cpplox::Evaluator {
using AST::AssignmentExprPtr;
using AST::BinaryExprPtr;
using AST::ConditionalExprPtr;
using AST::ExprPtrVariant;
using AST::GroupingExprPtr;
using AST::LiteralExprPtr;
using AST::PostfixExprPtr;
using AST::UnaryExprPtr;
using AST::VariableExprPtr;

using AST::BlockStmtPtr;
using AST::ExprStmtPtr;
using AST::PrintStmtPtr;
using AST::StmtPtrVariant;
using AST::VarStmtPtr;

using ErrorsAndDebug::ErrorReporter;

using Types::Literal;
using Types::Token;
using Types::TokenType;
using Types::Value;

using ErrorsAndDebug::ErrorReporter;

class Evaluator {
 public:
  explicit Evaluator(ErrorReporter& eReporter);
  void evaluate(const AST::StmtPtrVariant& stmt);
  auto evaluate(const ExprPtrVariant& expr) -> Value;
  void evaluate(const std::vector<AST::StmtPtrVariant>& stmts);

 private:
  // evaluation functions for Expr types
  auto evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value;
  auto evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value;
  static auto evaluateLiteralExpr(const LiteralExprPtr& expr) -> Value;
  auto evaluateUnaryExpr(const UnaryExprPtr& expr) -> Value;
  auto evaluateConditionalExpr(const ConditionalExprPtr& expr) -> Value;
  auto evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value;
  auto evaluateVariableExpr(const VariableExprPtr& expr) -> Value;
  auto evaluateAssignmentExpr(const AssignmentExprPtr& expr) -> Value;

  // evaluation functions for Stmt types
  void evaluateExprStmtPtr(const ExprStmtPtr& stmt);
  void evaluatePrintStmtPtr(const PrintStmtPtr& stmt);
  void evaluateBlockStmtPtr(const BlockStmtPtr& stmt);
  void evaluateVarStmtPtr(const VarStmtPtr& stmt);

  // Helper functions
  // throws RuntimeError if right isn't a double
  auto getDouble(const Token& token, Value right) -> double;

  ErrorReporter& eReporter;
  EnvironmentManager environManager;

  const int maxRuntimeErr = 20;
  int numRunTimeErr = 0;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_EVALUATOR__H