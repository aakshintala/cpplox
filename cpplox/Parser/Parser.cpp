#include "cpplox/Parser/Parser.h"

#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace cpplox::Parser {

using Types::Token;
using Types::TokenType;

RDParser::RDParser(const std::vector<Token>& p_tokens,
                   ErrorsAndDebug::ErrorReporter& eReporter)
    : tokens(p_tokens), eReporter(eReporter) {
  this->currentIter = this->tokens.begin();
}

// Helper functions
void RDParser::advance() {
  if (!isAtEnd()) ++currentIter;
}

auto RDParser::consumeOneOrMoreBinaryExpr(
    const std::initializer_list<Types::TokenType>& types,
    const ExprPtrVariant& expr, const parserFn& f) -> ExprPtrVariant {
  ExprPtrVariant result = expr;
  while (match(types)) {
    Token op = getTokenAndAdvance();
    ExprPtrVariant right = std::invoke(f, this);
    result = AST::createBinaryEPV(result, op, right);
  }
  return result;
}

void RDParser::consumeOrError(TokenType tType,
                              const std::string& errorMessage) {
  if (getCurrentTokenType() == tType) return advance();
  throw error(errorMessage);
}

auto RDParser::consumeOneLiteral(const std::string& str) -> ExprPtrVariant {
  advance();
  return AST::createLiteralEPV(str);
}

auto RDParser::consumeOneLiteral(const Token& token) -> ExprPtrVariant {
  advance();
  return AST::createLiteralEPV(token.getOptionalLiteral());
}

auto RDParser::error(const std::string& eMessage) -> RDParser::RDParseError {
  const Token& token = peek();
  std::string error = eMessage;
  if (token.getType() == TokenType::LOX_EOF)
    error = " at end: " + error;
  else
    error = " at '" + token.getLexeme() + "': " + error;
  eReporter.setError(token.getLine(), error);
  return RDParser::RDParseError();
}

auto RDParser::getCurrentTokenType() const -> TokenType {
  return currentIter->getType();
}

auto RDParser::getTokenAndAdvance() -> Token {
  Token token = peek();
  advance();
  return token;
}

void RDParser::handleErrorProduction(
    const std::initializer_list<Types::TokenType>& types, const parserFn& f) {
  if (match(types)) {
    auto errObj = error("Missing left hand operand");
    advance();
    ExprPtrVariant expr = std::invoke(f, this);
    throw errObj;
  }
}

void RDParser::handleErrorProductions() {
  handleErrorProduction({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL},
                        &RDParser::equality);
  handleErrorProduction({TokenType::GREATER, TokenType::GREATER_EQUAL,
                         TokenType::LESS, TokenType::LESS_EQUAL},
                        &RDParser::comparison);
  handleErrorProduction({TokenType::PLUS}, &RDParser::addition);
  handleErrorProduction({TokenType::STAR, TokenType::SLASH},
                        &RDParser::multiplication);
}

auto RDParser::isAtEnd() const -> bool {
  return peek().getType() == TokenType::LOX_EOF;
}

auto RDParser::match(Types::TokenType type) const -> bool {
  if (isAtEnd()) return false;
  return (type == getCurrentTokenType());
}

auto RDParser::match(const std::initializer_list<Types::TokenType>& types) const
    -> bool {
  bool result = false;
  for (const auto& type : types) result = (result || match(type));
  return result;
}

auto RDParser::peek() const -> Token { return *currentIter; }

void RDParser::synchronize() {
  while (!isAtEnd()) {
    switch (getCurrentTokenType()) {
      case TokenType::SEMICOLON: advance(); return;
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN: return;
      default: advance();
    }
  }
}

// ---------------- Grammar Production Rules -----------------------------------
// This is a recursive descent parser.
// The grammar for Lox is declared above each of the functions.

// expression → comma;
auto RDParser::expression() -> ExprPtrVariant { return comma(); }

// !This is going to cause some grief when we are looking at function arguments;
// !fun(1,2) will be treated as fun((1,2)) <- a single argument instead of two.
// !Needs to make sure we set function arguments to have higher precedence than
// !the comma operator
// comma → conditional ("," conditional)*
auto RDParser::comma() -> ExprPtrVariant {
  ExprPtrVariant expr = conditional();
  expr = consumeOneOrMoreBinaryExpr({TokenType::COMMA}, expr,
                                    &RDParser::conditional);
  return expr;
}

// conditional → equality ("?" expression ":" conditional)?
auto RDParser::conditional() -> ExprPtrVariant {
  ExprPtrVariant expr = equality();
  if (match(TokenType::QUESTION)) {
    Token op = getTokenAndAdvance();
    ExprPtrVariant thenBranch = expression();
    consumeOrError(TokenType::COLON, "Expected a colon after ternary operator");
    ExprPtrVariant elseBranch = conditional();
    expr = AST::createConditionalEPV(expr, thenBranch, elseBranch);
  }
  return expr;
}

// equality   → comparison(("!=" | "==") comparison) *;
auto RDParser::equality() -> ExprPtrVariant {
  ExprPtrVariant expr = comparison();
  auto equalityTypes = {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL};
  expr = consumeOneOrMoreBinaryExpr(equalityTypes, expr, &RDParser::comparison);
  return expr;
}

// comparison → addition((">" | ">=" | "<" | "<=") addition) *;
auto RDParser::comparison() -> ExprPtrVariant {
  ExprPtrVariant expr = addition();
  auto comparatorTypes = {TokenType::GREATER, TokenType::GREATER_EQUAL,
                          TokenType::LESS, TokenType::LESS_EQUAL};
  expr = consumeOneOrMoreBinaryExpr(comparatorTypes, expr, &RDParser::addition);
  return expr;
}

// addition   → multiplication(("-" | "+") multiplication) *;
auto RDParser::addition() -> ExprPtrVariant {
  ExprPtrVariant expr = multiplication();
  auto additionTypes = {TokenType::PLUS, TokenType::MINUS};
  expr = consumeOneOrMoreBinaryExpr(additionTypes, expr,
                                    &RDParser::multiplication);
  return expr;
}

// multi...   → unary(("/" | "*") unary) *;
auto RDParser::multiplication() -> ExprPtrVariant {
  ExprPtrVariant expr = unary();
  auto multTypes = {TokenType::SLASH, TokenType::STAR};
  expr = consumeOneOrMoreBinaryExpr(multTypes, expr, &RDParser::unary);
  return expr;
}

// unary      → ("!" | "-" | "--" | "++") unary | postfix;
auto RDParser::unary() -> ExprPtrVariant {
  auto unaryTypes = {TokenType::BANG_EQUAL, TokenType::MINUS,
                     TokenType::PLUS_PLUS, TokenType::MINUS_MINUS};
  if (match(unaryTypes)) {
    Token op = getTokenAndAdvance();
    ExprPtrVariant right = unary();
    return AST::createUnaryEPV(op, right);
  }
  return postfix();
}

// postfix    → primary ("++" | "--")*;
auto RDParser::postfix() -> ExprPtrVariant {
  ExprPtrVariant expr = primary();
  auto postfixTypes = {TokenType::PLUS_PLUS, TokenType::MINUS_MINUS};
  while (match(postfixTypes)) {
    Token op = getTokenAndAdvance();
    expr = AST::createPostfixEPV(expr, op);
  }
  return expr;
}

// primary    → NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")";
// Error Productions:
// primary    → ("!=" | "==") equality
// primary    → (">" | ">=" | "<" | "<=") comparison
// primary    → ("+")addition
// primary    → ("/" | "*") multiplication;
auto RDParser::primary() -> ExprPtrVariant {
  if (match(TokenType::FALSE)) return consumeOneLiteral("false");
  if (match(TokenType::TRUE)) return consumeOneLiteral("true");
  if (match(TokenType::NIL)) return consumeOneLiteral("null");
  if (match(TokenType::NUMBER)) return consumeOneLiteral(peek());
  if (match(TokenType::STRING)) return consumeOneLiteral(peek());
  if (match(TokenType::LEFT_PAREN)) {
    advance();
    ExprPtrVariant expr = expression();
    consumeOrError(TokenType::RIGHT_PAREN,
                   std::to_string(peek().getLine())
                       + " Expected a closing paren after expression.");
    return AST::createGroupingEPV(expr);
  }
  // Error Productions:
  handleErrorProductions();

  throw error("Expected an expression; Got something else.");
}  // namespace cpplox::Parser

auto RDParser::parse() -> std::optional<ExprPtrVariant> {
  try {
    return expression();
  } catch (const RDParseError& e) {
    return std::nullopt;
  }
}

}  // namespace cpplox::Parser