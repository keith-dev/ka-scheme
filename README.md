# Build Status

![Build Status](https://github.com/${{github.repository}}/actions/workflows/build-and-test.yml/badge.svg)

# Scheme
This project started of as an experiment with ChatGPT to generate a Common Lisp interpretter.

Of course, the code didn't work.
  * The parser was buggy and caused the app to crash.
  * Once fixed, I removed 'using namespace std'.
  * I refactored Expr from the multiplitiy of mutually exclusive members to std::variant.

That's when I realized I had a Scheme interpretter, not a Common Lisp interpretter.

I've fetched the various Scheme standards, and decided to implement r3, the original unified standard.

It now has interactive input using linenode.  I looked at readline initally, but discovered linenode and found that better as it uses a single drop in C file with a better interface.

The game/repl loop has been moved out of the lib and into main, allowing for a testable lib.

# packaging
The implementation is refactored into a include/ka/scheme lib/libka-scheme.a

Versioning to be determined.
