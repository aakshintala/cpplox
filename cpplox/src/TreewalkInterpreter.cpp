#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "Scanner.h"
#include "StatusCodes.h"
#include "TreewalkInterpreter.h"

namespace cpplox {

int TreewalkInterpreter::runScript(const char* const script) {
  const auto source = ([&]() {
    std::ifstream in(script, std::ifstream::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    return std::string{std::istreambuf_iterator<char>{in},
                       std::istreambuf_iterator<char>{}};
  })();

  this->interpret(source);

  if (hadError) return 65;
  return 0;
}

void TreewalkInterpreter::runREPL() {
  std::string line;
  while (std::getline(std::cin, line)) {
    this->interpret(line);
    hadError = false;
  }
}

void TreewalkInterpreter::interpret(const std::string& source) {
  std::list<Token> tokens;
  ErrorReporter eReporter;
  Scanner scanner(source, tokens, eReporter);

  scanner.tokenize();
  if (eReporter.getStatus() != LoxStatus::OK) {
    hadError = true;
    eReporter.printToStdErr();
  } else {
    std::cout << "Line TokenType lexeme literal" << std::endl;
    for (auto& t : tokens) {
      std::cout << t.toString() << std::endl;
    }
  }
}

}  // namespace cpplox