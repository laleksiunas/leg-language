#pragma once

#include "codegen/context/BlockContext.h"
#include "sleep.h"

namespace library {
    void registerTimeFunctions(codegen::BlockContext& rootContext) {
        registerSleepFunction(rootContext);
    }
}