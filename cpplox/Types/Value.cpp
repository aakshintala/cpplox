#include "cpplox/Types/Value.h"

// using Value = std::variant<std::string, double, bool, std::nullptr_t>;
namespace cpplox::Types {

auto getValueString(const Value& value) -> std::string {
  switch (value.index()) {
    case 0:  // string
      return std::get<0>(value);
    case 1:  // double
      return std::to_string(std::get<1>(value));
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

}  // namespace cpplox::Types