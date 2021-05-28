#include "ast/misc/TypeNode.h"
#include "codegen/context/context.h"
#include "codegen/keywords.h"

bool ast::TypeNode::isVoid() const {
    return getName() == codegen::keywords::Void;
}

bool ast::TypeNode::isLet() const {
    return getName() == codegen::keywords::Let;
}

bool ast::TypeNode::isValid() {
    return resolve() != nullptr;
}
