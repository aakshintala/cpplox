#include <iostream>

#include "cpplox/InterpreterDriver/InterpreterDriver.h"

// We are using SYSEXITS exit codes
auto main(int argc, char const *argv[]) -> int {
  if (argc > 2) {
    std::cout << "Usage: ./lox <script.lox> to execute a script or \
                  just ./lox to drop into a REPL"
              << std::endl;
    std::exit(64);
  }

  cpplox::InterpreterDriver interpreter;

  if (2 == argc) {
    return interpreter.runScript(argv[1]);
  }

  interpreter.runREPL();
  return 0;
}
