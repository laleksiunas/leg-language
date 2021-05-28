#include "io/io.h"
#include "time/time.h"
#include "multithreading/multithreading.h"
#include "memory/memory.h"
#include "codegen/context/BlockContext.h"

namespace library {
    void registerCoreLibraryFunctions(codegen::BlockContext& rootContext) {
        registerIOFunctions(rootContext);
        registerTimeFunctions(rootContext);
        registerPthreadFunctions(rootContext);
        registerMemoryFunctions(rootContext);
    }
}
