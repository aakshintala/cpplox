#ifndef CPPLOX_EVALUATOR_ENVIRONMENT__H
#define CPPLOX_EVALUATOR_ENVIRONMENT__H
#include <exception>
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

  void createNewEnviron();
  void discardCurrentEnviron();
  void define(const Types::Token& varToken, LoxObject object);
  void define(const Types::Token& varToken, FuncUniqPtr function);
  void define(const Types::Token& varToken, BuiltinFuncUniqPtr function);
  void assign(const Types::Token& varToken, LoxObject object);
  auto get(const Types::Token& varToken) -> LoxObject;

 private:
  class Environment;
  using EnvironmentPtr = std::unique_ptr<Environment>;

  class UndefinedVarAccess : public std::exception {};
  class UninitializedVarAccess : public std::exception {};

  class Environment : public Types::Uncopyable {
   public:
    // For all environs other than Global, use this constructor.
    explicit Environment(EnvironmentPtr parentEnviron);

    auto assign(size_t hashedVarName, LoxObject object) -> bool;
    void define(size_t hashedVarName, LoxObject object);
    void define(size_t hashedVarName, FuncUniqPtr function);
    void define(size_t hashedVarName, BuiltinFuncUniqPtr function);
    auto get(size_t hashedVarName) -> LoxObject;
    auto isGlobal() -> bool;
    auto releaseParentEnv() -> EnvironmentPtr;

   private:
    std::map<size_t, LoxObject> objects;
    EnvironmentPtr parentEnviron = nullptr;

    // The environment is repsonsible for the lifetimes of functions.
    // Do not access functions through this interface.
    // This is purely a construct for single ownership.
    std::list<std::unique_ptr<FuncObj>> funcOwnerVec;
    std::list<std::unique_ptr<BuiltinFunc>> builtinfuncOwnerVec;
  };

  ErrorReporter& eReporter;
  EnvironmentPtr currEnviron;
  std::hash<std::string> hasher;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_ENVIRONMENT__H