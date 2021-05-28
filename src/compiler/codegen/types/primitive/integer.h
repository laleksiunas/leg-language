#pragma once

#include "codegen/types/abstracts/Type.h"
#include "codegen/types/utils.h"
#include "codegen/keywords.h"
#include "codegen/operators.h"
#include "codegen/context/context.h"

namespace codegen::types {
    llvm::Value* castIntToFloat(llvm::BasicBlock* block, llvm::Value* intValue) {
        return llvm::BitCastInst::Create(
            llvm::Instruction::SIToFP,
            intValue,
            createFloatType(),
            "",
            block
        );
    }

    llvm::Value* intPlusIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateAdd(left, right);
    }

    llvm::Value* intMinusIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateSub(left, right);
    }

    llvm::Value* intMulIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateMul(left, right);
    }

    llvm::Value* intDivIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateSDiv(left, right);
    }

    llvm::Value* intModIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateSRem(left, right);
    }

    llvm::Value* intPlusFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFAdd(castIntToFloat(block, left), right);
    }

    llvm::Value* intMinusFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFSub(castIntToFloat(block, left), right);
    }

    llvm::Value* intMulFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFMul(castIntToFloat(block, left), right);
    }

    llvm::Value* intDivFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFDiv(castIntToFloat(block, left), right);
    }

    llvm::Value* intModFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFRem(castIntToFloat(block, left), right);
    }

    llvm::Value* intEqualIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpEQ(left, right);
    }

    llvm::Value* intNotEqualIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpNE(left, right);
    }

    llvm::Value* intGreaterThanIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpSGT(left, right);
    }

    llvm::Value* intGreaterThanEqualIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpSGE(left, right);
    }

    llvm::Value* intLessThanIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpSLT(left, right);
    }

    llvm::Value* intLessThanEqualIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateICmpSLE(left, right);
    }

    void registerIntegerType() {
        auto* type = new Type(keywords::Integer, createIntType());

        type->addOperator(operators::Plus, createIntType(), intPlusIntOperator);
        type->addOperator(operators::Plus, createFloatType(), intPlusFloatOperator);

        type->addOperator(operators::Minus, createIntType(), intMinusIntOperator);
        type->addOperator(operators::Minus, createFloatType(), intMinusFloatOperator);

        type->addOperator(operators::Multiply, createIntType(), intMulIntOperator);
        type->addOperator(operators::Multiply, createFloatType(), intMulFloatOperator);

        type->addOperator(operators::Divide, createIntType(), intDivIntOperator);
        type->addOperator(operators::Divide, createFloatType(), intDivFloatOperator);

        type->addOperator(operators::Modulus, createIntType(), intModIntOperator);
        type->addOperator(operators::Modulus, createFloatType(), intModFloatOperator);

        type->addOperator(operators::Equal, createIntType(), intEqualIntOperator);
        type->addOperator(operators::NotEqual, createIntType(), intNotEqualIntOperator);
        type->addOperator(operators::Greater, createIntType(), intGreaterThanIntOperator);
        type->addOperator(operators::GreaterEqual, createIntType(), intGreaterThanEqualIntOperator);
        type->addOperator(operators::LessThan, createIntType(), intLessThanIntOperator);
        type->addOperator(operators::LessThanEqual, createIntType(), intLessThanEqualIntOperator);

        Types().add(type);
    }
}
