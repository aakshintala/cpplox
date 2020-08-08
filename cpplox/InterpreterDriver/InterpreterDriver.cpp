#include "cpplox/InterpreterDriver/InterpreterDriver.h"

#include <fstream>
#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Parser/Parser.h"
#include "cpplox/Scanner/Scanner.h"
#include "cpplox/Types/Token.h"

namespace cpplox {

using ErrorsAndDebug::ErrorReporter;
using ErrorsAndDebug::LoxStatus;
using Parser::RDParser;
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
  std::list<Token> tokensList;
  ErrorReporter eReporter;
  Scanner scanner(source, tokensList, eReporter);
  scanner.tokenize();

  if (eReporter.getStatus() != LoxStatus::OK) {
    hadError = true;
    eReporter.printToStdErr();
    return;
  }

  std::cout << "Here are the tokens our scanner found:" << std::endl;
  for (auto& token : tokensList) {
    std::cout << token.toString() << std::endl;
  }

  eReporter = ErrorReporter();
  std::vector<Token> tokenVec(std::make_move_iterator(tokensList.begin()),
                              std::make_move_iterator(tokensList.end()));
  RDParser parser(tokenVec, eReporter);
  std::optional<AST::ExprPtrVariant> optionalExpr = parser.parse();

  if (eReporter.getStatus() != LoxStatus::OK || !optionalExpr.has_value()) {
    hadError = true;
    eReporter.printToStdErr();
  } else {
    AST::PrettyPrinter printer(optionalExpr.value());
    std::cout << "Here's the AST that was generated:" << std::endl;
    std::cout << printer.toString() << std::endl;
  }
}

}  // namespace cpplox