#pragma once

#include "ka/scheme/fwd.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace ka::scheme {

class Tokenizer {
    // Parser
    std::istringstream tokens;

    // get next token:
    //  ( ) or space delimited text
    //  ; comment to end of line
    std::string next_token();

    // an exmpression is a ( ) delimited set of tokens returned as an ordered set of ExpPtr
    ExprPtr read_expr();
    ExprPtr read_expr(std::string token);

public:
    std::vector<ExprPtr> parse_all(std::string input);
    ExprPtr parse(std::string input);
};

}  // namespace ka::scheme
