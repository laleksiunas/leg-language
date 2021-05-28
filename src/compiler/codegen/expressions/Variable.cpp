#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "codegen/context/BlockContext.h"
#include "ast/expressions/VariableNode.h"

llvm::Value* ast::VariableNode::codegen(codegen::BlockContext& blockContext) {
    if (!blockContext.hasVariable(name->value)) {
        codegen::Errors().reportUndeclaredVariable(name->value, lineNumber);
    }

    auto variable = blockContext.getVariable(name->value);
    auto value = variable.value;

    if (variable.isAllocated) {
        return new llvm::LoadInst(value->getType()->getPointerElementType(), value, "", blockContext.getBlock());
    }

    return value;
}
