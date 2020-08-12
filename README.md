# Cpp-lox

C++ implementation of the lox programming language described in Bob Nystrom's book, [Crafting Interpreters](http://www.craftinginterpreters.com/).

## Lox

Lox is a toy programming language described in Bob Nystrom's book: [Crafting Interpreters](http://www.craftinginterpreters.com/).
Lox has C-like syntax, but is dynamically typed and garbage collected.

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

Refer to the informal specification in the
[book](http://craftinginterpreters.com/the-lox-language.html) for more information.

I've made several extensions to the official language:

* Conditional expressions
* Postfix and Prefix operators
* strings concatenate with any other type.

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
