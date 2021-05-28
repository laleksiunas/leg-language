#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/misc/IdentifierNode.h"
#include "ast/misc/TypeNode.h"

namespace ast {
    class DeclarationNode : public StatementNode {
    public:
        TypeNode* type;
        IdentifierNode* variableName;
        ExpressionNode* value;

        explicit DeclarationNode(TypeNode* type, IdentifierNode* variableName, ExpressionNode* value)
            : type(type), variableName(variableName), value(value) {}

        ~DeclarationNode() override {
            delete type;
            delete variableName;
            delete value;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

        static void validateIdentifier(const std::string& name, codegen::BlockContext& blockContext, int lineNumber);

    private:
        void validateType(llvm::Type* receivedType);
    };
}
