#include <vector>
#include <set>
#include "fmt/format.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "ast/statements/FunctionDeclarationNode.h"
#include "ast/misc/FunctionArgumentNode.h"
#include "codegen/utils.h"
#include "codegen/keywords.h"

void ast::FunctionDeclarationNode::validateFunctionDeclarationScope(codegen::BlockContext& context) {
    if (context.getCurrentFunction() != nullptr) {
        codegen::Errors().report("Cannot declare a nested function", lineNumber);
    }
}

void ast::FunctionDeclarationNode::validateFunctionName() {
    if (codegen::keywords::isReservedKeyword(name->value)) {
        codegen::Errors().reportUseOfKeywordForIdentifier(name->value, lineNumber);
    }
}

void ast::FunctionDeclarationNode::validateFunctionReturnType() {
    if (returnType->isVoid()) {
        return;
    }

    if (returnType->isLet()) {
        codegen::Errors().report(fmt::format("Cannot use {} as a return type", codegen::keywords::Let), lineNumber);
    }

    if (!returnType->isValid()) {
        codegen::Errors().reportUndeclaredType(returnType->getName(), lineNumber);
    }
}

void ast::FunctionDeclarationNode::validateFunctionArgType(ast::TypeNode* type) {
    if (type->isVoid()) {
        codegen::Errors().report(
            fmt::format("Cannot use {} as a function argument type", codegen::keywords::Void),
            lineNumber
        );
    }

    if (type->isLet()) {
        codegen::Errors().report(
            fmt::format("Cannot use {} as a function argument type", codegen::keywords::Let),
            lineNumber
        );
    }

    if (!type->isValid()) {
        codegen::Errors().reportUndeclaredType(type->getName(), lineNumber);
    }
}

void ast::FunctionDeclarationNode::validateFunctionDefinition(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Type*>& argumentTypes
) {
    if (blockContext.getFunction(name->value, argumentTypes) != nullptr) {
        codegen::Errors().reportFunctionRedeclaration(name->value, argumentTypes, lineNumber);
    }
}

void ast::FunctionDeclarationNode::validateFunctionArguments(codegen::BlockContext& blockContext) {
    std::set<std::string> existingArgumentNames;

    for (auto argument : *arguments) {
        auto argumentName = argument->name->value;

        validateFunctionArgType(argument->type);

        if (codegen::keywords::isReservedKeyword(argumentName)) {
            codegen::Errors().reportUseOfKeywordForIdentifier(argumentName, lineNumber);
        }

        if (blockContext.hasIdentifier(argumentName) || existingArgumentNames.contains(argumentName)) {
            codegen::Errors().reportIdentifierRedeclaration(argumentName, lineNumber);
        }

        existingArgumentNames.insert(argumentName);
    }
}

void ast::FunctionDeclarationNode::validateFunctionReturn(codegen::BlockContext& blockContext) {
    if (!returnType->isVoid() && !blockContext.hasReturnValue()) {
        codegen::Errors().report(
            fmt::format("Missing return statement for a function with return type '{}'", returnType->getName()),
            lineNumber
        );
    }
}

llvm::Type* ast::FunctionDeclarationNode::resolveFunctionReturnType() const {
    return returnType->isVoid() ? codegen::createVoidType() : returnType->resolve()->getType();
}

std::vector<llvm::Type*> ast::FunctionDeclarationNode::createFunctionArgumentsTypes() const {
    auto argumentTypes = std::vector<llvm::Type*>();

    argumentTypes.reserve(arguments->size());

    for (auto argument : *arguments) {
        argumentTypes.push_back(argument->type->resolve()->getType());
    }

    return argumentTypes;
}

void ast::FunctionDeclarationNode::createFunctionArgumentsVariables(
    llvm::iterator_range<llvm::Function::arg_iterator>& definitionArgs,
    codegen::BlockContext& blockContext
) const {
    auto argumentNodeIndex = 0;

    for (auto& arg : definitionArgs) {
        if (arg.getType()->isVoidTy()) {
            return;
        }

        auto variable = codegen::Variable {
            &arg,
            false,
            false
        };

        blockContext.setVariable(arguments->at(argumentNodeIndex)->name->value, variable);

        argumentNodeIndex++;
    }
}

llvm::Function* ast::FunctionDeclarationNode::createFunctionDefinition(std::vector<llvm::Type*>& argumentTypes) const {
    auto* functionType = llvm::FunctionType::get(
        resolveFunctionReturnType(),
        llvm::makeArrayRef(argumentTypes),
        false
    );

    return llvm::Function::Create(
        functionType,
        llvm::GlobalValue::InternalLinkage,
        name->value,
        codegen::Module().get()
    );
}

llvm::Value* ast::FunctionDeclarationNode::codegen(codegen::BlockContext& blockContext) {
    validateFunctionDeclarationScope(blockContext);
    validateFunctionName();
    validateFunctionReturnType();
    validateFunctionArguments(blockContext);

    auto argumentTypes = createFunctionArgumentsTypes();

    validateFunctionDefinition(blockContext, argumentTypes);

    auto* function = createFunctionDefinition(argumentTypes);
    auto functionDefinitionArgs = function->args();

    blockContext.addFunction(name->value, function);

    auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", function);
    auto nestedBlockContext = blockContext.createNestedBlock(implementationBlock);

    nestedBlockContext.setCurrentFunction(function);
    createFunctionArgumentsVariables(functionDefinitionArgs, nestedBlockContext);
    body->codegen(nestedBlockContext);

    validateFunctionReturn(nestedBlockContext);

    if (!nestedBlockContext.hasReturnValue()) {
        llvm::ReturnInst::Create(codegen::Context(), nestedBlockContext.getBlock());
    }

    return function;
}
