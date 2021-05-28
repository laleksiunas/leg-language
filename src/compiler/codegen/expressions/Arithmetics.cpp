#include "llvm/IR/Value.h"
#include "ast/expressions/ArithmeticNode.h"
#include "codegen/context/BlockContext.h"

llvm::Value* ast::ArithmeticNode::codegen(codegen::BlockContext& blockContext) {
    auto* leftValue = left->codegen(blockContext);
    auto* rightValue = right->codegen(blockContext);

    auto leftType = codegen::Types().getByType(leftValue->getType());
    auto operatorHandler = leftType->getOperator(operatorName, rightValue->getType());

    if (!operatorHandler.has_value()) {
        codegen::Errors().reportInvalidOperatorUsage(
            operatorName,
            leftValue->getType(),
            rightValue->getType(),
            lineNumber
        );
    }

    return operatorHandler.value()(blockContext.getBlock(), leftValue, rightValue);
}
