#ifndef CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H
#define CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Token.h"

namespace cpplox::ErrorsAndDebug {

using Types::Token;

class RuntimeError : std::exception {};

auto reportRuntimeError(ErrorReporter& eReporter, const Token& token,
                        const std::string& message) -> RuntimeError;

}  // namespace cpplox::ErrorsAndDebug
#endif  // CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H