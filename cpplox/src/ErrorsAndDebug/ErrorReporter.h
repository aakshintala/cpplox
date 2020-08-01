#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H
#pragma once

#include <string>
#include <vector>

namespace cpplox {
namespace ErrorsAndDebug {

enum struct LoxStatus { OK, ERROR };

class ErrorReporter {
 public:
  void setError(int line, std::string message);

  LoxStatus getStatus();

  void printToStdErr();

 private:
  std::vector<std::string> errorMessages;
  LoxStatus status = LoxStatus::OK;
};

}  // namespace ErrorsAndDebug
}  // namespace cpplox
#endif  // ERRORREPORTER_H