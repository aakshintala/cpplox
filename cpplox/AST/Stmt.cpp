#include "cpplox/AST/Stmt.h"

#include <string>
#include <utility>

namespace cpplox::AST {

// Statment AST types;
ExprStmt::ExprStmt(ExprPtrVariant expr) : expression(std::move(expr)) {}

PrintStmt::PrintStmt(ExprPtrVariant expr) : expression(std::move(expr)) {}

BlockStmt::BlockStmt(std::vector<StmtPtrVariant> statements)
    : statements(std::move(statements)) {}

VarStmt::VarStmt(Token varName, std::optional<ExprPtrVariant> initializer)
    : varName(std::move(varName)), initializer(std::move(initializer)) {}

// Helper functions to create ExprPtrVariants for each Expr type
auto createExprSPV(ExprPtrVariant expr) -> StmtPtrVariant {
  return std::make_unique<ExprStmt>(std::move(expr));
}

auto createPrintSPV(ExprPtrVariant expr) -> StmtPtrVariant {
  return std::make_unique<PrintStmt>(std::move(expr));
}

auto createBlockSPV(std::vector<StmtPtrVariant> statements) -> StmtPtrVariant {
  return std::make_unique<BlockStmt>(std::move(statements));
}

auto createVarSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant {
  return std::make_unique<VarStmt>(varName, std::move(initializer));
}

}  // namespace cpplox::AST
