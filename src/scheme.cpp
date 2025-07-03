#include "ka/scheme/scheme.hpp"
#include "ka/scheme/eval.hpp"
#include "ka/scheme/tokenizer.hpp"

#include <fstream>
#include <istream>
#include <ostream>

namespace ka::scheme {
Engine::Engine() : env_{standard_env()} {
    // Try to preload "stdlib.lisp" if it exists
    if (std::ifstream stdlib{"etc/stdlib.lisp"}) {
        std::string code((std::istreambuf_iterator<char>(stdlib)), {});
        Tokenizer tokenizer;
        List exprs = tokenizer.parse_all(std::move(code));
        for (auto& expr : exprs)
            eval(expr, env_);
    }
}

// Builtins
Env Engine::standard_env() {
    Env env;
    env["+"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number sum{};
        for (auto& arg : args)
            sum += std::get<Number>(*arg);
        return std::make_shared<Expr>(sum);
    });
    env["-"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = std::get<Number>(*args[0]);
        for (size_t i = 1; i < args.size(); ++i)
            res -= std::get<Number>(*args[i]);
        return std::make_shared<Expr>(res);
    });
    env["*"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = 1;
        for (auto& arg : args)
            res *= std::get<Number>(*arg);
        return std::make_shared<Expr>(res);
    });
    env["/"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = std::get<Number>(*args[0]);
        for (size_t i = 1; i < args.size(); ++i)
            res /= std::get<Number>(*args[i]);
        return std::make_shared<Expr>(res);
    });
    env[">"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        return std::make_shared<Expr>(std::get<Number>(*args[0]) > std::get<Number>(*args[1]) ? 1.0 : 0.0);
    });
    return env;
}

bool Engine::evaluate(std::istream& cin, std::ostream& cout, std::ostream& cerr) {
    try {
        std::string line((std::istreambuf_iterator<char>(cin)), {});

        Tokenizer tokenizer;
        auto exprs = tokenizer.parse_all(std::move(line));
        for (auto& expr : exprs) {
            ExprPtr result = eval(expr, env_);
            //env["**"] = result;  // Save result to symbol '**'
            print_expr(cout, result) << std::endl;
        }
        return true;
    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << std::endl;
    }
    return {};
}
}  // ka::scheme
