#ifndef CPPLOX_AST_PRETTYPRINTER__H
#define CPPLOX_AST_PRETTYPRINTER__H

#include <string>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/Stmt.h"

namespace cpplox::AST {

class PrettyPrinter {
 public:
  explicit PrettyPrinter(const std::vector<AST::StmtPtrVariant>& statements);
  auto toString() -> std::vector<std::string>;
  [[nodiscard]] auto toString(const ExprPtrVariant& expression) const
      -> std::string;
  [[nodiscard]] auto toString(const StmtPtrVariant& statement) const
      -> std::vector<std::string>;

 private:
  const std::vector<AST::StmtPtrVariant>& statements;
  std::vector<std::string> stmtStrsVec;
};

}  // namespace cpplox::AST

#endif  // CPPLOX_AST_PRETTYPRINTER__H