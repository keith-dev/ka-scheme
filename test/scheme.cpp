#include "ka/scheme/scheme.hpp"

#include <gtest/gtest.h>

#include <array>

namespace ka::scheme::test {
class EngineTest : public Engine, public ::testing::Test {
};

TEST_F(EngineTest, EvaluateArithmetic) {
    const std::string given("(+ (* 2 3) (/ 12 2) (+ 1 0) (- 2 1))");
    const std::string expected{"14"};

    // evaluate expression
    std::istringstream is(given);
    std::stringstream os;
    std::stringstream err;
    evaluate(is, os, err);

    // extract output from first line
    std::string line;
    std::getline(os, line);

    EXPECT_EQ(expected, line);
}

TEST_F(EngineTest, EvaluateMultistepPerLine) {
    const std::string given("(define x 10) (* x x x)");
    const std::array<std::string, 2> expected{"10", "1000"};

    // evaluate expression
    std::istringstream is(given);
    std::stringstream os;
    std::stringstream err;
    evaluate(is, os, err);

    // extract output from lines
    std::string line;
    for (size_t i = 0; i != expected.size(); ++i) {
        std::getline(os, line);
        EXPECT_EQ(expected[i], line);
    }
}
}  // namespace ka::scheme::test
