#ifndef TOKEN_H
#define TOKEN_H
#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <variant>

namespace cpplox {
using Literal = std::variant<std::string, double>;
using OptionalLiteral = std::optional<Literal>;

template <class T> inline constexpr bool always_false_v = false;

enum struct TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  LOX_EOF
};

struct Token {
  Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
        int p_line)
      : type(p_type), lexeme(p_lexeme), literal(p_literal), line(p_line) {}

  std::string toString() {
    std::string result = std::to_string(line) + " " + TokenTypeString(type)
                         + " " + lexeme + " ";
    if (literal.has_value())
      result += std::visit(
          [](auto& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>)
              return arg;
            else if constexpr (std::is_same_v<T, double>)
              return std::to_string(arg);
            else
              static_assert(always_false_v<T>,
                            "Looks like you forgot to update this visitor!");
          },
          literal.value());
    else
      result += "No Literal";
    return result;
  }

 private:
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

  TokenType type;
  std::string lexeme;
  OptionalLiteral literal;
  int line;
};

}  // namespace cpplox
#endif  // TOKEN_H