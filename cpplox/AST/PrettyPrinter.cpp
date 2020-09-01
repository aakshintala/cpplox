#include "cpplox/AST/PrettyPrinter.h"

#include <iterator>
#include <utility>
#include <vector>

#include "NodeTypes.h"
#include "cpplox/Types/Literal.h"

namespace cpplox::AST {
//=========================//
// Expr Printing functions //
//=========================//
namespace {
auto parenthesize(const std::string& name, const ExprPtrVariant& expr)
    -> std::string {
  return "(" + name + " " + PrettyPrinter::toString(expr) + ")";
}

auto parenthesize(const std::string& name, const ExprPtrVariant& expr1,
                  const ExprPtrVariant& expr2) -> std::string {
  return "(" + name + " " + PrettyPrinter::toString(expr1) + " "
         + PrettyPrinter::toString(expr2) + ")";
}

auto printBinaryExpr(const BinaryExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->left, expr->right);
}
auto printGroupingExpr(const GroupingExprPtr& expr) -> std::string {
  std::string name = "group";
  return parenthesize(name, expr->expression);
}

auto printLiteralExpr(const LiteralExprPtr& expr) -> std::string {
  return expr->literalVal.has_value()
             ? Types::getLiteralString(expr->literalVal.value())
             : "nil";
}

auto printUnaryExpr(const UnaryExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->right);
}

auto printConditionalExpr(const ConditionalExprPtr& expr) -> std::string {
  return parenthesize(": " + parenthesize("?", expr->condition),
                      expr->thenBranch, expr->elseBranch);
}

auto printPostfixExpr(const PostfixExprPtr& expr) -> std::string {
  return parenthesize("POSTFIX " + expr->op.getLexeme(), expr->left);
}

auto printVariableExpr(const VariableExprPtr& expr) -> std::string {
  return "(" + expr->varName.getLexeme() + ")";
}

auto printAssignmentExpr(const AssignmentExprPtr& expr) -> std::string {
  return parenthesize("= " + expr->varName.getLexeme(), expr->right) + ";";
}

auto printLogicalExpr(const LogicalExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->left, expr->right);
}
// myGloriousFn(arg1, expr1+expr2)
// ( ((arg1), (+ expr1 expr2)) myGloriousFn )
auto printCallExpr(const CallExprPtr& expr) -> std::string {
  std::string result;
  for (size_t i = 0; i < expr->arguments.size(); ++i) {
    if (i > 0 && i < expr->arguments.size() - 1) result += ", ";
    result += "(" + PrettyPrinter::toString(expr->arguments[i]) + ")";
  }
  return parenthesize("(" + result + ")", expr->callee);
}

auto printFuncExpr(const FuncExprPtr& expr) -> std::string {
  std::string funcStr = "(";
  for (size_t i = 0; i < expr->parameters.size(); ++i) {
    if (0 != i) funcStr += ", ";
    funcStr += expr->parameters[i].getLexeme();
  }
  funcStr += ") {\n";
  for (auto& bodyStmt : expr->body) {
    for (const auto& str : PrettyPrinter::toString(bodyStmt)) {
      funcStr += str + "\n";
    }
  }
  funcStr += "\n}";
  return funcStr;
}

auto printGetExpr(const GetExprPtr& expr) -> std::string {
  std::string str = PrettyPrinter::toString(expr->expr);
  str += " .( get " + expr->name.getLexeme() + " )";
  return str;
}

auto printSetExpr(const SetExprPtr& expr) -> std::string {
  std::string str = PrettyPrinter::toString(expr->expr);
  std::string val = PrettyPrinter::toString(expr->value);
  str += " .( set " + expr->name.getLexeme() + " ) = " + val;
  return str;
}

auto printThisExpr(const ThisExprPtr& expr) -> std::string {
  return "( this )";
}

auto printSuperExpr(const SuperExprPtr& expr) -> std::string {
  return "( super." + expr->method.toString() + " )";
}

}  // namespace

auto PrettyPrinter::toString(const ExprPtrVariant& expression) -> std::string {
  switch (expression.index()) {
    case 0:  // BinaryExprPtr
      return printBinaryExpr(std::get<0>(expression));
    case 1:  // GroupingExprPtr
      return printGroupingExpr(std::get<1>(expression));
    case 2:  // LiteralExprPtr
      return printLiteralExpr(std::get<2>(expression));
    case 3:  // UnaryExprPtr
      return printUnaryExpr(std::get<3>(expression));
    case 4:  // ConditionalExprPtr
      return printConditionalExpr(std::get<4>(expression));
    case 5:  // PostfixExprPtr
      return printPostfixExpr(std::get<5>(expression));
    case 6:  // VariableExprPtr
      return printVariableExpr(std::get<6>(expression));
    case 7:  // AssignmentExprPtr
      return printAssignmentExpr(std::get<7>(expression));
    case 8:  // LogicalExprPtr
      return printLogicalExpr(std::get<8>(expression));
    case 9:  // CallExprPtr
      return printCallExpr(std::get<9>(expression));
    case 10:  // FuncExprPtr
      return printFuncExpr(std::get<10>(expression));
    case 11:  // GetExprPtr
      return printGetExpr(std::get<11>(expression));
    case 12:  // SetExprPtr
      return printSetExpr(std::get<12>(expression));
    case 13:  // ThisExprPtr
      return printThisExpr(std::get<13>(expression));
    case 14:  // SuperExprPtr
      return printSuperExpr(std::get<14>(expression));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 15,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinter::toString(const ExptrVariant&)!");
      return "";
  }
}

namespace {

auto printExprStmt(const ExprStmtPtr& stmt) -> std::string {
  return parenthesize("", stmt->expression) + ";";
}

auto printPrintStmt(const PrintStmtPtr& stmt) -> std::string {
  return parenthesize("print", stmt->expression) + ";";
}

auto printBlockStmt(const BlockStmtPtr& blkStmts) -> std::vector<std::string> {
  std::vector<std::string> blkStmtStrsVec;
  blkStmtStrsVec.emplace_back("{");
  for (auto& stmt : blkStmts->statements) {
    auto stmtStr = PrettyPrinter::toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(blkStmtStrsVec));
  }
  blkStmtStrsVec.emplace_back("}");
  return blkStmtStrsVec;
}

auto printVarStmt(const VarStmtPtr& stmt) -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) "
          + PrettyPrinter::toString(stmt->initializer.value()) + " )";
  }
  return str + ";";
}

auto printIfStmt(const IfStmtPtr& stmt) -> std::vector<std::string> {
  std::vector<std::string> ifStmtStrVec;
  ifStmtStrVec.emplace_back("( if (" + PrettyPrinter::toString(stmt->condition)
                            + ")");
  auto thenBranchVec = PrettyPrinter::toString(stmt->thenBranch);
  std::move(thenBranchVec.begin(), thenBranchVec.end(),
            std::back_inserter(ifStmtStrVec));
  if (stmt->elseBranch.has_value()) {
    ifStmtStrVec.emplace_back(" else ");
    auto elseBranchVec = PrettyPrinter::toString(stmt->elseBranch.value());
    std::move(elseBranchVec.begin(), elseBranchVec.end(),
              std::back_inserter(ifStmtStrVec));
  }
  ifStmtStrVec.emplace_back(" );");
  return ifStmtStrVec;
}

auto printWhileStmt(const WhileStmtPtr& stmt) {
  std::vector<std::string> whileStmtStrVec;
  whileStmtStrVec.emplace_back(
      "( while (" + PrettyPrinter::toString(stmt->condition) + ")");
  auto loopBodyVec = PrettyPrinter::toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(whileStmtStrVec));
  whileStmtStrVec.emplace_back(" );");
  return whileStmtStrVec;
}

auto printForStmt(const ForStmtPtr& stmt) {
  std::vector<std::string> forStmtStrVec;
  forStmtStrVec.emplace_back("( for (");
  if (stmt->initializer.has_value()) {
    auto initVec = PrettyPrinter::toString(stmt->initializer.value());
    std::move(initVec.begin(), initVec.end(),
              std::back_inserter(forStmtStrVec));
  }
  forStmtStrVec.emplace_back(
      "; "
      + (stmt->condition.has_value()
             ? PrettyPrinter::toString(stmt->condition.value())
             : std::string())
      + ";"
      + (stmt->increment.has_value()
             ? PrettyPrinter::toString(stmt->increment.value())
             : std::string())
      + ")");

  auto loopBodyVec = PrettyPrinter::toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(forStmtStrVec));
  forStmtStrVec.emplace_back(" );");
  return forStmtStrVec;
}

// Token funcName, FuncExprPtr funcExpr
auto printFuncStmt(const FuncStmtPtr& stmt) -> std::vector<std::string> {
  std::vector<std::string> funcStrVec;
  funcStrVec.emplace_back("( ( " + stmt->funcName.getLexeme() + ") ");
  funcStrVec.emplace_back(printFuncExpr(stmt->funcExpr));
  funcStrVec.emplace_back(")");
  return funcStrVec;
}

auto printRetStmt(const RetStmtPtr& stmt) -> std::string {
  std::string value = stmt->value.has_value()
                          ? PrettyPrinter::toString(stmt->value.value())
                          : " ";
  return "( return" + value + ");";
}

auto printClassStmt(const ClassStmtPtr& stmt) -> std::vector<std::string> {
  std::vector<std::string> strVec;
  strVec.emplace_back("(CLASS " + stmt->className.getLexeme() + " )");
  if (stmt->superClass.has_value())
    strVec.emplace_back(" < "
                        + PrettyPrinter::toString(stmt->superClass.value()));
  strVec.emplace_back("{");
  for (const StmtPtrVariant& method : stmt->methods) {
    std::vector<std::string> methodStr = PrettyPrinter::toString(method);
    std::move(methodStr.begin(), methodStr.end(), std::back_inserter(strVec));
  }
  strVec.emplace_back("}");
  return strVec;
}

}  // namespace

auto PrettyPrinter::toString(const StmtPtrVariant& statement)
    -> std::vector<std::string> {
  switch (statement.index()) {
    case 0:  // ExprStmtPtr
      return std::vector(1, printExprStmt(std::get<0>(statement)));
    case 1:  // PrintStmtPtr
      return std::vector(1, printPrintStmt(std::get<1>(statement)));
    case 2:  // BlockStmtPtr
      return printBlockStmt(std::get<2>(statement));
    case 3:  // VarStmtPtr
      return std::vector(1, printVarStmt(std::get<3>(statement)));
    case 4:  // VarStmtPtr
      return printIfStmt(std::get<4>(statement));
    case 5:  // WhileStmtPtr
      return printWhileStmt(std::get<5>(statement));
    case 6:  // ForStmtPtr
      return printForStmt(std::get<6>(statement));
    case 7:  // ForStmtPtr
      return printFuncStmt(std::get<7>(statement));
    case 8:  // RetStmtPtr
      return std::vector(1, printRetStmt(std::get<8>(statement)));
    case 9:  // ClassStmtPtr
      return printClassStmt(std::get<9>(statement));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 10,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
      return {};
  }
}

auto PrettyPrinter::toString(const std::vector<AST::StmtPtrVariant>& statements)
    -> std::vector<std::string> {
  std::vector<std::string> stmtStrsVec;
  for (const auto& stmt : statements) {
    auto stmtStr = PrettyPrinter::toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(stmtStrsVec));
  }
  return stmtStrsVec;
}

}  // namespace cpplox::AST
