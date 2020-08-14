#ifndef CPPLOX_EVALUATOR_ENVIRONMENT__H
#define CPPLOX_EVALUATOR_ENVIRONMENT__H
#pragma once

#include <map>
#include <memory>
#include <optional>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Token.h"
#include "cpplox/Types/Uncopyable.h"
#include "cpplox/Types/Value.h"

namespace cpplox::Evaluator {
using ErrorsAndDebug::ErrorReporter;

class EnvironmentManager : public Types::Uncopyable {
 public:
  explicit EnvironmentManager(ErrorReporter& eReporter);
  void createNewEnviron();
  void discardCurrentEnviron();
  void define(const Types::Token& varToken, std::optional<Types::Value> value);
  void assign(const Types::Token& varToken, const Types::Value& value);
  auto get(const Types::Token& varToken) -> Types::Value;

 private:
  class Environment;
  using EnvironmentPtr = std::unique_ptr<Environment>;

  class Environment : public Types::Uncopyable {
   public:
    explicit Environment(ErrorReporter& eReporter);
    explicit Environment(ErrorReporter& eReporter,
                         EnvironmentPtr parentEnviron);

    auto assign(const Types::Token& varToken, const Types::Value& value)
        -> bool;
    void define(const Types::Token& varToken,
                std::optional<Types::Value> value);
    auto get(const Types::Token& varToken) -> Types::Value;
    auto isGlobal() -> bool;
    auto releaseParentEnv() -> EnvironmentPtr;

   private:
    std::map<std::string, std::optional<Types::Value> > values;
    ErrorReporter& eReporter;
    EnvironmentPtr parentEnviron = nullptr;
  };

  ErrorReporter& eReporter;
  EnvironmentPtr currEnviron;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_ENVIRONMENT__H