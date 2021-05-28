#pragma once

#include "llvm/IR/Type.h"

namespace codegen {
    template<typename H>
    struct Indexer {
        llvm::Type* returnType;
        H handler;
    };
}
