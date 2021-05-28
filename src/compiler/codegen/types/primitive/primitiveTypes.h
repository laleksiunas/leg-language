#pragma once

#include "./integer.h"
#include "./float.h"
#include "./string.h"
#include "./boolean.h"

namespace codegen {
    void registerPrimitiveTypes() {
        types::registerIntegerType();
        types::registerFloatType();
        types::registerStringType();
        types::registerBooleanType();
    }
}
