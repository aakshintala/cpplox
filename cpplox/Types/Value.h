#ifndef CPPLOX_TYPES_VALUE_H
#define CPPLOX_TYPES_VALUE_H
#pragma once

#include <string>
#include <variant>

namespace cpplox::Types {

using Value = std::variant<std::string, double, bool, std::nullptr_t>;

auto getValueString(const Value& value) -> std::string;

}  // namespace cpplox::Types

#endif  // CPPLOX_TYPES_VALUE_H