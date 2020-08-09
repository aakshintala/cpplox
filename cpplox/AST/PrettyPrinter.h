#include "cpplox/AST/Expr.h"

#include <string>

namespace cpplox::AST {

class PrettyPrinter {
 public:
  explicit PrettyPrinter(ExprPtrVariant expression);
  auto toString() -> std::string;
  [[nodiscard]] auto toString(const ExprPtrVariant& expression) const
      -> std::string;

 private:
  ExprPtrVariant expression;
};

class PrettyPrinterRPN {
 public:
  explicit PrettyPrinterRPN(ExprPtrVariant expression);
  auto toString() -> std::string;
  [[nodiscard]] auto toString(const ExprPtrVariant& expression) const
      -> std::string;

 private:
  ExprPtrVariant expression;
};

}  // namespace cpplox::AST