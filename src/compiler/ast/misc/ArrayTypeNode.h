#pragma once

#include "codegen/types/abstracts/Type.h"
#include "TypeNode.h"

namespace ast {
    class ArrayTypeNode : public TypeNode {
    public:
        TypeNode* elementType;

        explicit ArrayTypeNode(TypeNode* elementType) : elementType(elementType) {}

        ~ArrayTypeNode() override {
            delete elementType;
        }

        codegen::Type* resolve() override;

        std::string getName() const override;
    };
}
