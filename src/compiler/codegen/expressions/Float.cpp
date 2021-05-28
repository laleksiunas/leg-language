#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "ast/expressions/FloatNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/utils.h"

llvm::Value* ast::FloatNode::codegen(codegen::BlockContext& blockContext) {
    return llvm::ConstantFP::get(codegen::createFloatType(), value);
}
