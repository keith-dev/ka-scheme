#include "ka/scheme/tokenizer.hpp"
#include "ka/scheme/eval.hpp"

#include <cctype>
#include <limits>
#include <stdexcept>
#include <memory>
#include <vector>

namespace ka::scheme {

// get next token:
//  ( ) or space delimited text
//  ; comment to end of line
std::string Tokenizer::next_token() {
    std::string tok;
    char c;
    while (tokens.get(c)) {
        if (isspace(c))
            continue;
        if (c == ';') {
            tokens.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (c == '(' || c == ')')
            return std::string(1, c);

        tokens.putback(c);
        while (tokens.get(c)) {
            switch (c) {
                case '(':
                case ')':
                case ' ':
                    tokens.putback(c);
                    return tok;
                default:
                    tok += c;
            }
        }
        return tok;
    }
    return {};
}

// an exmpression is a ( ) delimited set of tokens returned as an ordered set of ExpPtr
ExprPtr Tokenizer::read_expr() {
    std::string tok = next_token();
    if (tok.empty())
        throw std::runtime_error("Unexpected EOF");
    if (tok == "(") {
        List list;
        while (true) {
            tok = next_token();
            if (tok == ")")
                break;
            // prepend the stream with the token, so we can process it recursively
            auto offset = tokens.tellg();
            auto old_tokens = tokens.str().substr(offset);
            tokens.str(tok + old_tokens);
            list.push_back(read_expr());
        }
        return std::make_shared<Expr>(list);
    } else if (isdigit(tok[0]) || (tok[0] == '-' && tok.size() > 1)) {
        return std::make_shared<Expr>(std::stod(tok));
    } else {
        return std::make_shared<Expr>(tok);
    }
}

std::vector<ExprPtr> Tokenizer::parse_all(std::string input) {
    tokens.clear();
    tokens.str(std::move(input));
    std::vector<ExprPtr> exprs;
    while (tokens.peek() != EOF) {
        while (isspace(tokens.peek()))
            tokens.get(); // skip whitespace
        if (tokens.peek() == EOF)
            break;
        exprs.push_back(read_expr());
    }
    return exprs;
}

ExprPtr Tokenizer::parse(std::string input) {
    tokens.clear();
    tokens.str(std::move(input));
    return read_expr();
}

}  // ka::scheme
