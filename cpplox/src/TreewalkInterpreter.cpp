#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "TreewalkInterpreter.h"

namespace cpplox {

int TreewalkInterpreter::runScript(const char* const script) {
  const auto source = ([&]() {
    std::ifstream in(script, std::ifstream::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    return std::string{std::istreambuf_iterator<char>{in},
                       std::istreambuf_iterator<char>{}};
  })();
  int status = this->interpret(source);
  if (hadError) status = 65;
  return status;
}

int TreewalkInterpreter::runREPL() {
  std::string line;
  int lastStatus = 0;
  while (std::getline(std::cin, line)) {
    lastStatus = this->interpret(line);
    hadError = false;
  }
  return lastStatus;
}

int TreewalkInterpreter::interpret(const std::string& source) {
  // Call scanner on the source to tokenize
  // and then interpret tokens...
  // This is currently unimplemented so we just print out the string,
  // and return INTERNAL_SOFTWARE_ERRROR
  std::cout << source << std::endl;
  // hadError = true;
  return 70;
}

}  // namespace cpplox