#include "cpplox/ErrorsAndDebug/DebugPrint.h"

#include <iostream>
#include <string>

namespace cpplox::ErrorsAndDebug {

void debugPrint(const std::string& str) {
#if _CPPLOX_DEBUG_
  std::cout << str << std::endl;
#endif  // CPPLOXDEBUG
}

}  // namespace cpplox::ErrorsAndDebug