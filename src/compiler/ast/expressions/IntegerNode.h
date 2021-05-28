#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class IntegerNode : public ExpressionNode {
    public:
        int value;

        explicit IntegerNode(int value) : value(value) {}

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
