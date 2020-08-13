#ifndef CPPLOX_EVALUATOR_EXPREVALUATOR__H
#define CPPLOX_EVALUATOR_EXPREVALUATOR__H
#pragma once

// This base class is really kept around so the REPL can run really simple match
// calculator style expressions

#include <memory>
#include <string>

#include "cpplox/AST/Expr.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
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

using ErrorsAndDebug::ErrorReporter;

using Types::Literal;
using Types::Token;
using Types::TokenType;
using Types::Value;

class ExprEvaluator : Types::Uncopyable {
 public:
  explicit ExprEvaluator(ErrorReporter& eReporter);
  auto evaluate(const ExprPtrVariant& expression) -> Value;

 protected:
  // evaluation functions for Expr types
  auto evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value;
  auto evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value;
  static auto evaluateLiteralExpr(const LiteralExprPtr& expr) -> Value;
  auto evaluateUnaryExpr(const UnaryExprPtr& expr) -> Value;
  auto evaluateConditionalExpr(const ConditionalExprPtr& expr) -> Value;
  auto evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value;

  // Helper functions
  // throws RuntimeError if right isn't a double
  auto getDouble(const Token& token, Value right) -> double;

 private:
  // Parsed data we're operating on.
  ErrorReporter& eReporter;
};

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_EXPREVALUATOR__H