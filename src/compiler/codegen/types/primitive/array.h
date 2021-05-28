#pragma once

#include <string>
#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "codegen/types/abstracts/ArrayType.h"

namespace codegen::types {
    const int ARRAY_STRUCT_ARRAY_INDEX = 0;
    const int ARRAY_STRUCT_LENGTH_INDEX = 1;

    std::string createArrayTypeName(const std::string& elementTypeName) {
        return fmt::format("{}[]", elementTypeName);
    }

    llvm::Type* createArrayStructType(codegen::Type* elementType) {
        std::vector<llvm::Type*> memberTypes(2);

        memberTypes[ARRAY_STRUCT_ARRAY_INDEX] = elementType->getType()->getPointerTo();
        memberTypes[ARRAY_STRUCT_LENGTH_INDEX] = createIntType();

        return llvm::StructType::create(
            Context(),
            llvm::makeArrayRef(memberTypes),
            fmt::format("{}_array", elementType->getName())
        );
    }

    llvm::Value* createConstArrayWithDefaultValues(codegen::ArrayType* arrayType, size_t length, llvm::BasicBlock* block) {
        std::vector<llvm::Constant*> values(
            length,
            llvm::Constant::getNullValue(arrayType->getElementType()->getType())
        );

        auto* elementType = arrayType->getElementType()->getType();
        auto* array = llvm::ConstantArray::get(llvm::ArrayType::get(elementType, length), llvm::makeArrayRef(values));

        auto* variable = new llvm::GlobalVariable(
            *Module(),
            array->getType(),
            true,
            llvm::GlobalValue::PrivateLinkage,
            array,
            ""
        );

        return new llvm::BitCastInst(variable, elementType->getPointerTo(), "", block);
    }

    llvm::Value* createConstArray(
        codegen::ArrayType* arrayType,
        std::vector<llvm::Value*>& values,
        llvm::BasicBlock* block
    ) {
        auto* array = createConstArrayWithDefaultValues(arrayType, values.size(), block);

        for (int i = 0; i < values.size(); i++) {
            auto pointerOffsets = std::vector<llvm::Value*> {
                llvm::ConstantInt::get(createIntType(), i)
            };

            auto* pointer = llvm::GetElementPtrInst::CreateInBounds(array, pointerOffsets, "", block);

            new llvm::StoreInst(values[i], pointer, block);
        }

        return array;
    }

    llvm::Value* createConstArray(codegen::ArrayType* arrayType, size_t length, llvm::BasicBlock* block) {
        auto defaultValueCreator = arrayType->getElementType()->getDefaultValueCreator();

        if (!defaultValueCreator.has_value()) {
            return createConstArrayWithDefaultValues(arrayType, length, block);
        }

        std::vector<llvm::Value*> arrayValues(length, defaultValueCreator.value()(block));

        return createConstArray(arrayType, arrayValues, block);
    }

    llvm::GetElementPtrInst* getArrayPointer(llvm::BasicBlock* block, llvm::Value* arrayStruct) {
        auto pointerOffsets = std::vector<llvm::Value*> {
            llvm::ConstantInt::get(createIntType(), 0),
            llvm::ConstantInt::get(createIntType(), ARRAY_STRUCT_ARRAY_INDEX)
        };

        return llvm::GetElementPtrInst::Create(
            arrayStruct->getType()->getPointerElementType(),
            arrayStruct,
            llvm::makeArrayRef(pointerOffsets),
            "",
            block
        );
    }

    llvm::GetElementPtrInst* getArrayLengthPointer(llvm::BasicBlock* block, llvm::Value* arrayStruct) {
        auto pointerOffsets = std::vector<llvm::Value*> {
            llvm::ConstantInt::get(createIntType(), 0),
            llvm::ConstantInt::get(createIntType(), ARRAY_STRUCT_LENGTH_INDEX)
        };

        return llvm::GetElementPtrInst::Create(
            arrayStruct->getType()->getPointerElementType(),
            arrayStruct,
            llvm::makeArrayRef(pointerOffsets),
            "",
            block
        );
    }

    llvm::GetElementPtrInst* getElementPointer(llvm::BasicBlock* block, llvm::Value* arrayStruct, llvm::Value* index) {
        auto* arrayPointer = getArrayPointer(block, arrayStruct);

        auto* array = new llvm::LoadInst(
            arrayPointer->getType()->getPointerElementType(),
            arrayPointer,
            "",
            block
        );

        auto pointerOffsets = std::vector<llvm::Value*> { index };

        return llvm::GetElementPtrInst::CreateInBounds(
            array->getType()->getPointerElementType(),
            array,
            llvm::makeArrayRef(pointerOffsets),
            "",
            block
        );
    }

    llvm::Value* loadStructValueToVariable(llvm::BasicBlock* block, llvm::Value* arrayStruct) {
        auto* arrayStructVariable = new llvm::AllocaInst(arrayStruct->getType(), 0, "", block);

        new llvm::StoreInst(arrayStruct, arrayStructVariable, block);

        return arrayStructVariable;
    }

    llvm::Value* createConstArrayStruct(
        codegen::ArrayType* arrayType,
        llvm::Value* arrayPointer,
        size_t length,
        llvm::BasicBlock* block
    ) {
        auto* arrayLengthConst = llvm::ConstantInt::get(createIntType(), length);

        auto* arrayStructVariable = new llvm::AllocaInst(arrayType->getType(), 0, "", block);

        new llvm::StoreInst(arrayPointer, getArrayPointer(block, arrayStructVariable), block);
        new llvm::StoreInst(arrayLengthConst, getArrayLengthPointer(block, arrayStructVariable), block);

        return new llvm::LoadInst(arrayType->getType(), arrayStructVariable, "", block);
    }

    llvm::Value* createConstArrayStruct(codegen::ArrayType* arrayType, int length, llvm::BasicBlock* block) {
        auto* arrayPointer = createConstArray(arrayType, length, block);

        return createConstArrayStruct(arrayType, arrayPointer, length, block);
    }

    llvm::Value* createConstArrayStruct(
        codegen::ArrayType* arrayType,
        std::vector<llvm::Value*>& values,
        llvm::BasicBlock* block
    ) {
        auto* arrayPointer = createConstArray(arrayType, values, block);

        return createConstArrayStruct(arrayType, arrayPointer, values.size(), block);
    }

    llvm::Value* arrayGetterIndexer(llvm::BasicBlock* block, llvm::Value* left, llvm::Value* index) {
        auto* arrayValue = loadStructValueToVariable(block, left);
        auto* elementPointer = getElementPointer(block, arrayValue, index);

        return new llvm::LoadInst(elementPointer->getType()->getPointerElementType(), elementPointer, "", block);
    }

    llvm::Value* arraySetterIndexer(
        llvm::BasicBlock* block,
        llvm::Value* left,
        llvm::Value* index,
        llvm::Value* right
    ) {
        auto* arrayValue = loadStructValueToVariable(block, left);
        auto* elementPointer = getElementPointer(block, arrayValue, index);

        return new llvm::StoreInst(right, elementPointer, block);
    }

    llvm::Value* arrayLengthProperty(llvm::BasicBlock* block, llvm::Value* array) {
        auto* arrayValue = loadStructValueToVariable(block, array);
        auto* lengthPointer = getArrayLengthPointer(block, arrayValue);

        return new llvm::LoadInst(lengthPointer->getType()->getPointerElementType(), lengthPointer, "", block);
    }

    std::function<llvm::Value*(llvm::BasicBlock*)> createDefaultArrayValueCreator(codegen::ArrayType* arrayType) {
        return [arrayType](llvm::BasicBlock* block) { return createConstArrayStruct(arrayType, 0, block); };
    }

    codegen::ArrayType* registerArrayType(codegen::Type* elementType) {
        auto arrayTypeName = createArrayTypeName(elementType->getName());

        if (Types().has(arrayTypeName)) {
            return (codegen::ArrayType*) Types().getByName(arrayTypeName);
        }

        auto* arrayStructType = createArrayStructType(elementType);

        auto* type = new ArrayType(
            arrayTypeName,
            arrayStructType,
            elementType
        );

        Types().add(type);

        type->setDefaultValueCreator(createDefaultArrayValueCreator(type));

        type->addGetterIndexer(createIntType(), Indexer<GetterIndexer> { elementType->getType(), arrayGetterIndexer });
        type->addSetterIndexer(createIntType(), Indexer<SetterIndexer> { elementType->getType(), arraySetterIndexer });

        type->addGetterProperty("length", arrayLengthProperty);

        return type;
    }
}
