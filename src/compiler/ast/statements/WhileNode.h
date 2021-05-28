#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/expressions/BlockNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class WhileNode : public StatementNode {
    public:
        ExpressionNode* condition;
        BlockNode* block;

        explicit WhileNode(ExpressionNode* condition, BlockNode* block) : condition(condition), block(block) {}

        ~WhileNode() override {
            delete condition;
            delete block;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
