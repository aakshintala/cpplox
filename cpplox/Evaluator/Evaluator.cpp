#include "cpplox/Evaluator/Evaluator.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "Objects.h"
#include "cpplox/ErrorsAndDebug/RuntimeError.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"

namespace cpplox::Evaluator {

// throws RuntimeError if right isn't a double
auto Evaluator::getDouble(const Token& token, const LoxObject& right)
    -> double {
  if (!std::holds_alternative<double>(right))
    throw reportRuntimeError(
        eReporter, token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + getObjectString(right));
  return std::get<double>(right);
}

// Return Exception Methods
Evaluator::Return::Return(LoxObject value) : value(std::move(value)) {}

auto Evaluator::Return::get() -> LoxObject { return std::move(value); };

//===============================//
// Expression Evaluation Methods //
//===============================//
auto Evaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> LoxObject {
  auto left = evaluate(expr->left);
  auto right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::COMMA: return right;
    case TokenType::BANG_EQUAL: return !areEqual(left, right);
    case TokenType::EQUAL_EQUAL: return areEqual(left, right);
    case TokenType::MINUS:
      return getDouble(expr->op, left) - getDouble(expr->op, right);
    case TokenType::SLASH: {
      double denominator = getDouble(expr->op, right);
      if (denominator == 0.0)
        throw reportRuntimeError(eReporter, expr->op,
                                 "Division by zero is illegal");
      return getDouble(expr->op, left) / denominator;
    }
    case TokenType::STAR:
      return getDouble(expr->op, left) * getDouble(expr->op, right);
    case TokenType::LESS:
      return getDouble(expr->op, left) < getDouble(expr->op, right);
    case TokenType::LESS_EQUAL:
      return getDouble(expr->op, left) <= getDouble(expr->op, right);
    case TokenType::GREATER:
      return getDouble(expr->op, left) > getDouble(expr->op, right);
    case TokenType::GREATER_EQUAL:
      return getDouble(expr->op, left) >= getDouble(expr->op, right);
    case TokenType::PLUS: {
      if (std::holds_alternative<double>(left)
          && std::holds_alternative<double>(right)) {
        return std::get<double>(left) + std::get<double>(right);
      }
      if (std::holds_alternative<std::string>(left)
          || std::holds_alternative<std::string>(right)) {
        return getObjectString(left) + getObjectString(right);
      }
      throw reportRuntimeError(
          eReporter, expr->op,
          "Operands to 'plus' must be numbers or strings; This is invalid: "
              + getObjectString(left) + " + " + getObjectString(right));
    }
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Attempted to apply invalid operator to binary expr: "
              + expr->op.getTypeString());
  }
}

auto Evaluator::evaluateGroupingExpr(const GroupingExprPtr& expr) -> LoxObject {
  return evaluate(expr->expression);
}

namespace {
auto getLoxObjectfromStringLiteral(const Literal& strLiteral) -> LoxObject {
  const auto& str = std::get<std::string>(strLiteral);
  if (str == "true") return LoxObject(true);
  if (str == "false") return LoxObject(false);
  if (str == "nil") return LoxObject(nullptr);
  return LoxObject(str);
};
}  // namespace

auto Evaluator::evaluateLiteralExpr(const LiteralExprPtr& expr) -> LoxObject {
  return expr->literalVal.has_value()
             ? std::holds_alternative<std::string>(expr->literalVal.value())
                   ? getLoxObjectfromStringLiteral(expr->literalVal.value())
                   : LoxObject(std::get<double>(expr->literalVal.value()))
             : LoxObject(nullptr);
}

auto Evaluator::evaluateUnaryExpr(const UnaryExprPtr& expr) -> LoxObject {
  LoxObject right = evaluate(expr->right);
  switch (expr->op.getType()) {
    case TokenType::BANG: return !isTrue(right);
    case TokenType::MINUS: return -getDouble(expr->op, right);
    case TokenType::PLUS_PLUS: return getDouble(expr->op, right) + 1;
    case TokenType::MINUS_MINUS: return getDouble(expr->op, right) - 1;
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Illegal unary expression: " + expr->op.getLexeme()
              + getObjectString(right));
  }
}

auto Evaluator::evaluateConditionalExpr(const ConditionalExprPtr& expr)
    -> LoxObject {
  if (isTrue(evaluate(expr->condition))) return evaluate(expr->thenBranch);
  return evaluate(expr->elseBranch);
}

auto Evaluator::evaluateVariableExpr(const VariableExprPtr& expr) -> LoxObject {
  return environManager.get(expr->varName);
}

auto Evaluator::evaluateAssignmentExpr(const AssignmentExprPtr& expr)
    -> LoxObject {
  environManager.assign(expr->varName, evaluate(expr->right));
  return environManager.get(expr->varName);
}

namespace {
auto match(const Token& token, Types::TokenType tType) -> bool {
  return token.getType() == tType;
}

auto doPostfixOp(const Token& op, const LoxObject& val) -> LoxObject {
  if (std::holds_alternative<double>(val)) {
    double dVal = std::get<double>(val);
    if (match(op, TokenType::PLUS_PLUS)) return LoxObject(++dVal);
    if (match(op, TokenType::MINUS_MINUS)) return LoxObject(--dVal);
  }
  throw ErrorsAndDebug::RuntimeError();
}
}  // namespace

auto Evaluator::evaluatePostfixExpr(const PostfixExprPtr& expr) -> LoxObject {
  LoxObject leftVal = evaluate(expr->left);
  if (std::holds_alternative<VariableExprPtr>(expr->left)) {
    environManager.assign((std::get<VariableExprPtr>(expr->left))->varName,
                          doPostfixOp(expr->op, leftVal));
  }
  return leftVal;
}

auto Evaluator::evaluateLogicalExpr(const LogicalExprPtr& expr) -> LoxObject {
  LoxObject leftVal = evaluate(expr->left);
  if (expr->op.getType() == TokenType::OR)
    return isTrue(leftVal) ? leftVal : evaluate(expr->right);
  if (expr->op.getType() == TokenType::AND)
    return !isTrue(leftVal) ? leftVal : evaluate(expr->right);

  throw reportRuntimeError(eReporter, expr->op,
                           "Illegal logical operator: " + expr->op.getLexeme());
}

auto Evaluator::evaluateCallExpr(const CallExprPtr& expr) -> LoxObject {
  LoxObject callee = evaluate(expr->callee);
  if (std::holds_alternative<BuiltinFunc*>(callee)) {
    // TODO(aakshintala): Currently this doesn't check arity or copy params, cos
    // we don't need that at the moment cos we just have one builtin: clock.
    // A correct implementation would look more like the rest of function call.
    // Also just use the visitor pattern with double dispatch next time.
    return std::get<BuiltinFunc*>(callee)->run();
  }
  const FuncObj* funcObj = ([&]() -> FuncObj* {
    if (!std::holds_alternative<FuncObj*>(callee))
      throw reportRuntimeError(eReporter, expr->paren,
                               "Attempted to invoke a non-function");
    return std::get<FuncObj*>(callee);
  })();

  // Throw error if arity doesn't match the number of arguments supplied
  if (size_t arity = funcObj->arity(), numArgs = expr->arguments.size();
      arity != numArgs)
    throw reportRuntimeError(eReporter, expr->paren,
                             "Expected " + std::to_string(arity)
                                 + " arguments. Got " + std::to_string(numArgs)
                                 + " arguments. ");

  // Each function runs inside its own environment
  environManager.createNewEnviron(funcObj->toString());

  {  // Define each parameter with supplied argument
    const auto& params = funcObj->getParams();
    const auto& args = expr->arguments;
    auto param = params.begin();
    auto arg = args.begin();
    for (; param != params.end() && arg != args.end(); ++param, ++arg) {
      environManager.define(*param, evaluate(*arg));
    }
  }

  // Evaluate the function
  LoxObject result = nullptr;
  try {
    evaluate(funcObj->getFnBody());
  } catch (Return ret) {
    result = ret.get();
  }
  // Discard the function's environment
  environManager.discardCurrentEnviron(funcObj->toString());

  return result;
}

auto Evaluator::evaluate(const ExprPtrVariant& expr) -> LoxObject {
  switch (expr.index()) {
    case 0:  // BinaryExprPtr
      return evaluateBinaryExpr(std::get<0>(expr));
    case 1:  // GroupingExprPtr
      return evaluateGroupingExpr(std::get<1>(expr));
    case 2:  // LiteralExprPtr
      return evaluateLiteralExpr(std::get<2>(expr));
    case 3:  // UnaryExprPtr
      return evaluateUnaryExpr(std::get<3>(expr));
    case 4:  // ConditionalExprPtr
      return evaluateConditionalExpr(std::get<4>(expr));
    case 5:  // PostfixExprPtr
      return evaluatePostfixExpr(std::get<5>(expr));
    case 6:  // VariableExprPtr
      return evaluateVariableExpr(std::get<6>(expr));
    case 7:  // AssignmentExprPtr
      return evaluateAssignmentExpr(std::get<7>(expr));
    case 8:  // LogicalExpr
      return evaluateLogicalExpr(std::get<8>(expr));
    case 9:  // CallExpr
      return evaluateCallExpr(std::get<9>(expr));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 10,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return "";
  }
}

//==============================//
// Statement Evaluation Methods //
//==============================//
void Evaluator::evaluateExprStmt(const ExprStmtPtr& stmt) {
  evaluate(stmt->expression);
}

void Evaluator::evaluatePrintStmt(const PrintStmtPtr& stmt) {
  std::cout << getObjectString(evaluate(stmt->expression)) << std::endl;
}

void Evaluator::evaluateBlockStmt(const BlockStmtPtr& stmt) {
  // TODO(aakshintala): Add a random number after block.
  environManager.createNewEnviron("block");
  evaluate(stmt->statements);
  environManager.discardCurrentEnviron("block");
}

void Evaluator::evaluateVarStmt(const VarStmtPtr& stmt) {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName, evaluate(stmt->initializer.value()));
  } else {
    environManager.define(stmt->varName, LoxObject(nullptr));
  }
}

void Evaluator::evaluateIfStmt(const IfStmtPtr& stmt) {
  if (isTrue(evaluate(stmt->condition)))
    evaluate(stmt->thenBranch);
  else if (stmt->elseBranch.has_value())
    return evaluate(stmt->elseBranch.value());
}

void Evaluator::evaluateWhileStmt(const WhileStmtPtr& stmt) {
  while (isTrue(evaluate(stmt->condition))) {
    evaluate(stmt->loopBody);
  }
}

void Evaluator::evaluateForStmt(const ForStmtPtr& stmt) {
  if (stmt->initializer.has_value()) evaluate(stmt->initializer.value());
  while (true) {
    if (stmt->condition.has_value()
        && !isTrue(evaluate(stmt->condition.value()))) {
      break;
    }
    evaluate(stmt->loopBody);
    if (stmt->increment.has_value()) evaluate(stmt->increment.value());
  }
}

void Evaluator::evaluateFuncStmt(const FuncStmtPtr& stmt) {
  // We need to explicitly copy this because of the std::move on stmt
  // We can't deref stmt->funcName in the define function below, as the compiler
  // doesn't guarantee sequencing of the move and the deref (which means that
  // the stmt variable may have moved before its deref'd to access funcName)
  Token funcName = stmt->funcName;
  // Create a FuncObj for the function, and hand it off to environment to store
  environManager.define(funcName, std::make_unique<FuncObj>(stmt));
}

void Evaluator::evaluateRetStmt(const RetStmtPtr& stmt) {
  LoxObject value
      = stmt->value.has_value() ? evaluate(stmt->value.value()) : nullptr;
  throw Return(std::move(value));
}

void Evaluator::evaluate(const AST::StmtPtrVariant& stmt) {
  switch (stmt.index()) {
    case 0:  // ExprStmtPtr
      return evaluateExprStmt(std::get<0>(stmt));
    case 1:  // PrintStmtPtr
      return evaluatePrintStmt(std::get<1>(stmt));
    case 2:  // BlockStmtPtr
      return evaluateBlockStmt(std::get<2>(stmt));
    case 3:  // VarStmtPtr
      return evaluateVarStmt(std::get<3>(stmt));
    case 4:  // IfStmtPtr
      return evaluateIfStmt(std::get<4>(stmt));
    case 5:  // WhileStmtPtr
      return evaluateWhileStmt(std::get<5>(stmt));
    case 6:  // ForStmtPtr
      return evaluateForStmt(std::get<6>(stmt));
    case 7:  // FuncStmtPtr
      return evaluateFuncStmt(std::get<7>(stmt));
    case 8:  // RetStmtPtr
      return evaluateRetStmt(std::get<8>(stmt));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 9,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
  }
}

void Evaluator::evaluate(const std::vector<AST::StmtPtrVariant>& stmts) {
  for (const AST::StmtPtrVariant& stmt : stmts) {
    try {
      evaluate(stmt);
    } catch (const ErrorsAndDebug::RuntimeError& e) {
      if (++numRunTimeErr > MAX_RUNTIME_ERR) {
        std::cerr << "Too many errors occurred. Exiting evaluation."
                  << std::endl;
        throw e;
      }
    }
  }
}

class clockBuiltin : public BuiltinFunc {
 public:
  clockBuiltin() : BuiltinFunc("clock") {}
  auto arity() -> size_t override { return 0; }
  auto run() -> LoxObject override {
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count());
  }
  auto toString() -> std::string override { return "< builtin-fn_clock>"; }
};

Evaluator::Evaluator(ErrorReporter& eReporter)
    : eReporter(eReporter), environManager(eReporter) {
  environManager.define(Types::Token(TokenType::FUN, "clock"),
                        std::make_unique<clockBuiltin>());
}

}  // namespace cpplox::Evaluator
