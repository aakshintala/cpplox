#ifndef CPPLOX_EVALUATOR_STMTEXPREVALUATOR__H
#define CPPLOX_EVALUATOR_STMTEXPREVALUATOR__H
#pragma once

#include <memory>
#include <vector>

#include "cpplox/AST/Stmt.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Evaluator/Environment.h"
#include "cpplox/Evaluator/ExprEvaluator.h"

namespace cpplox::Evaluator {
using AST::BlockStmtPtr;
using AST::ExprStmtPtr;
using AST::PrintStmtPtr;
using AST::StmtPtrVariant;
using AST::VarStmtPtr;

using ErrorsAndDebug::ErrorReporter;

class StmtExprEvaluator : ExprEvaluator {
 public:
  explicit StmtExprEvaluator(std::vector<AST::StmtPtrVariant> stmts,
                             ErrorReporter& eReporter);
  void evaluate(const AST::StmtPtrVariant& stmt);
  auto evaluate(const ExprPtrVariant& expr) -> Value;
  void evaluate();

 private:
  // evaluation functions for Expr types not supported in base ExprEvaluator
  // (these operations rely on Environment and can't be implemented in the base
  // Exprvaluator class)
  auto evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value;
  auto evaluateVariableExpr(const VariableExprPtr& expr) -> Value;
  auto evaluateAssignmentExpr(const AssignmentExprPtr& expr) -> Value;

  // evaluation functions for Stmt types
  void evaluateExprStmtPtr(const ExprStmtPtr& stmt);
  void evaluatePrintStmtPtr(const PrintStmtPtr& stmt);
  void evaluateBlockStmtPtr(const BlockStmtPtr& stmt);
  void evaluateVarStmtPtr(const VarStmtPtr& stmt);

  EnvironmentManager environManager;
  std::vector<AST::StmtPtrVariant> statements;

  const int maxRuntimeErr = 20;
  int numRunTimeErr = 0;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_STMTEXPREVALUATOR__H