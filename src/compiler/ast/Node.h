#pragma once

#include "llvm/IR/Value.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class Node {
    public:
        int lineNumber = yylineno;

        virtual ~Node() = default;

        virtual llvm::Value* codegen(codegen::BlockContext& blockContext) = 0;
    };
}
