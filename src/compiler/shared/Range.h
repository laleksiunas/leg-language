#pragma once

#include "llvm/IR/Value.h"

namespace shared {
    struct Range {
        llvm::Value* begin;
        llvm::Value* end;
        llvm::Value* step;
        llvm::Type* type;
    };
}
