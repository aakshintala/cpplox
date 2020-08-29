#ifndef CPPLOX_EVALUATOR_FUNCTION__H
#define CPPLOX_EVALUATOR_FUNCTION__H
#pragma once

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

using LoxObject = std::variant<std::string, double, bool, std::nullptr_t,
                               FuncShrdPtr, BuiltinFuncShrdPtr>;

auto areEqual(const LoxObject& left, const LoxObject& right) -> bool;

auto getObjectString(const LoxObject& object) -> std::string;

auto isTrue(const LoxObject& object) -> bool;

class Environment;

class FuncObj : public Types::Uncopyable {
  const AST::FuncExprPtr& declaration;
  const std::string funcName;
  std::shared_ptr<Environment> closure;

 public:
  explicit FuncObj(const AST::FuncExprPtr& declaration, std::string funcName,
                   std::shared_ptr<Environment> closure);

  [[nodiscard]] auto arity() const -> size_t;
  [[nodiscard]] auto getClosure() const -> std::shared_ptr<Environment>;
  [[nodiscard]] auto getFnBodyStmts() const
      -> const std::vector<AST::StmtPtrVariant>&;
  [[nodiscard]] auto getFnName() const -> const std::string&;
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

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_FUNCTION__H