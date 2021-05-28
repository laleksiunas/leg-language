#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "codegen/types/TypesContainer.h"
#include "codegen/errors/ErrorsContainer.h"

namespace codegen {
    llvm::LLVMContext& Context() {
        static llvm::LLVMContext context;

        return context;
    }

    llvm::IRBuilder<>& Builder() {
        static llvm::IRBuilder<> builder(Context());

        return builder;
    }

    std::unique_ptr<llvm::Module>& Module() {
        static std::unique_ptr<llvm::Module> moduleRef = std::make_unique<llvm::Module>("Leg", Context());

        return moduleRef;
    }

    codegen::TypesContainer& Types() {
        static codegen::TypesContainer container;

        return container;
    }

    codegen::ErrorsContainer& Errors() {
        static codegen::ErrorsContainer container(Types());

        return container;
    }
}
