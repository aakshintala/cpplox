#ifndef SCANNER_H
#define SCANNER_H
#pragma once

#include <list>
#include <string>

#include "ErrorsAndDebug/ErrorReporter.h"
#include "Types/Token.h"

namespace cpplox {

using ErrorsAndDebug::ErrorReporter;
using Types::Token;
using Types::TokenType;

class Scanner {
 public:
  Scanner(const std::string &p_source, std::list<Token> &p_tokens,
          ErrorReporter &p_eReporter);

  void tokenize();

 private:
  auto isAtEnd() -> bool;
  void tokenizeOne();
  void advance();
  auto matchNext(char expected) -> bool;
  auto peek() -> char;
  auto peekNext() -> char;
  void skipComment();
  void skipBlockComment();
  void eatIdentifier();
  void eatNumber();
  void eatString();
  void addToken(TokenType t);

  const std::string &source;
  std::list<Token> &tokens;
  ErrorReporter &eReporter;
  int start = 0;
  int current = 0;
  int line = 1;
};

}  // namespace cpplox

#endif  // SCANNER_H