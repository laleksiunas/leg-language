#include "fmt/format.h"
#include "llvm/IR/Constants.h"
#include "ast/expressions/CreateArrayNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/types/abstracts/ArrayType.h"
#include "codegen/types/primitive/array.h"
#include "codegen/utils.h"

void ast::CreateArrayNode::validateArrayTypeName(ast::TypeNode* type, int lineNumber) {
    if (type->isVoid() || type->isLet()) {
        codegen::Errors().report(
            fmt::format("Type '{}' cannot be used as array element type", type->getName()),
            lineNumber
        );
    }
}

codegen::ArrayType* ast::CreateArrayNode::createArrayType() {
    validateArrayTypeName(elementType, lineNumber);

    auto arrayElementType = elementType->resolve();

    if (arrayElementType == nullptr) {
        codegen::Errors().reportUndeclaredType(elementType->getName(), lineNumber);
    }

    return codegen::types::registerArrayType(arrayElementType);
}

void ast::CreateArrayNode::validateArrayLength() {
    if (length->value < 0) {
        codegen::Errors().report(
            fmt::format("Expected array length to be greater or equal zero, got '{}'", length->value),
            lineNumber
        );
    }
}

llvm::Value* ast::CreateArrayNode::codegen(codegen::BlockContext& blockContext) {
    auto* arrayType = createArrayType();

    validateArrayLength();

    return codegen::types::createConstArrayStruct(arrayType, length->value, blockContext.getBlock());
}
