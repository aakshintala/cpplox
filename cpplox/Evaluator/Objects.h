#ifndef CPPLOX_EVALUATOR_FUNCTION__H
#define CPPLOX_EVALUATOR_FUNCTION__H
#include <optional>
#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>

#include "cpplox/AST/NodeTypes.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::Evaluator {
class FuncObj;
using FuncShrdPtr = std::shared_ptr<FuncObj>;

class BuiltinFunc;
using BuiltinFuncShrdPtr = std::shared_ptr<BuiltinFunc>;

class LoxClass;
using LoxClassShrdPtr = std::shared_ptr<LoxClass>;

class LoxInstance;
using LoxInstanceShrdPtr = std::shared_ptr<LoxInstance>;

using LoxObject
    = std::variant<std::string, double, bool, std::nullptr_t, FuncShrdPtr,
                   BuiltinFuncShrdPtr, LoxClassShrdPtr, LoxInstanceShrdPtr>;

auto areEqual(const LoxObject& left, const LoxObject& right) -> bool;

auto getObjectString(const LoxObject& object) -> std::string;

auto isTrue(const LoxObject& object) -> bool;

class Environment;

class FuncObj : public Types::Uncopyable {
  const AST::FuncExprPtr& declaration;
  const std::string funcName;
  std::shared_ptr<Environment> closure;
  bool isMethod;
  bool isInitializer;

 public:
  explicit FuncObj(const AST::FuncExprPtr& declaration, std::string funcName,
                   std::shared_ptr<Environment> closure, bool isMethod = false,
                   bool isInitializer = false);

  [[nodiscard]] auto arity() const -> size_t;
  [[nodiscard]] auto getClosure() const -> std::shared_ptr<Environment>;
  [[nodiscard]] auto getDecl() const -> const AST::FuncExprPtr&;
  [[nodiscard]] auto getFnBodyStmts() const
      -> const std::vector<AST::StmtPtrVariant>&;
  [[nodiscard]] auto getFnName() const -> const std::string&;
  [[nodiscard]] auto getIsMethod() const -> bool;
  [[nodiscard]] auto getIsInitializer() const -> bool;
  [[nodiscard]] auto getParams() const -> const std::vector<Types::Token>&;
};

class BuiltinFunc : public Types::Uncopyable {
  std::string funcName = "";
  std::shared_ptr<Environment> closure;

 public:
  explicit BuiltinFunc(std::string funcName,
                       std::shared_ptr<Environment> closure);

  virtual auto arity() -> size_t = 0;
  virtual auto run() -> LoxObject = 0;
  virtual auto getFnName() -> std::string = 0;
};

class LoxClass : public Types::Uncopyable {
  const std::string className;
  std::hash<std::string> hasher;
  std::map<size_t, LoxObject> methods;

 public:
  explicit LoxClass(
      std::string name,
      const std::vector<std::pair<std::string, LoxObject>>& methodPairs);

  auto getClassName() -> std::string;
  auto findMethod(const std::string& methodName) -> std::optional<LoxObject>;
};

class LoxInstance : public Types::Uncopyable {
  const LoxClassShrdPtr klass;
  std::hash<std::string> hasher;
  std::map<size_t, LoxObject> fields;

 public:
  explicit LoxInstance(LoxClassShrdPtr klass);

  auto toString() -> std::string;
  auto get(const std::string& propName) -> LoxObject;
  void set(const std::string& propName, LoxObject value);
};

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_FUNCTION__H