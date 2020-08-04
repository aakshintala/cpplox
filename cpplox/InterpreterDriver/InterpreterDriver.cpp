#include "cpplox/InterpreterDriver/InterpreterDriver.h"

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "cpplox/AST/Expr.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Scanner/Scanner.h"
#include "cpplox/Types/Token.h"

namespace cpplox {

using ErrorsAndDebug::ErrorReporter;
using ErrorsAndDebug::LoxStatus;
using Types::Token;
using Types::TokenType;
const int EXIT_DATAERR = 65;

auto InterpreterDriver::runScript(const char* const script) -> int {
  const auto source = ([&]() {
    std::ifstream in(script, std::ifstream::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    return std::string{std::istreambuf_iterator<char>{in},
                       std::istreambuf_iterator<char>{}};
  })();

  this->interpret(source);

  if (hadError) return EXIT_DATAERR;
  return 0;
}

void InterpreterDriver::runREPL() {
  std::string line;
  std::cout
      << "# Greetings! I am a Tree-Walk Interpreter for the lox language\n"
         "# from Bob Nystrom's excellent book, Crafting Interpreters.\n"
         "# If you haven't already, check it out at "
         "http://www.craftinginterpreters.com/"
      << std::endl;
  while (std::cout << "> " && std::getline(std::cin, line)) {
    this->interpret(line);
    hadError = false;
  }
  std::cout << std::endl << "# Goodbye!" << std::endl;
}

void InterpreterDriver::interpret(const std::string& source) {
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