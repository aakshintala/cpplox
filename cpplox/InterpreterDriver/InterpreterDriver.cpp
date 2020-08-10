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
#include "cpplox/Evaluator/Evaluator.h"
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
const int EXIT_SOFTWARE = 70;

auto InterpreterDriver::runScript(const char* const scriptFile) -> int {
  const auto source = ([&]() -> std::string {
    try {
      std::ifstream in(scriptFile, std::ios::in);
      in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
      return std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};
    } catch (std::exception& e) {
      debugPrint("Couldn't open Input source file.");
      return "";
    }
  })();

  if (source.empty()) return EXIT_DATAERR;

  this->runInterpreter(source);

  if (hadError) return EXIT_DATAERR;
  if (hadRunTimeError) return EXIT_SOFTWARE;
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
    this->runInterpreter(line);
    hadError = false;
    hadRunTimeError = false;
  }
  std::cout << std::endl << "# Goodbye!" << std::endl;
}

namespace {

auto scan(const std::string& source, std::list<Token>& tokensList) -> bool {
  ErrorReporter eReporter;
  Scanner scanner(source, tokensList, eReporter);
  scanner.tokenize();

  if (eReporter.getStatus() != LoxStatus::OK) {
    eReporter.printToStdErr();
    return false;
  }
  debugPrint("Here are the tokens the scanner recognized:");
  for (auto& token : tokensList) debugPrint(token.toString());

  return true;
}

auto parse(const std::vector<Token>& tokenVec)
    -> std::optional<AST::ExprPtrVariant> {
  ErrorReporter eReporter;
  RDParser parser(tokenVec, eReporter);

  std::optional<AST::ExprPtrVariant> optionalExpr = parser.parse();

  if (eReporter.getStatus() != LoxStatus::OK) {
    eReporter.printToStdErr();
  }

  if (optionalExpr.has_value()) {
    AST::PrettyPrinter printer(optionalExpr.value());
    debugPrint("Here's the AST that was generated:");
    debugPrint(printer.toString());
  } else {
    debugPrint("Parser returned nullopt.");
  }

  return optionalExpr;
}

}  // namespace

void InterpreterDriver::runInterpreter(const std::string& source) {
  // First we have to tokenize the input source.
  std::list<Token> tokensList;
  if (!scan(source, tokensList)) {
    hadError = true;
    return;
  }

  // Once we have the tokenized source, time to parse it!
  std::vector<Token> tokenVec(std::make_move_iterator(tokensList.begin()),
                              std::make_move_iterator(tokensList.end()));

  auto optionalExpr = parse(tokenVec);
  if (!optionalExpr.has_value()) {
    hadError = true;
    return;
  }

  // And, now we interpret it!
  AST::ExprPtrVariant expr = optionalExpr.value();
  ErrorReporter eReporter;
  try {
    Evaluator::Evaluator evaluator(expr, eReporter);
    std::cout << Types::getValueString(evaluator.evaluate()) << std::endl;
  } catch (const Evaluator::RuntimeError& e) {
    if (eReporter.getStatus() != LoxStatus::OK) {
      eReporter.printToStdErr();
    }
    hadRunTimeError = true;
  }
}

}  // namespace cpplox