#ifndef CPPLOX_AST_PRETTYPRINTER__H
#define CPPLOX_AST_PRETTYPRINTER__H

#include <string>
#include <vector>

#include "cpplox/AST/NodeTypes.h"

namespace cpplox::AST::PrettyPrinter {

[[nodiscard]] auto toString(const std::vector<AST::StmtPtrVariant>& statements)
    -> std::vector<std::string>;
[[nodiscard]] auto toString(const ExprPtrVariant& expression) -> std::string;
[[nodiscard]] auto toString(const StmtPtrVariant& statement)
    -> std::vector<std::string>;

}  // namespace cpplox::AST::PrettyPrinter

#endif  // CPPLOX_AST_PRETTYPRINTER__H