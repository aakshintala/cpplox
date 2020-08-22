#ifndef CPPLOX_EVALUATOR_EVALUATOR__H
#define CPPLOX_EVALUATOR_EVALUATOR__H
#include <exception>
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cpplox/AST/NodeTypes.h"
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
using AST::FuncExprPtr;
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
  auto evaluateExpr(const ExprPtrVariant& expr) -> LoxObject;
  auto evaluateStmt(const AST::StmtPtrVariant& stmt)
      -> std::optional<LoxObject>;
  auto evaluateStmts(const std::vector<AST::StmtPtrVariant>& stmts)
      -> std::optional<LoxObject>;

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
  static auto evaluateFuncExpr(const FuncExprPtr& expr) -> LoxObject;

  // evaluation functions for Stmt types
  auto evaluateExprStmt(const ExprStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluatePrintStmt(const PrintStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateBlockStmt(const BlockStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateVarStmt(const VarStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateIfStmt(const IfStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateWhileStmt(const WhileStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateForStmt(const ForStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateFuncStmt(const FuncStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateRetStmt(const RetStmtPtr& stmt) -> std::optional<LoxObject>;

  // throws RuntimeError if right isn't a double
  auto getDouble(const Token& token, const LoxObject& right) -> double;

  ErrorReporter& eReporter;
  EnvironmentManager environManager;

  static const int MAX_RUNTIME_ERR = 20;
  int numRunTimeErr = 0;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_EVALUATOR__H