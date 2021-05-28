#pragma once

#include "ast/StatementNode.h"
#include "ast/expressions/BlockNode.h"
#include "codegen/context/BlockContext.h"
#include "ast/ExpressionNode.h"

namespace ast {
    class IfNode : public StatementNode {
    public:
        ExpressionNode* condition;
        BlockNode* thenBlock;
        BlockNode* elseBlock;
        IfNode* elseIfStatement;

        explicit IfNode(ExpressionNode* condition, BlockNode* thenBlock)
            : condition(condition), thenBlock(thenBlock), elseBlock(nullptr), elseIfStatement(nullptr) {}

        explicit IfNode(ExpressionNode* condition, BlockNode* thenBlock, BlockNode* elseBlock)
            : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock), elseIfStatement(nullptr) {}

        explicit IfNode(ExpressionNode* condition, BlockNode* thenBlock, IfNode* elseIfStatement)
            : condition(condition), thenBlock(thenBlock), elseBlock(nullptr), elseIfStatement(elseIfStatement) {}

        ~IfNode() override {
            delete condition;
            delete thenBlock;
            delete elseBlock;
            delete elseIfStatement;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        void validateCondition(llvm::Value* conditionValue);

        void createThenBranch(
            codegen::BlockContext& thenBlockContext,
            llvm::BasicBlock* branchBlock,
            llvm::BasicBlock* mergeBlock
        ) const;

        void createElseBranch(
            codegen::BlockContext& elseBlockContext,
            llvm::BasicBlock* branchBlock,
            llvm::BasicBlock* mergeBlock
        ) const;

        static std::tuple<llvm::BasicBlock*, llvm::BasicBlock*, llvm::BasicBlock*> createBranches(
            llvm::Function* wrapperFunction
        );

        static void setDefaultReturnValue(llvm::Function* function, llvm::BasicBlock* block);
    };
}
 