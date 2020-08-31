#include "cpplox/Evaluator/Objects.h"

#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "cpplox/ErrorsAndDebug/RuntimeError.h"

namespace cpplox::Evaluator {

// FuncObj
FuncObj::FuncObj(const AST::FuncExprPtr& declaration, std::string funcName,
                 std::shared_ptr<Environment> closure, bool isMethod,
                 bool isInitializer)
    : declaration(declaration),
      funcName(std::move(funcName)),
      closure(std::move(closure)),
      isMethod(isMethod),
      isInitializer(isInitializer) {}

auto FuncObj::arity() const -> size_t { return declaration->parameters.size(); }

auto FuncObj::getParams() const -> const std::vector<Types::Token>& {
  return declaration->parameters;
}

auto FuncObj::getClosure() const -> std::shared_ptr<Environment> {
  return closure;
}

auto FuncObj::getDecl() const -> const AST::FuncExprPtr& { return declaration; }

auto FuncObj::getFnBodyStmts() const
    -> const std::vector<AST::StmtPtrVariant>& {
  return declaration->body;
}

auto FuncObj::getFnName() const -> const std::string& { return funcName; }

auto FuncObj::getIsMethod() const -> bool { return isMethod; }

auto FuncObj::getIsInitializer() const -> bool { return isInitializer; }

// BuiltinFunc
BuiltinFunc::BuiltinFunc(std::string funcName,
                         std::shared_ptr<Environment> closure)
    : funcName(std::move(funcName)), closure(std::move(closure)) {}

// LoxClass
LoxClass::LoxClass(
    std::string name,
    const std::vector<std::pair<std::string, LoxObject>>& methodPairs)
    : className(std::move(name)) {
  for (const std::pair<std::string, LoxObject>& mPair : methodPairs)
    methods.insert_or_assign(hasher(mPair.first), mPair.second);
}

auto LoxClass::getClassName() -> std::string { return className; }

auto LoxClass::findMethod(const std::string& methodName)
    -> std::optional<LoxObject> {
  auto iter = methods.find(hasher(methodName));
  if (iter != methods.end()) return iter->second;
  return std::nullopt;
}

// LoxInstance
LoxInstance::LoxInstance(LoxClassShrdPtr klass) : klass(std::move(klass)) {}

auto LoxInstance::toString() -> std::string {
  return "Instance of " + klass->getClassName();
}

auto LoxInstance::get(const std::string& propName) -> LoxObject {
  auto iter = fields.find(hasher(propName));
  if (iter != fields.end()) {
    return iter->second;
  }
  std::optional<LoxObject> method = klass->findMethod(propName);
  if (method.has_value()) return method.value();

  throw ErrorsAndDebug::RuntimeError();
}

void LoxInstance::set(const std::string& propName, LoxObject value) {
  fields[hasher(propName)] = std::move(value);
}

// LoxObject Functions
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
      case 4:  // FuncShrdPtr
        return std::get<FuncShrdPtr>(left)->getFnName()
               == std::get<FuncShrdPtr>(right)->getFnName();
      case 5:  // BuiltinFuncShrdPtr
        return std::get<BuiltinFuncShrdPtr>(left)->getFnName()
               == std::get<BuiltinFuncShrdPtr>(right)->getFnName();
      case 6:  // LoxClassShrdPtr
        return std::get<LoxClassShrdPtr>(left)->getClassName()
               == std::get<LoxClassShrdPtr>(right)->getClassName();
      case 7:  // LoxInstanceShrdPtr
        return std::get<LoxInstanceShrdPtr>(left).get()
               == std::get<LoxInstanceShrdPtr>(right).get();
      default:
        static_assert(std::variant_size_v<LoxObject> == 8,
                      "Looks like you forgot to update the cases in "
                      "ExprEvaluator::areEqual(const LoxObject&, const "
                      "LoxObject&)!");
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
    case 4:  // FuncShrdPtr
      return std::get<FuncShrdPtr>(object)->getFnName();
    case 5:  // BuiltinFuncShrdPtr
      return std::get<BuiltinFuncShrdPtr>(object)->getFnName();
    case 6:  // LoxClassShrdPtr
      return std::get<LoxClassShrdPtr>(object)->getClassName();
    case 7:  // LoxInstanceShrdPtr
      return std::get<LoxInstanceShrdPtr>(object)->toString();
    default:
      static_assert(std::variant_size_v<LoxObject> == 8,
                    "Looks like you forgot to update the cases in "
                    "getLiteralString()!");
      return "";
  }
}

auto isTrue(const LoxObject& object) -> bool {
  if (std::holds_alternative<std::nullptr_t>(object)) return false;
  if (std::holds_alternative<bool>(object)) return std::get<bool>(object);
  if (std::holds_alternative<FuncShrdPtr>(object))
    return (std::get<FuncShrdPtr>(object) == nullptr);
  if (std::holds_alternative<BuiltinFuncShrdPtr>(object))
    return (std::get<BuiltinFuncShrdPtr>(object) == nullptr);
  if (std::holds_alternative<LoxClassShrdPtr>(object))
    return (std::get<LoxClassShrdPtr>(object) == nullptr);
  if (std::holds_alternative<LoxInstanceShrdPtr>(object))
    return (std::get<LoxInstanceShrdPtr>(object) == nullptr);
  return true;  // for all else we go to true
}

}  // namespace cpplox::Evaluator