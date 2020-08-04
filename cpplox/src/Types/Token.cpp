#include "Token.h"

#include <map>
#include <type_traits>
#include <utility>

namespace cpplox::Types {

template <class T> inline constexpr bool always_false_v = false;

namespace {
auto TokenTypeString(const TokenType value) -> std::string {
  static const std::map<TokenType, std::string> lookUpTable{
      {TokenType::LEFT_PAREN, "LEFT_PAREN"},
      {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
      {TokenType::LEFT_BRACE, "LEFT_BRACE"},
      {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
      {TokenType::COMMA, "COMMA"},
      {TokenType::DOT, "DOT"},
      {TokenType::MINUS, "MINUS"},
      {TokenType::PLUS, "PLUS"},
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

auto getLiteralString(Literal &value) -> std::string {
  // Literal = std::variant<std::string, DoubleLiteral>;
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
      break;
    case 1:  // DoubleLiteral
      return std::get<1>(value).stringLiteral;
      break;
    default:
      static_assert(
          std::variant_size_v<Literal> == 2,
          "Looks like you forgot to update the cases in getLiteralString()!");
  }
  return "";
}
}  // namespace

Token::Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
             int p_line)
    : type(p_type),
      lexeme(std::move(p_lexeme)),
      literal(std::move(p_literal)),
      line(p_line) {}

auto Token::toString() -> std::string {
  std::string result
      = std::to_string(line) + " " + TokenTypeString(type) + " " + lexeme + " ";
  result
      += literal.has_value() ? getLiteralString(literal.value()) : "No Literal";
  return result;
}

}  // namespace cpplox::Types
