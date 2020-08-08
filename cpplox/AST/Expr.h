#ifndef CPPLOX_AST_EXPR_H
#define CPPLOX_AST_EXPR_H
#pragma once
// This header file describes the Types for Expression nodes in the AST
#include <memory>
#include <string>
#include <variant>

#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox {

using Types::Literal;
using Types::OptionalLiteral;
using Types::Token;
using Types::TokenType;
using Types::Uncopyable;

namespace AST {

// Forward declare all the Expression Types so we can define their shared
// pointer types and then the variant that holds all of those pointers.
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;

// Shared pointer sugar.
using BinaryExprPtr = std::shared_ptr<BinaryExpr>;
using GroupingExprPtr = std::shared_ptr<GroupingExpr>;
using LiteralExprPtr = std::shared_ptr<LiteralExpr>;
using UnaryExprPtr = std::shared_ptr<UnaryExpr>;

// The variant that we will use to pass around pointers to each of these
// expression types. I'm exploring this so we don't have to rely on vTables for
// dynamic dispatch on visitor functions.
using ExprPtrVariant = std::variant<BinaryExprPtr, GroupingExprPtr,
                                    LiteralExprPtr, UnaryExprPtr>;

// Helper functions to create ExprPtrVariants for each Expr type
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant;
auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant;
auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant;
auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant;
// auto createLiteralEPV(const std::string& str) -> ExprPtrVariant;

struct BinaryExpr final : public Uncopyable,
                          public std::enable_shared_from_this<BinaryExpr> {
  ExprPtrVariant left;
  Token op;
  ExprPtrVariant right;
  BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right);
};

struct GroupingExpr final : public Uncopyable,
                            public std::enable_shared_from_this<GroupingExpr> {
  ExprPtrVariant expression;
  explicit GroupingExpr(ExprPtrVariant expression);
};

struct LiteralExpr final : public Uncopyable,
                           public std::enable_shared_from_this<LiteralExpr> {
  OptionalLiteral literalVal;
  explicit LiteralExpr(OptionalLiteral value);
};

struct UnaryExpr final : public Uncopyable,
                         public std::enable_shared_from_this<UnaryExpr> {
  Token op;
  ExprPtrVariant right;
  UnaryExpr(Token op, ExprPtrVariant right);
};

}  // namespace AST
}  // namespace cpplox
#endif  // CPPLOX_AST_EXPR_H
