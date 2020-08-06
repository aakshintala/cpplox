#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/Types/Literal.h"

#include <utility>

namespace cpplox::AST {
PrettyPrinterRPN::PrettyPrinterRPN(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

namespace {

auto printBinaryExpr(const PrettyPrinterRPN& printer, const BinaryExprPtr& expr)
    -> std::string {
  return printer.toString(expr->left) + " " + printer.toString(expr->right)
         + " " + expr->op.getLexeme();
}

auto printGroupingExpr(const PrettyPrinterRPN& printer,
                       const GroupingExprPtr& expr) -> std::string {
  return printer.toString(expr->expression);
}

auto printLiteralExpr(const PrettyPrinterRPN& printer,
                      const LiteralExprPtr& expr) -> std::string {
  return expr->literalVal.has_value()
             ? Types::getLiteralString(expr->literalVal.value())
             : "nil";
}

auto printUnaryExpr(const PrettyPrinterRPN& printer, const UnaryExprPtr& expr)
    -> std::string {
  std::string op = expr->op.getLexeme();
  if (expr->op.getType() == TokenType::MINUS) op = "~";
  return printer.toString(expr->right) + " " + op;
}

}  // namespace

auto PrettyPrinterRPN::toString() -> std::string {
  return toString(expression);
}

auto PrettyPrinterRPN::toString(const ExprPtrVariant& expression) const
    -> std::string {
  // std::variant<BinaryExprPtr, GroupingExprPtr, LiteralExprPtr, UnaryExprPtr>;
  switch (expression.index()) {
    case 0:  // BinaryExprPtr
      return printBinaryExpr(*this, std::get<0>(expression));
      break;
    case 1:  // GroupingExprPtr
      return printGroupingExpr(*this, std::get<1>(expression));
      break;
    case 2:  // LiteralExprPtr
      return printLiteralExpr(*this, std::get<2>(expression));
      break;
    case 3:  // UnaryExprPtr
      return printUnaryExpr(*this, std::get<3>(expression));
      break;
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 4,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinterRPN::toString(const ExptrVariant&)!");
      return "";
      break;
  }
}

}  // namespace cpplox::AST