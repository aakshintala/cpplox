#pragma once
#include <string>

namespace cpplox {

class TreewalkInterpreter {
public:
  int runScript(const char const *script);
  int runREPL();

private:
  int interpret(const std::string &source);
  void TreewalkInterpreter::error(int line, std::string message);

  bool hadError = false;
};

} // namespace cpplox
