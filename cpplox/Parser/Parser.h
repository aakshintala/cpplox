#ifndef CPPLOX_PARSER_PARSER_H
#define CPPLOX_PARSER_PARSER_H
#pragma once

#include <exception>
#include <functional>
#include <iterator>
#include <vector>

#include "cpplox/AST/NodeTypes.h"
#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Types/Token.h"

// This is a recursive descent parser for the lox language.
// Currently it only parses expressions.

// clang-format off
// Grammar production rules:
// program     → declaration* LOX_EOF;
// declaration → varDecl | funcDecl | classDecl | statement;
// varDecl     → "var" IDENTIFIER ("=" expression)? ";" ;
// classDecl   → "class" IDENTIFIER ("<" IDENTIFIER)? "{" funcDecl* "}" ;
// funcDecl    → IDENTIFIER funcBody;
// funcBody     → "(" parameters? ")" "{" declaration "}";
// statement   → exprStmt | printStmt | blockStmt | ifStmt | whileStmt |
// statement   → forStmt | returnStmt;
// exprStmt    → expression ';' ;
// printStmt   → "print" expression ';' ;
// blockStmt   → "{" declaration "}"
// ifStmt      → "if" "(" expression ")" statement ("else" statement)? ;
// whileStmt   → "while" "(" expression ")" statement;
// forStmt     → "for" "(" varDecl | exprStmnt | ";"
//                          expression? ";"
//                          expression? ")" statement;
// returnStmt  → "return" (expression)? ";";
// expression  → comma;
// comma       → assignment ("," assignment)*;
// arguments   → assignment  ( "," assignment )* ;
// assignment  → (call ".")? IDENTIFIER "=" assignment | condititional;
// conditional → logical_or ("?" expression ":" conditional)?;
// logical_or  → logical_and ("or" logical_and)*;
// logical_and → equality ("and" equality)*;
// equality    → comparison(("!=" | "==") comparison) *;
// comparison  → addition((">" | ">=" | "<" | "<=") addition) *;
// addition    → multiplication(("-" | "+") multiplication) *;
// multipli... → unary(("/" | "*") unary) *;
// unary       → ("!" | "-" | "--" | "++") unary | postfix;
// postfix     → call ("++" | "--")*;
// call        → primary ( "(" arguments? ")" | "." IDENTIFIER )*;
// primary     → NUMBER | STRING | "false" | "true" | "nil";
// primary     → "(" expression ")";
// primary     → IDENTIFIER;
// primary     → "fun" funcBody;
// primary     → "super" "." IDENTIFIER;
// Error Productions:
// primary     → ("!=" | "==") equality;
// primary     → (">" | ">=" | "<" | "<=") comparison;
// primary     → ("+")addition;
// primary     → ("/" | "*") multiplication;
// clang-format on

namespace cpplox::Parser {
using AST::ExprPtrVariant;
using AST::StmtPtrVariant;

class RDParser {
 public:
  explicit RDParser(const std::vector<Types::Token>& tokens,
                    ErrorsAndDebug::ErrorReporter& eReporter);

  class RDParseError : std::exception {};  // Exception types

  // The public method to kick off parsing.
  // Stands in for "program" in the grammar
  // Currently it's mainly used to catch any exceptions that may be produced
  // (e.g., RDParseError) and deal with them.
  auto parse() -> std::vector<StmtPtrVariant>;

 private:
  // Grammar parsing functions
  // Statment parsing
  void program();
  auto declaration() -> std::optional<StmtPtrVariant>;
  auto varDecl() -> StmtPtrVariant;
  auto classDecl() -> StmtPtrVariant;
  auto funcDecl(const std::string& kind) -> StmtPtrVariant;
  auto funcBody(const std::string& kind) -> ExprPtrVariant;
  auto parameters() -> std::vector<Types::Token>;
  auto statement() -> StmtPtrVariant;
  auto exprStmt() -> StmtPtrVariant;
  auto printStmt() -> StmtPtrVariant;
  auto blockStmt() -> StmtPtrVariant;
  auto ifStmt() -> StmtPtrVariant;
  auto whileStmt() -> StmtPtrVariant;
  auto forStmt() -> StmtPtrVariant;
  auto returnStmt() -> StmtPtrVariant;

  // Expression Parsing
  auto expression() -> ExprPtrVariant;
  auto comma() -> ExprPtrVariant;
  auto assignment() -> ExprPtrVariant;
  auto conditional() -> ExprPtrVariant;
  auto logical_or() -> ExprPtrVariant;
  auto logical_and() -> ExprPtrVariant;
  auto equality() -> ExprPtrVariant;
  auto comparison() -> ExprPtrVariant;
  auto addition() -> ExprPtrVariant;
  auto multiplication() -> ExprPtrVariant;
  auto unary() -> ExprPtrVariant;
  auto postfix() -> ExprPtrVariant;
  auto call() -> ExprPtrVariant;
  auto arguments() -> std::vector<ExprPtrVariant>;
  auto primary() -> ExprPtrVariant;

  // Helper functions to implement the parser
  void advance();
  void consumeOrError(Types::TokenType tType, const std::string& errorMessage);
  using parserFn = ExprPtrVariant (RDParser::*)();
  auto consumeAnyBinaryExprs(
      const std::initializer_list<Types::TokenType>& types, ExprPtrVariant expr,
      const parserFn& f) -> ExprPtrVariant;
  auto consumeOneLiteral() -> ExprPtrVariant;
  auto consumeOneLiteral(const std::string& str) -> ExprPtrVariant;
  auto consumeGroupingExpr() -> ExprPtrVariant;
  auto consumePostfixExpr(ExprPtrVariant expr) -> ExprPtrVariant;
  void consumeSemicolonOrError();
  auto consumeSuper() -> ExprPtrVariant;
  auto consumeUnaryExpr() -> ExprPtrVariant;
  auto consumeVarExpr() -> ExprPtrVariant;
  auto error(const std::string& eMessage) -> RDParseError;
  [[nodiscard]] auto getCurrentTokenType() const -> Types::TokenType;
  auto getTokenAndAdvance() -> Types::Token;
  [[nodiscard]] auto isAtEnd() const -> bool;
  [[nodiscard]] auto match(
      const std::initializer_list<Types::TokenType>& types) const -> bool;
  [[nodiscard]] auto match(Types::TokenType type) const -> bool;
  [[nodiscard]] auto matchNext(Types::TokenType type) -> bool;
  [[nodiscard]] auto peek() const -> Types::Token;
  void reportError(const std::string& message);
  void synchronize();
  void throwOnErrorProduction(
      const std::initializer_list<Types::TokenType>& types, const parserFn& f);
  void throwOnErrorProductions();

  // The data the parser operates on.
  const std::vector<Types::Token>& tokens;
  std::vector<cpplox::Types::Token>::const_iterator currentIter;
  // std::__wrap_iter<std::vector<cpplox::Types::Token>::const_pointer>
  // currentIter;
  ErrorsAndDebug::ErrorReporter& eReporter;
  std::vector<StmtPtrVariant> statements;

  static const int MAX_ARGS = 255;

};  // class RDParser

}  // namespace cpplox::Parser
#endif  // CPPLOX_PARSER_PARSER_H
