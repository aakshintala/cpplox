#include "cpplox/Evaluator/Environment.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <variant>

#include "Objects.h"
#include "cpplox/ErrorsAndDebug/RuntimeError.h"

namespace cpplox::Evaluator {

// ================= //
// class Environment
// ================= //
EnvironmentManager::Environment::Environment(ErrorReporter& eReporter)
    : eReporter(eReporter) {}

EnvironmentManager::Environment::Environment(ErrorReporter& eReporter,
                                             EnvironmentPtr parentEnviron)
    : eReporter(eReporter), parentEnviron(std::move(parentEnviron)) {}

auto EnvironmentManager::Environment::assign(const Types::Token& varToken,
                                             LoxObject object) -> bool {
  auto iter = objects.find(varToken.getLexeme());
  if (iter != objects.end()) {
    objects.insert_or_assign(varToken.getLexeme(), object);
    return true;
  }
  if (parentEnviron) return parentEnviron->assign(varToken, std::move(object));

  throw ErrorsAndDebug::reportRuntimeError(  // throws only in Global scope
      eReporter, varToken, "Attempt to assign to an undefined variable");
}

void EnvironmentManager::Environment::define(const Types::Token& varToken,
                                             LoxObject object) {
  objects.insert_or_assign(varToken.getLexeme(), object);
}

void EnvironmentManager::Environment::define(const Types::Token& varToken,
                                             FuncUniqPtr function) {
  // Free the older function FuncObj if one exists.
  // We technically don't need to do this, but it's good hygiene.
  if (auto iter = objects.find(varToken.getLexeme()); iter != objects.end()) {
    funcOwnerVec.remove_if([iter](const FuncUniqPtr& funcObjPtr) -> bool {
      return std::get<FuncObj*>(iter->second) == funcObjPtr.get();
    });
  }

  // Insert a LoxObject with a raw ptr to the FuncObj into the object map;
  objects.insert_or_assign(varToken.getLexeme(), LoxObject(function.get()));

  // Assume ownership of the function's FuncObj
  funcOwnerVec.push_back(std::move(function));
}

void EnvironmentManager::Environment::define(const Types::Token& varToken,
                                             BuiltinFuncUniqPtr function) {
  // Free the older BuiltinFunc if one exists.
  // We technically don't need to do this, but it's good hygiene.
  if (auto iter = objects.find(varToken.getLexeme()); iter != objects.end()) {
    builtinfuncOwnerVec.remove_if(
        [iter](const BuiltinFuncUniqPtr& funcObjPtr) -> bool {
          return std::get<BuiltinFunc*>(iter->second) == funcObjPtr.get();
        });
  }

  // Store a LoxObject with a raw ptr to the BuiltinFunc;
  objects.insert_or_assign(varToken.getLexeme(), LoxObject(function.get()));

  // Assume ownership of the function's BuiltinFunc
  builtinfuncOwnerVec.push_back(std::move(function));
}

auto EnvironmentManager::Environment::get(const Types::Token& varToken)
    -> LoxObject {
  auto iter = objects.find(varToken.getLexeme());
  if (iter != objects.end()) {
    if (std::holds_alternative<std::nullptr_t>(iter->second))
      throw ErrorsAndDebug::reportRuntimeError(
          eReporter, varToken, "Attempt to access an uninitialized variable");
    return iter->second;
  }
  if (parentEnviron) return parentEnviron->get(varToken);

  throw ErrorsAndDebug::reportRuntimeError(  // throws only in Global scope
      eReporter, varToken, "Attempt to access an undefined variable");
}

auto EnvironmentManager::Environment::isGlobal() -> bool {
  return (parentEnviron == nullptr);
}

auto EnvironmentManager::Environment::releaseParentEnv() -> EnvironmentPtr {
  // Safe to not set parentEnviron to nullptr cos as soon as this function
  // returns this environment goes out of scope and the unique_ptr mechanism
  // takes care of deleting this environemnt.
  return std::move(parentEnviron);
}

// ======================== //
// class EnvironmentManager
// ======================== //
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
  if (!currEnviron->isGlobal()) {
    currEnviron = currEnviron->releaseParentEnv();
  } else {
    eReporter.setError(
        -1, "Internal Error. Attempted to discard Global environment.");
    throw ErrorsAndDebug::RuntimeError();
  }
}

void EnvironmentManager::define(const Types::Token& varToken,
                                LoxObject object) {
  currEnviron->define(varToken, std::move(object));
}

void EnvironmentManager::define(const Types::Token& varToken,
                                FuncUniqPtr function) {
  currEnviron->define(varToken, std::move(function));
}

void EnvironmentManager::define(const Types::Token& varToken,
                                BuiltinFuncUniqPtr function) {
  currEnviron->define(varToken, std::move(function));
}

void EnvironmentManager::assign(const Types::Token& varToken,
                                LoxObject object) {
  currEnviron->assign(varToken, std::move(object));
}

auto EnvironmentManager::get(const Types::Token& varToken) -> LoxObject {
  return currEnviron->get(varToken);
}

}  // namespace cpplox::Evaluator