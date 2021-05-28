#pragma once

#include <string>
#include <utility>
#include "ast/Node.h"

namespace ast {
    class IdentifierNode {
    public:
        std::string value;

        explicit IdentifierNode(std::string value) : value(std::move(value)) {}

        bool isDiscard() const {
            return value == "_";
        }
    };
}
