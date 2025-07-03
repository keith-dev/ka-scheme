#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ka::scheme {
struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

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
}  // namespace ka::scheme
