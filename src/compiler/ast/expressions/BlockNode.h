#pragma once

#include <vector>
#include "ast/ExpressionNode.h"
#include "ast/StatementNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class BlockNode : public ExpressionNode {
    public:
        std::vector<StatementNode*> statements;

        ~BlockNode() override {
            for (auto statement : statements) {
                delete statement;
            }

            statements.clear();
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
