#ifndef CPPLOX_EVALUATOR_ENVIRONMENT__H
#define CPPLOX_EVALUATOR_ENVIRONMENT__H
#pragma once

#include <cstddef>
#include <exception>
#include <list>
#include <map>
#include <memory>
#include <optional>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Evaluator/Objects.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"

namespace cpplox::Evaluator {
using ErrorsAndDebug::ErrorReporter;

// Do not use the Environment directly. Use the EnvironmentManager class below
// instead to manage it.
class Environment : public Types::Uncopyable,
                    std::enable_shared_from_this<Environment> {
 public:
  using EnvironmentPtr = std::shared_ptr<Environment>;

  explicit Environment(EnvironmentPtr parentEnviron);

  auto assign(size_t hashedVarName, LoxObject object) -> bool;
  void define(size_t hashedVarName, LoxObject object);
  auto get(size_t hashedVarName) -> LoxObject;
  auto getParentEnv() -> EnvironmentPtr;
  auto isGlobal() -> bool;

 private:
  std::map<size_t, LoxObject> objects;
  EnvironmentPtr parentEnviron = nullptr;
};

class EnvironmentManager : public Types::Uncopyable {
 public:
  explicit EnvironmentManager(ErrorReporter& eReporter);

  void assign(const Types::Token& varToken, LoxObject object);
  void createNewEnviron(const std::string& caller = __builtin_FUNCTION());
  void discardEnvironsTill(const Environment::EnvironmentPtr& environToRestore,
                           const std::string& caller = __builtin_FUNCTION());
  void define(const std::string& tokenStr, LoxObject object);
  void define(const Types::Token& varToken, LoxObject object);
  auto get(const Types::Token& varToken) -> LoxObject;
  auto getCurrEnv() -> Environment::EnvironmentPtr;
  void setCurrEnv(Environment::EnvironmentPtr newCurr,
                  const std::string& caller = __builtin_FUNCTION());

 private:
  ErrorReporter& eReporter;
  Environment::EnvironmentPtr currEnviron;
  std::hash<std::string> hasher;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_ENVIRONMENT__H