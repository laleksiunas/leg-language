#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class ExpressionStatementNode : public StatementNode {
    public:
        ExpressionNode* expression;

        explicit ExpressionStatementNode(ExpressionNode* expression) : expression(expression) {}

        ~ExpressionStatementNode() override {
            delete expression;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
