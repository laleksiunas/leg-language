#pragma once

#include <vector>
#include "ast/expressions/BlockNode.h"
#include "ast/misc/FunctionArgumentNode.h"
#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/misc/IdentifierNode.h"
#include "ast/misc/TypeNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class FunctionDeclarationNode : public StatementNode {
    public:
        IdentifierNode* name;
        BlockNode* body;
        TypeNode* returnType;
        std::vector<FunctionArgumentNode*>* arguments;

        explicit FunctionDeclarationNode(
            IdentifierNode* name,
            std::vector<FunctionArgumentNode*>* arguments,
            TypeNode* returnType,
            BlockNode* body
        ) : name(name), arguments(arguments), returnType(returnType), body(body) {}

        ~FunctionDeclarationNode() override {
            delete name;
            delete body;
            delete returnType;

            for (auto argument : *arguments) {
                delete argument;
            }

            delete arguments;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        llvm::Type* resolveFunctionReturnType() const;

        std::vector<llvm::Type*> createFunctionArgumentsTypes() const;

        void createFunctionArgumentsVariables(
            llvm::iterator_range<llvm::Function::arg_iterator>& definitionArgs,
            codegen::BlockContext& blockContext
        ) const;

        void validateFunctionName();

        void validateFunctionReturnType();

        void validateFunctionArguments(codegen::BlockContext& blockContext);

        void validateFunctionArgType(ast::TypeNode* type);

        llvm::Function* createFunctionDefinition(std::vector<llvm::Type*>& argumentTypes) const;

        void validateFunctionDefinition(codegen::BlockContext& blockContext, std::vector<llvm::Type*>& argumentTypes);

        void validateFunctionReturn(codegen::BlockContext& blockContext);

        void validateFunctionDeclarationScope(codegen::BlockContext& context);
    };
}

