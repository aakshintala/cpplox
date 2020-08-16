#ifndef CPPLOX_AST_EXPR_H
#define CPPLOX_AST_EXPR_H
#pragma once
// This header file describes Expression nodes in the AST
#include <memory>
#include <string>
#include <variant>

#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::AST {
using Types::Literal;
using Types::OptionalLiteral;
using Types::Token;
using Types::TokenType;
using Types::Uncopyable;

// Forward declare all the Expression Types so we can define their pointers
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct ConditionalExpr;
struct PostfixExpr;
struct VariableExpr;
struct AssignmentExpr;
struct LogicalExpr;

// Unique_pointer sugar.
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using ConditionalExprPtr = std::unique_ptr<ConditionalExpr>;
using PostfixExprPtr = std::unique_ptr<PostfixExpr>;
using VariableExprPtr = std::unique_ptr<VariableExpr>;
using AssignmentExprPtr = std::unique_ptr<AssignmentExpr>;
using LogicalExprPtr = std::unique_ptr<LogicalExpr>;

// The variant that we will use to pass around pointers to each of these
// expression types. I'm exploring this so we don't have to rely on vTables for
// dynamic dispatch on visitor functions.
using ExprPtrVariant
    = std::variant<BinaryExprPtr, GroupingExprPtr, LiteralExprPtr, UnaryExprPtr,
                   ConditionalExprPtr, PostfixExprPtr, VariableExprPtr,
                   AssignmentExprPtr, LogicalExprPtr>;

// Helper functions to create ExprPtrVariants for each Expr type
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant;
auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant;
auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant;
auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant;
auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant;
auto createPostfixEPV(ExprPtrVariant left, Token op) -> ExprPtrVariant;
auto createVariableEPV(Token varName) -> ExprPtrVariant;
auto createAssignmentEPV(Token varName, ExprPtrVariant expr) -> ExprPtrVariant;
auto createLogicalEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant;

// Expression types;
struct BinaryExpr final : public Uncopyable {
  ExprPtrVariant left;
  Token op;
  ExprPtrVariant right;
  BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right);
};

struct GroupingExpr final : public Uncopyable {
  ExprPtrVariant expression;
  explicit GroupingExpr(ExprPtrVariant expression);
};

struct LiteralExpr final : public Uncopyable {
  OptionalLiteral literalVal;
  explicit LiteralExpr(OptionalLiteral value);
};

struct UnaryExpr final : public Uncopyable {
  Token op;
  ExprPtrVariant right;
  UnaryExpr(Token op, ExprPtrVariant right);
};

struct ConditionalExpr final : public Uncopyable {
  ExprPtrVariant condition;
  ExprPtrVariant thenBranch;
  ExprPtrVariant elseBranch;
  ConditionalExpr(ExprPtrVariant condition, ExprPtrVariant thenBranch,
                  ExprPtrVariant elseBranch);
};

struct PostfixExpr final : public Uncopyable {
  ExprPtrVariant left;
  Token op;
  PostfixExpr(ExprPtrVariant left, Token op);
};

struct VariableExpr final : public Uncopyable {
  Token varName;
  explicit VariableExpr(Token varName);
};

struct AssignmentExpr final : public Uncopyable {
  Token varName;
  ExprPtrVariant right;
  AssignmentExpr(Token varName, ExprPtrVariant right);
};

struct LogicalExpr final : public Uncopyable {
  ExprPtrVariant left;
  Token op;
  ExprPtrVariant right;
  LogicalExpr(ExprPtrVariant left, Token op, ExprPtrVariant right);
};

}  // namespace cpplox::AST
#endif  // CPPLOX_AST_EXPR_H
