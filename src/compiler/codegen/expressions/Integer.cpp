#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "ast/expressions/IntegerNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/utils.h"

llvm::Value* ast::IntegerNode::codegen(codegen::BlockContext& blockContext) {
    return llvm::ConstantInt::get(codegen::createIntType(), value, true);
}
