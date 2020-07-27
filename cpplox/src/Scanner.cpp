#include <map>
#include <string>
#include <vector>

#include "Scanner.h"
#include "StatusCodes.h"

namespace cpplox {

int Scanner::tokenize() {
  while (!isAtEnd()) {
    start = current;
    tokenizeOne();
  }
  tokens.emplace_back(TokenType::EOF, "", std::nullopt_t, line);
  return LoxStatus::OK;  // Replace with a real ErrorMessage class.
}

bool Scanner::isAtEnd() { return (current >= source.size()); }

char Scanner::advance() { return source[current++]; }

void Scanner::tokenizeOne() {
  char c = advance();
  switch (c) {
    case '(': addToken(LEFT_PAREN); break;
    case ')': addToken(RIGHT_PAREN); break;
    case '{': addToken(LEFT_BRACE); break;
    case '}': addToken(RIGHT_BRACE); break;
    case ',': addToken(COMMA); break;
    case '.': addToken(DOT); break;
    case '-': addToken(MINUS); break;
    case '+': addToken(PLUS); break;
    case ';': addToken(SEMICOLON); break;
    case '*': addToken(STAR); break;
    default:
      assert(false);  // make an ErrorReporter class and pass that here.
                      // this is unusable
      break;
  }
}

void Scanner::addToken(TokenType t) { addToken(t, std::nullopt_t); }

void Scanner::addToken(TokenType t, optionalLiteral literal) {
  std::string lexeme = source.substr(start, current - start + 1);
  tokens.emplace_back(t, lexeme, literal, line);
}

}  // namespace cpplox