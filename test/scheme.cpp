#include "ka/scheme/scheme.hpp"

#include <gtest/gtest.h>

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
