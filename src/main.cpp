#include "ka/scheme/scheme.hpp"

#include <iostream>

int main() {
    std::cout << "Minimal LISP in C++ (type 'exit' to quit)" << std::endl;
    ka::scheme::repl();
}
