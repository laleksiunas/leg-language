#pragma once

#include <string>
#include "ast/Node.h"
#include "IdentifierNode.h"
#include "codegen/types/abstracts/Type.h"

namespace ast {
    class TypeNode {
    public:
        virtual ~TypeNode() = default;

        bool isVoid() const;

        bool isLet() const;

        bool isValid();

        virtual std::string getName() const = 0;

        virtual codegen::Type* resolve() = 0;
    };
}
