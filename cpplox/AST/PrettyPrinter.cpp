#include "cpplox/AST/PrettyPrinter.h"
#include "Stmt.h"
#include "cpplox/AST/Expr.h"
#include "cpplox/Types/Literal.h"

#include <iterator>
#include <utility>
#include <vector>

namespace cpplox::AST {
//=========================//
// Expr Printing functions //
//=========================//
namespace {
auto parenthesize(const PrettyPrinter& printer, const std::string& name,
                  const ExprPtrVariant& expr) -> std::string {
  return "(" + name + " " + printer.toString(expr) + ")";
}

auto parenthesize(const PrettyPrinter& printer, const std::string& name,
                  const ExprPtrVariant& expr1, const ExprPtrVariant& expr2)
    -> std::string {
  return "(" + name + " " + printer.toString(expr1) + " "
         + printer.toString(expr2) + ")";
}

auto printBinaryExpr(const PrettyPrinter& printer, const BinaryExprPtr& expr)
    -> std::string {
  return parenthesize(printer, expr->op.getLexeme(), expr->left, expr->right);
}
auto printGroupingExpr(const PrettyPrinter& printer,
                       const GroupingExprPtr& expr) -> std::string {
  std::string name = "group";
  return parenthesize(printer, name, expr->expression);
}

auto printLiteralExpr(const PrettyPrinter& printer, const LiteralExprPtr& expr)
    -> std::string {
  return expr->literalVal.has_value()
             ? Types::getLiteralString(expr->literalVal.value())
             : "nil";
}

auto printUnaryExpr(const PrettyPrinter& printer, const UnaryExprPtr& expr)
    -> std::string {
  return parenthesize(printer, expr->op.getLexeme(), expr->right);
}

auto printConditionalExpr(const PrettyPrinter& printer,
                          const ConditionalExprPtr& expr) -> std::string {
  return parenthesize(printer,
                      ": " + parenthesize(printer, "?", expr->condition),
                      expr->thenBranch, expr->elseBranch);
}

auto printPostfixExpr(const PrettyPrinter& printer, const PostfixExprPtr& expr)
    -> std::string {
  return parenthesize(printer, "POSTFIX " + expr->op.getLexeme(), expr->left);
}

auto printVariableExpr(const VariableExprPtr& expr) -> std::string {
  return "(" + expr->varName.getLexeme() + ")";
}

auto printAssignmentExpr(const PrettyPrinter& printer,
                         const AssignmentExprPtr& expr) -> std::string {
  return parenthesize(printer, "= " + expr->varName.getLexeme(), expr->right)
         + ";";
}

auto printLogicalExpr(const PrettyPrinter& printer, const LogicalExprPtr& expr)
    -> std::string {
  return parenthesize(printer, expr->op.getLexeme(), expr->left, expr->right);
}

}  // namespace

auto PrettyPrinter::toString(const ExprPtrVariant& expression) const
    -> std::string {
  switch (expression.index()) {
    case 0:  // BinaryExprPtr
      return printBinaryExpr(*this, std::get<0>(expression));
    case 1:  // GroupingExprPtr
      return printGroupingExpr(*this, std::get<1>(expression));
    case 2:  // LiteralExprPtr
      return printLiteralExpr(*this, std::get<2>(expression));
    case 3:  // UnaryExprPtr
      return printUnaryExpr(*this, std::get<3>(expression));
    case 4:  // ConditionalExprPtr
      return printConditionalExpr(*this, std::get<4>(expression));
    case 5:  // PostfixExprPtr
      return printPostfixExpr(*this, std::get<5>(expression));
    case 6:  // VariableExprPtr
      return printVariableExpr(std::get<6>(expression));
    case 7:  // AssignmentExprPtr
      return printAssignmentExpr(*this, std::get<7>(expression));
    case 8:  // LogicalExprPtr
      return printLogicalExpr(*this, std::get<8>(expression));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 9,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinter::toString(const ExptrVariant&)!");
      return "";
  }
}

//=========================//
// Stmt Printing functions //
//=========================//
namespace {
auto printExprStmt(const PrettyPrinter& printer, const ExprStmtPtr& stmt)
    -> std::string {
  return parenthesize(printer, "", stmt->expression) + ";";
}

auto printPrintStmt(const PrettyPrinter& printer, const PrintStmtPtr& stmt)
    -> std::string {
  return parenthesize(printer, "print", stmt->expression) + ";";
}

auto printBlockStmt(const PrettyPrinter& printer, const BlockStmtPtr& blkStmts)
    -> std::vector<std::string> {
  std::vector<std::string> blkStmtStrsVec;
  blkStmtStrsVec.emplace_back("{");
  for (auto& stmt : blkStmts->statements) {
    auto stmtStr = printer.toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(blkStmtStrsVec));
  }
  blkStmtStrsVec.emplace_back("}");
  return blkStmtStrsVec;
}

auto printVarStmt(const PrettyPrinter& printer, const VarStmtPtr& stmt)
    -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) " + printer.toString(stmt->initializer.value())
          + " )";
  }
  return str + ";";
}

auto printIfStmt(const PrettyPrinter& printer, const IfStmtPtr& stmt)
    -> std::vector<std::string> {
  std::vector<std::string> ifStmtStrVec;
  ifStmtStrVec.emplace_back("( if (" + printer.toString(stmt->condition) + ")");
  auto thenBranchVec = printer.toString(stmt->thenBranch);
  std::move(thenBranchVec.begin(), thenBranchVec.end(),
            std::back_inserter(ifStmtStrVec));
  if (stmt->elseBranch.has_value()) {
    ifStmtStrVec.emplace_back(" else ");
    auto elseBranchVec = printer.toString(stmt->elseBranch.value());
    std::move(elseBranchVec.begin(), elseBranchVec.end(),
              std::back_inserter(ifStmtStrVec));
  }
  ifStmtStrVec.emplace_back(" );");
  return ifStmtStrVec;
}

//   explicit WhileStmt(ExprPtrVariant condition, StmtPtrVariant loopBody);
auto printWhileStmt(const PrettyPrinter& printer, const WhileStmtPtr& stmt) {
  std::vector<std::string> whileStmtStrVec;
  whileStmtStrVec.emplace_back("( while (" + printer.toString(stmt->condition)
                               + ")");
  auto loopBodyVec = printer.toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(whileStmtStrVec));
  whileStmtStrVec.emplace_back(" );");
  return whileStmtStrVec;
}

auto printForStmt(const PrettyPrinter& printer, const ForStmtPtr& stmt) {
  std::vector<std::string> forStmtStrVec;
  forStmtStrVec.emplace_back("( for (");
  if (stmt->initializer.has_value()) {
    auto initVec = printer.toString(stmt->initializer.value());
    std::move(initVec.begin(), initVec.end(),
              std::back_inserter(forStmtStrVec));
  }
  forStmtStrVec.emplace_back(
      "; "
      + (stmt->condition.has_value() ? printer.toString(stmt->condition.value())
                                     : std::string())
      + ";"
      + (stmt->increment.has_value() ? printer.toString(stmt->increment.value())
                                     : std::string())
      + ")");

  auto loopBodyVec = printer.toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(forStmtStrVec));
  forStmtStrVec.emplace_back(" );");
  return forStmtStrVec;
}

// explicit ForStmt(std::optional<StmtPtrVariant> initializer,
//                  std::optional<ExprPtrVariant> condition,
//                  std::optional<ExprPtrVariant> increment,
//                  StmtPtrVariant loopBody);

}  // namespace

auto PrettyPrinter::toString(const StmtPtrVariant& statement) const
    -> std::vector<std::string> {
  switch (statement.index()) {
    case 0:  // ExprStmtPtr
      return std::vector(1, printExprStmt(*this, std::get<0>(statement)));
    case 1:  // PrintStmtPtr
      return std::vector(1, printPrintStmt(*this, std::get<1>(statement)));
    case 2:  // BlockStmtPtr
      return printBlockStmt(*this, std::get<2>(statement));
    case 3:  // VarStmtPtr
      return std::vector(1, printVarStmt(*this, std::get<3>(statement)));
    case 4:  // VarStmtPtr
      return printIfStmt(*this, std::get<4>(statement));
    case 5:  // WhileStmtPtr
      return printWhileStmt(*this, std::get<5>(statement));
    case 6:  // ForStmtPtr
      return printForStmt(*this, std::get<6>(statement));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 7,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
      return {};
  }
}

auto PrettyPrinter::toString() -> std::vector<std::string> {
  for (const auto& stmt : statements) {
    auto stmtStr = toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(stmtStrsVec));
  }
  return stmtStrsVec;
}

PrettyPrinter::PrettyPrinter(const std::vector<AST::StmtPtrVariant>& statements)
    : statements(statements) {}

}  // namespace cpplox::AST