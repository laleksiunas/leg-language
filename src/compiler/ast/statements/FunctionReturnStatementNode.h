#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"

namespace ast {
    class FunctionReturnStatementNode : public StatementNode {
    public:
        ExpressionNode* returnValue;

        explicit FunctionReturnStatementNode(ExpressionNode* value) : returnValue(value) {}

        explicit FunctionReturnStatementNode() : returnValue(nullptr) {}

        ~FunctionReturnStatementNode() override {
            delete returnValue;
        }

        bool isVoid() const {
            return returnValue == nullptr;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
