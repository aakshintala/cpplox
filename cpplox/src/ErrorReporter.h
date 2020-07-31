#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H
#pragma once

#include <string>
#include <vector>

namespace cpplox {

enum struct LoxStatus { OK, ERROR };

class ErrorReporter {
 public:
  ErrorReporter();

  void setError(int line, std::string message);

  LoxStatus getStatus();

  void printToStdErr();

 private:
  std::vector<std::string> errorMessages;
  LoxStatus status;
};

}  // namespace cpplox
#endif  // ERRORREPORTER_H