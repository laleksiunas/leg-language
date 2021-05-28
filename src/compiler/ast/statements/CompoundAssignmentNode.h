#pragma once

#include <string>
#include <utility>
#include "ast/StatementNode.h"
#include "ast/misc/IdentifierNode.h"
#include "ast/ExpressionNode.h"
#include "ast/statements/AssignmentNode.h"

namespace ast {
    class CompoundAssignmentNode : public StatementNode {
    public:
        AssignmentNode* assignment;

        explicit CompoundAssignmentNode(
            IdentifierNode* variableName,
            ExpressionNode* value,
            const std::string& operatorName
        ) {
            assignment = createAssignmentNode(variableName, value, operatorName);
        }

        ~CompoundAssignmentNode() override {
            delete assignment;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        static AssignmentNode* createAssignmentNode(
            IdentifierNode* variableName,
            ExpressionNode* value,
            const std::string& operatorName
        );
    };
}
