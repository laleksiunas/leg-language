#pragma once

#include <string>
#include <utility>
#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"

namespace ast {
    class ArithmeticNode : public ExpressionNode {
    public:
        ExpressionNode* left;
        ExpressionNode* right;
        std::string operatorName;

        explicit ArithmeticNode(ExpressionNode* left, ExpressionNode* right, std::string operatorName)
            : left(left), right(right), operatorName(std::move(operatorName)) {}

        ~ArithmeticNode() override {
            delete left;
            delete right;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
