#include "llvm/IR/Value.h"
#include "ast/expressions/GetterIndexerNode.h"
#include "codegen/context/BlockContext.h"

void ast::GetterIndexerNode::validateIndexer(codegen::Type* variableType, llvm::Value* indexValue) {
    if (!variableType->hasGetterIndexer(indexValue->getType())) {
        auto indexValueTypeName = codegen::Types().getByType(indexValue->getType())->getName();

        auto errorMessage = fmt::format(
            "Getter indexer '[{}]' cannot be applied to a value of type '{}'",
            indexValueTypeName,
            variableType->getName()
        );

        codegen::Errors().report(errorMessage, lineNumber);
    }
}

llvm::Value* ast::GetterIndexerNode::codegen(codegen::BlockContext& blockContext) {
    auto* loadedVariable = variable->codegen(blockContext);;
    auto* variableType = codegen::Types().getByType(loadedVariable->getType());

    auto* indexValue = index->codegen(blockContext);

    validateIndexer(variableType, indexValue);

    auto getterIndexer = variableType->getGetterIndexer(indexValue->getType()).value().handler;

    return getterIndexer(blockContext.getBlock(), loadedVariable, indexValue);
}
