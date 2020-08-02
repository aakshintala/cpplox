Things to implement:

* End-to-end testing (lox programs under lox-tests)
  * Figure out if we can use Google Test infrastructure to achieve this
* Implement better debugging information:
  * Storing more information than just the line a lexeme occurs on would help?
  * Challenge: Could we pin point the exact location where the error occurred,
    and display it to the user?