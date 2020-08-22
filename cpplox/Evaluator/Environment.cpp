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

EnvironmentManager::Environment::Environment(EnvironmentPtr parentEnviron)
    : parentEnviron(std::move(parentEnviron)) {}

auto EnvironmentManager::Environment::assign(size_t hashedVarName,
                                             LoxObject object) -> bool {
  auto iter = objects.find(hashedVarName);
  if (iter != objects.end()) {
    objects.insert_or_assign(hashedVarName, object);
    return true;
  }
  if (parentEnviron)
    return parentEnviron->assign(hashedVarName, std::move(object));

  throw UndefinedVarAccess();
}

void EnvironmentManager::Environment::define(size_t hashedVarName,
                                             LoxObject object) {
  objects.insert_or_assign(hashedVarName, object);
}

void EnvironmentManager::Environment::define(size_t hashedVarName,
                                             FuncUniqPtr function) {
  // Free the older function FuncObj if one exists.
  // We technically don't need to do this, but it's good hygiene.
  if (auto iter = objects.find(hashedVarName); iter != objects.end()) {
    funcOwnerVec.remove_if([iter](const FuncUniqPtr& funcObjPtr) -> bool {
      return std::get<FuncObj*>(iter->second) == funcObjPtr.get();
    });
  }

  // Insert a LoxObject with a raw ptr to the FuncObj into the object map;
  objects.insert_or_assign(hashedVarName, LoxObject(function.get()));

  // Assume ownership of the function's FuncObj
  funcOwnerVec.push_back(std::move(function));
}

void EnvironmentManager::Environment::define(size_t hashedVarName,
                                             BuiltinFuncUniqPtr function) {
  // Free the older BuiltinFunc if one exists.
  // We technically don't need to do this, but it's good hygiene.
  if (auto iter = objects.find(hashedVarName); iter != objects.end()) {
    builtinfuncOwnerVec.remove_if(
        [iter](const BuiltinFuncUniqPtr& funcObjPtr) -> bool {
          return std::get<BuiltinFunc*>(iter->second) == funcObjPtr.get();
        });
  }

  // Store a LoxObject with a raw ptr to the BuiltinFunc;
  objects.insert_or_assign(hashedVarName, LoxObject(function.get()));

  // Assume ownership of the function's BuiltinFunc
  builtinfuncOwnerVec.push_back(std::move(function));
}

auto EnvironmentManager::Environment::get(size_t hashedVarName) -> LoxObject {
  auto iter = objects.find(hashedVarName);
  if (iter != objects.end()) {
    if (std::holds_alternative<std::nullptr_t>(iter->second))
      throw UninitializedVarAccess();
    return iter->second;
  }
  if (parentEnviron) return parentEnviron->get(hashedVarName);

  throw UndefinedVarAccess();  // throws only in the Global Environ
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
      currEnviron(std::make_unique<Environment>(nullptr)) {}

void EnvironmentManager::createNewEnviron() {
  currEnviron = std::make_unique<Environment>(std::move(currEnviron));
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
  currEnviron->define(hasher(varToken.getLexeme()), std::move(object));
}

void EnvironmentManager::define(const Types::Token& varToken,
                                FuncUniqPtr function) {
  currEnviron->define(hasher(varToken.getLexeme()), std::move(function));
}

void EnvironmentManager::define(const Types::Token& varToken,
                                BuiltinFuncUniqPtr function) {
  currEnviron->define(hasher(varToken.getLexeme()), std::move(function));
}

void EnvironmentManager::assign(const Types::Token& varToken,
                                LoxObject object) {
  try {
    currEnviron->assign(hasher(varToken.getLexeme()), std::move(object));
  } catch (const UndefinedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Can't assign to an undefined variable.");
  }
}

auto EnvironmentManager::get(const Types::Token& varToken) -> LoxObject {
  try {
    return currEnviron->get(hasher(varToken.getLexeme()));
  } catch (const UndefinedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an undefined variable.");
  } catch (const UninitializedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an uninitialized variable.");
  }
}

}  // namespace cpplox::Evaluator