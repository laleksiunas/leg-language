#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "ast/statements/SetterIndexerNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/types/abstracts/Indexer.h"

void ast::SetterIndexerNode::validateIndexer(codegen::Type* variableType, llvm::Value* indexValue) {
    if (!variableType->hasSetterIndexer(indexValue->getType())) {
        auto indexValueTypeName = codegen::Types().getByType(indexValue->getType())->getName();

        auto errorMessage = fmt::format(
            "Setter indexer '[{}]' cannot be applied to a value of type '{}'",
            indexValueTypeName,
            variableType->getName()
        );

        codegen::Errors().report(errorMessage, lineNumber);
    }
}

void ast::SetterIndexerNode::validateAssignment(
    codegen::Type* variableType,
    llvm::Type* indexValueType,
    codegen::Indexer<codegen::SetterIndexer>& setterIndexer,
    llvm::Value* assignmentValue
) {
    if (setterIndexer.returnType != assignmentValue->getType()) {
        auto variableNameText = fmt::format(
            "{}[{}]",
            variableType->getName(),
            codegen::Types().getByType(indexValueType)->getName()
        );

        codegen::Errors().reportAssignmentTypesMismatch(
            variableNameText,
            setterIndexer.returnType,
            assignmentValue->getType(),
            lineNumber
        );
    }
}

llvm::Value* ast::SetterIndexerNode::codegen(codegen::BlockContext& blockContext) {
    auto loadedVariable = indexer->variable->codegen(blockContext);
    auto* variableType = codegen::Types().getByType(loadedVariable->getType());

    auto* indexValue = indexer->index->codegen(blockContext);
    auto* assignmentValue = value->codegen(blockContext);

    validateIndexer(variableType, indexValue);

    auto setterIndexer = variableType->getSetterIndexer(indexValue->getType()).value();

    validateAssignment(variableType, indexValue->getType(), setterIndexer, assignmentValue);

    return setterIndexer.handler(blockContext.getBlock(), loadedVariable, indexValue, assignmentValue);
}
