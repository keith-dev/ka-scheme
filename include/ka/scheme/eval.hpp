#pragma once

#include "ka/scheme/fwd.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ka::scheme {

enum class Type { Number, Symbol, List, Function, Lambda };
using Number = long double;
using Symbol = std::string;
using List = std::vector<ExprPtr>;
using Function = std::function<ExprPtr(const List&)>;
using Env = std::map<Symbol, ExprPtr>;
struct Lambda {
    std::vector<Symbol> params;
    ExprPtr body;
    std::shared_ptr<Env> closure;
};
using ExpressionTypes = std::variant<Number, Symbol, List, Function, Lambda>;

struct Expr : public ExpressionTypes {
    using inherited = ExpressionTypes;
    Type type;

    Expr(Number n) : ExpressionTypes(n), type(Type::Number) {}
    Expr(Symbol s) : ExpressionTypes(std::move(s)), type(Type::Symbol) {}
    Expr(List list) : ExpressionTypes(std::move(list)), type(Type::List) {}
    Expr(Function func) : ExpressionTypes(func), type(Type::Function) {}
    Expr(std::vector<Symbol> params, ExprPtr body, std::shared_ptr<Env> closure)
        : ExpressionTypes(Lambda{std::move(params), std::move(body), std::move(closure)}), type(Type::Lambda) {}
};

// Forward declarations
ExprPtr eval(ExprPtr, std::shared_ptr<Env>);
ExprPtr eval_list(const List& list, std::shared_ptr<Env> env);

}  // namespace ka::scheme
