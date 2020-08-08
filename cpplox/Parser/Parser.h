#ifndef CPPLOX_PARSER_PARSER_H
#define CPPLOX_PARSER_PARSER_H
#pragma once

#include <exception>
#include <iterator>
#include <vector>

#include "cpplox/AST/Expr.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Token.h"

namespace cpplox::Parser {
using AST::ExprPtrVariant;
// This is a recursive descent parser for the lox language. The grammar this
// parser accepts is shown below.
class RDParser {
 public:
  explicit RDParser(const std::vector<Types::Token>& tokens,
                    ErrorsAndDebug::ErrorReporter& eReporter);

  class RDParseError : std::exception {};
  auto parse() -> std::optional<ExprPtrVariant>;

 private:
  // Exception type
  // Grammar production rules
  // expression → equality;
  auto expression() -> ExprPtrVariant;
  // equality   → comparison(("!=" | "==") comparison) *;
  auto equality() -> ExprPtrVariant;
  // comparison → addition((">" | ">=" | "<" | "<=") addition) *;
  auto comparison() -> ExprPtrVariant;
  // addition   → multiplication(("-" | "+") multiplication) *;
  auto addition() -> ExprPtrVariant;
  // multi...   → unary(("/" | "*") unary) *;
  auto multiplication() -> ExprPtrVariant;
  // unary      → ("!" | "-") unary | primary;
  auto unary() -> ExprPtrVariant;
  // primary    → NUMBER | STRING | "false" | "true" | "nil" | "(" expression
  // ")";
  auto primary() -> ExprPtrVariant;

  // Helper functions to implement the parser
  void advance();
  void consumeOrError(TokenType tType, const std::string& errorMessage);
  using parserFn = ExprPtrVariant (RDParser::*)();
  auto consumeOneOrMoreBinaryExpr(
      const std::initializer_list<Types::TokenType>& types,
      const ExprPtrVariant& expr, const parserFn& f) -> ExprPtrVariant;
  auto consumeOneLiteral(const std::string& str) -> ExprPtrVariant;
  auto consumeOneLiteral(const Token& token) -> ExprPtrVariant;
  auto error(const Token& token, const std::string&) -> RDParseError;
  [[nodiscard]] auto getCurrentTokenType() const -> TokenType;
  auto getTokenAndAdvance() -> Token;
  [[nodiscard]] auto isAtEnd() const -> bool;
  [[nodiscard]] auto match(
      const std::initializer_list<Types::TokenType>& types) const -> bool;
  [[nodiscard]] auto match(Types::TokenType type) const -> bool;
  [[nodiscard]] auto peek() const -> Token;
  void synchronize();

  // Private data
  const std::vector<Types::Token>& tokens;
  std::__wrap_iter<std::vector<cpplox::Types::Token>::const_pointer>
      currentIter;
  ErrorsAndDebug::ErrorReporter& eReporter;

};  // class RDParser

}  // namespace cpplox::Parser
#endif  // CPPLOX_PARSER_PARSER_H