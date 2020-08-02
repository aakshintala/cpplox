#ifndef TREEWALKINTERPRETER_H
#define TREEWALKINTERPRETER_H
#pragma once

#include <string>

namespace cpplox {

class TreewalkInterpreter {
 public:
  auto runScript(const char* script) -> int;
  void runREPL();

 private:
  void interpret(const std::string& source);

  bool hadError = false;
};

}  // namespace cpplox

#endif  // TREEWALKINTERPRETER_H