#include <iostream>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"

namespace cpplox::ErrorsAndDebug {

void ErrorReporter::clearErrors() {
  errorMessages.clear();
  status = LoxStatus::OK;
}

auto ErrorReporter::getStatus() -> LoxStatus { return status; }

void ErrorReporter::printToStdErr() {
  for (auto& s : errorMessages) {
    std::cerr << s << std::endl;
  }
}

void ErrorReporter::setError(int line, const std::string& message) {
  errorMessages.emplace_back("[Line " + std::to_string(line)
                             + "] Error: " + message);
  status = LoxStatus::ERROR;
}

}  // namespace cpplox::ErrorsAndDebug