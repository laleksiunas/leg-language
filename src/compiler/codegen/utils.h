#pragma once

#include <string>
#include <vector>
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "CodeGenerator.h"
#include "types/utils.h"

namespace codegen {
    llvm::Function* createNativeFunction(
        const std::string& name,
        llvm::Type* returnType,
        std::vector<llvm::Type*>& arguments,
        bool isVarArgs = false
    ) {
        auto* functionType = llvm::FunctionType::get(returnType, arguments, isVarArgs);

        auto* function = llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            name,
            codegen::Module().get()
        );

        function->setCallingConv(llvm::CallingConv::C);

        return function;
    }

    std::vector<llvm::Value*> generateFunctionArguments(
        codegen::BlockContext& blockContext,
        std::vector<ast::ExpressionNode*>& arguments
    ) {
        auto callArguments = std::vector<llvm::Value*>();

        callArguments.reserve(arguments.size());

        for (auto& argument : arguments) {
            callArguments.push_back(argument->codegen(blockContext));
        }

        return callArguments;
    }

    llvm::Value* storeValue(
        codegen::BlockContext& blockContext,
        const std::string& variableName,
        llvm::Value* variableValue
    ) {
        auto* variable = new llvm::AllocaInst(
            variableValue->getType(),
            0,
            variableName,
            blockContext.getBlock()
        );

        new llvm::StoreInst(
            variableValue,
            variable,
            blockContext.getBlock()
        );

        return variable;
    }

    llvm::Value* declareValue(
        codegen::BlockContext& blockContext,
        const std::string& variableName,
        llvm::Value* variableValue
    ) {
        return blockContext.setVariable(variableName, storeValue(blockContext, variableName, variableValue)).value;
    }

    llvm::Value* declareValue(
        codegen::BlockContext& blockContext,
        const std::string& variableName,
        Variable variable
    ) {
        variable.value = storeValue(blockContext, variableName, variable.value);

        return blockContext.setVariable(variableName, variable).value;
    }
}
