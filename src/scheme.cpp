#include "ka/scheme/fwd.hpp"
#include "ka/scheme/eval.hpp"
#include "ka/scheme/tokenizer.hpp"

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream> // std::cerr

namespace ka::scheme {
namespace {
bool is_stdcout(std::ostream& cout) {
    return &cout == &std::cout;
}
}  // namespace

// Builtins
std::shared_ptr<Env> standard_env() {
    auto env = std::make_shared<Env>();
    (*env)["+"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number sum{};
        for (auto& arg : args)
            sum += std::get<Number>(*arg);
        return std::make_shared<Expr>(sum);
    });
    (*env)["-"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = std::get<Number>(*args[0]);
        for (size_t i = 1; i < args.size(); ++i)
            res -= std::get<Number>(*args[i]);
        return std::make_shared<Expr>(res);
    });
    (*env)["*"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = 1;
        for (auto& arg : args)
            res *= std::get<Number>(*arg);
        return std::make_shared<Expr>(res);
    });
    (*env)["/"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        Number res = std::get<Number>(*args[0]);
        for (size_t i = 1; i < args.size(); ++i)
            res /= std::get<Number>(*args[i]);
        return std::make_shared<Expr>(res);
    });
    (*env)[">"] = std::make_shared<Expr>([](const std::vector<ExprPtr>& args) {
        return std::make_shared<Expr>(std::get<Number>(*args[0]) > std::get<Number>(*args[1]) ? 1.0 : 0.0);
    });
    return env;
}

// Read Evaluate Print Loop
void repl(std::istream& cin, std::ostream& cout) {
    auto env = standard_env();

    // Try to preload "stdlib.lisp" if it exists
    std::ifstream stdlib("etc/stdlib.lisp");
    if (stdlib) {
        std::string code((std::istreambuf_iterator<char>(stdlib)), {});
        Tokenizer tokenizer;
        auto exprs = tokenizer.parse_all(std::move(code));
        for (auto& expr : exprs)
            eval(expr, env);
    }

    std::string line;
    while (true) {
        if (is_stdcout(cout))
            cout << "lisp> ";
        if (!getline(cin, line))
            break;
        if (line == "exit")
            break;

        try {
            Tokenizer tokenizer;
#ifdef MULTI_EXPR_PER_LINE
            auto exprs = tokenizer.parse_all(std::move(line));
            for (auto& expr : exprs) {
                ExprPtr result = eval(expr, env);
                (*env)["*"] = result;  // Save result to symbol '*'
                if (result->type == Type::Number) {
                    print_expr(result);
                    cout << std::endl;
                    //std::cout << result->number << endl;
                } else if (result->type == Type::Symbol) {
                    print_expr(result);
                    cout << std::endl;
                    //std::cout << result->symbol << std::endl;
                } else
                    cout << "<expr>" << std::endl;
            }
#else
            ExprPtr expr = tokenizer.parse(std::move(line));
            ExprPtr result = eval(expr, env);
            if (result->type == Type::Number)
                cout << std::get<Number>(*result) << std::endl;
            else if (result->type == Type::Symbol)
                cout << std::get<Number>(*result) << std::endl;
            else
                cout << "<expr>" << std::endl;
#endif
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
}  // ka::scheme
