#pragma once

#include <vector>
#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "ast/misc/IdentifierNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class FunctionCallNode : public ExpressionNode {
    public:
        IdentifierNode* functionName;
        std::vector<ExpressionNode*>* arguments;

        explicit FunctionCallNode(IdentifierNode* functionName, std::vector<ExpressionNode*>* arguments)
            : functionName(functionName), arguments(arguments) {}

        ~FunctionCallNode() override {
            delete functionName;

            for (auto argument : *arguments) {
                delete argument;
            }

            delete arguments;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

        void validateFunction(codegen::BlockContext& blockContext, std::vector<llvm::Value*>& callArguments);
    };
}
