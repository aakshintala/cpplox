#include "cpplox/AST/NodeTypes.h"

#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace cpplox::AST {
// ========================== //
// Expr AST Type Constructors //
// ========================== //
BinaryExpr::BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

GroupingExpr::GroupingExpr(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

LiteralExpr::LiteralExpr(OptionalLiteral value)
    : literalVal(std::move(value)) {}

UnaryExpr::UnaryExpr(Token op, ExprPtrVariant right)
    : op(std::move(op)), right(std::move(right)) {}

ConditionalExpr::ConditionalExpr(ExprPtrVariant condition,
                                 ExprPtrVariant thenBranch,
                                 ExprPtrVariant elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

PostfixExpr::PostfixExpr(ExprPtrVariant left, Token op)
    : left(std::move(left)), op(std::move(op)) {}

VariableExpr::VariableExpr(Token varName) : varName(std::move(varName)) {}

AssignmentExpr::AssignmentExpr(Token varName, ExprPtrVariant right)
    : varName(std::move(varName)), right(std::move(right)) {}

LogicalExpr::LogicalExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

CallExpr::CallExpr(ExprPtrVariant callee, Token paren,
                   std::vector<ExprPtrVariant> arguments)
    : callee(std::move(callee)),
      paren(std::move(paren)),
      arguments(std::move(arguments)) {}

FuncExpr::FuncExpr(std::vector<Token> parameters,
                   std::vector<StmtPtrVariant> body)
    : parameters(std::move(parameters)), body(std::move(body)) {}

GetExpr::GetExpr(ExprPtrVariant expr, Token name)
    : expr(std::move(expr)), name(std::move(name)) {}

SetExpr::SetExpr(ExprPtrVariant expr, Token name, ExprPtrVariant value)
    : expr(std::move(expr)), name(std::move(name)), value(std::move(value)) {}

ThisExpr::ThisExpr(Token keyword) : keyword(std::move(keyword)) {}

// ==============================//
// EPV creation helper functions //
// ==============================//
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<UnaryExpr>(op, std::move(right));
}

auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<GroupingExpr>(std::move(right));
}

auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant {
  return std::make_unique<LiteralExpr>(std::move(literal));
}

auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant {
  return std::make_unique<ConditionalExpr>(
      std::move(condition), std::move(then), std::move(elseBranch));
}

auto createPostfixEPV(ExprPtrVariant left, Token op) -> ExprPtrVariant {
  return std::make_unique<PostfixExpr>(std::move(left), op);
}

auto createVariableEPV(Token varName) -> ExprPtrVariant {
  return std::make_unique<VariableExpr>(varName);
}

auto createAssignmentEPV(Token varName, ExprPtrVariant expr) -> ExprPtrVariant {
  return std::make_unique<AssignmentExpr>(varName, std::move(expr));
}

auto createLogicalEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<LogicalExpr>(std::move(left), op, std::move(right));
}

auto createCallEPV(ExprPtrVariant callee, Token paren,
                   std::vector<ExprPtrVariant> arguments) -> ExprPtrVariant {
  return std::make_unique<CallExpr>(std::move(callee), std::move(paren),
                                    std::move(arguments));
}

auto createFuncEPV(std::vector<Token> params,
                   std::vector<StmtPtrVariant> fnBody) -> ExprPtrVariant {
  return std::make_unique<FuncExpr>(std::move(params), std::move(fnBody));
}

auto createGetEPV(ExprPtrVariant expr, Token name) -> ExprPtrVariant {
  return std::make_unique<GetExpr>(std::move(expr), std::move(name));
}

auto createSetEPV(ExprPtrVariant expr, Token name, ExprPtrVariant value)
    -> ExprPtrVariant {
  return std::make_unique<SetExpr>(std::move(expr), std::move(name),
                                   std::move(value));
}

auto createThisEPV(Token keyword) -> ExprPtrVariant {
  return std::make_unique<ThisExpr>(std::move(keyword));
}

// =================== //
// Statment AST types; //
// =================== //
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

FuncStmt::FuncStmt(Token funcName, FuncExprPtr funcExpr)
    : funcName(std::move(funcName)), funcExpr(std::move(funcExpr)) {}

RetStmt::RetStmt(Token ret, std::optional<ExprPtrVariant> value)
    : ret(std::move(ret)), value(std::move(value)) {}

ClassStmt::ClassStmt(Token className, std::vector<StmtPtrVariant> methods)
    : className(std::move(className)), methods(std::move(methods)) {}

// ============================================================= //
// Helper functions to create StmtPtrVariants for each Stmt type //
// ============================================================= //
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

auto createFuncSPV(Token fName, FuncExprPtr funcExpr) -> StmtPtrVariant {
  return std::make_unique<FuncStmt>(std::move(fName), std::move(funcExpr));
}

auto createRetSPV(Token ret, std::optional<ExprPtrVariant> value)
    -> StmtPtrVariant {
  return std::make_unique<RetStmt>(std::move(ret), std::move(value));
}

auto createClassSPV(Token className, std::vector<StmtPtrVariant> methods)
    -> StmtPtrVariant {
  return std::make_unique<ClassStmt>(std::move(className), std::move(methods));
}

}  // namespace cpplox::AST