#pragma once

#include <vector>
#include "llvm/IR/Function.h"
#include "codegen/context/BlockContext.h"

namespace library {
    const auto MALLOC = "malloc";
    const auto FREE = "free";

    namespace memory {
        llvm::Function* createMallocFunction() {
            auto arguments = std::vector<llvm::Type*> { codegen::createIntType() };

            return codegen::createNativeFunction(MALLOC, codegen::createVoidType()->getPointerTo(), arguments);
        }

        llvm::Function* createFreeFunction() {
            auto arguments = std::vector<llvm::Type*> { codegen::createVoidType()->getPointerTo() };

            return codegen::createNativeFunction(FREE, codegen::createVoidType(), arguments);
        }
    }

    void registerMemoryFunctions(codegen::BlockContext& rootContext) {
        rootContext.addPrivateFunction(MALLOC, memory::createMallocFunction());
        rootContext.addPrivateFunction(FREE, memory::createFreeFunction());
    }
}
