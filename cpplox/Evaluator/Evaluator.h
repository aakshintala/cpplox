#ifndef CPPLOX_EVALUATOR_EVALUATOR__H
#define CPPLOX_EVALUATOR_EVALUATOR__H
#include <exception>
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/Stmt.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Evaluator/Environment.h"
#include "cpplox/Evaluator/Objects.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::Evaluator {
using AST::AssignmentExprPtr;
using AST::BinaryExprPtr;
using AST::CallExprPtr;
using AST::ConditionalExprPtr;
using AST::ExprPtrVariant;
using AST::FuncStmtPtr;
using AST::GroupingExprPtr;
using AST::LiteralExprPtr;
using AST::LogicalExprPtr;
using AST::PostfixExprPtr;
using AST::UnaryExprPtr;
using AST::VariableExprPtr;

using AST::BlockStmtPtr;
using AST::ExprStmtPtr;
using AST::ForStmtPtr;
using AST::IfStmtPtr;
using AST::PrintStmtPtr;
using AST::RetStmtPtr;
using AST::StmtPtrVariant;
using AST::VarStmtPtr;
using AST::WhileStmtPtr;

using ErrorsAndDebug::ErrorReporter;

using Types::Literal;
using Types::Token;
using Types::TokenType;

using ErrorsAndDebug::ErrorReporter;

class Evaluator {
 public:
  explicit Evaluator(ErrorReporter& eReporter);
  void evaluate(const AST::StmtPtrVariant& stmt);
  auto evaluate(const ExprPtrVariant& expr) -> LoxObject;
  void evaluate(const std::vector<AST::StmtPtrVariant>& stmts);

 private:
  // evaluation functions for Expr types
  auto evaluateBinaryExpr(const BinaryExprPtr& expr) -> LoxObject;
  auto evaluateGroupingExpr(const GroupingExprPtr& expr) -> LoxObject;
  static auto evaluateLiteralExpr(const LiteralExprPtr& expr) -> LoxObject;
  auto evaluateUnaryExpr(const UnaryExprPtr& expr) -> LoxObject;
  auto evaluateConditionalExpr(const ConditionalExprPtr& expr) -> LoxObject;
  auto evaluatePostfixExpr(const PostfixExprPtr& expr) -> LoxObject;
  auto evaluateVariableExpr(const VariableExprPtr& expr) -> LoxObject;
  auto evaluateAssignmentExpr(const AssignmentExprPtr& expr) -> LoxObject;
  auto evaluateLogicalExpr(const LogicalExprPtr& expr) -> LoxObject;
  auto evaluateCallExpr(const CallExprPtr& expr) -> LoxObject;

  // evaluation functions for Stmt types
  void evaluateExprStmt(const ExprStmtPtr& stmt);
  void evaluatePrintStmt(const PrintStmtPtr& stmt);
  void evaluateBlockStmt(const BlockStmtPtr& stmt);
  void evaluateVarStmt(const VarStmtPtr& stmt);
  void evaluateIfStmt(const IfStmtPtr& stmt);
  void evaluateWhileStmt(const WhileStmtPtr& stmt);
  void evaluateForStmt(const ForStmtPtr& stmt);
  void evaluateFuncStmt(const FuncStmtPtr& stmt);
  void evaluateRetStmt(const RetStmtPtr& stmt);

  class Return : std::exception {
    LoxObject value;

   public:
    explicit Return(LoxObject value);
    auto get() -> LoxObject;
  };

  // throws RuntimeError if right isn't a double
  auto getDouble(const Token& token, const LoxObject& right) -> double;

  ErrorReporter& eReporter;
  EnvironmentManager environManager;

  static const int MAX_RUNTIME_ERR = 20;
  int numRunTimeErr = 0;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_EVALUATOR__H