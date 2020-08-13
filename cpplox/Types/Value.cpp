#include "cpplox/Types/Value.h"

#include <cstddef>
#include <type_traits>

// using Value = std::variant<std::string, double, bool, std::nullptr_t>;
namespace cpplox::Types {

auto areEqual(const Value& left, const Value& right) -> bool {
  if (left.index() == right.index()) {
    switch (left.index()) {
      case 0:  // string
        return std::get<std::string>(left) == std::get<std::string>(right);
      case 1:  // double
        return std::get<double>(left) == std::get<double>(right);
      case 2:  // bool
        return std::get<bool>(left) == std::get<bool>(right);
      case 3:  // std::nullptr_t
        // The case where one is null and the other isn't is handled by the
        // outer condition;
        return true;
      default:
        static_assert(std::variant_size_v<Value> == 4,
                      "Looks like you forgot to update the cases in "
                      "ExprEvaluator::areEqual(const Value&, const Value&)!");
    }
  }
  return false;
}

auto getValueString(const Value& value) -> std::string {
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
    case 1: {  // double
      std::string result = std::to_string(std::get<1>(value));
      auto pos = result.find(".000000");
      if (pos != std::string::npos)
        result.erase(pos, std::string::npos);
      else
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
      return result;
    }
    case 2:  // bool
      return std::get<2>(value) == true ? "true" : "false";
    case 3:  // null
      return "nil";
    default:
      static_assert(
          std::variant_size_v<Value> == 4,
          "Looks like you forgot to update the cases in getLiteralString()!");
      return "";
  }
}

auto isTrue(const Value& value) -> bool {
  if (std::holds_alternative<std::nullptr_t>(value)) return false;
  if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
  return true;  // for all else we go to true
}

}  // namespace cpplox::Types