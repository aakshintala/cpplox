#ifndef CPPLOX_TYPES_LITERAL_H
#define CPPLOX_TYPES_LITERAL_H
#pragma once

#include <optional>
#include <string>
#include <variant>

namespace cpplox::Types {

using Literal = std::variant<std::string, double>;
using OptionalLiteral = std::optional<Literal>;

auto getLiteralString(const Literal& value) -> std::string;

auto makeOptionalLiteral(double dVal) -> OptionalLiteral;

auto makeOptionalLiteral(const std::string& lexeme) -> OptionalLiteral;

}  // namespace cpplox::Types

#endif  // CPPLOX_TYPES_LITERAL_H