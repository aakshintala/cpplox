#include <iostream>

#include "ErrorReporter.h"

namespace cpplox {
namespace ErrorsAndDebug {

void ErrorReporter::setError(int line, std::string message) {
  errorMessages.emplace_back("[Line " + std::to_string(line)
                             + "] Error: " + message);
  status = LoxStatus::ERROR;
}

LoxStatus ErrorReporter::getStatus() { return status; }

void ErrorReporter::printToStdErr() {
  std::cerr << "Oh-Oh, Your Lox code appears to contain errors!" << std::endl;
  for (auto& s : errorMessages) {
    std::cerr << s << std::endl;
  }
}

}  // namespace ErrorsAndDebug
}  // namespace cpplox