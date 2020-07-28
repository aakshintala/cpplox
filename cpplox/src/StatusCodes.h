#ifndef STATUSCODES_H
#define STATUSCODES_H
#pragma once

#include <string>

namespace cpplox {

enum struct LoxStatus { OK, BASIC_ERROR, UNKNOWN_ERROR };

class ErrorReporter {
 public:
  void error(int line, std::string message) {
    errorMessage += "[Line " + to_string(line) + "] Error: " + message;
  }

 private:
  std::string errorMessage;
};

}  // namespace cpplox
#endif  // STATUSCODES_H