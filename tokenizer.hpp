#pragma once

#include "cpp-lisp-fwd.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace ka::common_lisp {

class Tokenizer {
    // Parser
    std::istringstream tokens;

    // get next token:
    //  ( ) or space delimited text
    //  ; comment to end of line
    std::string next_token();

    // an exmpression is a ( ) delimited set of tokens returned as an ordered set of ExpPtr
    ExprPtr read_expr();

public:
    std::vector<ExprPtr> parse_all(std::string input);
    ExprPtr parse(std::string input);
};

}  // namespace ka::common_lisp
