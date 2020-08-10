#ifndef CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H
#define CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H
#pragma once

#include <string>

namespace cpplox {

struct InterpreterDriver {
 public:
  auto runScript(const char* script) -> int;
  void runREPL();

 private:
  void runInterpreter(const std::string& source);
  bool hadError = false;
  bool hadRunTimeError = false;
};

}  // namespace cpplox

#endif  // CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H