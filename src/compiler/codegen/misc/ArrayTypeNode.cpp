#include <string>
#include "fmt/format.h"
#include "ast/misc/ArrayTypeNode.h"
#include "codegen/types/abstracts/Type.h"
#include "codegen/context/context.h"
#include "codegen/types/primitive/array.h"

codegen::Type* ast::ArrayTypeNode::resolve() {
    auto* innerType = elementType->resolve();

    return innerType == nullptr ? nullptr : codegen::types::registerArrayType(innerType);
}

std::string ast::ArrayTypeNode::getName() const {
    return fmt::format("{}[]", elementType->getName());
}
