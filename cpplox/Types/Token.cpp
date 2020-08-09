#include "cpplox/Types/Token.h"
#include "cpplox/Types/Literal.h"

#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace cpplox::Types {

namespace {
auto TokenTypeString(const TokenType value) -> std::string {
  static const std::map<TokenType, std::string> lookUpTable{
      {TokenType::LEFT_PAREN, "LEFT_PAREN"},
      {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
      {TokenType::LEFT_BRACE, "LEFT_BRACE"},
      {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
      {TokenType::COMMA, "COMMA"},
      {TokenType::COLON, "COLON"},
      {TokenType::DOT, "DOT"},
      {TokenType::QUESTION, "QUESTION"},
      {TokenType::SEMICOLON, "SEMICOLON"},
      {TokenType::SLASH, "SLASH"},
      {TokenType::STAR, "STAR"},
      {TokenType::BANG, "BANG"},
      {TokenType::BANG_EQUAL, "BANG_EQUAL"},
      {TokenType::EQUAL, "EQUAL"},
      {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
      {TokenType::GREATER, "GREATER"},
      {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
      {TokenType::LESS, "LESS"},
      {TokenType::LESS_EQUAL, "LESS_EQUAL"},
      {TokenType::MINUS, "MINUS"},
      {TokenType::MINUS_MINUS, "MINUS_MINUS"},
      {TokenType::PLUS, "PLUS"},
      {TokenType::PLUS_PLUS, "PLUS_PLUS"},
      {TokenType::IDENTIFIER, "IDENTIFIER"},
      {TokenType::STRING, "STRING"},
      {TokenType::NUMBER, "NUMBER"},
      {TokenType::AND, "AND"},
      {TokenType::CLASS, "CLASS"},
      {TokenType::ELSE, "ELSE"},
      {TokenType::FALSE, "FALSE"},
      {TokenType::FUN, "FUN"},
      {TokenType::FOR, "FOR"},
      {TokenType::IF, "IF"},
      {TokenType::NIL, "NIL"},
      {TokenType::OR, "OR"},
      {TokenType::PRINT, "PRINT"},
      {TokenType::RETURN, "RETURN"},
      {TokenType::SUPER, "SUPER"},
      {TokenType::THIS, "THIS"},
      {TokenType::TRUE, "TRUE"},
      {TokenType::VAR, "VAR"},
      {TokenType::WHILE, "WHILE"},
      {TokenType::LOX_EOF, "EOF"}};

  return lookUpTable.find(value)->second;
}

}  // namespace

Token::Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
             int p_line)
    : type(p_type),
      lexeme(std::move(p_lexeme)),
      literal(std::move(p_literal)),
      line(p_line) {}

Token::Token(TokenType p_type, const char* p_lexeme, OptionalLiteral p_literal,
             int p_line)
    : type(p_type),
      lexeme(p_lexeme),
      literal(std::move(p_literal)),
      line(p_line) {}

Token::Token(TokenType p_type, const char* p_lexeme)
    : type(p_type), lexeme(p_lexeme) {}

auto Token::toString() const -> std::string {
  std::string result
      = std::to_string(line) + " " + TokenTypeString(type) + " " + lexeme + " ";
  result
      += literal.has_value() ? getLiteralString(literal.value()) : "No Literal";
  return result;
}

auto Token::getType() const -> TokenType { return this->type; }
auto Token::getTypeString() const -> std::string {
  return TokenTypeString(this->type);
}
auto Token::getLexeme() const -> std::string { return this->lexeme; };
auto Token::getOptionalLiteral() const -> OptionalLiteral {
  return this->literal;
}
auto Token::getLine() const -> int { return this->line; }

}  // namespace cpplox::Types
