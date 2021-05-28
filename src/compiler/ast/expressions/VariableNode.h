#pragma once

#include "ast/ExpressionNode.h"
#include "ast/misc/IdentifierNode.h"

namespace ast {
    class VariableNode : public ExpressionNode {
    public:
        IdentifierNode* name;

        explicit VariableNode(IdentifierNode* name) : name(name) {}

        ~VariableNode() override {
            delete name;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
