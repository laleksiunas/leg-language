#pragma once

#include <string>
#include <utility>
#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class StringNode : public ExpressionNode {
    public:
        std::string value;

        explicit StringNode(std::string value) : value(std::move(value)) {}

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;
    };
}
