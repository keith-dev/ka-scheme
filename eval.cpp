#include "eval.hpp"
#include "tokenizer.hpp"

#include <fstream>
#include <iostream>

namespace ka::common_lisp {

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

ExprPtr eval(ExprPtr expr, std::shared_ptr<Env> env) {
    const auto visitor = overloads{
        [&](Symbol& symbol) -> ExprPtr {
            if (env->count(symbol))
                return (*env)[symbol];
            throw std::runtime_error("Unbound symbol: " + symbol);
        },
        [&](Number&) -> ExprPtr { return expr; },
        [&](Function&) -> ExprPtr  { return expr; },
        [&](Lambda&) -> ExprPtr { return expr; },
        [&](List& list) -> ExprPtr { return eval_list(list, env); }
    };
    return std::visit(visitor, *expr);

/*
    switch (expr->type) {
        case Type::Symbol:
            if (env->count(std::get<Symbol>(*expr)))
                return (*env)[std::get<Symbol>(*expr)];
            throw std::runtime_error("Unbound symbol: " + std::get<Symbol>(*expr));
        case Type::Number:
        case Type::Function:
        case Type::Lambda:
            return expr;
        case Type::List:
            return eval_list(std::get<List>(*expr), env);
    }
    throw std::runtime_error("Unknown expression type");
 */
}

// Evaluator
ExprPtr eval_list(const List& list, std::shared_ptr<Env> env) {
    if (list.empty())
        return std::make_shared<Expr>(list);
    std::string op = std::get<Symbol>(*list[0]);

    if (op == "quote") {
        return list[1];
    } else if (op == "if") {
        ExprPtr cond = eval(list[1], env);
        return eval((std::get<Number>(*cond) != 0) ? list[2] : list[3], env);
    } else if (op == "define") {
        std::string var = std::get<Symbol>(*list[1]);
        ExprPtr val = eval(list[2], env);
        (*env)[var] = val;
        return val;
    } else if (op == "lambda") {
        std::vector<std::string> params;
        for (auto& param : std::get<List>(*list[1]))
            params.push_back(std::get<Symbol>(*param));
        return std::make_shared<Expr>(params, list[2], env);
    } else if (op == "load") {
        std::string filename = std::get<Symbol>(*list[1]);
        std::ifstream infile(filename);
        if (!infile)
            throw std::runtime_error("Cannot open file: " + filename);
        std::string code((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        Tokenizer tokenizer;
        auto exprs = tokenizer.parse_all(std::move(code));
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
            return std::get<Function>(*proc)(args);
        } else if (proc->type == Type::Lambda) {
            auto new_env = std::make_shared<Env>(*std::get<Lambda>(*proc).closure);
            for (size_t i = 0; i < std::get<Lambda>(*proc).params.size(); ++i)
                (*new_env)[std::get<Lambda>(*proc).params[i]] = args[i];
            return eval(std::get<Lambda>(*proc).body, new_env);
        } else {
            throw std::runtime_error("Not a function");
        }
    }
}

void print_expr(ExprPtr expr) {
    switch (expr->type) {
        case Type::Number:
            std::cout << std::get<Number>(*expr);
            break;
        case Type::Symbol:
            std::cout << std::get<Symbol>(*expr);
            break;
        case Type::Function:
            std::cout << "<builtin-function>";
            break;
        case Type::Lambda:
            std::cout << "<lambda>";
            break;
        case Type::List:
            std::cout << "(";
            for (size_t i = 0; i < std::get<List>(*expr).size(); ++i) {
                print_expr(std::get<List>(*expr)[i]);
                if (i < std::get<List>(*expr).size() - 1)
                    std::cout << " ";
            }
            std::cout << ")";
            break;
    }
}

}  // ka::common_lisp
