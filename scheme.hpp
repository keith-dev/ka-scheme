#pragma once

#include "fwd.hpp"

#include <iosfwd>

namespace ka::scheme {

// Builtins
//std::shared_ptr<Env> standard_env();

// Read Evaluate Print Loop
void repl(std::istream& is, std::ostream& os);

}  // ka::scheme
