#include "cpplox/AST/PrettyPrinter.h"
#include "Stmt.h"
#include "cpplox/AST/Expr.h"
#include "cpplox/Types/Literal.h"

#include <iterator>
#include <utility>
#include <vector>

namespace cpplox::AST {

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

auto printExprStmtPtr(const PrettyPrinter& printer, const ExprStmtPtr& stmt)
    -> std::string {
  return parenthesize(printer, "", stmt->expression) + ";";
}
auto printPrintStmtPtr(const PrettyPrinter& printer, const PrintStmtPtr& stmt)
    -> std::string {
  return parenthesize(printer, "print", stmt->expression) + ";";
}
auto printBlockStmtPtr(const PrettyPrinter& printer,
                       const BlockStmtPtr& blkStmts)
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

auto printVarStmtPtr(const PrettyPrinter& printer, const VarStmtPtr& stmt)
    -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) " + printer.toString(stmt->initializer.value())
          + " )";
  }
  return str + ";";
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
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 8,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinter::toString(const ExptrVariant&)!");
      return "";
  }
}
auto PrettyPrinter::toString(const StmtPtrVariant& statement) const
    -> std::vector<std::string> {
  switch (statement.index()) {
    case 0:  // ExprStmtPtr
      return std::vector(1, printExprStmtPtr(*this, std::get<0>(statement)));
    case 1:  // PrintStmtPtr
      return std::vector(1, printPrintStmtPtr(*this, std::get<1>(statement)));
    case 2:  // BlockStmtPtr
      return printBlockStmtPtr(*this, std::get<2>(statement));
    case 3:  // VarStmtPtr
      return std::vector(1, printVarStmtPtr(*this, std::get<3>(statement)));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 4,
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