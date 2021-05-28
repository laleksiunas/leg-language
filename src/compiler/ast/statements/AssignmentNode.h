#pragma once

#include "ast/StatementNode.h"
#include "ast/misc/IdentifierNode.h"
#include "ast/ExpressionNode.h"

namespace ast {
    class AssignmentNode : public StatementNode {
    public:
        IdentifierNode* variableName;
        ExpressionNode* value;

        explicit AssignmentNode(IdentifierNode* variableName, ExpressionNode* value)
            : variableName(variableName), value(value) {}

        ~AssignmentNode() override {
            delete variableName;
            delete value;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        void validateVariable(codegen::BlockContext& blockContext);

        void validateAssignmentTypes(llvm::Type* variableType, llvm::Type* valueType);
    };
}
