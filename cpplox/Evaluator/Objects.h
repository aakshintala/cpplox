#ifndef CPPLOX_EVALUATOR_FUNCTION__H
#define CPPLOX_EVALUATOR_FUNCTION__H
#include <memory>
#pragma once

#include <string>
#include <variant>

#include "cpplox/AST/NodeTypes.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::Evaluator {
class FuncObj;
using FuncUniqPtr = std::unique_ptr<FuncObj>;

class BuiltinFunc;
using BuiltinFuncUniqPtr = std::unique_ptr<BuiltinFunc>;

// A raw pointer to the FuncObj is provided here to allow access to the
// underlying FuncObj without transferring ownership. The underlying object
// is guaranteed to exist at least as long as the caller; Do not store
// LoxObjects
using LoxObject = std::variant<std::string, double, bool, std::nullptr_t,
                               FuncObj*, BuiltinFunc*>;

auto areEqual(const LoxObject& left, const LoxObject& right) -> bool;

auto getObjectString(const LoxObject& object) -> std::string;

auto isTrue(const LoxObject& object) -> bool;

class FuncObj : public Types::Uncopyable {
  const AST::FuncExprPtr& declaration;
  const std::string funcName;

 public:
  explicit FuncObj(const AST::FuncExprPtr& declaration, std::string funcName);
  [[nodiscard]] auto arity() const -> size_t;
  [[nodiscard]] auto getParams() const -> const std::vector<Types::Token>&;
  [[nodiscard]] auto getFnBodyStmts() const
      -> const std::vector<AST::StmtPtrVariant>&;
  [[nodiscard]] auto getFnName() const -> const std::string&;
};

class BuiltinFunc : public Types::Uncopyable {
  std::string funcName = "";

 public:
  explicit BuiltinFunc(std::string funcName);
  virtual auto arity() -> size_t = 0;
  virtual auto run() -> LoxObject = 0;
  virtual auto getFnName() -> std::string = 0;
};

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_FUNCTION__H