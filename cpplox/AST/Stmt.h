#ifndef CPPLOX_AST_STMT_H
#define CPPLOX_AST_STMT_H
#pragma once

// This header file describes Statment nodes in the AST

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::AST {

using Types::Uncopyable;

// Forward Declaration of Statement Node types;
struct ExprStmt;
struct PrintStmt;
struct BlockStmt;
struct VarStmt;

// Shared pointer sugar.
using ExprStmtPtr = std::unique_ptr<ExprStmt>;
using PrintStmtPtr = std::unique_ptr<PrintStmt>;
using BlockStmtPtr = std::unique_ptr<BlockStmt>;
using VarStmtPtr = std::unique_ptr<VarStmt>;

// We use this variant to pass around pointers to each of these Stmt types,
// without having to resort to virtual functions and dynamic dispatch
using StmtPtrVariant
    = std::variant<ExprStmtPtr, PrintStmtPtr, BlockStmtPtr, VarStmtPtr>;

// Helper functions to create ExprPtrVariants for each Expr type
auto createExprSPV(ExprPtrVariant expr) -> StmtPtrVariant;
auto createPrintSPV(ExprPtrVariant expr) -> StmtPtrVariant;
auto createBlockSPV(std::vector<StmtPtrVariant> statements) -> StmtPtrVariant;
auto createVarSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant;

// Statment AST types;
struct ExprStmt final : public Uncopyable {
  ExprPtrVariant expression;
  explicit ExprStmt(ExprPtrVariant expr);
};

struct PrintStmt final : public Uncopyable {
  ExprPtrVariant expression;
  explicit PrintStmt(ExprPtrVariant expression);
};

struct BlockStmt final : public Uncopyable {
  std::vector<StmtPtrVariant> statements;
  explicit BlockStmt(std::vector<StmtPtrVariant> statements);
};

struct VarStmt final : public Uncopyable {
  Token varName;
  std::optional<ExprPtrVariant> initializer;
  explicit VarStmt(Token varName, std::optional<ExprPtrVariant> initializer);
};

}  // namespace cpplox::AST
#endif  // CPPLOX_AST_STMT_H
