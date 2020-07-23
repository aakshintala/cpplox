#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "TreewalkInterpreter.h"

namespace cpplox {

int TreewalkInterpreter::runScript(const char* const script) {
  const auto source = ([&]() {
    std::fstream in(script, std::ios::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    return std::string{std::istreambuf_iterator<char>{in},
                        std::istreambuf_iterator<char>{}};
  })();
  return this->interpret(source);
}

int TreewalkInterpreter::runREPL() {
  std::string line;
  int lastStatus = 0;
  while (std::getline(std::cin, line))
    lastStatus = this->interpret(line);
  return lastStatus;
}

int TreewalkInterpreter::interpret(const std::string &source) {
  std::cout << source << std::endl;
  return 70;
}

} // namespace cpplox