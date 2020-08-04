#include <iostream>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"

namespace cpplox::ErrorsAndDebug {

void ErrorReporter::setError(int line, const std::string& message) {
  errorMessages.emplace_back("[Line " + std::to_string(line)
                             + "] Error: " + message);
  status = LoxStatus::ERROR;
}

auto ErrorReporter::getStatus() -> LoxStatus { return status; }

void ErrorReporter::printToStdErr() {
  std::cerr << "Oh-Oh, Your Lox code appears to contain errors!" << std::endl;
  for (auto& s : errorMessages) {
    std::cerr << s << std::endl;
  }
}

}  // namespace cpplox::ErrorsAndDebug