#ifndef SCANNER_H
#define SCANNER_H
#pragma once

#include <list>
#include <string>

namespace cpplox {
namespace ErrorsAndDebug {
class ErrorReporter;
}

namespace Types {
class Token;
enum class TokenType;
}  // namespace Types

using ErrorsAndDebug::ErrorReporter;
using Types::Token;
using Types::TokenType;

class Scanner {
 public:
  Scanner(const std::string &p_source, std::list<Token> &p_tokens,
          ErrorReporter &p_eReporter);

  void tokenize();

 private:
  bool isAtEnd();
  void tokenizeOne();
  void advance();
  bool matchNext(char expected);
  char peek();
  char peekNext();
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