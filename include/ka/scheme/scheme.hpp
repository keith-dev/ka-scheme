#pragma once

#include "ka/scheme/types.hpp"

#include <iosfwd>

namespace ka::scheme {
class Engine {
public:
    Engine();
    virtual ~Engine() = default;
    virtual bool evaluate(std::istream& cin, std::ostream& cout, std::ostream& cerr);

protected:
    static Env standard_env();

private:
    Env env_;
};
}  // ka::scheme
