#ifndef CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H
#define CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H
#pragma once

#include <string>
#include <vector>

#include "cpplox/AST/NodeTypes.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Evaluator/Evaluator.h"

namespace cpplox {

struct InterpreterDriver {
 public:
  InterpreterDriver();
  auto runScript(const char* script) -> int;
  void runREPL();

 private:
  void interpret(const std::string& source);

  ErrorsAndDebug::ErrorReporter eReporter;
  Evaluator::Evaluator evaluator;

  std::vector<std::vector<AST::StmtPtrVariant>> lines;

  bool hadError = false;
  bool hadRunTimeError = false;
};

}  // namespace cpplox

#endif  // CPPLOX_INTERPRETERDRIVER_INTERPRETERDRIVER_H