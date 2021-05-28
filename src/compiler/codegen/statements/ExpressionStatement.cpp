#include "ast/statements/ExpressionStatementNode.h"
#include "codegen/context/BlockContext.h"

llvm::Value* ast::ExpressionStatementNode::codegen(codegen::BlockContext& blockContext) {
    return expression->codegen(blockContext);
}
