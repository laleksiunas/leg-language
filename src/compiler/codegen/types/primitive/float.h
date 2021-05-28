#pragma once

#include "codegen/types/abstracts/Type.h"
#include "codegen/types/utils.h"
#include "codegen/keywords.h"
#include "codegen/operators.h"
#include "codegen/context/context.h"

namespace codegen::types {
    llvm::Value* floatPlusFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFAdd(left, right);
    }

    llvm::Value* floatMinusFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFSub(left, right);
    }

    llvm::Value* floatMulFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFMul(left, right);
    }

    llvm::Value* floatDivFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFDiv(left, right);
    }

    llvm::Value* floatModFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFRem(left, right);
    }

    llvm::Value* floatPlusIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFAdd(left, castIntToFloat(block, right));
    }

    llvm::Value* floatMinusIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFSub(left, castIntToFloat(block, right));
    }

    llvm::Value* floatMulIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFMul(left, castIntToFloat(block, right));
    }

    llvm::Value* floatDivIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFDiv(left, castIntToFloat(block, right));
    }

    llvm::Value* floatModIntOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFRem(left, castIntToFloat(block, right));
    }

    llvm::Value* floatEqualFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpOEQ(left, right);
    }

    llvm::Value* floatNotEqualFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpONE(left, right);
    }

    llvm::Value* floatGreaterThanFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpOGT(left, right);
    }

    llvm::Value* floatGreaterThanEqualFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpOGE(left, right);
    }

    llvm::Value* floatLessThanFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpOLT(left, right);
    }

    llvm::Value* floatLessThanEqualFloatOperator(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* right) {
        Builder().SetInsertPoint(block);

        return Builder().CreateFCmpOLE(left, right);
    }

    void registerFloatType() {
        auto* type = new Type(keywords::Float, createFloatType());

        type->addOperator(operators::Plus, createFloatType(), floatPlusFloatOperator);
        type->addOperator(operators::Plus, createIntType(), floatPlusIntOperator);

        type->addOperator(operators::Minus, createFloatType(), floatMinusFloatOperator);
        type->addOperator(operators::Minus, createIntType(), floatMinusIntOperator);

        type->addOperator(operators::Multiply, createFloatType(), floatMulFloatOperator);
        type->addOperator(operators::Multiply, createIntType(), floatMulIntOperator);

        type->addOperator(operators::Divide, createFloatType(), floatDivFloatOperator);
        type->addOperator(operators::Divide, createIntType(), floatDivIntOperator);

        type->addOperator(operators::Modulus, createFloatType(), floatModFloatOperator);
        type->addOperator(operators::Modulus, createIntType(), floatModIntOperator);

        type->addOperator(operators::Equal, createFloatType(), floatEqualFloatOperator);
        type->addOperator(operators::NotEqual, createFloatType(), floatNotEqualFloatOperator);
        type->addOperator(operators::Greater, createFloatType(), floatGreaterThanFloatOperator);
        type->addOperator(operators::GreaterEqual, createFloatType(), floatGreaterThanEqualFloatOperator);
        type->addOperator(operators::LessThan, createFloatType(), floatLessThanFloatOperator);
        type->addOperator(operators::LessThanEqual, createFloatType(), floatLessThanEqualFloatOperator);

        Types().add(type);
    }
}
