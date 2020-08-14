#ifndef CPPLOX_ERRORSANDDEBUG_ERRORREPORTER_H
#define CPPLOX_ERRORSANDDEBUG_ERRORREPORTER_H
#pragma once

#include <string>
#include <vector>

namespace cpplox::ErrorsAndDebug {

enum struct LoxStatus { OK, ERROR };

class ErrorReporter {
 public:
  void clearErrors();
  auto getStatus() -> LoxStatus;
  void printToStdErr();
  void setError(int line, const std::string& message);

 private:
  std::vector<std::string> errorMessages;
  LoxStatus status = LoxStatus::OK;
};

}  // namespace cpplox::ErrorsAndDebug
#endif  // CPPLOX_ERRORSANDDEBUG_ERRORREPORTER_H