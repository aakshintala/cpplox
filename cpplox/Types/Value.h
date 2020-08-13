#ifndef CPPLOX_TYPES_VALUE_H
#define CPPLOX_TYPES_VALUE_H
#pragma once

#include <string>
#include <variant>

namespace cpplox::Types {

enum class ValVarEnum { STRING, DOUBLE, BOOL, NIL, LAST_VAL };
using Value = std::variant<std::string, double, bool, std::nullptr_t>;

auto areEqual(const Value& left, const Value& right) -> bool;

auto getValueString(const Value& value) -> std::string;

auto isTrue(const Value& value) -> bool;

}  // namespace cpplox::Types

#endif  // CPPLOX_TYPES_VALUE_H