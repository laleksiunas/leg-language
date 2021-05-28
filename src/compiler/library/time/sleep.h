#pragma once

#include "llvm/IR/Function.h"
#include "codegen/utils.h"
#include "codegen/context/BlockContext.h"

namespace library {
    const auto SLEEP = "sleep";

    void registerSleepFunction(codegen::BlockContext& rootContext) {
        auto sleepArguments = std::vector<llvm::Type*> { llvm::Type::getInt32Ty(codegen::Context()) };

        rootContext.addFunction(
            SLEEP,
            codegen::createNativeFunction(SLEEP, codegen::createVoidType(), sleepArguments)
        );
    }
}
