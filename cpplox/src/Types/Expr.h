#ifndef TYPES_EXPR_H
#define TYPES_EXPR_H
#pragma once

#include <memory>
#include <string>
#include <variant>

#include "Token.h"

namespace cpplox::Types {

struct Expr {
  explicit Expr() = default;
  virtual ~Expr() = default;
  Expr(const Expr&) = delete;
  auto operator=(const Expr&) -> Expr& = delete;
  Expr(Expr&&) = delete;
  auto operator=(Expr &&) -> Expr& = delete;
};  // struct Expr

using ExprPtr = std::shared_ptr<Expr>;

class BinaryExpr final : public Expr,
                         public std::enable_shared_from_this<BinaryExpr> {
  ExprPtr left;
  Token op;
  ExprPtr right;

 public:
  BinaryExpr(const ExprPtr& left, const Token& op, const ExprPtr& right);
};

using BinaryExprPtr = std::shared_ptr<BinaryExpr>;

class GroupingExpr final : public Expr,
                           public std::enable_shared_from_this<GroupingExpr> {
  ExprPtr expression;

 public:
  explicit GroupingExpr(const ExprPtr& expression);
};

using GroupingExprPtr = std::shared_ptr<GroupingExpr>;

class LiteralExpr final : public Expr,
                          public std::enable_shared_from_this<LiteralExpr> {
  Literal value;

 public:
  explicit LiteralExpr(const Literal& value);
};

using LiteralExprPtr = std::shared_ptr<LiteralExpr>;

class UnaryExpr final : public Expr,
                        public std::enable_shared_from_this<UnaryExpr> {
  Token op;
  ExprPtr right;

 public:
  UnaryExpr(const Token& op, const ExprPtr& right);
};

using UnaryExprPtr = std::shared_ptr<UnaryExpr>;

}  // namespace cpplox::Types
#endif  // TYPES_EXPR_H
