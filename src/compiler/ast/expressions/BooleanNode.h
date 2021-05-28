#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class BooleanNode : public ExpressionNode {
    public:
        bool value;

        explicit BooleanNode(bool value) : value(value) {}

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
