#include "ka/scheme/scheme.hpp"

#include <gtest/gtest.h>

#include <array>

TEST(libscheme, EvaluateArithmetic) {
    const std::string given("(+ (* 2 3) (/ 12 2) (+ 1 0) (- 2 1))");
    const std::string expected{"14"};

    // evaluate expression
    std::istringstream is(given);
    std::stringstream ios;
    ka::scheme::repl(is, ios);

    // extract output from first line
    std::string line;
    std::getline(ios, line);

    EXPECT_EQ(expected, line);
}

TEST(libscheme, EvaluateMultistepPerLine) {
    const std::string given("(define x 10) (* x x x)");
    const std::array<std::string, 2> expected{"10", "1000"};

    // evaluate expression
    std::istringstream is(given);
    std::stringstream ios;
    ka::scheme::repl(is, ios);

    // extract output from lines
    std::string line;
    for (size_t i = 0; i != expected.size(); ++i) {
        std::getline(ios, line);
        EXPECT_EQ(expected[i], line);
    }
}
