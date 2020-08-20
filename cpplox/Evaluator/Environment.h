#ifndef CPPLOX_EVALUATOR_ENVIRONMENT__H
#define CPPLOX_EVALUATOR_ENVIRONMENT__H
#pragma once

#include <cstddef>
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

class EnvironmentManager : public Types::Uncopyable {
 public:
  explicit EnvironmentManager(ErrorReporter& eReporter);
  void createNewEnviron(std::string environName);
  void discardCurrentEnviron(const std::string& environName);
  void define(const Types::Token& varToken, LoxObject object);
  void define(const Types::Token& varToken, FuncUniqPtr function);
  void define(const Types::Token& varToken, BuiltinFuncUniqPtr function);
  void assign(const Types::Token& varToken, LoxObject object);
  auto get(const Types::Token& varToken) -> LoxObject;

 private:
  class Environment;
  using EnvironmentPtr = std::unique_ptr<Environment>;

  class Environment : public Types::Uncopyable {
   public:
    // This constructor should only be used to construct the global environ.
    explicit Environment(ErrorReporter& eReporter);
    // For all other environs, use this one instead.
    explicit Environment(ErrorReporter& eReporter, EnvironmentPtr parentEnviron,
                         std::string environName);

    auto assign(const Types::Token& varToken, LoxObject object) -> bool;
    void define(const Types::Token& varToken, LoxObject object);
    void define(const Types::Token& varToken, FuncUniqPtr function);
    void define(const Types::Token& varToken, BuiltinFuncUniqPtr function);
    auto get(const Types::Token& varToken) -> LoxObject;
    auto getEnvironName() -> const std::string&;
    auto isGlobal() -> bool;
    auto releaseParentEnv() -> EnvironmentPtr;

   private:
    std::map<std::string, LoxObject> objects;
    ErrorReporter& eReporter;
    EnvironmentPtr parentEnviron = nullptr;
    std::string environName = "Global";

    // The environment is repsonsible for the lifetimes of functions.
    // Do not access functions through this interface.
    // This is purely a construct for single ownership.
    std::list<std::unique_ptr<FuncObj>> funcOwnerVec;
    std::list<std::unique_ptr<BuiltinFunc>> builtinfuncOwnerVec;
  };

  ErrorReporter& eReporter;
  EnvironmentPtr currEnviron;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_ENVIRONMENT__H