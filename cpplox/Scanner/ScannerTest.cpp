#include "gtest/gtest.h"

#include <string>
#include <vector>

#include "cpplox/ErrorsAndDebug/ErrorReporter.h"
#include "cpplox/Scanner/Scanner.h"
#include "cpplox/Types/Token.h"

namespace cpplox {

TEST(ScannerTests, all_valid_tokens) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source
      = "( ) { } , . - + ; / * ! != = == > >= < <= arg1 \"string\" 1 and class "
        "else false fun "
        "for if nil or print return super this true var while";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();

  std::vector<Types::TokenType> expected = {TokenType::LEFT_PAREN,
                                            TokenType::RIGHT_PAREN,
                                            TokenType::LEFT_BRACE,
                                            TokenType::RIGHT_BRACE,
                                            TokenType::COMMA,
                                            TokenType::DOT,
                                            TokenType::MINUS,
                                            TokenType::PLUS,
                                            TokenType::SEMICOLON,
                                            TokenType::SLASH,
                                            TokenType::STAR,
                                            TokenType::BANG,
                                            TokenType::BANG_EQUAL,
                                            TokenType::EQUAL,
                                            TokenType::EQUAL_EQUAL,
                                            TokenType::GREATER,
                                            TokenType::GREATER_EQUAL,
                                            TokenType::LESS,
                                            TokenType::LESS_EQUAL,
                                            TokenType::IDENTIFIER,
                                            TokenType::STRING,
                                            TokenType::NUMBER,
                                            TokenType::AND,
                                            TokenType::CLASS,
                                            TokenType::ELSE,
                                            TokenType::FALSE,
                                            TokenType::FUN,
                                            TokenType::FOR,
                                            TokenType::IF,
                                            TokenType::NIL,
                                            TokenType::OR,
                                            TokenType::PRINT,
                                            TokenType::RETURN,
                                            TokenType::SUPER,
                                            TokenType::THIS,
                                            TokenType::TRUE,
                                            TokenType::VAR,
                                            TokenType::WHILE,
                                            TokenType::LOX_EOF};
  auto expectedIter = expected.begin();
  for (auto &token : tokensVec) {
    ASSERT_EQ(token.getType(), *expectedIter);
    ++expectedIter;
  }
}

TEST(ScannerTests, empty_buffer) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(eReporter.getStatus(), ErrorsAndDebug::LoxStatus::OK);
}

TEST(ScannerTests, unterminated_string) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "\"";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(eReporter.getStatus(), ErrorsAndDebug::LoxStatus::ERROR);
}

TEST(ScannerTests, unexpected_char) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "foo(a | b);";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(eReporter.getStatus(), ErrorsAndDebug::LoxStatus::ERROR);
}

TEST(ScannerTests, single_line_comment) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "// foo(a | b);";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(tokensVec.size(), 1);
  auto tokenIter = tokensVec.front();
  ASSERT_EQ(tokenIter.getType(), Types::TokenType::LOX_EOF);
}

TEST(ScannerTests, block_comment) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "/* foo(a | b); */";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(tokensVec.size(), 1);
  auto tokenIter = tokensVec.front();
  ASSERT_EQ(tokenIter.getType(), Types::TokenType::LOX_EOF);
}

TEST(ScannerTests, nested_block_comment) {
  cpplox::ErrorsAndDebug::ErrorReporter eReporter;
  std::string source = "/* foo(a /* | */ b); */";
  cpplox::Scanner scanner(source, eReporter);
  std::vector<Types::Token> tokensVec = scanner.tokenize();
  ASSERT_EQ(tokensVec.size(), 1);
  auto tokenIter = tokensVec.front();
  ASSERT_EQ(tokenIter.getType(), Types::TokenType::LOX_EOF);
}

}  // namespace cpplox