#include <string>
#include "ast/misc/PrimitiveTypeNode.h"
#include "codegen/types/abstracts/Type.h"
#include "codegen/context/context.h"

codegen::Type* ast::PrimitiveTypeNode::resolve() {
    return codegen::Types().getByName(name->value);
}

std::string ast::PrimitiveTypeNode::getName() const {
    return name->value;
}
