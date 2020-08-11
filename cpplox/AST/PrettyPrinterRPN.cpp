// This file implements a Pretty Printer that prints in Reverse Polish notation.
// I've retained the file as an example of how to do RPN printing, but it's no
// longer maintained and is incomplete.

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/Types/Literal.h"

#include <utility>

namespace cpplox::AST {

class PrettyPrinterRPN {
 public:
  explicit PrettyPrinterRPN(ExprPtrVariant expression);
  auto toString() -> std::string;
  [[nodiscard]] auto toString(const ExprPtrVariant& expression) const
      -> std::string;

 private:
  ExprPtrVariant expression;
};

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

auto printConditionalExpr(const PrettyPrinterRPN& printer,
                          const ConditionalExprPtr& expr) -> std::string {
  return printer.toString(expr->condition) + " "
         + printer.toString(expr->thenBranch) + " "
         + printer.toString(expr->elseBranch) + "  ?:";
}

auto printPostfixExpr(const PrettyPrinterRPN& printer,
                      const PostfixExprPtr& expr) -> std::string {
  return printer.toString(expr->left) + " " + expr->op.getLexeme();
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
    case 1:  // GroupingExprPtr
      return printGroupingExpr(*this, std::get<1>(expression));
    case 2:  // LiteralExprPtr
      return printLiteralExpr(*this, std::get<2>(expression));
    case 3:  // UnaryExprPtr
      return printUnaryExpr(*this, std::get<3>(expression));
    case 4:  // ConditionalExpr
      return printConditionalExpr(*this, std::get<4>(expression));
    case 5:  // PostfixExpr
      return printPostfixExpr(*this, std::get<5>(expression));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 6,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinterRPN::toString(const ExptrVariant&)!");
      return "";
  }
}

}  // namespace cpplox::AST