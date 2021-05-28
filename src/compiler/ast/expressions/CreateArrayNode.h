#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "ast/misc/TypeNode.h"
#include "ast/expressions/IntegerNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/types/abstracts/ArrayType.h"

namespace ast {
    class CreateArrayNode : public ExpressionNode {
    public:
        TypeNode* elementType;
        IntegerNode* length;

        explicit CreateArrayNode(TypeNode* elementType, IntegerNode* size) : elementType(elementType), length(size) {}

        ~CreateArrayNode() override {
            delete elementType;
            delete length;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

        static void validateArrayTypeName(TypeNode* type, int lineNumber);

    private:
        codegen::ArrayType* createArrayType();

        void validateArrayLength();
    };
}
