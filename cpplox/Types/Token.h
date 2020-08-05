#ifndef TYPES_TOKEN_H
#define TYPES_TOKEN_H
#include <optional>
#pragma once

#include <string>

#include "cpplox/Types/Literal.h"

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

class Token {
 public:
  Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
        int p_line);

  Token(TokenType p_type, const char* p_lexeme, OptionalLiteral p_literal,
        int p_line);

  Token(TokenType p_type, const char* p_lexeme);

  auto toString() -> std::string;
  auto getType() -> TokenType;
  auto getLexeme() -> std::string;

 private:
  TokenType type;
  const std::string lexeme;
  OptionalLiteral literal = std::nullopt;
  int line = 1;
};  // class Token

}  // namespace cpplox::Types
#endif  // TYPES_TOKEN_H