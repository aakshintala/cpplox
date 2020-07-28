#ifndef STATUSCODES_H
#define STATUSCODES_H
#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace cpplox {

enum struct LoxStatus { OK, ERROR };

class ErrorReporter {
 public:
  ErrorReporter() : status(LoxStatus::OK) {}

  void setError(int line, std::string message) {
    errorMessages.emplace_back("[Line " + std::to_string(line)
                               + "] Error: " + message);
    status = LoxStatus::ERROR;
  }

  LoxStatus getStatus() { return status; }

  void printToStdErr() {
    std::cerr << "Oh-Oh, Your Lox code appears to contain errors!" << std::endl;
    for (auto& s : errorMessages) {
      std::cerr << s << std::endl;
    }
  }

 private:
  std::vector<std::string> errorMessages;
  LoxStatus status;
};

}  // namespace cpplox
#endif  // STATUSCODES_H