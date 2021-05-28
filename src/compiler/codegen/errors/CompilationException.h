#pragma once

#include <exception>

namespace codegen {
    struct CompilationException : public std::exception {};
}
