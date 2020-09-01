# Cpp-lox

C++ implementation of the Tree-Walk Interpreter for the
lox programming language described in Part II of Bob Nystrom's book,
[Crafting Interpreters](http://www.craftinginterpreters.com/).

## Lox

Lox is a toy programming language described in Bob Nystrom's book:
[Crafting Interpreters](http://www.craftinginterpreters.com/).
Lox has C-like syntax, but is dynamically typed and garbage collected.
Refer to the [informal specification](http://craftinginterpreters.com/the-lox-language.html) for more information.

Here's how you would implement a program to print the fibonacci sequence in lox:

``` python
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}

var start = clock();
print fib(35) == 9227465;
print clock() - start;
```

CPPLox Extensions:

* Conditional expressions
* Postfix and Prefix operators
* strings concatenate with any other type.

Differences from the implementation in the book:

* Used C++17's variants instead of the visitor pattern.
* I implemented a closure-based solution to scope resolution and variable binding,
instead of the resolution pass over the AST Bob employed.

## Build Dependencies

* Build tool: [Bazel](https://bazel.build/)
* Testing Framework: [GoogleTest](https://github.com/google/googletest) release v1.10.0

## Optional Dependencies

* GrailBio's [Bazel Compilation Database Generator](https://github.com/grailbio/bazel-compilation-database)

In order to use LLVM's linting tool [clangd](https://github.com/clangd/clangd),
you need a `compile_commands.json` file that is essentially a dump of the exact
commands used to compile your program. Bazel doesn't seem to be able to produce
this as yet. This is a community maintained tool that will generate such a
database.

## License and Copyright

All code in this repository is released under the MIT license,
unless it is code brought in from an external project (noted below).

### External projects

All files under **_lox-tests_** are copied verbatim from Bob Nystrom's
[repo](https://github.com/munificent/craftinginterpreters/).
All code in there is licensed under the MIT license as well, but copyrighted to Bob.

All code under third_party is copyrighted by the respective owners and license
files are provided where available.

## Notes

* The cpplox REPL interprets input one line at a time, i.e.,
multi-line expressions will not be handled properly. I chose to live
with this limitation for now, as implementing support for multi-line
input results in going down several complex paths:
  1. Building a special multi-line input mode into the REPL. This
  quickly devolves into writing a small text editor (perhaps this is
  why there are so many editors?)
  1. Writing a simple brace/bracket matcher that determines if the
  input has an unbalanced brace or bracket. This could be implemented
  over the tokens returned  by the scanner. When an unbalanced
  bracket is detected, the REPL keeps waiting for input unless it
  sees 2 newlines. The real issue with this approach is that we're
  essentially introducing parts of the language parsing logic into
  the REPL. While this is certainly doable, it's probably overkill
  for this toy implementation.
  1. Throwing a special exception for unmatched braces from the
  parser that can be caught and handled by the REPL driver as a signal
  to capture multi-line input. The most efficient way of implementing
  this would be to pass each new line through the scanner only once.
  The deteceted tokens would then be appended to those from earlier
  lines and this set would be passes through the parser each time
  until the parser is satisfied or complains about something else
  entirely. Again two new lines in a row would indicate end of input
  from the REPL user. This approach preserves the separation of
  concerns, but results in the parser being invoked multiple times.
  Overall, given that human input is probably slower than the parser,
  this approach might be an acceptable trade-off.
