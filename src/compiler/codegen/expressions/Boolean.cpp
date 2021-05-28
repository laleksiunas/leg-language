#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "ast/expressions/BooleanNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/utils.h"

llvm::Value* ast::BooleanNode::codegen(codegen::BlockContext& blockContext) {
    return llvm::ConstantInt::get(codegen::createBooleanType(), value, false);
}
