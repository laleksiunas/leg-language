#pragma once

#include "llvm/IR/Value.h"

namespace codegen {
    struct Variable {
        llvm::Value* value;
        bool isAllocated;
        bool isMutable;

        static Variable empty() {
            return Variable();
        }
    };
}
