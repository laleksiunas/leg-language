#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class FloatNode : public ExpressionNode {
    public:
        float value;

        explicit FloatNode(float value) : value(value) {}

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
