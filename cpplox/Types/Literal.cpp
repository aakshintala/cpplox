#include "cpplox/Types/Literal.h"

namespace cpplox::Types {

auto getLiteralString(const Literal& value) -> std::string {
  // Literal = std::variant<std::string, double>;
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
    case 1:  // double
      return std::to_string(std::get<1>(value));
    default:
      static_assert(
          std::variant_size_v<Literal> == 2,
          "Looks like you forgot to update the cases in getLiteralString()!");
      return "";
  }
}

auto makeOptionalLiteral(double dVal) -> OptionalLiteral {
  return OptionalLiteral(std::in_place, dVal);
}

auto makeOptionalLiteral(const std::string& lexeme) -> OptionalLiteral {
  return OptionalLiteral(std::in_place, lexeme);
}

}  // namespace cpplox::Types