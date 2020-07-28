#ifndef TOKEN_H
#define TOKEN_H
#pragma once

#include <optional>
#include <string>
#include <variant>

namespace cpplox {
using Literal = std::variant<std::string, double>;
using OptionalLiteral = std::optional<Literal>;

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

const std::string TokenTypeString(const TokenType value) {
  static const std::string lookUpTable[] = {
      "LEFT_PAREN", "RIGHT_PAREN",   "LEFT_BRACE", "RIGHT_BRACE", "COMMA",
      "DOT",        "MINUS",         "PLUS",       "SEMICOLON",   "SLASH",
      "STAR",       "BANG",          "BANG_EQUAL", "EQUAL",       "EQUAL_EQUAL",
      "GREATER",    "GREATER_EQUAL", "LESS",       "LESS_EQUAL",  "IDENTIFIER",
      "STRING",     "NUMBER",        "AND",        "CLASS",       "ELSE",
      "FALSE",      "FUN",           "FOR",        "IF",          "NIL",
      "OR",         "PRINT",         "RETURN",     "SUPER",       "THIS",
      "TRUE",       "VAR",           "WHILE",      "EOF"};
  return lookUpTable[static_cast<int>(value)];
}

struct Token {
  Token(TokenType p_type, std::string p_lexeme, OptionalLiteral p_literal,
        int p_line)
      : type(p_type), lexeme(p_lexeme), literal(p_literal), line(p_line);

  std::string toString() {
    std::string result
        = to_string(line) + " " + TokenTypeString(type) + " " + lexeme;
    if (literal.has_value())
      result += " " + literal.value().visit([](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>)
          return arg;
        else if constexpr (std::is_same_v<T, double>)
          return to_string(arg);
        else
          static_assert(always_false_v<T>,
                        "Looks like you forgot to update this visitor!");
      });
    return result;
  }

  TokenType type;
  std::string lexeme;
  OptionalLiteral literal;
  int line;
};

}  // namespace cpplox
#endif  // TOKEN_H