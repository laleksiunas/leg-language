#pragma once

#include <string>
#include "TypeNode.h"
#include "IdentifierNode.h"

namespace ast {
    class PrimitiveTypeNode : public TypeNode {
    public:
        IdentifierNode* name;

        explicit PrimitiveTypeNode(IdentifierNode* name) : name(name) {}

        ~PrimitiveTypeNode() override {
            delete name;
        }

        std::string getName() const override;

        codegen::Type* resolve() override;
    };
}
