#include "cpplox/InterpreterDriver/InterpreterDriver.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/AST/Stmt.h"
#include "cpplox/ErrorsAndDebug/DebugPrint.h"
#include "cpplox/ErrorsAndDebug/RuntimeError.h"
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

  this->interpret(source);

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
    this->interpret(line);
    hadError = false;
    hadRunTimeError = false;
  }
  std::cout << std::endl << "# Goodbye!" << std::endl;
}

namespace {

class InterpreterError : std::exception {};

auto scan(const std::string& source) -> std::vector<Token> {
  ErrorReporter eReporter;
  Scanner scanner(source, eReporter);

  std::vector<Token> tokensVec = scanner.tokenize();

  if (eReporter.getStatus() != LoxStatus::OK) {
    eReporter.printToStdErr();
    throw InterpreterError();
  }
#ifdef SCANNER_DEBUG
  debugPrint("Here are the tokens the scanner recognized:");
  for (auto& token : tokensVec) debugPrint(token.toString());
#endif  // SCANNER_DEBUG

  return tokensVec;
}

auto parse(const std::vector<Token>& tokenVec)
    -> std::vector<AST::StmtPtrVariant> {
  ErrorReporter eReporter;
  RDParser parser(tokenVec, eReporter);

  std::vector<AST::StmtPtrVariant> statements = parser.parse();

  if (eReporter.getStatus() != LoxStatus::OK) {
    eReporter.printToStdErr();
    throw InterpreterError();
  }

#ifdef PARSER_DEBUG
  if (!statements.empty()) {
    AST::PrettyPrinter printer(statements);
    debugPrint("Here's the AST that was generated:");
    for (const auto& str : printer.toString()) debugPrint(str);
  } else {
    debugPrint("Parser returned no valid statements.");
  }
#endif  // PARSER_DEBUG

  return statements;
}

}  // namespace

void InterpreterDriver::interpret(const std::string& source) {
  try {
    evaluator.evaluate(parse(scan(source)));
    if (eReporter.getStatus() != LoxStatus::OK) {
      eReporter.printToStdErr();
    }
  } catch (const InterpreterError& e) {
    hadError = true;
    return;
  } catch (const ErrorsAndDebug::RuntimeError& e) {
    hadRunTimeError = true;
    if (eReporter.getStatus() != LoxStatus::OK) {
      eReporter.printToStdErr();
    }
    return;
  }
}

InterpreterDriver::InterpreterDriver() : eReporter(), evaluator(eReporter) {}

}  // namespace cpplox