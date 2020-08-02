#ifndef TOKEN_H
#define TOKEN_H
#pragma once

#include <optional>
#include <string>
#include <variant>

namespace cpplox::Types {

enum class TokenType {
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

struct DoubleLiteral {
  double value;
  std::string stringLiteral;
};

using Literal = std::variant<std::string, DoubleLiteral>;
using OptionalLiteral = std::optional<Literal>;

class Token {
 public:
  Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
        int p_line);

  auto toString() -> std::string;

 private:
  TokenType type;
  const std::string lexeme;
  OptionalLiteral literal;
  int line;
};

}  // namespace cpplox::Types

#endif  // TOKEN_H