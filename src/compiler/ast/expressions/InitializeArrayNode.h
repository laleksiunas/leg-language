#pragma once

#include <vector>
#include "ast/ExpressionNode.h"
#include "ast/misc/TypeNode.h"

namespace ast {
    class InitializeArrayNode : public ExpressionNode {
    public:
        TypeNode* arrayTypeDeclaration;
        std::vector<ExpressionNode*>* values;

        explicit InitializeArrayNode(TypeNode* arrayTypeDeclaration, std::vector<ExpressionNode*>* values)
            : arrayTypeDeclaration(arrayTypeDeclaration), values(values) {}

        ~InitializeArrayNode() override {
            delete arrayTypeDeclaration;

            for (auto* value : *values) {
                delete value;
            }

            delete values;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        std::vector<llvm::Value*> createArrayValues(codegen::BlockContext& blockContext);

        codegen::ArrayType* resolveArrayType();

        void validateArrayTypeWithValuesTypes(codegen::ArrayType* arrayType, std::vector<llvm::Value*> arrayValues);
    };
}
