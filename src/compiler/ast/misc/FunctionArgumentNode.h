#pragma once

#include "ast/Node.h"
#include "IdentifierNode.h"
#include "TypeNode.h"

namespace ast {
    class FunctionArgumentNode {
    public:
        TypeNode* type;
        IdentifierNode* name;

        explicit FunctionArgumentNode(TypeNode* type, IdentifierNode* name) : type(type), name(name) {}

        ~FunctionArgumentNode() {
            delete type;
            delete name;
        }
    };
}
