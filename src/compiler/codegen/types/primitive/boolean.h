#pragma once

#include "codegen/types/abstracts/Type.h"
#include "codegen/types/utils.h"
#include "codegen/keywords.h"
#include "codegen/operators.h"
#include "codegen/context/context.h"

namespace codegen::types {
    llvm::Value* boolAndBoolOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateAnd(left, right);
    }

    llvm::Value* boolOrBoolOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateOr(left, right);
    }

    llvm::Value* boolEqualBoolOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpEQ(left, right);
    }

    llvm::Value* boolNotEqualBoolOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpNE(left, right);
    }

    void registerBooleanType() {
        auto* type = new Type(keywords::Boolean, createBooleanType());

        type->addOperator(operators::And, createBooleanType(), boolAndBoolOperator);
        type->addOperator(operators::Or, createBooleanType(), boolOrBoolOperator);
        type->addOperator(operators::Equal, createBooleanType(), boolEqualBoolOperator);
        type->addOperator(operators::NotEqual, createBooleanType(), boolNotEqualBoolOperator);

        Types().add(type);
    }
}
