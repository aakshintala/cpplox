#include "cpplox/Evaluator/Environment.h"

#include <memory>
#include <utility>

#include "cpplox/ErrorsAndDebug/RuntimeError.h"

namespace cpplox::Evaluator {

// class Environment
EnvironmentManager::Environment::Environment(ErrorReporter& eReporter)
    : eReporter(eReporter) {}

EnvironmentManager::Environment::Environment(ErrorReporter& eReporter,
                                             EnvironmentPtr parentEnviron)
    : eReporter(eReporter), parentEnviron(std::move(parentEnviron)) {}

auto EnvironmentManager::Environment::assign(const Types::Token& varToken,
                                             const Types::Value& value)
    -> bool {
  auto iter = values.find(varToken.getLexeme());
  if (iter != values.end()) {
    iter->second = std::make_optional(value);
    return true;
  }
  if (parentEnviron) return parentEnviron->assign(varToken, value);

  throw ErrorsAndDebug::reportRuntimeError(  // throws only in Global scope
      eReporter, varToken, "Attempt to access an undefined variable");
}

void EnvironmentManager::Environment::define(
    const Types::Token& varToken, std::optional<Types::Value> value) {
  values.insert_or_assign(varToken.getLexeme(), value);
}

auto EnvironmentManager::Environment::get(const Types::Token& varToken)
    -> Types::Value {
  auto iter = values.find(varToken.getLexeme());
  if (iter != values.end()) {
    if (iter->second.has_value()) {
      return iter->second.value();
    }
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempt to access an uninitialized variable");
  }
  if (parentEnviron) return parentEnviron->get(varToken);

  throw ErrorsAndDebug::reportRuntimeError(  // throws only in Global scope
      eReporter, varToken, "Attempt to access an undefined variable");
}

auto EnvironmentManager::Environment::isGlobal() -> bool {
  return (parentEnviron == nullptr);
}

auto EnvironmentManager::Environment::releaseParentEnv() -> EnvironmentPtr {
  return std::move(parentEnviron);
}

// class EnvironmentManager
EnvironmentManager::EnvironmentManager(ErrorReporter& eReporter)
    : eReporter(eReporter),
      currEnviron(std::make_unique<Environment>(eReporter)) {}

void EnvironmentManager::createNewEnviron() {
  currEnviron
      = std::make_unique<Environment>(eReporter, std::move(currEnviron));
}

void EnvironmentManager::discardCurrentEnviron() {
  // Global environment should only be destroyed when the Environment Manager
  // goes away.
  if (!currEnviron->isGlobal()) currEnviron = currEnviron->releaseParentEnv();
}

void EnvironmentManager::define(const Types::Token& varToken,
                                std::optional<Types::Value> value) {
  currEnviron->define(varToken, std::move(value));
}

void EnvironmentManager::assign(const Types::Token& varToken,
                                const Types::Value& value) {
  currEnviron->assign(varToken, value);
}

auto EnvironmentManager::get(const Types::Token& varToken) -> Types::Value {
  return currEnviron->get(varToken);
}

}  // namespace cpplox::Evaluator