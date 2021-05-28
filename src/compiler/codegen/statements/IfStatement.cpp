#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "ast/statements/IfNode.h"
#include "codegen/context/BlockContext.h"

void ast::IfNode::validateCondition(llvm::Value* conditionValue) {
    if (conditionValue->getType() != codegen::createBooleanType()) {
        auto conditionTypeName = codegen::Types().getByType(conditionValue->getType())->getName();
        auto booleanTypeName = codegen::Types().getByType(codegen::createBooleanType())->getName();

        codegen::Errors().report(
            fmt::format("Expected if condition to be of type '{}', got '{}'", booleanTypeName, conditionTypeName),
            lineNumber
        );
    }
}

std::tuple<llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*> ast::IfNode::createBranches(
    llvm::Function* wrapperFunction
) {
    auto* thenBasicBlock = llvm::BasicBlock::Create(codegen::Context(), "then", wrapperFunction);
    auto* elseBasicBlock = llvm::BasicBlock::Create(codegen::Context(), "else", wrapperFunction);
    auto* mergeBlock = llvm::BasicBlock::Create(codegen::Context(), "merge", wrapperFunction);

    return std::tuple(thenBasicBlock, elseBasicBlock, mergeBlock);
}

void ast::IfNode::createThenBranch(
    codegen::BlockContext& thenBlockContext,
    llvm::BasicBlock* branchBlock,
    llvm::BasicBlock* mergeBlock
) const {
    codegen::Builder().SetInsertPoint(branchBlock);

    thenBlock->codegen(thenBlockContext);

    if (!thenBlockContext.hasReturnValue()) {
        codegen::Builder().CreateBr(mergeBlock);
    }
}

void ast::IfNode::createElseBranch(
    codegen::BlockContext& elseBlockContext,
    llvm::BasicBlock* branchBlock,
    llvm::BasicBlock* mergeBlock
) const {
    codegen::Builder().SetInsertPoint(branchBlock);

    if (elseBlock == nullptr && elseIfStatement == nullptr) {
        codegen::Builder().CreateBr(mergeBlock);

        return;
    }

    if (elseBlock == nullptr) {
        elseIfStatement->codegen(elseBlockContext);
    } else {
        elseBlock->codegen(elseBlockContext);
    }

    if (!elseBlockContext.hasReturnValue()) {
        codegen::Builder().CreateBr(mergeBlock);
    }
}

void ast::IfNode::setDefaultReturnValue(llvm::Function* function, llvm::BasicBlock* block) {
    if (function->getReturnType()->isVoidTy()) {
        llvm::ReturnInst::Create(codegen::Context(), block);
    } else {
        llvm::ReturnInst::Create(codegen::Context(), llvm::ConstantInt::getNullValue(function->getReturnType()), block);
    }
}

llvm::Value* ast::IfNode::codegen(codegen::BlockContext& blockContext) {
    auto* conditionValue = condition->codegen(blockContext);

    validateCondition(conditionValue);

    auto [thenBasicBlock, elseBasicBlock, mergeBlock] = createBranches(blockContext.getCurrentFunction());

    codegen::Builder().SetInsertPoint(blockContext.getBlock());
    codegen::Builder().CreateCondBr(conditionValue, thenBasicBlock, elseBasicBlock);

    auto thenBlockContext = blockContext.createNestedBlock(thenBasicBlock);
    auto elseBlockContext = blockContext.createNestedBlock(elseBasicBlock);

    createThenBranch(thenBlockContext, thenBasicBlock, mergeBlock);
    createElseBranch(elseBlockContext, elseBasicBlock, mergeBlock);

    if (thenBlockContext.hasReturnValue() && elseBlockContext.hasReturnValue()) {
        blockContext.setHasReturnValue();
        setDefaultReturnValue(blockContext.getCurrentFunction(), mergeBlock);
    }

    codegen::Builder().SetInsertPoint(mergeBlock);
    blockContext.setBlock(mergeBlock);

    return nullptr;
}
