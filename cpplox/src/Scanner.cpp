#include <map>
#include <string>
#include <vector>

#include "Scanner.h"
#include "StatusCodes.h"

namespace cpplox {

using TokenType;

int Scanner::tokenize() {
  while (!isAtEnd()) {
    start = current;
    tokenizeOne();
  }
  tokens.emplace_back(EOF, "", std::nullopt_t, line);
  return LoxStatus::OK;  // Replace with a real ErrorMessage class.
}

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
    case '!': addToken(matchNext('=') ? BANG_EQUAL : BANG); break;
    case '=': addToken(matchNext('=') ? EQUAL_EQUAL : EQUAL); break;
    case '>': addToken(matchNext('=') ? GREATER_EQUAL : GREATER); break;
    case '<': addToken(matchNext('=') ? LESS_EQUAL : LESS); break;
    // comment?
    case '/': matchNext('/') ? eatComment() : addToken(SLASH); break;
    // whitespace
    case ' ':
    case '\t':
    case '\r': break;
    // newline
    case '\n': ++line; break;

    case '"': eatString(); break;

    default:
      if (isDigit(c))
        eatNumber();
      else
        eReporter.error(line, "Unexpected character: " + c + "\n");
      break;
  }
}

void Scanner::addToken(TokenType t) { addToken(t, std::nullopt_t); }

void Scanner::addToken(TokenType t, optionalLiteral &literal) {
  std::string lexeme = source.substr(start, current - start + 1);
  tokens.emplace_back(t, lexeme, literal, line);
}

char Scanner::advance() { return source[current++]; }

void Scanner::eatComment() {
  while (!isAtEnd() && peek() != '\n') advance();
}

void Scanner::eatNumber() {}

void Scanner::eatString() {
  while (!isAtEnd && peek() != '"') {
    if (peek() == '\n') ++line;
    advance();
  }

  if (isAtEnd()) {
    eReporter.error(line, "Unterminated String!");
  } else {
    advance();  // consume the remaining '"'

    OptionalLiteral ol(std::in_place,
                       source.substr(start + 1, current - start));
    addToken(STRING, ol)
  }
}

void Scanner::eatNumbers() {
  void eatDigits() {
    while (isDigit(peek())) advance();
  }

  eatDigits();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    eatDigits();
  }

  // Literal l(stod(source.substr(start, current - start + 1)));
  OptionalLiteral ol(std::in_place,
                     stod(source.substr(start, current - start + 1)));
  addToken(NUMBER, ol);
}

bool Scanner::isAtEnd() { return current >= source.size(); }

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Scanner::matchNext(char expected) {
  if (!isAtEnd() && source[current] == expected) {
    ++current;
    return true;
  }
  return false;
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return source[current];
}

char Scanner::peekNext() {
  if ((current + 1) >= source.size()) return '\0';
  return source[current + 1];
}

}  // namespace cpplox