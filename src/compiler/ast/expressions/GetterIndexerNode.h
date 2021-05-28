#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class GetterIndexerNode : public ExpressionNode {
    public:
        ExpressionNode* variable;
        ExpressionNode* index;

        explicit GetterIndexerNode(ExpressionNode* variable, ExpressionNode* index)
            : variable(variable), index(index) {}

        ~GetterIndexerNode() override {
            delete variable;
            delete index;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        void validateIndexer(codegen::Type* variableType, llvm::Value* indexValue);
    };
}
