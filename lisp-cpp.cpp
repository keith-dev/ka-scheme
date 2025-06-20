#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;
using List = std::vector<ExprPtr>;
using Env = std::map<std::string, ExprPtr>;
using BuiltinFunc = std::function<ExprPtr(const List&)>;
struct Lambda {
    std::vector<std::string> params;
    ExprPtr body;
    std::shared_ptr<Env> closure;
};

enum class Type { Number, Symbol, List, Function, Lambda };

struct Expr {
    Type type;
    //std::variant<double number, std::string symbol, List list, BuiltinFunc func> data;
    using Content = std::variant<double, std::string, List, BuiltinFunc, Lambda>;
    Content data;

    Expr(double n) : type(Type::Number), data(n) {}
    Expr(std::string s) : type(Type::Symbol), data(std::move(s)) {}
    Expr(List l) : type(Type::List), data(std::move(l)) {}
    Expr(BuiltinFunc f) : type(Type::Function), data(f) {}
    Expr(std::vector<std::string> p, ExprPtr b, std::shared_ptr<Env> c)
        : type(Type::Lambda), data(Lambda{std::move(p), std::move(b), std::move(c)}) {}
};

// Forward declarations
ExprPtr eval(ExprPtr, std::shared_ptr<Env>);
ExprPtr parse(const std::string& input);
List parse_all(const std::string& input);

// Parser
std::istringstream tokens;

// get next token:
//  ( ) or space delimited text
//  ; comment to end of line
std::string next_token() {
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
ExprPtr read_expr() {
    std::string tok = next_token();
    if (tok.empty())
        throw std::runtime_error("Unexpected EOF");
    if (tok == "(") {
        List list;
        while (true) {
            tok = next_token();
            if (tok == ")") break;
            // prepend the stream with the token, so we can process it recursively
            auto offset = tokens.tellg();
            auto old_tokens = tokens.str().substr(offset);
            tokens.str(tok + old_tokens);
            list.push_back(read_expr());
        }
        return std::make_shared<Expr>(list);
    } else if (isdigit(tok[0]) || (tok[0] == '-' && tok.size() > 1)) {
        return std::make_shared<Expr>(stod(tok));
    } else {
        return std::make_shared<Expr>(tok);
    }
}

std::vector<ExprPtr> parse_all(const std::string& input) {
    tokens.clear();
    tokens.str(input);
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

ExprPtr parse(const std::string& input) {
    tokens.clear();
    tokens.str(input);
    return read_expr();
}

// Evaluator
ExprPtr eval_list(const List& list, std::shared_ptr<Env> env) {
    if (list.empty())
        return std::make_shared<Expr>(list);
    std::string op = std::get<std::string>(list[0]->data);

    if (op == "quote") {
        return list[1];
    } else if (op == "if") {
        ExprPtr cond = eval(list[1], env);
        return eval((std::get<double>(cond->data) != 0) ? list[2] : list[3], env);
    } else if (op == "define") {
        std::string var = std::get<std::string>(list[1]->data);
        ExprPtr val = eval(list[2], env);
        (*env)[var] = val;
        return val;
    } else if (op == "lambda") {
        std::vector<std::string> params;
        for (auto& p : std::get<List>(list[1]->data))
            params.push_back(std::get<std::string>(p->data));
        return std::make_shared<Expr>(params, list[2], env);
    } else if (op == "load") {
        std::string filename = std::get<std::string>(list[1]->data);
        std::ifstream infile(filename);
        if (!infile)
            throw std::runtime_error("Cannot open file: " + filename);
        std::string code((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        auto exprs = parse_all(code);
        ExprPtr result;
        for (auto& expr : exprs)
            result = eval(expr, env);
        return result;
    } else {
        ExprPtr proc = eval(list[0], env);
        List args;
        for (size_t i = 1; i < list.size(); ++i)
            args.push_back(eval(list[i], env));
        if (proc->type == Type::Function) {
            return std::get<BuiltinFunc>(proc->data)(args);
        } else if (proc->type == Type::Lambda) {
            auto new_env = std::make_shared<Env>(*std::get<Lambda>(proc->data).closure);
            for (size_t i = 0; i < std::get<Lambda>(proc->data).params.size(); ++i)
                (*new_env)[std::get<Lambda>(proc->data).params[i]] = args[i];
            return eval(std::get<Lambda>(proc->data).body, new_env);
        } else {
            throw std::runtime_error("Not a function");
        }
    }
}

ExprPtr eval(ExprPtr expr, std::shared_ptr<Env> env) {
    switch (expr->type) {
        case Type::Symbol:
            if (env->count(std::get<std::string>(expr->data)))
                return (*env)[std::get<std::string>(expr->data)];
            throw std::runtime_error("Unbound symbol: " + std::get<std::string>(expr->data));
        case Type::Number:
        case Type::Function:
        case Type::Lambda:
            return expr;
        case Type::List:
            return eval_list(std::get<List>(expr->data), env);
    }
    throw std::runtime_error("Unknown expression type");
}

void print_expr(ExprPtr expr) {
    switch (expr->type) {
        case Type::Number:
            std::cout << std::get<double>(expr->data);
            break;
        case Type::Symbol:
            std::cout << std::get<std::string>(expr->data);
            break;
        case Type::Function:
            std::cout << "<builtin-function>";
            break;
        case Type::Lambda:
            std::cout << "<lambda>";
            break;
        case Type::List:
            std::cout << "(";
            for (size_t i = 0; i < std::get<List>(expr->data).size(); ++i) {
                print_expr(std::get<List>(expr->data)[i]);
                if (i < std::get<List>(expr->data).size() - 1)
                    std::cout << " ";
            }
            std::cout << ")";
            break;
    }
}

// Builtins
std::shared_ptr<Env> standard_env() {
    auto env = std::make_shared<Env>();
    (*env)["+"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        double sum = 0;
        for (auto& arg : args)
            sum += std::get<double>(arg->data);
        return std::make_shared<Expr>(sum);
    });
    (*env)["-"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        double res = std::get<double>(args[0]->data);
        for (size_t i = 1; i < args.size(); ++i)
            res -= std::get<double>(args[i]->data);
        return std::make_shared<Expr>(res);
    });
    (*env)["*"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        double res = 1;
        for (auto& arg : args)
            res *= std::get<double>(arg->data);
        return std::make_shared<Expr>(res);
    });
    (*env)["/"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        double res = std::get<double>(args[0]->data);
        for (size_t i = 1; i < args.size(); ++i)
            res /= std::get<double>(args[i]->data);
        return std::make_shared<Expr>(res);
    });
    (*env)[">"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        return std::make_shared<Expr>(std::get<double>(args[0]->data) > std::get<double>(args[1]->data) ? 1.0 : 0.0);
    });
    return env;
}

// REPL
void repl() {
    std::string line;
    auto env = standard_env();

    // Try to preload "stdlib.lisp" if it exists
    std::ifstream stdlib("stdlib.lisp");
    if (stdlib) {
        std::string code((std::istreambuf_iterator<char>(stdlib)), {});
        auto exprs = parse_all(code);
        for (auto& expr : exprs)
            eval(expr, env);
    }

    std::cout << "Minimal LISP in C++ (type 'exit' to quit)" << std::endl;
    while (true) {
        std::cout << "lisp> ";
        if (!getline(std::cin, line))
            break;
        if (line == "exit")
            break;
        try {
#ifdef MULTI_EXPR_PER_LINE
            auto exprs = parse_all(line);
            for (auto& expr : exprs) {
                ExprPtr result = eval(expr, env);
                (*env)["*"] = result;  // Save result to symbol '*'
                if (result->type == Type::Number) {
                    print_expr(result);
                    std::cout << std::endl;
                    //std::cout << result->number << endl;
                } else if (result->type == Type::Symbol) {
                    print_expr(result);
                    std::cout << std::endl;
                    //std::cout << result->symbol << std::endl;
                } else
                    std::cout << "<expr>" << std::endl;
            }
#else
            ExprPtr expr = parse(line);
            ExprPtr result = eval(expr, env);
            if (result->type == Type::Number)
                std::cout << std::get<double>(result->data) << std::endl;
            else if (result->type == Type::Symbol)
                std::cout << std::get<double>(result->data) << std::endl;
            else
                std::cout << "<expr>" << std::endl;
#endif
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main() {
    repl();
    return 0;
}
