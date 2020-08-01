#include "Token.h"

#include <map>
#include <type_traits>

namespace cpplox {
namespace Types {

template <class T> inline constexpr bool always_false_v = false;

namespace {
const std::string TokenTypeString(const TokenType value) {
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
}  // namespace

Token::Token(TokenType p_type, const std::string p_lexeme,
             OptionalLiteral p_literal, int p_line)
    : type(p_type), lexeme(p_lexeme), literal(p_literal), line(p_line) {}

std::string Token::toString() {
  std::string result
      = std::to_string(line) + " " + TokenTypeString(type) + " " + lexeme + " ";
  if (literal.has_value())
    result += std::visit(
        [](auto& arg) -> std::string {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, std::string>)
            return arg;
          else if constexpr (std::is_same_v<T, DoubleLiteral>)
            return arg.stringLiteral;
          else
            static_assert(always_false_v<T>,
                          "Looks like you forgot to update this visitor!");
        },
        literal.value());
  else
    result += "No Literal";
  return result;
}

}  // namespace Types
}  // namespace cpplox
