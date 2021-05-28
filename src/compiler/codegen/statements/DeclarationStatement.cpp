#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "ast/statements/DeclarationNode.h"
#include "codegen/keywords.h"

void ast::DeclarationNode::validateIdentifier(
    const std::string& name,
    codegen::BlockContext& blockContext,
    int lineNumber
) {
    if (codegen::keywords::isReservedKeyword(name)) {
        codegen::Errors().reportUseOfKeywordForIdentifier(name, lineNumber);
    }

    if (blockContext.hasIdentifier(name)) {
        codegen::Errors().reportIdentifierRedeclaration(name, lineNumber);
    }
}

void ast::DeclarationNode::validateType(llvm::Type* receivedType) {
    if (type->isLet()) {
        return;
    }

    if (type->isVoid()) {
        codegen::Errors().report(fmt::format("Variable cannot be of type {}", codegen::keywords::Void), lineNumber);
    }

    auto* expectedType = type->resolve();

    if (expectedType == nullptr) {
        return codegen::Errors().reportUndeclaredType(type->getName(), lineNumber);
    }

    if (expectedType->getType() != receivedType) {
        codegen::Errors().reportAssignmentTypesMismatch(
            variableName->value,
            expectedType->getType(),
            receivedType,
            lineNumber
        );
    }
}

llvm::Value* ast::DeclarationNode::codegen(codegen::BlockContext& blockContext) {
    validateIdentifier(variableName->value, blockContext, lineNumber);

    auto* variableValue = value->codegen(blockContext);

    validateType(variableValue->getType());

    return codegen::declareValue(blockContext, variableName->value, variableValue);
}
