#include "cpplox/AST/Expr.h"

#include <string>

namespace cpplox::AST {

struct PrettyPrinter {
  explicit PrettyPrinter(ExprPtrVariant expression);
  auto toString() -> std::string;
  [[nodiscard]] auto toString(const ExprPtrVariant& expression) const
      -> std::string;

 private:
  ExprPtrVariant expression;
};

}  // namespace cpplox::AST