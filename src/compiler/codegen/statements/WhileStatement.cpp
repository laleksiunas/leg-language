#include "llvm/IR/Value.h"
#include "ast/statements/WhileNode.h"
#include "codegen/context/BlockContext.h"

namespace codegen::while_loop {
    struct WhileLoopBlocks {
        llvm::BasicBlock* body;
        llvm::BasicBlock* condition;
        llvm::BasicBlock* end;
    };

    WhileLoopBlocks createWhileLoopBlocks(llvm::Function* wrapperFunction) {
        return WhileLoopBlocks {
            llvm::BasicBlock::Create(codegen::Context(), "loop", wrapperFunction),
            llvm::BasicBlock::Create(codegen::Context(), "loop_condition", wrapperFunction),
            llvm::BasicBlock::Create(codegen::Context(), "loop_end", wrapperFunction)
        };
    }

    void validateConditionType(llvm::Value* conditionValue, ast::ExpressionNode* condition) {
        if (conditionValue->getType() != codegen::createBooleanType()) {
            auto conditionValueTypeName = codegen::Types().getName(conditionValue->getType());
            auto booleanTypeName = codegen::Types().getName(codegen::createBooleanType());

            codegen::Errors().report(
                fmt::format(
                    "Expected a while condition expression to be of type '{}', got '{}'",
                    booleanTypeName,
                    conditionValueTypeName
                ),
                condition->lineNumber
            );
        }
    }

    void createConditionBlock(
        codegen::BlockContext& blockContext,
        WhileLoopBlocks& blocks,
        ast::ExpressionNode* condition
    ) {
        codegen::Builder().SetInsertPoint(blocks.condition);

        auto conditionBlockContext = blockContext.createNestedBlock(blocks.condition);
        auto* conditionValue = condition->codegen(conditionBlockContext);

        validateConditionType(conditionValue, condition);

        codegen::Builder().CreateCondBr(conditionValue, blocks.body, blocks.end);
    }

    void createLoopBlock(
        codegen::BlockContext& blockContext,
        WhileLoopBlocks& blocks,
        ast::BlockNode* block
    ) {
        codegen::Builder().SetInsertPoint(blocks.body);

        auto bodyBlockContext = blockContext.createNestedBlock(blocks.body);
        block->codegen(bodyBlockContext);

        if (!bodyBlockContext.hasReturnValue()) {
            codegen::Builder().SetInsertPoint(bodyBlockContext.getBlock());
            codegen::Builder().CreateBr(blocks.condition);
        }
    }
}

llvm::Value* ast::WhileNode::codegen(codegen::BlockContext& blockContext) {
    auto blocks = codegen::while_loop::createWhileLoopBlocks(blockContext.getCurrentFunction());

    codegen::Builder().SetInsertPoint(blockContext.getBlock());
    codegen::Builder().CreateBr(blocks.condition);

    codegen::while_loop::createConditionBlock(blockContext, blocks, condition);
    codegen::while_loop::createLoopBlock(blockContext, blocks, block);

    codegen::Builder().SetInsertPoint(blocks.end);
    blockContext.setBlock(blocks.end);

    return nullptr;
}
