#include "llvm/IR/Value.h"
#include "ast/expressions/BlockNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/errors/CompilationException.h"

llvm::Value* ast::BlockNode::codegen(codegen::BlockContext& blockContext) {
    llvm::Value* lastStatementValue = nullptr;

    for (auto& statement : statements) {
        if (blockContext.hasReturnValue()) {
            break;
        }

        try {
            lastStatementValue = statement->codegen(blockContext);
        } catch (codegen::CompilationException& e) {}
    }

    return lastStatementValue;
}
