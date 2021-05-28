#pragma once

#include <string>
#include <utility>
#include "llvm/IR/Type.h"
#include "Type.h"

namespace codegen {
    class ArrayType : public Type {
    public:
        explicit ArrayType(std::string name, llvm::Type* type, Type* elementType)
            : Type(std::move(name), type), elementType(elementType) {}

        Type* getElementType() {
            return elementType;
        }

    private:
        Type* elementType;
    };
}
