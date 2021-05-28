#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/expressions/GetterIndexerNode.h"

namespace ast {
    class SetterIndexerNode : public StatementNode {
    public:
        ast::GetterIndexerNode* indexer;
        ast::ExpressionNode* value;

        explicit SetterIndexerNode(ast::GetterIndexerNode* indexer, ast::ExpressionNode* value)
            : indexer(indexer), value(value) {}

        ~SetterIndexerNode() override {
            delete indexer;
            delete value;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        void validateIndexer(codegen::Type* variableType, llvm::Value* indexValue);

        void validateAssignment(
            codegen::Type* variableType,
            llvm::Type* indexValueType,
            codegen::Indexer<codegen::SetterIndexer>& setterIndexer,
            llvm::Value* assignmentValue
        );
    };
}
