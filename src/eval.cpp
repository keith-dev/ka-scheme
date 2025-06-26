#include "ka/scheme/eval.hpp"
#include "ka/scheme/tokenizer.hpp"

#include <fstream>
#include <iostream>

namespace ka::scheme {

ExprPtr eval(ExprPtr& expr, Env& env) {
    const auto visitor = ExprOverloads{
        [&](Symbol& symbol) -> ExprPtr {
            auto iter = env.find(symbol);
            if (iter != env.end())
                return iter->second;
            throw std::runtime_error("Unbound symbol: " + symbol);
        },
        [&](Number&) -> ExprPtr { return expr; },
        [&](Function&) -> ExprPtr  { return expr; },
        [&](Lambda&) -> ExprPtr { return expr; },
        [&](List& list) -> ExprPtr { return eval_list(list, env); }
    };
    return std::visit(visitor, *expr);
}

// Evaluator
ExprPtr eval_list(List& list, Env& env) {
    if (list.empty())
        return std::make_shared<Expr>(list);
    Symbol op = std::get<Symbol>(*list[0]);

    if (op == "quote") {
        return list[1];
    } else if (op == "if") {
        ExprPtr cond = eval(list[1], env);
        return eval((std::get<Number>(*cond) != 0) ? list[2] : list[3], env);
    } else if (op == "define") {
        Symbol var = std::get<Symbol>(*list[1]);
        ExprPtr val = eval(list[2], env);
        env[var] = val;
        return val;
    } else if (op == "lambda") {
        std::vector<Symbol> params;
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

        const auto visitor = ExprOverloads{
            [&](Symbol&) -> ExprPtr { throw std::runtime_error("Symbol: Not a function"); },
            [&](Number&) -> ExprPtr { throw std::runtime_error("Number: Not a function"); },
            [&](Function& func) -> ExprPtr { return func(args); },
            [&](Lambda& lambda) -> ExprPtr {
                Env& new_env = lambda.closure;
                for (size_t i = 0; i < lambda.params.size(); ++i)
                    new_env[lambda.params[i]] = args[i];
                return eval(lambda.body, new_env);
            },
            [&](List&) -> ExprPtr { throw std::runtime_error("List: Not a function"); }
        };
        return std::visit(visitor, *proc);
    }
    throw std::runtime_error("unexpected return");
}

std::ostream& print_expr(std::ostream& cout, ExprPtr expr) {
    const auto visitor = ExprOverloads{
        [&](Symbol& symbol) { cout << symbol; },
        [&](Number& number) { cout << number; },
        [&](Function&) { cout << "<builtin-function>"; },
        [&](Lambda&) { cout << "<lambda>"; },
        [&](List& list) {
            cout << "(";
            for (size_t i = 0; i < list.size(); ++i) {
                print_expr(cout, list[i]);
                if (i < list.size() - 1)
                    cout << " ";
            }
            cout << ")";
        }
    };
    std::visit(visitor, *expr);
    return cout;
}

}  // ka::scheme
