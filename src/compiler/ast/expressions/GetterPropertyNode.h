#pragma once

#include "llvm/IR/Value.h"
#include "ast/ExpressionNode.h"
#include "ast/misc/IdentifierNode.h"
#include "codegen/context/BlockContext.h"

namespace ast {
    class GetterPropertyNode : public ExpressionNode {
    public:
        ExpressionNode* variable;
        IdentifierNode* propertyName;

        explicit GetterPropertyNode(ExpressionNode* variable, IdentifierNode* propertyName)
            : variable(variable), propertyName(propertyName) {}

        ~GetterPropertyNode() override {
            delete variable;
            delete propertyName;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

    private:
        void validateProperty(codegen::Type* variableType, const std::string& name);
    };
}
