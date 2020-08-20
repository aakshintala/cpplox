#include "cpplox/AST/Stmt.h"

#include <iterator>
#include <memory>
#include <optional>
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

IfStmt::IfStmt(ExprPtrVariant condition, StmtPtrVariant thenBranch,
               std::optional<StmtPtrVariant> elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

WhileStmt::WhileStmt(ExprPtrVariant condition, StmtPtrVariant loopBody)
    : condition(std::move(condition)), loopBody(std::move(loopBody)) {}

ForStmt::ForStmt(std::optional<StmtPtrVariant> initializer,
                 std::optional<ExprPtrVariant> condition,
                 std::optional<ExprPtrVariant> increment,
                 StmtPtrVariant loopBody)
    : initializer(std::move(initializer)),
      condition(std::move(condition)),
      increment(std::move(increment)),
      loopBody(std::move(loopBody)) {}

FuncStmt::FuncStmt(Token funcName, std::vector<Token> parameters,
                   std::vector<StmtPtrVariant> body)
    : funcName(std::move(funcName)),
      parameters(std::move(parameters)),
      body(std::move(body)) {}

RetStmt::RetStmt(Token ret, std::optional<ExprPtrVariant> value)
    : ret(std::move(ret)), value(std::move(value)) {}

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

auto createIfSPV(ExprPtrVariant condition, StmtPtrVariant thenBranch,
                 std::optional<StmtPtrVariant> elseBranch) -> StmtPtrVariant {
  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

auto createWhileSPV(ExprPtrVariant condition, StmtPtrVariant loopBody)
    -> StmtPtrVariant {
  return std::make_unique<WhileStmt>(std::move(condition), std::move(loopBody));
}

auto createForSPV(std::optional<StmtPtrVariant> initializer,
                  std::optional<ExprPtrVariant> condition,
                  std::optional<ExprPtrVariant> increment,
                  StmtPtrVariant loopBody) -> StmtPtrVariant {
  return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                   std::move(increment), std::move(loopBody));
}

auto createFuncSPV(Token fName, std::vector<Token> params,
                   std::vector<StmtPtrVariant> fnBody) -> StmtPtrVariant {
  return std::make_unique<FuncStmt>(std::move(fName), std::move(params),
                                    std::move(fnBody));
}

auto createRetSPV(Token ret, std::optional<ExprPtrVariant> value)
    -> StmtPtrVariant {
  return std::make_unique<RetStmt>(std::move(ret), std::move(value));
}

}  // namespace cpplox::AST
