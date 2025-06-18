#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <cctype>

using namespace std;

struct Expr;
using ExprPtr = shared_ptr<Expr>;
using Env = map<string, ExprPtr>;
using BuiltinFunc = function<ExprPtr(const vector<ExprPtr>&)>;

enum class Type { Number, Symbol, List, Function, Lambda };

struct Expr {
    Type type;
    double number;
    string symbol;
    vector<ExprPtr> list;
    BuiltinFunc func;
    vector<string> params;
    ExprPtr body;
    shared_ptr<Env> closure;

    Expr(double n) : type(Type::Number), number(n) {}
    Expr(const string& s) : type(Type::Symbol), symbol(s) {}
    Expr(const vector<ExprPtr>& l) : type(Type::List), list(l) {}
    Expr(BuiltinFunc f) : type(Type::Function), func(f) {}
    Expr(const vector<string>& p, ExprPtr b, shared_ptr<Env> c)
        : type(Type::Lambda), params(p), body(b), closure(c) {}
};

// Forward declarations
ExprPtr eval(ExprPtr, shared_ptr<Env>);
ExprPtr parse(const string& input);
vector<ExprPtr> parse_all(const string& input);

// Parser
istringstream tokens;

string next_token() {
    string tok;
    char c;
    while (tokens.get(c)) {
        if (isspace(c)) continue;
        //if (c == ';') { tokens.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        if (c == ';') { std::string line; std::getline(tokens, line); continue; }
        if (c == '(' || c == ')') return string(1, c);
        tokens.putback(c);
        tokens >> tok;
        return tok;
    }
    return "";
}

ExprPtr read_expr() {
    string tok = next_token();
    if (tok.empty()) throw runtime_error("Unexpected EOF");
    if (tok == "(") {
        vector<ExprPtr> list;
        while (true) {
            tok = next_token();
            if (tok == ")") break;
            //tokens.putback(' ');
            //tokens.str(tok + tokens.str());
            list.push_back(read_expr());
        }
        return make_shared<Expr>(list);
    } else if (isdigit(tok[0]) || (tok[0] == '-' && tok.size() > 1)) {
        return make_shared<Expr>(stod(tok));
    } else {
        return make_shared<Expr>(tok);
    }
}

ExprPtr parse(const string& input) {
    tokens.clear();
    tokens.str(input);
    return read_expr();
}

vector<ExprPtr> parse_all(const string& input) {
    tokens.clear();
    tokens.str(input);
    vector<ExprPtr> exprs;
    while (tokens.peek() != EOF) {
        while (isspace(tokens.peek())) tokens.get(); // skip whitespace
        if (tokens.peek() == EOF) break;
        exprs.push_back(read_expr());
    }
    return exprs;
}

// Evaluator
ExprPtr eval_list(const vector<ExprPtr>& list, shared_ptr<Env> env) {
    if (list.empty()) return make_shared<Expr>(list);
    string op = list[0]->symbol;

    if (op == "quote") {
        return list[1];
    } else if (op == "if") {
        auto cond = eval(list[1], env);
        return eval((cond->number != 0) ? list[2] : list[3], env);
    } else if (op == "define") {
        string var = list[1]->symbol;
        ExprPtr val = eval(list[2], env);
        (*env)[var] = val;
        return val;
    } else if (op == "lambda") {
        vector<string> params;
        for (auto& p : list[1]->list)
            params.push_back(p->symbol);
        return make_shared<Expr>(params, list[2], env);
    } else if (op == "load") {
        string filename = list[1]->symbol;
        ifstream infile(filename);
        if (!infile) throw runtime_error("Cannot open file: " + filename);
        string code((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
        auto exprs = parse_all(code);
        ExprPtr result;
        for (auto& expr : exprs)
            result = eval(expr, env);
        return result;
    } else {
        ExprPtr proc = eval(list[0], env);
        vector<ExprPtr> args;
        for (size_t i = 1; i < list.size(); ++i)
            args.push_back(eval(list[i], env));
        if (proc->type == Type::Function) {
            return proc->func(args);
        } else if (proc->type == Type::Lambda) {
            auto new_env = make_shared<Env>(*proc->closure);
            for (size_t i = 0; i < proc->params.size(); ++i)
                (*new_env)[proc->params[i]] = args[i];
            return eval(proc->body, new_env);
        } else {
            throw runtime_error("Not a function");
        }
    }
}

ExprPtr eval(ExprPtr expr, shared_ptr<Env> env) {
    switch (expr->type) {
        case Type::Symbol:
            if (env->count(expr->symbol))
                return (*env)[expr->symbol];
            throw runtime_error("Unbound symbol: " + expr->symbol);
        case Type::Number:
        case Type::Function:
        case Type::Lambda:
            return expr;
        case Type::List:
            return eval_list(expr->list, env);
    }
    throw runtime_error("Unknown expression type");
}

void print_expr(ExprPtr expr) {
    switch (expr->type) {
        case Type::Number:
            cout << expr->number;
            break;
        case Type::Symbol:
            cout << expr->symbol;
            break;
        case Type::Function:
            cout << "<builtin-function>";
            break;
        case Type::Lambda:
            cout << "<lambda>";
            break;
        case Type::List:
            cout << "(";
            for (size_t i = 0; i < expr->list.size(); ++i) {
                print_expr(expr->list[i]);
                if (i < expr->list.size() - 1) cout << " ";
            }
            cout << ")";
            break;
    }
}

// Builtins
shared_ptr<Env> standard_env() {
    auto env = make_shared<Env>();
    (*env)["+"] = make_shared<Expr>([](const vector<ExprPtr>& args) {
        double sum = 0;
        for (auto& a : args) sum += a->number;
        return make_shared<Expr>(sum);
    });
    (*env)["-"] = make_shared<Expr>([](const vector<ExprPtr>& args) {
        double res = args[0]->number;
        for (size_t i = 1; i < args.size(); ++i)
            res -= args[i]->number;
        return make_shared<Expr>(res);
    });
    (*env)["*"] = make_shared<Expr>([](const vector<ExprPtr>& args) {
        double res = 1;
        for (auto& a : args) res *= a->number;
        return make_shared<Expr>(res);
    });
    (*env)["/"] = make_shared<Expr>([](const vector<ExprPtr>& args) {
        double res = args[0]->number;
        for (size_t i = 1; i < args.size(); ++i)
            res /= args[i]->number;
        return make_shared<Expr>(res);
    });
    (*env)[">"] = make_shared<Expr>([](const vector<ExprPtr>& args) {
        return make_shared<Expr>(args[0]->number > args[1]->number ? 1.0 : 0.0);
    });
    return env;
}

// REPL
void repl() {
    string line;
    auto env = standard_env();

    // Try to preload "stdlib.lisp" if it exists
    ifstream stdlib("stdlib.lisp");
    if (stdlib) {
        string code((istreambuf_iterator<char>(stdlib)), {});
        auto exprs = parse_all(code);
        for (auto& expr : exprs)
            eval(expr, env);
    }

    cout << "Minimal LISP in C++ (type 'exit' to quit)" << endl;
    while (true) {
        cout << "lisp> ";
        if (!getline(cin, line)) break;
        if (line == "exit") break;
        try {
            //ExprPtr expr = parse(line);
            //ExprPtr result = eval(expr, env);
            auto exprs = parse_all(line);
            for (auto& expr : exprs) {
                ExprPtr result = eval(expr, env);
                (*env)["*"] = result;  // Save result to symbol '*'
                if (result->type == Type::Number) {
                    print_expr(result);
                    cout << endl;
                    //cout << result->number << endl;
                } else if (result->type == Type::Symbol) {
                    print_expr(result);
                    cout << endl;
                    //cout << result->symbol << endl;
                } else
                    cout << "<expr>" << endl;
            }
/*
            if (result->type == Type::Number)
                cout << result->number << endl;
            else if (result->type == Type::Symbol)
                cout << result->symbol << endl;
            else
                cout << "<expr>" << endl;
 */
        } catch (exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}

int main() {
    repl();
    return 0;
}

