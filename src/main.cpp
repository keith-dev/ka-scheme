#include "ka/scheme/scheme.hpp"

#include "../contrib/linenoise/linenoise.h"

#include <cstring>
#include <iostream>
#include <sstream>

int main() {
    ka::scheme::Engine scheme;

    std::cout << "Minimal LISP in C++ (type 'exit' to quit)" << std::endl;

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

        std::stringstream cin{std::move(line)};
        scheme.evaluate(cin, std::cout, std::clog);

        // save history without duplicates
        if (!last.get() || std::strcmp(last.get(), buf.get())) {
            linenoiseHistoryAdd(buf.get());
        }
        std::swap(buf, last);
    }
    linenoiseHistorySave(".ka-scheme.history");
}
