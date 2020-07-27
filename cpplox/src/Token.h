#ifndef TOKEN_H
#define TOKEN_H
#pragma once

#include <optional>
#include <string>
#include <variant>

using optionalLiteral as std::optional<std::variant<std::string, double>>;
namespace cpplox {

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

  EOF
};

struct Token {
  Token(TokenType p_type, std::string p_lexeme, optionalLiteral p_literal,
        int p_line)
      : type(p_type), lexeme(p_lexeme), literal(p_literal), line(p_line);

  std::string toString() {
    return to_string(type) + " " + lexeme + " " + to_string(literal);
  }

  TokenType type;
  std::string lexeme;
  optionalLiteral literal;
  int line;
};

}  // namespace cpplox

#endif  // TOKEN_H