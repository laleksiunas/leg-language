#include "fmt/format.h"
#include "llvm/IR/Function.h"
#include "ast/statements/FunctionReturnStatementNode.h"
#include "codegen/context/context.h"

llvm::Value* ast::FunctionReturnStatementNode::codegen(codegen::BlockContext& blockContext) {
    auto* wrapperFunction = blockContext.getCurrentFunction();

    if (wrapperFunction == nullptr) {
        codegen::Errors().report("Cannot use return statement outside the function", lineNumber);
    }

    blockContext.setHasReturnValue();

    auto* returnVal = isVoid() ? nullptr : returnValue->codegen(blockContext);
    auto* returnValueType = isVoid() ? codegen::createVoidType() : returnVal->getType();
    auto* functionReturnType = wrapperFunction->getReturnType();

    if (returnValueType != functionReturnType) {
        auto returnValueTypeName = codegen::Types().getName(returnValueType);
        auto functionReturnTypeName = codegen::Types().getName(functionReturnType);

        codegen::Errors().report(
            fmt::format("Expected return value of type '{}', got '{}'", functionReturnTypeName, returnValueTypeName),
            lineNumber
        );
    }

    return isVoid()
        ? llvm::ReturnInst::Create(codegen::Context(), blockContext.getBlock())
        : llvm::ReturnInst::Create(codegen::Context(), returnVal, blockContext.getBlock());
}
