#include "ka/scheme/tokenizer.hpp"
#include "ka/scheme/eval.hpp"

#include <cctype>
#include <limits>
#include <stdexcept>
#include <memory>
#include <vector>

namespace ka::scheme {
namespace {
bool is_numeric(const std::string& tok) {
    return isdigit(tok[0]) || (tok[0] == '-' && tok.size() > 1);
}
}  // namespace

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
    return read_expr(std::move(tok));
}

ExprPtr Tokenizer::read_expr(std::string tok) {
    if (tok.empty())
        throw std::runtime_error("Unexpected EOF");
    if (tok == "(") {
        List list;
        while (true) {
            tok = next_token();
            if (tok == ")")
                break;
            list.push_back(read_expr(std::move(tok)));
        }
        return std::make_shared<Expr>(list);
    } else if (is_numeric(tok)) {
        return std::make_shared<Expr>(std::stod(tok));
    } else {
        return std::make_shared<Expr>(std::move(tok));
    }
}

List Tokenizer::parse_all(std::string input) {
    tokens.clear();
    tokens.str(std::move(input));
    List exprs;
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
