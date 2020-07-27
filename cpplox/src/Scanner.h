#ifndef SCANNER_H
#define SCANNER_H
#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "Token.h"

namespace cpplox {

class Scanner {
 public:
  Scanner(std::string &p_source, std::list<Token> &p_tokens)
      : source(p_source), tokens(p_tokens);

  int tokenize();

 private:
  bool isAtEnd();
  void tokenizeOne();
  char advance();
  void addToken(TokenType t);
  void addToken(TokenType t, optionalLiteral literal);

  std::string source;
  std::list<Token> &tokens;
  int start = 0;
  int current = 0;
  int line = 1;
};

}  // namespace cpplox

#endif  // SCANNER_H