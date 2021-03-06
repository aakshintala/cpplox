#include "cpplox/Evaluator/Evaluator.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "cpplox/AST/PrettyPrinter.h"
#include "cpplox/ErrorsAndDebug/DebugPrint.h"
#include "cpplox/ErrorsAndDebug/RuntimeError.h"
#include "cpplox/Evaluator/Objects.h"
#include "cpplox/Types/Literal.h"
#include "cpplox/Types/Token.h"

#define EXPECT_TRUE(x) __builtin_expect(static_cast<int64_t>(x), 1)
#define EXPECT_FALSE(x) __builtin_expect(static_cast<int64_t>(x), 0)

namespace cpplox::Evaluator {

// throws RuntimeError if right isn't a double
auto Evaluator::getDouble(const Token& token, const LoxObject& right)
    -> double {
  if (EXPECT_FALSE(!std::holds_alternative<double>(right)))
    throw reportRuntimeError(
        eReporter, token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + getObjectString(right));
  return std::get<double>(right);
}

auto Evaluator::bindInstance(const FuncShrdPtr& method,
                             LoxInstanceShrdPtr instance) -> FuncShrdPtr {
  auto environToRestore = environManager.getCurrEnv();
  // Set the currentEnviron to the function's closure,
  environManager.setCurrEnv(method->getClosure());
  // Create a new environment and define 'this' to point to the instance
  environManager.createNewEnviron();
  auto methodClosure = environManager.getCurrEnv();
  environManager.define("this", instance);
  // restore the environ.
  environManager.setCurrEnv(environToRestore);
  // create and return a new FuncObj that uses this new environ as its closure.
  return std::make_shared<FuncObj>(method->getDecl(), method->getFnName(),
                                   methodClosure, method->getIsMethod(),
                                   method->getIsInitializer());
}

//===============================//
// Expression Evaluation Methods //
//===============================//
auto Evaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> LoxObject {
  auto left = evaluateExpr(expr->left);
  auto right = evaluateExpr(expr->right);
  switch (expr->op.getType()) {
    case TokenType::COMMA: return right;
    case TokenType::BANG_EQUAL: return !areEqual(left, right);
    case TokenType::EQUAL_EQUAL: return areEqual(left, right);
    case TokenType::MINUS:
      return getDouble(expr->op, left) - getDouble(expr->op, right);
    case TokenType::SLASH: {
      double denominator = getDouble(expr->op, right);
      if (EXPECT_FALSE(denominator == 0.0))
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
  return evaluateExpr(expr->expression);
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
  LoxObject right = evaluateExpr(expr->right);
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
  if (isTrue(evaluateExpr(expr->condition)))
    return evaluateExpr(expr->thenBranch);
  return evaluateExpr(expr->elseBranch);
}

auto Evaluator::evaluateVariableExpr(const VariableExprPtr& expr) -> LoxObject {
  return environManager.get(expr->varName);
}

auto Evaluator::evaluateAssignmentExpr(const AssignmentExprPtr& expr)
    -> LoxObject {
  environManager.assign(expr->varName, evaluateExpr(expr->right));
  return environManager.get(expr->varName);
}

namespace {
auto match(const Token& token, Types::TokenType tType) -> bool {
  return token.getType() == tType;
}

auto doPostfixOp(const Token& op, const LoxObject& val) -> LoxObject {
  if (EXPECT_TRUE(std::holds_alternative<double>(val))) {
    double dVal = std::get<double>(val);
    if (match(op, TokenType::PLUS_PLUS)) return LoxObject(++dVal);
    if (match(op, TokenType::MINUS_MINUS)) return LoxObject(--dVal);
  }
  throw ErrorsAndDebug::RuntimeError();
}
}  // namespace

auto Evaluator::evaluatePostfixExpr(const PostfixExprPtr& expr) -> LoxObject {
  LoxObject leftVal = evaluateExpr(expr->left);
  if (EXPECT_TRUE(std::holds_alternative<VariableExprPtr>(expr->left))) {
    environManager.assign((std::get<VariableExprPtr>(expr->left))->varName,
                          doPostfixOp(expr->op, leftVal));
  }
  return leftVal;
}

auto Evaluator::evaluateLogicalExpr(const LogicalExprPtr& expr) -> LoxObject {
  LoxObject leftVal = evaluateExpr(expr->left);
  if (expr->op.getType() == TokenType::OR)
    return isTrue(leftVal) ? leftVal : evaluateExpr(expr->right);
  if (expr->op.getType() == TokenType::AND)
    return !isTrue(leftVal) ? leftVal : evaluateExpr(expr->right);

  throw reportRuntimeError(eReporter, expr->op,
                           "Illegal logical operator: " + expr->op.getLexeme());
}

auto Evaluator::evaluateCallExpr(const CallExprPtr& expr) -> LoxObject {
  LoxObject callee = evaluateExpr(expr->callee);

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateCallExpr called. Callee:"
                             + getObjectString(callee));
#endif  // EVAL_DEBUG

  if (EXPECT_FALSE(std::holds_alternative<BuiltinFuncShrdPtr>(callee))) {
    // TODO(aakshintala): Currently this doesn't check arity or copy params, cos
    // we don't need that at the moment cos we just have one builtin: clock.
    // A correct implementation would look more like the rest of function call.
    // Also just use the visitor pattern with double dispatch next time.
    return std::get<BuiltinFuncShrdPtr>(callee)->run();
  }

  LoxObject instanceOrNull = ([&]() -> LoxObject {
    if (EXPECT_FALSE(std::holds_alternative<LoxClassShrdPtr>(callee)))
      return LoxObject(
          std::make_shared<LoxInstance>(std::get<LoxClassShrdPtr>(callee)));
    return LoxObject(nullptr);
  })();

  const FuncShrdPtr funcObj = ([&]() -> FuncShrdPtr {
    if (std::holds_alternative<LoxClassShrdPtr>(callee)) {
      auto instance = std::get<LoxInstanceShrdPtr>(instanceOrNull);
      try {
        return bindInstance(std::get<FuncShrdPtr>(instance->get("init")),
                            instance);
      } catch (const ErrorsAndDebug::RuntimeError& e) {
        return nullptr;
      }
    }

    if (EXPECT_TRUE(std::holds_alternative<FuncShrdPtr>(callee)))
      return std::get<FuncShrdPtr>(callee);

    throw reportRuntimeError(eReporter, expr->paren,
                             "Attempted to invoke a non-function");
  })();

  if (funcObj.get() == nullptr) {
    // exit early if there is no initializer; safe because we only come here if
    // this callee is a constructor and there is no initializer.
    return instanceOrNull;
  }

  // Throw error if arity doesn't match the number of arguments supplied
  if (size_t arity = funcObj->arity(), numArgs = expr->arguments.size();
      EXPECT_FALSE(arity != numArgs))
    throw reportRuntimeError(eReporter, expr->paren,
                             "Expected " + std::to_string(arity)
                                 + " arguments. Got " + std::to_string(numArgs)
                                 + " arguments. ");

  // Evaluate Arguments before switching to the next context as the arguments
  // may rely on values in this context (e.g., passing a local variable to a
  // function call.)
  std::vector<LoxObject> evaldArgs;
  for (const auto& arg : expr->arguments)
    evaldArgs.push_back(evaluateExpr(arg));

  // Save caller's environ so we can restore it later
  auto environToRestore = environManager.getCurrEnv();
  // Set the currentEnviron to the function's closure,
  environManager.setCurrEnv(funcObj->getClosure());
  // Create a new Environ for the function so it doesn't dirty the closure.
  environManager.createNewEnviron();

  {  // Define each parameter with evaluated argument
    const auto& params = funcObj->getParams();
    auto param = params.begin();
    auto arg = evaldArgs.begin();
    for (; param != params.end() && arg != evaldArgs.end(); ++param, ++arg) {
      environManager.define(*param, *arg);
    }
  }

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("FnBodyStmts:");
  for (const auto& stmt :
       AST::PrettyPrinter::toString(funcObj->getFnBodyStmts()))
    ErrorsAndDebug::debugPrint(stmt);
#endif  // EVAL_DEBUG

  // Evaluate the function
  std::optional<LoxObject> fnRet = evaluateStmts(funcObj->getFnBodyStmts());

  // Teardown any environments created by the function.
  if (!funcObj->getIsMethod())
    environManager.discardEnvironsTill(funcObj->getClosure());
  else
    environManager.discardEnvironsTill(funcObj->getClosure()->getParentEnv());
  // Restore caller's environment.
  environManager.setCurrEnv(environToRestore);

  // return result or LoxObject(nullptr);
  if (fnRet.has_value()) {
    if (EXPECT_FALSE(funcObj->getIsInitializer()))
      throw ErrorsAndDebug::reportRuntimeError(
          eReporter, expr->paren,
          "Initializer can't return a value other than 'this'");
    return fnRet.value();
  }
  return instanceOrNull;
}

auto Evaluator::evaluateFuncExpr(const FuncExprPtr& expr) -> LoxObject {
  // The current Environment becomes the closure for the function.
  auto closure = environManager.getCurrEnv();
  // We also create a new environment because we don't want any redefinitions of
  // variables that are later in the program lexical order to be visible to this
  // function (in case it's stored and passed around). This environment creation
  // isn't paired with a destruction in this function. The environment will be
  // discarded when exiting wrapping scope this function is defined in (and the
  // FuncObj goes out of scope.)
  environManager.createNewEnviron();
  return std::make_shared<FuncObj>(expr, "LoxAnonFuncDoNotUseThisNameAADWAED",
                                   std::move(closure));
}

auto Evaluator::evaluateGetExpr(const GetExprPtr& expr) -> LoxObject {
  LoxObject instObj = evaluateExpr(expr->expr);
  if (EXPECT_FALSE(!std::holds_alternative<LoxInstanceShrdPtr>(instObj)))
    throw reportRuntimeError(eReporter, expr->name,
                             "Only instances have properties");
  try {
    LoxObject property
        = std::get<LoxInstanceShrdPtr>(instObj)->get(expr->name.getLexeme());
    if (std::holds_alternative<FuncShrdPtr>(property)) {
      // if it's a method that we just looked up, then we need to create a
      // binding for 'this'
      property = LoxObject(bindInstance(std::get<FuncShrdPtr>(property),
                                        std::get<LoxInstanceShrdPtr>(instObj)));
    }
    return property;
  } catch (const ErrorsAndDebug::RuntimeError& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, expr->name,
        "Attempted to access undefined property: " + expr->name.getLexeme()
            + " on " + std::get<LoxInstanceShrdPtr>(instObj)->toString());
  }
}

auto Evaluator::evaluateSetExpr(const AST::SetExprPtr& expr) -> LoxObject {
  LoxObject object = evaluateExpr(expr->expr);
  if (EXPECT_FALSE(!std::holds_alternative<LoxInstanceShrdPtr>(object)))
    throw ErrorsAndDebug::reportRuntimeError(eReporter, expr->name,
                                             "Only instances have fields.");
  LoxObject value = evaluateExpr(expr->value);
  std::get<LoxInstanceShrdPtr>(object)->set(expr->name.getLexeme(), value);
  return value;
}

auto Evaluator::evaluateThisExpr(const ThisExprPtr& expr) -> LoxObject {
  return environManager.get(expr->keyword);
}

auto Evaluator::evaluateSuperExpr(const SuperExprPtr& expr) -> LoxObject {
  LoxClassShrdPtr superClass
      = std::get<LoxClassShrdPtr>(environManager.get(expr->keyword));
  auto optionalMethod = superClass->findMethod(expr->method.getLexeme());
  if (!optionalMethod.has_value())
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, expr->keyword,
        "Attempted to access undefined property " + expr->keyword.getLexeme()
            + " on super.");

  return bindInstance(std::get<FuncShrdPtr>(optionalMethod.value()),
                      std::get<LoxInstanceShrdPtr>(
                          environManager.get(Token(TokenType::THIS, "this"))));
}

auto Evaluator::evaluateExpr(const ExprPtrVariant& expr) -> LoxObject {
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
    case 8:  // LogicalExprPtr
      return evaluateLogicalExpr(std::get<8>(expr));
    case 9:  // CallExprPtr
      return evaluateCallExpr(std::get<9>(expr));
    case 10:  // FuncExprPtr
      return evaluateFuncExpr(std::get<10>(expr));
    case 11:  // GetExprPtr
      return evaluateGetExpr(std::get<11>(expr));
    case 12:  // SetExprPtr
      return evaluateSetExpr(std::get<12>(expr));
    case 13:  // ThisExprPtr
      return evaluateThisExpr(std::get<13>(expr));
    case 14:  // SuperExprPtr
      return evaluateSuperExpr(std::get<14>(expr));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 15,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return "";
  }
}

//==============================//
// Statement Evaluation Methods //
//==============================//
auto Evaluator::evaluateExprStmt(const ExprStmtPtr& stmt)
    -> std::optional<LoxObject> {
#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateExprStmt called.");
#endif  // EVAL_DEBUG

  LoxObject result = evaluateExpr(stmt->expression);

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateExprStmt: expression evaluation result: "
                             + getObjectString(result));
#endif  // EVAL_DEBUG
  return std::nullopt;
}

auto Evaluator::evaluatePrintStmt(const PrintStmtPtr& stmt)
    -> std::optional<LoxObject> {
#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluatePrintStmt called.");
#endif  // EVAL_DEBUG

  LoxObject objectToPrint = evaluateExpr(stmt->expression);
  std::cout << ">" << getObjectString(objectToPrint) << std::endl;

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluatePrintStmt should have printed."
                             + getObjectString(objectToPrint));
#endif  // EVAL_DEBUG
  return std::nullopt;
}

auto Evaluator::evaluateBlockStmt(const BlockStmtPtr& stmt)
    -> std::optional<LoxObject> {
  auto currEnviron = environManager.getCurrEnv();
  environManager.createNewEnviron();
  std::optional<LoxObject> result = evaluateStmts(stmt->statements);
  environManager.discardEnvironsTill(currEnviron);
  return result;
}

auto Evaluator::evaluateVarStmt(const VarStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName,
                          evaluateExpr(stmt->initializer.value()));
  } else {
    environManager.define(stmt->varName, LoxObject(nullptr));
  }
  return std::nullopt;
}

auto Evaluator::evaluateIfStmt(const IfStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (isTrue(evaluateExpr(stmt->condition)))
    return evaluateStmt(stmt->thenBranch);
  if (stmt->elseBranch.has_value())
    return evaluateStmt(stmt->elseBranch.value());
  return std::nullopt;
}

auto Evaluator::evaluateWhileStmt(const WhileStmtPtr& stmt)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  while (isTrue(evaluateExpr(stmt->condition)) && !result.has_value()) {
    result = evaluateStmt(stmt->loopBody);
  }
  return result;
}

auto Evaluator::evaluateForStmt(const ForStmtPtr& stmt)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  if (stmt->initializer.has_value()) evaluateStmt(stmt->initializer.value());
  while (true) {
    if (stmt->condition.has_value()
        && !isTrue(evaluateExpr(stmt->condition.value())))
      break;
    result = evaluateStmt(stmt->loopBody);
    if (result.has_value()) break;
    if (stmt->increment.has_value()) evaluateExpr(stmt->increment.value());
  }
  return result;
}

auto Evaluator::evaluateFuncStmt(const FuncStmtPtr& stmt)
    -> std::optional<LoxObject> {
  // The current Environment becomes the closure for the function.
  std::shared_ptr<Environment> closure = environManager.getCurrEnv();
  // Create a FuncObj for the function, and hand it off to environment to store
  environManager.define(
      stmt->funcName,
      std::make_shared<FuncObj>(stmt->funcExpr, stmt->funcName.getLexeme(),
                                std::move(closure)));
  // We also create a new environment because we don't want any redefinitions of
  // variables that are later in the program lexical order to be visible to this
  // function (in case it's stored and passed around). This environment creation
  // isn't paired with a destruction in this function. The environment will be
  // discarded when exiting wrapping scope this function is defined in (and the
  // FuncObj goes out of scope.)
  environManager.createNewEnviron();
  return std::nullopt;
}

auto Evaluator::evaluateRetStmt(const RetStmtPtr& stmt)
    -> std::optional<LoxObject> {
  return stmt->value.has_value()
             ? std::make_optional(evaluateExpr(stmt->value.value()))
             : std::nullopt;
}

auto Evaluator::evaluateClassStmt(const ClassStmtPtr& stmt)
    -> std::optional<LoxObject> {
  // Determine if this class has a super class or not;
  auto superClass = [&]() -> std::optional<LoxClassShrdPtr> {
    if (stmt->superClass.has_value()) {
      auto superClassObj = evaluateExpr(stmt->superClass.value());
      if (!std::holds_alternative<LoxClassShrdPtr>(superClassObj))
        throw ErrorsAndDebug::reportRuntimeError(
            eReporter, stmt->className,
            "Superclass must be a class; Can't inherit from non-class");
      return std::get<LoxClassShrdPtr>(superClassObj);
    }
    return std::nullopt;
  }();

  // Define the class name in the current environment
  environManager.define(stmt->className, LoxObject(nullptr));

  // If there is a super class, create a new environ and define 'super' there
  if (superClass.has_value()) {
    environManager.createNewEnviron();
    environManager.define("super", superClass.value());
  }

  std::vector<std::pair<std::string, LoxObject>> methods;
  std::shared_ptr<Environment> closure = environManager.getCurrEnv();
  for (const auto& stmt : stmt->methods) {
    const auto& functionStmt = std::get<FuncStmtPtr>(stmt);
    bool isInitializer = functionStmt->funcName.getLexeme() == "init";
    LoxObject method = std::make_shared<FuncObj>(
        functionStmt->funcExpr, functionStmt->funcName.getLexeme(),
        closure, true, isInitializer);
    methods.emplace_back(functionStmt->funcName.getLexeme(), method);
  }

  // Discard the environment created for defining 'super'
  if (superClass.has_value()) {
    environManager.setCurrEnv(environManager.getCurrEnv()->getParentEnv());
  }

  // Declare the class
  environManager.assign(stmt->className,
                        std::make_shared<LoxClass>(stmt->className.getLexeme(),
                                                   superClass, methods));

  // Create a new environment so changes that occur afterwards in lexical order
  // aren't visible to the class defn.
  environManager.createNewEnviron();

  return std::nullopt;
}

auto Evaluator::evaluateStmt(const AST::StmtPtrVariant& stmt)
    -> std::optional<LoxObject> {
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
    case 9:  // ClassStmtPtr
      return evaluateClassStmt(std::get<9>(stmt));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 10,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
      return std::nullopt;
  }
}

auto Evaluator::evaluateStmts(const std::vector<AST::StmtPtrVariant>& stmts)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  for (const AST::StmtPtrVariant& stmt : stmts) {
    try {
      result = evaluateStmt(stmt);
      if (result.has_value()) break;
    } catch (const ErrorsAndDebug::RuntimeError& e) {
      ErrorsAndDebug::debugPrint("Caught unhandled exception.");
      if (EXPECT_FALSE(++numRunTimeErr > MAX_RUNTIME_ERR)) {
        std::cerr << "Too many errors occurred. Exiting evaluation."
                  << std::endl;
        throw e;
      }
    }
  }
  return result;
}

class clockBuiltin : public BuiltinFunc {
 public:
  explicit clockBuiltin(Environment::EnvironmentPtr closure)
      : BuiltinFunc("clock", std::move(closure)) {}
  auto arity() -> size_t override { return 0; }
  auto run() -> LoxObject override {
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count());
  }
  auto getFnName() -> std::string override { return "< builtin-fn_clock >"; }
};

Evaluator::Evaluator(ErrorReporter& eReporter)
    : eReporter(eReporter), environManager(eReporter) {
  environManager.define(
      Types::Token(TokenType::FUN, "clock"),
      static_cast<BuiltinFuncShrdPtr>(
          std::make_shared<clockBuiltin>(environManager.getCurrEnv())));
}

}  // namespace cpplox::Evaluator
