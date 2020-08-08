#include "cpplox/Types/Literal.h"

namespace cpplox::Types {

auto getLiteralString(const Literal& value) -> std::string {
  // Literal = std::variant<std::string, DoubleLiteral>;
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
      break;
    case 1:  // DoubleLiteral
      return std::get<1>(value).stringLiteral;
      break;
    default:
      static_assert(
          std::variant_size_v<Literal> == 2,
          "Looks like you forgot to update the cases in getLiteralString()!");
      return "";
      break;
  }
}

auto makeOptionalDoubleLiteral(double dVal, const char* lexeme)
    -> OptionalLiteral {
  std::string sLexeme = lexeme;
  return makeOptionalDoubleLiteral(dVal, sLexeme);
}

auto makeOptionalDoubleLiteral(double dVal, const std::string& lexeme)
    -> OptionalLiteral {
  DoubleLiteral dLiteral = {dVal, lexeme};
  OptionalLiteral literal(std::in_place, dLiteral);
  return literal;
}

auto makeOptionalStringLiteral(const std::string& lexeme) -> OptionalLiteral {
  OptionalLiteral literal(std::in_place, lexeme);
  return literal;
}

}  // namespace cpplox::Types