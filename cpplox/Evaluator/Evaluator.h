#ifndef CPPLOX_EVALUATOR_EVALUATOR__H
#define CPPLOX_EVALUATOR_EVALUATOR__H
#include <exception>
#pragma once

#include "cpplox/AST/Expr.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Value.h"

#include <string>

namespace cpplox::Evaluator {
using AST::BinaryExprPtr;
using AST::ConditionalExprPtr;
using AST::ExprPtrVariant;
using AST::GroupingExprPtr;
using AST::LiteralExprPtr;
using AST::PostfixExprPtr;
using AST::UnaryExprPtr;
using ErrorsAndDebug::ErrorReporter;
using Types::Literal;
using Types::Token;
using Types::Value;

class RuntimeError : std::exception {};

class Evaluator : Uncopyable {
 public:
  explicit Evaluator(ExprPtrVariant expression, ErrorReporter& eReporter);
  auto evaluate() -> Value;
  auto evaluate(const ExprPtrVariant& expression) -> Value;

 private:
  // evaluation functions for each type
  auto evaluateBinaryExpr(const BinaryExprPtr& expr) -> Value;
  auto evaluateGroupingExpr(const GroupingExprPtr& expr) -> Value;
  static auto evaluateLiteralExpr(const LiteralExprPtr& expr) -> Value;
  auto evaluateUnaryExpr(const UnaryExprPtr& expr) -> Value;
  auto evaluateConditionalExpr(const ConditionalExprPtr& expr) -> Value;
  auto evaluatePostfixExpr(const PostfixExprPtr& expr) -> Value;

  // Helper functions
  auto error(const Token& token, const std::string& message) -> RuntimeError;
  auto getDoubleElseError(const Token& token, Value right) -> double;
  static auto isTrue(const Value& value) -> bool;
  static auto isEqual(const Value& left, const Value& right) -> bool;

  ExprPtrVariant expression;
  ErrorReporter& eReporter;
};

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_EVALUATOR__H