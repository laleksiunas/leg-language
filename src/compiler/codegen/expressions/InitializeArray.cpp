#include <vector>
#include <set>
#include <numeric>
#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "ast/expressions/InitializeArrayNode.h"
#include "ast/expressions/CreateArrayNode.h"
#include "codegen/types/primitive/array.h"
#include "codegen/types/abstracts/ArrayType.h"

codegen::ArrayType* ast::InitializeArrayNode::resolveArrayType() {
    ast::CreateArrayNode::validateArrayTypeName(arrayTypeDeclaration, lineNumber);

    auto* arrayType = (codegen::ArrayType*) arrayTypeDeclaration->resolve();

    if (arrayType == nullptr) {
        codegen::Errors().reportUndeclaredType(arrayTypeDeclaration->getName(), lineNumber);
    }

    return arrayType;
}

std::vector<llvm::Value*> ast::InitializeArrayNode::createArrayValues(codegen::BlockContext& blockContext) {
    std::vector<llvm::Value*> arrayValues;
    std::set<llvm::Type*> valueTypes;

    arrayValues.reserve(values->size());

    for (auto* valueNode : *values) {
        auto* value = valueNode->codegen(blockContext);

        valueTypes.insert(value->getType());
        arrayValues.push_back(value);
    }

    if (valueTypes.size() > 1) {
        auto receivedTypesText = std::accumulate(
            valueTypes.begin(),
            valueTypes.end(),
            std::string(),
            [](std::string& acc, llvm::Type* type)
            {
                auto typeName = codegen::Types().getByType(type)->getName();

                return acc.empty() ? acc.append(typeName) : acc.append(", ").append(typeName);
            }
        );

        codegen::Errors().report(
            fmt::format("Expected values of a single type, got types: [{}]", receivedTypesText),
            lineNumber
        );
    }

    return arrayValues;
}

void ast::InitializeArrayNode::validateArrayTypeWithValuesTypes(
    codegen::ArrayType* arrayType,
    std::vector<llvm::Value*> arrayValues
) {
    if (arrayValues.empty()) {
        return;
    }

    auto* arrayValuesType = arrayValues.front()->getType();

    if (arrayType->getElementType()->getType() != arrayValuesType) {
        auto arrayValuesTypeName = codegen::Types().getByType(arrayValuesType)->getName();

        codegen::Errors().report(
            fmt::format(
                "Expected array values to be of type '{}', got: '{}'",
                arrayType->getElementType()->getName(),
                arrayValuesTypeName
            ),
            lineNumber
        );
    }
}

llvm::Value* ast::InitializeArrayNode::codegen(codegen::BlockContext& blockContext) {
    auto arrayValues = createArrayValues(blockContext);
    auto* arrayType = resolveArrayType();

    validateArrayTypeWithValuesTypes(arrayType, arrayValues);

    return codegen::types::createConstArrayStruct(arrayType, arrayValues, blockContext.getBlock());
}
