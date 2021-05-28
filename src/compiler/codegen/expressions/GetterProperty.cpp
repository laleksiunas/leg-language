#include <string>
#include "llvm/IR/Value.h"
#include "ast/expressions/GetterPropertyNode.h"
#include "codegen/context/BlockContext.h"

void ast::GetterPropertyNode::validateProperty(codegen::Type* variableType, const std::string& name) {
    if (!variableType->hasGetterProperty(name)) {
        auto errorMessage = fmt::format(
            "Type '{}' does not have getter property '{}'",
            variableType->getName(),
            name
        );

        codegen::Errors().report(errorMessage, lineNumber);
    }
}

llvm::Value* ast::GetterPropertyNode::codegen(codegen::BlockContext& blockContext) {
    auto* loadedVariable = variable->codegen(blockContext);
    auto* variableType = codegen::Types().getByType(loadedVariable->getType());

    validateProperty(variableType, propertyName->value);

    auto property = variableType->getGetterProperty(propertyName->value);

    return property.value()(blockContext.getBlock(), loadedVariable);
}
