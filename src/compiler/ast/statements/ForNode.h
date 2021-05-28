#pragma once

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/expressions/BlockNode.h"
#include "ast/misc/IdentifierNode.h"
#include "codegen/context/BlockContext.h"
#include "shared/Range.h"

namespace ast {
    class ForNode : public StatementNode {
    public:
        IdentifierNode* variableName;
        ExpressionNode* rangeBegin;
        ExpressionNode* rangeEnd;
        ExpressionNode* step;
        BlockNode* block;

        explicit ForNode(
            IdentifierNode* variableName,
            ExpressionNode* rangeBegin,
            ExpressionNode* rangeEnd,
            BlockNode* block
        ) : variableName(variableName), rangeBegin(rangeBegin), rangeEnd(rangeEnd), step(nullptr), block(block) {}

        explicit ForNode(
            IdentifierNode* variableName,
            ExpressionNode* rangeBegin,
            ExpressionNode* rangeEnd,
            ExpressionNode* step,
            BlockNode* block
        ) : ForNode(variableName, rangeBegin, rangeEnd, block) {
            this->step = step;
        }

        ~ForNode() override {
            delete variableName;
            delete rangeBegin;
            delete rangeEnd;
            delete step;
            delete block;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        shared::Range resolveRange(codegen::BlockContext& blockContext);

        llvm::Value* resolveRangeStepValue(codegen::Type* rangeType, codegen::BlockContext& blockContext);

        static llvm::Value* getDefaultStepValue(llvm::Type* rangeType);

        void validateForVoidRangeType(llvm::Type* rangeType);

        void validateRangeStepOperator(
            codegen::Type* rangeType,
            codegen::Type* stepType,
            const std::string& operatorName
        );

        llvm::Value* declareLoopVariable(codegen::BlockContext& blockContext, llvm::Value* initialValue) const;

        llvm::BasicBlock* createLoopBody(codegen::BlockContext& blockContext, llvm::BasicBlock* loopEntryBlock) const;

        void validateInRangeOperators(codegen::Type* rangeType, const std::string& operatorName);
    };
}
