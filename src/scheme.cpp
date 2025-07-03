#include "ka/scheme/fwd.hpp"
#include "ka/scheme/eval.hpp"
#include "ka/scheme/tokenizer.hpp"

#include "../contrib/linenoise/linenoise.h"

#include <cstring>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream> // std::cerr

namespace ka::scheme {
// Builtins
Env standard_env() {
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

// Read Evaluate Print Loop
void repl() {
    Env env = standard_env();

    // Try to preload "stdlib.lisp" if it exists
    if (std::ifstream stdlib{"etc/stdlib.lisp"}) {
        std::string code((std::istreambuf_iterator<char>(stdlib)), {});
        Tokenizer tokenizer;
        List exprs = tokenizer.parse_all(std::move(code));
        for (auto& expr : exprs)
            eval(expr, env);
    }

    using unique_malloc_ptr = std::unique_ptr<char, decltype(&::free)>;
    unique_malloc_ptr buf = {nullptr, ::free};
    unique_malloc_ptr last = {nullptr, ::free};
    linenoiseHistorySetMaxLen(1000);
    linenoiseHistoryLoad(".ka-scheme.history");
    while (true) {
        buf.reset(linenoise("lisp> "));
        if (!buf.get())
            break;
        std::string line = buf.get();
        if (line == "exit")
            break;

        try {
            Tokenizer tokenizer;
            auto exprs = tokenizer.parse_all(std::move(line));
            for (auto& expr : exprs) {
                ExprPtr result = eval(expr, env);
                //env["**"] = result;  // Save result to symbol '**'
                print_expr(std::cout, result) << std::endl;
            }
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // save history without duplicates
        if (!last.get() || std::strcmp(last.get(), buf.get())) {
            linenoiseHistoryAdd(buf.get());
        }
        std::swap(buf, last);
    }
    linenoiseHistorySave(".ka-scheme.history");
}
}  // ka::scheme
