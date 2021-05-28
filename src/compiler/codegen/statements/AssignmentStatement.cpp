#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "ast/statements/AssignmentNode.h"

void ast::AssignmentNode::validateVariable(codegen::BlockContext& blockContext) {
    if (!blockContext.hasVariable(variableName->value)) {
        codegen::Errors().reportUndeclaredVariable(variableName->value, lineNumber);
    }

    auto variableToAssign = blockContext.getVariable(variableName->value);

    if (!variableToAssign.isAllocated || !variableToAssign.isMutable) {
        codegen::Errors().reportAssignmentToImmutableVariable(variableName->value, lineNumber);
    }
}

void ast::AssignmentNode::validateAssignmentTypes(llvm::Type* variableType, llvm::Type* valueType) {
    if (variableType != valueType) {
        codegen::Errors().reportAssignmentTypesMismatch(
            variableName->value,
            variableType,
            valueType,
            lineNumber
        );
    }
}

llvm::Value* ast::AssignmentNode::codegen(codegen::BlockContext& blockContext) {
    validateVariable(blockContext);

    auto* variableToAssign = blockContext.getVariable(variableName->value).value;
    auto* valueToAssign = value->codegen(blockContext);

    auto* variableType = variableToAssign->getType()->getPointerElementType();
    auto* valueType = valueToAssign->getType();

    validateAssignmentTypes(variableType, valueType);

    return new llvm::StoreInst(
        valueToAssign,
        variableToAssign,
        blockContext.getBlock()
    );
}
