#ifndef CPPLOX_TYPES_LITERAL_H
#define CPPLOX_TYPES_LITERAL_H
#pragma once

#include <optional>
#include <string>
#include <variant>

namespace cpplox::Types {

struct DoubleLiteral {
  double value;
  std::string stringLiteral;
};

using Literal = std::variant<std::string, DoubleLiteral>;
using OptionalLiteral = std::optional<Literal>;

auto getLiteralString(Literal& value) -> std::string;

auto makeOptionalDoubleLiteral(double dVal, const std::string& lexeme)
    -> OptionalLiteral;

auto makeOptionalDoubleLiteral(double dVal, const char* lexeme)
    -> OptionalLiteral;

auto makeOptionalStringLiteral(const std::string& lexeme) -> OptionalLiteral;

}  // namespace cpplox::Types

#endif  // CPPLOX_TYPES_LITERAL_H