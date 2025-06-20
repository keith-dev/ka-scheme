#pragma once

#include "cpp-lisp-fwd.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ka::common_lisp {

using List = std::vector<ExprPtr>;
using Env = std::map<std::string, ExprPtr>;
using Function = std::function<ExprPtr(const List&)>;
struct Lambda {
    std::vector<std::string> params;
    ExprPtr body;
    std::shared_ptr<Env> closure;
};

enum class Type { Number, Symbol, List, Function, Lambda };
using Number = double;
using Symbol = std::string;
using ExpressionTypes = std::variant<Number, Symbol, List, Function, Lambda>;

struct Expr : public ExpressionTypes {
    using inherited = ExpressionTypes;
    Type type;

    Expr(double n) : ExpressionTypes(n), type(Type::Number) {}
    Expr(std::string s) : ExpressionTypes(std::move(s)), type(Type::Symbol) {}
    Expr(List list) : ExpressionTypes(std::move(list)), type(Type::List) {}
    Expr(Function func) : ExpressionTypes(func), type(Type::Function) {}
    Expr(std::vector<std::string> params, ExprPtr body, std::shared_ptr<Env> closure)
        : ExpressionTypes(Lambda{std::move(params), std::move(body), std::move(closure)}), type(Type::Lambda) {}
};

// Forward declarations
ExprPtr eval(ExprPtr, std::shared_ptr<Env>);
ExprPtr eval_list(const List& list, std::shared_ptr<Env> env);

}  // namespace ka::common_lisp
