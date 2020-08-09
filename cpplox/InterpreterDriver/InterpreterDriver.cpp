#include "cpplox/InterpreterDriver/InterpreterDriver.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/ErrorsAndDebug/DebugPrint.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Parser/Parser.h"
#include "cpplox/Scanner/Scanner.h"
#include "cpplox/Types/Token.h"

namespace cpplox {

using ErrorsAndDebug::debugPrint;
using ErrorsAndDebug::ErrorReporter;
using ErrorsAndDebug::LoxStatus;
using Parser::RDParser;
using Types::Token;
using Types::TokenType;

const int EXIT_DATAERR = 65;

auto InterpreterDriver::runScript(const char* const scriptFile) -> int {
  const auto source = ([&]() -> std::string {
    std::ifstream in(scriptFile, std::ios::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    return std::string{std::istreambuf_iterator<char>{in},
                       std::istreambuf_iterator<char>{}};
  })();

  if (source.empty()) return EXIT_DATAERR;

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
  ErrorReporter scannerEReporter;
  Scanner scanner(source, tokensList, scannerEReporter);
  scanner.tokenize();

  if (scannerEReporter.getStatus() != LoxStatus::OK) {
    hadError = true;
    scannerEReporter.printToStdErr();
    return;
  }

  debugPrint("Here are the tokens our scanner found:");
  for (auto& token : tokensList) {
    debugPrint(token.toString());
  }

  ErrorReporter parserEReporter = ErrorReporter();
  std::vector<Token> tokenVec(std::make_move_iterator(tokensList.begin()),
                              std::make_move_iterator(tokensList.end()));
  RDParser parser(tokenVec, parserEReporter);

  std::optional<AST::ExprPtrVariant> optionalExpr = std::nullopt;
  try {
    optionalExpr = parser.parse();
  } catch (const std::exception& e) {
    std::string errorMessage = "Caught unhandled exception: ";
    errorMessage += e.what();
    debugPrint(errorMessage);
  }

  if (parserEReporter.getStatus() != LoxStatus::OK) {
    hadError = true;
    parserEReporter.printToStdErr();
  } else if (!optionalExpr.has_value()) {
    debugPrint("Parser returned nullopt.");
  } else {
    AST::PrettyPrinter printer(optionalExpr.value());
    debugPrint("Here's the AST that was generated:");
    debugPrint(printer.toString());
  }
}

}  // namespace cpplox