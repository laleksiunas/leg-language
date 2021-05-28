#pragma once

#include "llvm/IR/Constants.h"
#include "codegen/types/abstracts/Type.h"
#include "codegen/types/utils.h"
#include "codegen/keywords.h"

namespace codegen::types {
    llvm::Constant* createConstString(const std::string& value) {
        auto* constCharArray = llvm::ConstantDataArray::getString(Context(), value);
        auto nullTerminatedStringSize = value.size() + 1;
        auto constCharArrayType = llvm::ArrayType::get(
            llvm::IntegerType::getInt8Ty(codegen::Context()),
            nullTerminatedStringSize
        );

        auto* globalStringVariable = new llvm::GlobalVariable(
            *codegen::Module(),
            constCharArrayType,
            true,
            llvm::GlobalValue::PrivateLinkage,
            constCharArray,
            ".str"
        );

        auto* zeroValue = llvm::Constant::getNullValue(createIntType());

        auto pointerOffsets = std::vector<llvm::Constant*> { zeroValue, zeroValue };

        return llvm::ConstantExpr::getGetElementPtr(
            constCharArrayType,
            globalStringVariable,
            pointerOffsets
        );
    }

    llvm::Value* defaultStringValueCreator(llvm::BasicBlock* _) {
        return createConstString("");
    }

    void registerStringType() {
        auto* type = new Type(keywords::String, createStringType());

        type->setDefaultValueCreator(defaultStringValueCreator);

        Types().add(type);
    }
}
