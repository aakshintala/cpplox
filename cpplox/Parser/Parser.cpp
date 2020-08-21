#include "cpplox/Parser/Parser.h"

#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "cpplox/ErrorsAndDebug/DebugPrint.h"

namespace cpplox::Parser {

using Types::Token;
using Types::TokenType;

RDParser::RDParser(const std::vector<Token>& p_tokens,
                   ErrorsAndDebug::ErrorReporter& eReporter)
    : tokens(p_tokens), eReporter(eReporter) {
  this->currentIter = this->tokens.begin();
}

// Helper functions; Sorted by name
void RDParser::advance() {
  if (!isAtEnd()) ++currentIter;
}

auto RDParser::consumeAnyBinaryExprs(
    const std::initializer_list<TokenType>& types, ExprPtrVariant expr,
    const parserFn& f) -> ExprPtrVariant {
  while (match(types)) {
    Token op = getTokenAndAdvance();
    expr = AST::createBinaryEPV(std::move(expr), op, std::invoke(f, this));
  }
  return expr;
}

void RDParser::consumeOrError(TokenType tType,
                              const std::string& errorMessage) {
  if (getCurrentTokenType() == tType) return advance();
  throw error(errorMessage + " Got: " + peek().toString());
}

auto RDParser::consumeOneLiteral(const std::string& str) -> ExprPtrVariant {
  advance();
  return AST::createLiteralEPV(Types::makeOptionalLiteral(str));
}

auto RDParser::consumeOneLiteral() -> ExprPtrVariant {
  return AST::createLiteralEPV(getTokenAndAdvance().getOptionalLiteral());
}

auto RDParser::consumeGroupingExpr() -> ExprPtrVariant {
  advance();
  ExprPtrVariant expr = expression();
  consumeOrError(TokenType::RIGHT_PAREN,
                 std::to_string(peek().getLine())
                     + " Expected a closing paren after expression.");
  return AST::createGroupingEPV(std::move(expr));
}

auto RDParser::consumePostfixExpr(ExprPtrVariant expr) -> ExprPtrVariant {
  while (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS})) {
    expr = AST::createPostfixEPV(std::move(expr), getTokenAndAdvance());
  }
  return expr;
}

auto RDParser::consumeUnaryExpr() -> ExprPtrVariant {
  return AST::createUnaryEPV(getTokenAndAdvance(), unary());
}

auto RDParser::consumeVarExpr() -> ExprPtrVariant {
  Token varName = getTokenAndAdvance();
  return AST::createVariableEPV(varName);
}

void RDParser::consumeSemicolonOrError() {
  consumeOrError(TokenType::SEMICOLON, "Expected a ';'");
}

auto RDParser::error(const std::string& eMessage) -> RDParser::RDParseError {
  reportError(eMessage);
  return RDParser::RDParseError();
}

auto RDParser::getCurrentTokenType() const -> TokenType {
  return currentIter->getType();
}

auto RDParser::getTokenAndAdvance() -> Token {
  Token token = peek();
  advance();
  return token;
}

auto RDParser::getVarNameOrThrowError(const ExprPtrVariant& expr) -> Token {
  if (std::holds_alternative<AST::VariableExprPtr>(expr))
    return std::get<AST::VariableExprPtr>(expr)->varName;
  throw error("Invalid assignment target");
}

auto RDParser::isAtEnd() const -> bool {
  return peek().getType() == TokenType::LOX_EOF;
}

auto RDParser::match(Types::TokenType type) const -> bool {
  if (isAtEnd()) return false;
  return (type == getCurrentTokenType());
}

auto RDParser::match(const std::initializer_list<Types::TokenType>& types) const
    -> bool {
  bool result = false;
  for (const auto& type : types) result = (result || match(type));
  return result;
}

auto RDParser::matchNext(Types::TokenType type) -> bool {
  advance();
  bool result = match(type);
  --currentIter;
  return result;
}

auto RDParser::peek() const -> Token { return *currentIter; }

void RDParser::reportError(const std::string& message) {
  const Token& token = peek();
  std::string error = message;
  if (token.getType() == TokenType::LOX_EOF)
    error = " at end: " + error;
  else
    error = " at '" + token.getLexeme() + "': " + error;
  eReporter.setError(token.getLine(), error);
}

void RDParser::synchronize() {
  while (!isAtEnd()) {
    switch (getCurrentTokenType()) {
      case TokenType::SEMICOLON: advance(); return;
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN: return;
      default:
        ErrorsAndDebug::debugPrint("Discarding extranuous token:"
                                   + peek().getLexeme());
        advance();
    }
  }
}

void RDParser::throwOnErrorProduction(
    const std::initializer_list<Types::TokenType>& types, const parserFn& f) {
  if (match(types)) {
    auto errObj = error("Missing left hand operand");
    advance();
    ExprPtrVariant expr
        = std::invoke(f, this);  // We check the rest of the expression anyways
                                 // to see if there are any errors there
    throw errObj;
  }
}

void RDParser::throwOnErrorProductions() {
  throwOnErrorProduction({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL},
                         &RDParser::equality);
  throwOnErrorProduction({TokenType::GREATER, TokenType::GREATER_EQUAL,
                          TokenType::LESS, TokenType::LESS_EQUAL},
                         &RDParser::comparison);
  throwOnErrorProduction({TokenType::PLUS}, &RDParser::addition);
  throwOnErrorProduction({TokenType::STAR, TokenType::SLASH},
                         &RDParser::multiplication);
}

// ---------------- Grammar Production Rules
// ----------------------------------- This is a recursive descent parser. The
// grammar for Lox is declared above each of the functions. The functions are
// sorted based on lowest to highest precedence; program     → declaration*
// LOX_EOF;

void RDParser::program() {
  try {
    while (!isAtEnd()) {
      std::optional<StmtPtrVariant> optStmt = declaration();
      if (optStmt.has_value()) statements.push_back(std::move(optStmt.value()));
    }
  } catch (const std::exception& e) {
    std::string errorMessage = "Caught unhandled exception: ";
    errorMessage += e.what();
    eReporter.setError(peek().getLine(), errorMessage);
  }
}

// declaration → varDecl | funcDecl | statement;
auto RDParser::declaration() -> std::optional<StmtPtrVariant> {
  try {
    if (match(TokenType::VAR)) {
      advance();
      return varDecl();
    }

    if (match(TokenType::FUN) && matchNext(TokenType::IDENTIFIER)) {
      advance();
      return funcDecl("function");
    }

    return statement();

  } catch (const RDParseError& e) {
    ErrorsAndDebug::debugPrint(
        "Caught ParseError; Calling synchronize. Current Token:"
        + peek().toString());
    synchronize();
    return std::nullopt;
  }
}

// varDecl     → "var" IDENTIFIER ("=" expression)? ";" ;
auto RDParser::varDecl() -> StmtPtrVariant {
  if (match(TokenType::IDENTIFIER)) {
    Token varName = getTokenAndAdvance();
    std::optional<ExprPtrVariant> intializer = std::nullopt;
    if (match(TokenType::EQUAL)) {
      advance();
      intializer = expression();
    }
    consumeSemicolonOrError();
    return AST::createVarSPV(varName, std::move(intializer));
  }
  throw error("Expected a variable name after the var keyword");
}

// parameters → IDENTIFIER ( "," IDENTIFIER )* ;
auto RDParser::parameters() -> std::vector<Token> {
  std::vector<Token> params;
  do {
    if (peek().getType() != TokenType::IDENTIFIER)
      throw error("Expected an indentifier for parameter.");
    params.emplace_back(getTokenAndAdvance());
  } while ([this]() -> bool {
    if (match(TokenType::COMMA)) {
      advance();
      return true;
    }
    return false;
  }());
  return params;
}
// funcBody     → "(" parameters? ")" "{" declaration "}";
auto RDParser::funcBody(const std::string& kind) -> ExprPtrVariant {
  consumeOrError(TokenType::LEFT_PAREN, "Expecte '(' after " + kind + " decl.");
  // Grab any parameters for the function
  std::vector<Token> params
      = match(TokenType::RIGHT_PAREN) ? std::vector<Token>() : parameters();
  consumeOrError(TokenType::RIGHT_PAREN,
                 "Expecte ')' after " + kind + " params.");
  consumeOrError(TokenType::LEFT_BRACE,
                 "Expecte '{' after " + kind + " params.");
  std::vector<StmtPtrVariant> fnBody;
  while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    if (auto optStmnt = declaration(); optStmnt.has_value())
      fnBody.push_back(std::move(optStmnt.value()));
  }
  consumeOrError(TokenType::RIGHT_BRACE,
                 "Expecte '}' after " + kind + " body.");
  return AST::createFuncEPV(std::move(params), std::move(fnBody));
}

// funcDecl    → "fun" IDENTIFIER funcBody;
auto RDParser::funcDecl(const std::string& kind) -> StmtPtrVariant {
  if (match(TokenType::IDENTIFIER)) {
    return AST::createFuncSPV(getTokenAndAdvance(),
                              std::get<AST::FuncExprPtr>(funcBody(kind)));
  }
  throw error("Expected a " + kind + " name after the fun keyword");
}

// statement   → exprStmt | printStmt | blockStmt | ifStmt | whileStmt |
// statement   → forStmt;
auto RDParser::statement() -> StmtPtrVariant {
  if (match(TokenType::PRINT)) return printStmt();
  if (match(TokenType::LEFT_BRACE)) return blockStmt();
  if (match(TokenType::IF)) return ifStmt();
  if (match(TokenType::WHILE)) return whileStmt();
  if (match(TokenType::FOR)) return forStmt();
  if (match(TokenType::RETURN)) return returnStmt();
  return exprStmt();
}

// exprStmt    → expression ';' ;
auto RDParser::exprStmt() -> StmtPtrVariant {
  auto expr = expression();
  consumeSemicolonOrError();
  return AST::createExprSPV(std::move(expr));
}

// printStmt   → "print" expression ';' ;
auto RDParser::printStmt() -> StmtPtrVariant {
  advance();
  ExprPtrVariant printExpr = expression();
  consumeSemicolonOrError();
  return AST::createPrintSPV(std::move(printExpr));
}

// blockStmt   → "{" declaration "}"
auto RDParser::blockStmt() -> StmtPtrVariant {
  // Do something special for scopes;
  advance();  // consume '{'
  std::vector<StmtPtrVariant> statements;
  while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    auto optStmnt = declaration();
    if (optStmnt.has_value()) statements.push_back(std::move(optStmnt.value()));
  }
  consumeOrError(TokenType::RIGHT_BRACE, "Expect '}' after block");
  return AST::createBlockSPV(std::move(statements));
}

// ifStmt      → "if" "(" expression ")" statement ("else" statement)? ;
auto RDParser::ifStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expecte '(' after if.");
  ExprPtrVariant condition = expression();
  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after if condition.");

  StmtPtrVariant thenBranch = statement();

  std::optional<StmtPtrVariant> elseBranch = std::nullopt;
  if (match(TokenType::ELSE)) {
    advance();
    elseBranch = std::make_optional(statement());
  }

  return AST::createIfSPV(std::move(condition), std::move(thenBranch),
                          std::move(elseBranch));
}

// whileStmt   → "while" "(" expression ")" statement;
auto RDParser::whileStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expecte '(' after while.");
  ExprPtrVariant condition = expression();
  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after while condition.");
  return AST::createWhileSPV(std::move(condition), statement());
}

// forStmt     → "for" "("
//                          (varDecl | exprStmnt | ";")
//                          expression? ";"
//                          expression?
//                     ")"
//                statement;
auto RDParser::forStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expected '(' after for.");

  std::optional<StmtPtrVariant> initializer = std::nullopt;
  if (match(TokenType::SEMICOLON)) {
    advance();
  } else if (match(TokenType::VAR)) {
    advance();
    initializer = std::make_optional(varDecl());
  } else {
    initializer = std::make_optional(exprStmt());
  }

  std::optional<ExprPtrVariant> condition = std::nullopt;
  if (!match(TokenType::SEMICOLON))
    condition = std::make_optional(expression());

  consumeSemicolonOrError();

  std::optional<ExprPtrVariant> increment = std::nullopt;
  if (!match(TokenType::RIGHT_PAREN))
    increment = std::make_optional(expression());

  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after 'for' clauses.");

  StmtPtrVariant loopBody = statement();

  return AST::createForSPV(std::move(initializer), std::move(condition),
                           std::move(increment), std::move(loopBody));
}

// returnStmt  → "return" (expression)? ";";
auto RDParser::returnStmt() -> StmtPtrVariant {
  Token ret = getTokenAndAdvance();
  std::optional<AST::ExprPtrVariant> value = std::nullopt;
  if (!match(TokenType::SEMICOLON)) {
    value = expression();
  }
  consumeSemicolonOrError();
  return AST::createRetSPV(std::move(ret), std::move(value));
}

//=============//
// Expressions //
//=============//
// expression → comma;
auto RDParser::expression() -> ExprPtrVariant { return comma(); }

// !This is going to cause some grief when we are looking at function
// arguments; !fun(1,2) will be treated as fun((1,2)) <- a single argument
// instead of two. !Needs to make sure we set function arguments to have
// higher precedence than !the comma operator
// comma → assignment ("," assignment)*;
auto RDParser::comma() -> ExprPtrVariant {
  return consumeAnyBinaryExprs({TokenType::COMMA}, assignment(),
                               &RDParser::assignment);
}

// assignment  → IDENTIFIER "=" assignment | condititional;
auto RDParser::assignment() -> ExprPtrVariant {
  ExprPtrVariant expr = conditional();
  if (match(TokenType::EQUAL)) {
    advance();
    Token varName = getVarNameOrThrowError(expr);
    return AST::createAssignmentEPV(varName, assignment());
  }
  return expr;
}

// conditional → logical_or ("?" expression ":" conditional)?;
// conditional → equality ("?" expression ":" conditional)?
auto RDParser::conditional() -> ExprPtrVariant {
  ExprPtrVariant expr = logical_or();
  if (match(TokenType::QUESTION)) {
    Token op = getTokenAndAdvance();
    ExprPtrVariant thenBranch = expression();
    consumeOrError(TokenType::COLON, "Expected a colon after ternary operator");
    return AST::createConditionalEPV(std::move(expr), std::move(thenBranch),
                                     conditional());
  }
  return expr;
}

// logical_or  → logical_and ("or" logical_and)*;
auto RDParser::logical_or() -> ExprPtrVariant {
  ExprPtrVariant expr = logical_and();
  while (match(TokenType::OR)) {
    Token op = getTokenAndAdvance();
    expr = AST::createLogicalEPV(std::move(expr), op, logical_and());
  }
  return expr;
}

// logical_and → equality ("and" equality)*;
auto RDParser::logical_and() -> ExprPtrVariant {
  ExprPtrVariant expr = equality();
  while (match(TokenType::AND)) {
    Token op = getTokenAndAdvance();
    expr = AST::createLogicalEPV(std::move(expr), op, equality());
  }
  return expr;
}

// equality   → comparison(("!=" | "==") comparison) *;
auto RDParser::equality() -> ExprPtrVariant {
  auto equalityTypes = {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL};
  return consumeAnyBinaryExprs(equalityTypes, comparison(),
                               &RDParser::comparison);
}

// comparison → addition((">" | ">=" | "<" | "<=") addition) *;
auto RDParser::comparison() -> ExprPtrVariant {
  auto comparatorTypes = {TokenType::GREATER, TokenType::GREATER_EQUAL,
                          TokenType::LESS, TokenType::LESS_EQUAL};
  return consumeAnyBinaryExprs(comparatorTypes, addition(),
                               &RDParser::addition);
}

// addition   → multiplication(("-" | "+") multiplication) *;
auto RDParser::addition() -> ExprPtrVariant {
  auto additionTypes = {TokenType::PLUS, TokenType::MINUS};
  return consumeAnyBinaryExprs(additionTypes, multiplication(),
                               &RDParser::multiplication);
}

// multi...   → unary(("/" | "*") unary) *;
auto RDParser::multiplication() -> ExprPtrVariant {
  auto multTypes = {TokenType::SLASH, TokenType::STAR};
  return consumeAnyBinaryExprs(multTypes, unary(), &RDParser::unary);
}

// unary      → ("!" | "-" | "--" | "++") unary | postfix;
auto RDParser::unary() -> ExprPtrVariant {
  auto unaryTypes = {TokenType::BANG, TokenType::MINUS, TokenType::PLUS_PLUS,
                     TokenType::MINUS_MINUS};
  if (match(unaryTypes)) return consumeUnaryExpr();
  return postfix();
}

// postfix    → primary ("++" | "--")*;
auto RDParser::postfix() -> ExprPtrVariant {
  return consumePostfixExpr(call());
}

// call        → primary ( "(" arguments? ")" )*;
auto RDParser::call() -> ExprPtrVariant {
  auto expr = primary();
  while (true) {
    if (match(TokenType::LEFT_PAREN)) {
      advance();
      std::vector<ExprPtrVariant> args;
      if (!match(TokenType::RIGHT_PAREN)) args = arguments();
      if (!match(TokenType::RIGHT_PAREN))
        throw error("Expected ')' after function invocation.");
      expr = createCallEPV(std::move(expr), getTokenAndAdvance(),
                           std::move(args));
    } else {
      break;
    }
  }
  return expr;
}

// Go to assignment to avoid comma
// arguments   → assignment  ( "," assignment )* ;
auto RDParser::arguments() -> std::vector<ExprPtrVariant> {
  std::vector<ExprPtrVariant> args;
  args.push_back(assignment());
  while (match(TokenType::COMMA)) {
    advance();
    if (args.size() >= MAX_ARGS)
      throw error("A function can't be invoked with more than 255 arguments");
    args.push_back(assignment());
  }
  return args;
}

// primary    → NUMBER | STRING | "false" | "true" | "nil";
// primary    →  "(" expression ")" | IDENTIFIER;
// primary     → "fun" funBody;
//  Error Productions: primary    → ("!=" | "==") equality primary    →
// (">" | ">=" | "<" | "<=") comparison primary    → ("+")addition primary →
// ("/" | "*") multiplication;
auto RDParser::primary() -> ExprPtrVariant {
  if (match(TokenType::FALSE)) return consumeOneLiteral("false");
  if (match(TokenType::TRUE)) return consumeOneLiteral("true");
  if (match(TokenType::NIL)) return consumeOneLiteral("nil");
  if (match(TokenType::NUMBER)) return consumeOneLiteral();
  if (match(TokenType::STRING)) return consumeOneLiteral();
  if (match(TokenType::LEFT_PAREN)) return consumeGroupingExpr();
  if (match(TokenType::IDENTIFIER)) return consumeVarExpr();
  if (match(TokenType::FUN)) return funcBody("Anon-Function");

  // Check for known error productions. throws RDParseError;
  throwOnErrorProductions();

  // if none of the known error productions match,
  // give up and throw generic RDParseError;
  throw error("Expected an expression; Got something else.");
}

// _________

auto RDParser::parse() -> std::vector<StmtPtrVariant> {
  program();
  return std::move(this->statements);
}

}  // namespace cpplox::Parser