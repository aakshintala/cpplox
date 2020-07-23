#include <iostream>

#include "TreewalkInterpreter.h"

// We are using SYSEXITS exit codes
int main(int argc, char const *argv[]) {
  if (argc > 2) {
    std::cout << "Usage: ./lox <script.lox> to execute a script or \
                  just ./lox to drop into a REPL"
              << std::endl;
    std::exit(64);
  }

  cpplox::TreewalkInterpreter interpreter;
  if (2 == argc) {
    return interpreter.runScript(argv[1]);
  } else {
    return interpreter.runREPL();
  }
}
