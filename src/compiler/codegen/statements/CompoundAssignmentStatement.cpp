#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "ast/statements/CompoundAssignmentNode.h"
#include "ast/statements/AssignmentNode.h"
#include "ast/expressions/ArithmeticNode.h"
#include "ast/expressions/VariableNode.h"

ast::AssignmentNode* ast::CompoundAssignmentNode::createAssignmentNode(
    ast::IdentifierNode* variableName,
    ast::ExpressionNode* value,
    const std::string& operatorName
) {
    auto* arithmeticVariableNode = new ast::VariableNode(new ast::IdentifierNode(variableName->value));
    auto* arithmeticNode = new ast::ArithmeticNode(arithmeticVariableNode, value, operatorName);

    return new ast::AssignmentNode(variableName, arithmeticNode);
}

llvm::Value* ast::CompoundAssignmentNode::codegen(codegen::BlockContext& blockContext) {
    return assignment->codegen(blockContext);
}
