#include "cpplox/Evaluator/Objects.h"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

namespace cpplox::Evaluator {

FuncObj::FuncObj(const AST::FuncExprPtr& declaration, std::string funcName)
    : declaration(declaration), funcName(std::move(funcName)) {}

auto FuncObj::arity() const -> size_t { return declaration->parameters.size(); }

auto FuncObj::getParams() const -> const std::vector<Types::Token>& {
  return declaration->parameters;
}

auto FuncObj::getFnBody() const -> const std::vector<AST::StmtPtrVariant>& {
  return declaration->body;
}

auto FuncObj::getFnName() const -> const std::string& { return funcName; }

BuiltinFunc::BuiltinFunc(std::string funcName)
    : funcName(std::move(funcName)) {}

auto areEqual(const LoxObject& left, const LoxObject& right) -> bool {
  if (left.index() == right.index()) {
    switch (left.index()) {
      case 0:  // string
        return std::get<std::string>(left) == std::get<std::string>(right);
      case 1:  // double
        return std::get<double>(left) == std::get<double>(right);
      case 2:  // bool
        return std::get<bool>(left) == std::get<bool>(right);
      case 3:  // std::nullptr_t
        // The case where one is null and the other isn't is handled by the
        // outer condition;
        return true;
      case 4:  // FuncObj*
        return std::get<FuncObj*>(left)->getFnName()
               == std::get<FuncObj*>(right)->getFnName();
      case 5:  // BuiltinFunc*
        return std::get<BuiltinFunc*>(left)->getFnName()
               == std::get<BuiltinFunc*>(right)->getFnName();
      default:
        static_assert(
            std::variant_size_v<LoxObject> == 6,
            "Looks like you forgot to update the cases in "
            "ExprEvaluator::areEqual(const LoxObject&, const LoxObject&)!");
    }
  }
  return false;
}

auto getObjectString(const LoxObject& object) -> std::string {
  switch (object.index()) {
    case 0:  // string
      return std::get<0>(object);
    case 1: {  // double
      std::string result = std::to_string(std::get<1>(object));
      auto pos = result.find(".000000");
      if (pos != std::string::npos)
        result.erase(pos, std::string::npos);
      else
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
      return result;
    }
    case 2:  // bool
      return std::get<2>(object) == true ? "true" : "false";
    case 3:  // nullptr
      return "nil";
    case 4:  // FuncObj*
      return std::get<FuncObj*>(object)->getFnName();
    case 5:  // BuiltinFunc*
      return std::get<BuiltinFunc*>(object)->getFnName();
    default:
      static_assert(std::variant_size_v<LoxObject> == 6,
                    "Looks like you forgot to update the cases in "
                    "getLiteralString()!");
      return "";
  }
}

auto isTrue(const LoxObject& object) -> bool {
  if (std::holds_alternative<std::nullptr_t>(object)) return false;
  if (std::holds_alternative<bool>(object)) return std::get<bool>(object);
  if (std::holds_alternative<FuncObj*>(object))
    return (std::get<FuncObj*>(object) == nullptr);
  if (std::holds_alternative<BuiltinFunc*>(object))
    return (std::get<BuiltinFunc*>(object) == nullptr);
  return true;  // for all else we go to true
}

}  // namespace cpplox::Evaluator