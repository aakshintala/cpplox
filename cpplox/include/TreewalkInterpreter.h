#include <string>

namespace cpplox {

class TreewalkInterpreter {
public:
  int runScript(const char const *script);
  int runREPL();

private:
  int interpret(const std::string &source);
};

} // namespace cpplox
