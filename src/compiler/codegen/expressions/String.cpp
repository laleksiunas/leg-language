#include "llvm/IR/Value.h"
#include "ast/expressions/StringNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/types/primitive/string.h"

llvm::Value* ast::StringNode::codegen(codegen::BlockContext& blockContext) {
    return codegen::types::createConstString(value);
}
