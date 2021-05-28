#pragma once

#include "llvm/IR/Type.h"
#include "codegen/context/context.h"

namespace codegen {
    llvm::Type* createArchIntegerType() {
        auto pointerSize = Module()->getDataLayout().getPointerSize();

        return pointerSize == 8
            ? llvm::Type::getInt64Ty(Context())
            : llvm::Type::getInt32Ty(Context());
    }

    llvm::Type* createVoidType() {
        return llvm::Type::getVoidTy(Context());
    }

    llvm::Type* createBooleanType() {
        return llvm::Type::getInt1Ty(Context());
    }

    llvm::Type* createIntType() {
        return llvm::Type::getInt32Ty(Context());
    }

    llvm::Type* createFloatType() {
        return llvm::Type::getDoubleTy(Context());
    }

    llvm::Type* createStringType() {
        return llvm::Type::getInt8PtrTy(Context());
    }

    llvm::Value* createNullptr() {
        return llvm::ConstantPointerNull::get(createVoidType()->getPointerTo());
    }
}
