#ifndef SCANNER_H
#define SCANNER_H
#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "StatusCodes.h"
#include "Token.h"

namespace cpplox {

class Scanner {
 public:
  Scanner(const std::string &p_source, std::list<Token> &p_tokens,
          ErrorReporter &p_eReporter)
      : source(p_source), tokens(p_tokens), eReporter(p_eReporter) {}

  void tokenize();

 private:
  bool isAtEnd();
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);
  bool isDigit(char c);
  void tokenizeOne();
  char advance();
  bool matchNext(char expected);
  char peek();
  char peekNext();
  void eatComment();
  void eatIdentifier();
  void eatNumber();
  void eatString();
  void addToken(TokenType t);
  void addToken(TokenType t, OptionalLiteral literal);

  static TokenType ReservedOrIdentifier(const std::string &str);

  const std::string &source;
  std::list<Token> &tokens;
  ErrorReporter &eReporter;
  int start = 0;
  int current = 0;
  int line = 1;
};

}  // namespace cpplox

#endif  // SCANNER_H