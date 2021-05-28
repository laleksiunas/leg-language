#pragma once

#include "codegen/context/BlockContext.h"

namespace library {
    const auto PTHREAD_CREATE = "pthread_create";
    const auto PTHREAD_JOIN = "pthread_join";

    namespace multithreading {
        llvm::FunctionType* createRoutineFunctionType() {
            auto arguments = std::vector<llvm::Type*> { codegen::createVoidType()->getPointerTo() };

            return llvm::FunctionType::get(codegen::createVoidType()->getPointerTo(), arguments, false);
        }

        llvm::Function* createPthreadCreateFunction() {
            auto arguments = std::vector<llvm::Type*> {
                codegen::createArchIntegerType()->getPointerTo()->getPointerTo(),
                codegen::createVoidType()->getPointerTo(),
                createRoutineFunctionType()->getPointerTo(),
                codegen::createVoidType()->getPointerTo()
            };

            return codegen::createNativeFunction(PTHREAD_CREATE, codegen::createIntType(), arguments);
        }

        llvm::Function* createPthreadJoinFunction() {
            auto arguments = std::vector<llvm::Type*> {
                codegen::createArchIntegerType()->getPointerTo(),
                codegen::createVoidType()->getPointerTo()->getPointerTo()
            };

            return codegen::createNativeFunction(PTHREAD_JOIN, codegen::createIntType(), arguments);
        }
    }

    void registerPthreadFunctions(codegen::BlockContext& rootContext) {
        rootContext.addPrivateFunction(PTHREAD_CREATE, multithreading::createPthreadCreateFunction());
        rootContext.addPrivateFunction(PTHREAD_JOIN, multithreading::createPthreadJoinFunction());
    }
}
