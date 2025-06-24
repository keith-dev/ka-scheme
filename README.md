# Scheme
This project started of in an experiment with ChatGPT to generate a Common LISP interpretter.

Of course, the code didn't work.
  * The parser was buggy and caused the app to crash.
  * Once fixed, I removed 'using namespace std'.
  * I refactored Expr from the multiplitiy of mutually exclusive members to std::variant.

That's when I realized I had a Scheme interpretter, not a Common Lisp interpretter.

I've fetched the various Scheme standards, and decided to implement r3, the original unified standard.
