#include "Scanner.h"

namespace cpplox {

void Scanner::addToken(TokenType t) { addToken(t, std::nullopt); }

void Scanner::addToken(TokenType t, OptionalLiteral literal) {
  std::string lexeme = source.substr(start, current - start + 1);
  tokens.emplace_back(t, lexeme, literal, line);
}

char Scanner::advance() { return source[current++]; }

void Scanner::eatComment() {
  while (!isAtEnd() && peek() != '\n') advance();
}

void Scanner::eatIdentifier() {
  while (isAlphaNumeric(peek())) advance();

  std::string str = source.substr(start, current - start + 1);
  OptionalLiteral ol(std::in_place, str);
  addToken(ReservedOrIdentifier(str), ol);
}

void Scanner::eatNumber() {
  while (isDigit(peek())) advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) advance();
  }

  std::string str = source.substr(start, current - start + 1);
  // Literal l(stod(str));
  OptionalLiteral ol(std::in_place, stod(str));
  addToken(TokenType::NUMBER, ol);
}

void Scanner::eatString() {
  while (!isAtEnd() && peek() != '"') {
    if (peek() == '\n') ++line;
    advance();
  }

  if (isAtEnd()) {
    eReporter.setError(line, "Unterminated String!");
  } else {
    advance();  // consume the closing quote '"'

    // The offsets used below are different from the ones in the other
    // functions because we want to get rid of the enclosing quotation marks
    std::string str = source.substr(start + 1, current - start);
    OptionalLiteral ol(std::in_place, str);
    addToken(TokenType::STRING, ol);
  }
}

bool Scanner::isAtEnd() { return current >= source.size(); }

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'a' && c <= 'z') || (c == '_');
}

bool Scanner::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

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

void Scanner::tokenize() {
  while (!isAtEnd()) {
    start = current;
    tokenizeOne();
  }
  tokens.emplace_back(TokenType::LOX_EOF, "", std::nullopt, line);
}

void Scanner::tokenizeOne() {
  char c = advance();
  switch (c) {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*': addToken(TokenType::STAR); break;
    case '!':
      addToken(matchNext('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      addToken(matchNext('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '>':
      addToken(matchNext('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '<':
      addToken(matchNext('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '/': matchNext('/') ? eatComment() : addToken(TokenType::SLASH); break;
    case ' ':
    case '\t':
    case '\r': break;  // whitespace
    case '\n': ++line; break;
    case '"': eatString(); break;
    default:
      if (isDigit(c))
        eatNumber();
      else if (isAlpha(c))
        eatIdentifier();
      else
        eReporter.setError(line, "Unexpected character: " + std::to_string(c));
      break;
  }
}

TokenType Scanner::ReservedOrIdentifier(const std::string& str) {
  static const std::map<std::string, TokenType> lookUpTable{
      {"AND", TokenType::AND},       {"CLASS", TokenType::CLASS},
      {"ELSE", TokenType::ELSE},     {"FALSE", TokenType::FALSE},
      {"FUN", TokenType::FUN},       {"FOR", TokenType::FOR},
      {"IF", TokenType::IF},         {"NIL", TokenType::NIL},
      {"OR", TokenType::OR},         {"PRINT", TokenType::PRINT},
      {"RETURN", TokenType::RETURN}, {"SUPER", TokenType::SUPER},
      {"THIS", TokenType::THIS},     {"TRUE", TokenType::TRUE},
      {"VAR", TokenType::VAR},       {"WHILE", TokenType::WHILE}};

  auto iter = lookUpTable.find(str);
  if (iter == lookUpTable.end())
    return TokenType::IDENTIFIER;
  else
    return iter->second;
}

}  // namespace cpplox