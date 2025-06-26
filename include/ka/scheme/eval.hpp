#pragma once

#include "ka/scheme/fwd.hpp"

#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ka::scheme {

using Number = long double;
using Symbol = std::string;
using List = std::vector<ExprPtr>;
using Function = std::function<ExprPtr(const List&)>;
using Env = std::map<Symbol, ExprPtr>;
struct Lambda {
    std::vector<Symbol> params;
    ExprPtr body;
    Env closure;
};
using ExpressionTypes = std::variant<Symbol, Number, List, Function, Lambda>;

struct Expr : public ExpressionTypes {
    using inherited = ExpressionTypes;

    Expr(Symbol s) noexcept : ExpressionTypes(std::move(s)) {}
    Expr(Number n) noexcept : ExpressionTypes(n) {}
    Expr(List list) noexcept : ExpressionTypes(std::move(list)) {}
    Expr(Function func) noexcept : ExpressionTypes(func) {}
    Expr(std::vector<Symbol> params, ExprPtr body, Env closure) noexcept
        : ExpressionTypes(Lambda{std::move(params), std::move(body), std::move(closure)}) {}
};

// Forward declarations
template <class... Ts>
struct ExprOverloads : Ts... { using Ts::operator()...; };

ExprPtr eval(ExprPtr&, Env&);
ExprPtr eval_list(List& list, Env& env);
std::ostream& print_expr(std::ostream& cout, ExprPtr expr);

}  // namespace ka::scheme
