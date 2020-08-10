#ifndef CPPLOX_PARSER_PARSER_H
#define CPPLOX_PARSER_PARSER_H
#pragma once

#include <exception>
#include <iterator>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/ErrorsAndDebug/DebugPrint.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Token.h"

// This is a recursive descent parser for the lox language.
// Currently it only parses expressions.

// Grammar production rules:
// expression → comma;
// comma → conditional ("," conditional)*
// conditional → equality ("?" expression ":" conditional)?
// equality   → comparison(("!=" | "==") comparison) *;
// comparison → addition((">" | ">=" | "<" | "<=") addition) *;
// addition   → multiplication(("-" | "+") multiplication) *;
// multi...   → unary(("/" | "*") unary) *;
// unary      → ("!" | "-" | "--" | "++") unary | postfix;
// postfix    → primary ("++" | "--")*;
// primary    → NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")";
// Error Productions:
// primary    → ("!=" | "==") equality
// primary    → (">" | ">=" | "<" | "<=") comparison
// primary    → ("+")addition
// primary    → ("/" | "*") multiplication;

namespace cpplox::Parser {
using AST::ExprPtrVariant;
class RDParser {
 public:
  explicit RDParser(const std::vector<Types::Token>& tokens,
                    ErrorsAndDebug::ErrorReporter& eReporter);

  class RDParseError : std::exception {};  // Exception types

  // The public method to kick off parsing.
  // Currently it's mainly used to catch any exceptions that may be produced
  // (e.g., RDParseError) and deal with them.
  auto parse() -> std::optional<ExprPtrVariant>;

 private:
  auto expression() -> ExprPtrVariant;
  auto comma() -> ExprPtrVariant;
  auto conditional() -> ExprPtrVariant;
  auto equality() -> ExprPtrVariant;
  auto comparison() -> ExprPtrVariant;
  auto addition() -> ExprPtrVariant;
  auto multiplication() -> ExprPtrVariant;
  auto unary() -> ExprPtrVariant;
  auto postfix() -> ExprPtrVariant;
  auto primary() -> ExprPtrVariant;

  // Helper functions to implement the parser
  void advance();
  void consumeOrError(TokenType tType, const std::string& errorMessage);
  using parserFn = ExprPtrVariant (RDParser::*)();
  auto consumeOneOrMoreBinaryExpr(
      const std::initializer_list<Types::TokenType>& types,
      const ExprPtrVariant& expr, const parserFn& f) -> ExprPtrVariant;
  auto consumeOneLiteral() -> ExprPtrVariant;
  auto consumeOneLiteral(const std::string& str) -> ExprPtrVariant;
  auto consumeGroupingExpr() -> ExprPtrVariant;
  auto consumePostfixExpr(ExprPtrVariant expr) -> ExprPtrVariant;
  auto consumeUnaryExpr() -> ExprPtrVariant;
  auto error(const std::string& eMessage) -> RDParseError;
  [[nodiscard]] auto getCurrentTokenType() const -> TokenType;
  auto getTokenAndAdvance() -> Token;
  [[nodiscard]] auto isAtEnd() const -> bool;
  [[nodiscard]] auto match(
      const std::initializer_list<Types::TokenType>& types) const -> bool;
  [[nodiscard]] auto match(Types::TokenType type) const -> bool;
  [[nodiscard]] auto peek() const -> Token;
  void synchronize();
  void throwOnErrorProduction(
      const std::initializer_list<Types::TokenType>& types, const parserFn& f);
  void throwOnErrorProductions();

  // The data the parser operates on.
  const std::vector<Types::Token>& tokens;
  std::__wrap_iter<std::vector<cpplox::Types::Token>::const_pointer>
      currentIter;
  ErrorsAndDebug::ErrorReporter& eReporter;

};  // class RDParser

}  // namespace cpplox::Parser
#endif  // CPPLOX_PARSER_PARSER_H