#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/AST/Expr.h"
#include "cpplox/Types/Literal.h"

#include <utility>

namespace cpplox::AST {
PrettyPrinter::PrettyPrinter(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

namespace {
auto parenthesize(const PrettyPrinter& printer, const std::string& name,
                  std::initializer_list<ExprPtrVariant> exprs) -> std::string {
  std::string result = "(";
  result += name;
  for (const auto& exprPtrVariant : exprs) {
    result += " ";
    result += printer.toString(exprPtrVariant);
  }
  return result + ")";
}

auto printBinaryExpr(const PrettyPrinter& printer, const BinaryExprPtr& expr)
    -> std::string {
  return parenthesize(printer, expr->op.getLexeme(), {expr->left, expr->right});
}
auto printGroupingExpr(const PrettyPrinter& printer,
                       const GroupingExprPtr& expr) -> std::string {
  std::string name = "group";
  return parenthesize(printer, name, {expr->expression});
}

auto printLiteralExpr(const PrettyPrinter& printer, const LiteralExprPtr& expr)
    -> std::string {
  return expr->literalVal.has_value()
             ? Types::getLiteralString(expr->literalVal.value())
             : "nil";
}

auto printUnaryExpr(const PrettyPrinter& printer, const UnaryExprPtr& expr)
    -> std::string {
  return parenthesize(printer, expr->op.getLexeme(), {expr->right});
}

auto printConditionalExpr(const PrettyPrinter& printer,
                          const ConditionalExprPtr& expr) -> std::string {
  return parenthesize(printer,
                      ": " + parenthesize(printer, "?", {expr->condition}),
                      {expr->thenBranch, expr->elseBranch});
}

auto printPostfixExpr(const PrettyPrinter& printer, const PostfixExprPtr& expr)
    -> std::string {
  return parenthesize(printer, "POSTFIX " + expr->op.getLexeme(), {expr->left});
}

}  // namespace

auto PrettyPrinter::toString() -> std::string { return toString(expression); }

auto PrettyPrinter::toString(const ExprPtrVariant& expression) const
    -> std::string {
  // std::variant<BinaryExprPtr, GroupingExprPtr, LiteralExprPtr, UnaryExprPtr>;
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
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 6,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinter::toString(const ExptrVariant&)!");
      return "";
  }
}

}  // namespace cpplox::AST