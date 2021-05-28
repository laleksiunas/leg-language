#pragma once

#include "print.h"
#include "files.h"
#include "codegen/context/BlockContext.h"

namespace library {
    void registerIOFunctions(codegen::BlockContext& rootContext) {
        registerPrintFunctions(rootContext);
        registerFilesFunctions(rootContext);
    }
}
