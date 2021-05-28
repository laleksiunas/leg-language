#pragma once

#include "llvm/IR/Value.h"

namespace shared {
    struct KickResult {
        llvm::Value* threadId;
        llvm::Function* routineFunction;
    };
}
