#pragma once

#include <vector>
#include "ast/StatementNode.h"
#include "ast/misc/IdentifierNode.h"
#include "ast/expressions/KickNode.h"

namespace ast {
    class CatchNode : public StatementNode {
    public:
        std::vector<KickNode*>* arguments;
        std::vector<IdentifierNode*>* destructuringIdentifiers;

        explicit CatchNode(
            std::vector<IdentifierNode*>* destructuringIdentifiers,
            std::vector<KickNode*>* arguments
        ) : destructuringIdentifiers(destructuringIdentifiers), arguments(arguments) {}

        ~CatchNode() override {
            for (auto identifier : *destructuringIdentifiers) {
                delete identifier;
            }

            for (auto argument : *arguments) {
                delete argument;
            }

            delete destructuringIdentifiers;
            delete arguments;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        std::vector<shared::KickResult> spawnThreads(codegen::BlockContext& blockContext) const;

        static void joinVoidThread(codegen::BlockContext& blockContext, shared::KickResult& kickResult);

        static llvm::Value* joinResultThread(codegen::BlockContext& blockContext, shared::KickResult& kickResult);

        static std::vector<llvm::Value*> joinThreads(
            codegen::BlockContext& blockContext,
            std::vector<shared::KickResult>& kickResults
        );

        static std::vector<llvm::Value*> loadReturnValues(
            codegen::BlockContext& blockContext,
            std::vector<llvm::Value*>& returnValuePointers
        );

        static void deallocateReturnValuePointers(
            codegen::BlockContext& blockContext,
            std::vector<llvm::Value*>& returnValuePointers
        );

        void assignValuesToVariables(codegen::BlockContext& blockContext, std::vector<llvm::Value*>& values) const;

        void validateDestructuringIdentifiers(codegen::BlockContext& blockContext);
    };
}
