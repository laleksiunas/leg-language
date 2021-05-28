#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "ast/expressions/FunctionCallNode.h"
#include "codegen/context/context.h"
#include "codegen/context/BlockContext.h"

void ast::FunctionCallNode::validateFunction(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Value*>& callArguments
) {
    if (!blockContext.hasFunction(functionName->value)) {
        auto argumentTypes = codegen::mapValuesToTypes(callArguments);

        codegen::Errors().reportUndeclaredFunctionCall(functionName->value, argumentTypes, lineNumber);
    }

    if (blockContext.getFunction(functionName->value, callArguments) == nullptr) {
        auto availableFunctions = blockContext.getFunctions(functionName->value);
        auto argumentTypes = codegen::mapValuesToTypes(callArguments);

        codegen::Errors().reportUndeclaredFunctionCall(
            functionName->value,
            argumentTypes,
            availableFunctions,
            lineNumber
        );
    }
}

llvm::Value* ast::FunctionCallNode::codegen(codegen::BlockContext& blockContext) {
    auto callArguments = generateFunctionArguments(blockContext, *arguments);

    validateFunction(blockContext, callArguments);

    auto* function = blockContext.getFunction(functionName->value, callArguments);

    return llvm::CallInst::Create(function, llvm::makeArrayRef(callArguments), "", blockContext.getBlock());
}
