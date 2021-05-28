#pragma once

#include <map>
#include <vector>
#include <utility>
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "Variable.h"

namespace codegen {
    typedef std::map<std::string, Variable> VariablesScope;
    typedef std::multimap<std::string, llvm::Function*> FunctionsScope;

    class BlockContext {
    public:
        static BlockContext root() {
            return BlockContext();
        }

        llvm::BasicBlock* getBlock() {
            return block;
        }

        void setBlock(llvm::BasicBlock* newBlock) {
            block = newBlock;
        }

        bool hasIdentifier(const std::string& name);

        bool hasVariable(const std::string& name);

        Variable getVariable(const std::string& name);

        Variable setVariable(const std::string& name, llvm::Value* value);

        Variable setVariable(const std::string& name, Variable& value);

        void removeVariable(const std::string& name);

        bool hasFunction(const std::string& name);

        llvm::Function* getFunction(const std::string& name, std::vector<llvm::Type*>& functionArguments);

        llvm::Function* getFunction(const std::string& name, std::vector<llvm::Value*>& functionArguments);

        std::vector<llvm::Function*> getFunctions(const std::string& name);

        llvm::Function* addFunction(const std::string& name, llvm::Function* function);

        llvm::Function* getPrivateFunction(const std::string& name, std::vector<llvm::Type*>& functionArguments);

        llvm::Function* getPrivateFunction(const std::string& name, std::vector<llvm::Value*>& functionArguments);

        llvm::Function* getPrivateFunction(const std::string& name);

        llvm::Function* addPrivateFunction(const std::string& name, llvm::Function* function);

        bool hasReturnValue() const {
            return returnValueSpecified;
        }

        void setHasReturnValue(bool hasReturnValue = true) {
            returnValueSpecified = hasReturnValue;
        }

        llvm::Function* getCurrentFunction() {
            return function;
        }

        void setCurrentFunction(llvm::Function* wrapperFunction) {
            function = wrapperFunction;
        }

        BlockContext createNestedBlock(llvm::BasicBlock* nestedBlock);

    private:
        llvm::BasicBlock* block;
        bool returnValueSpecified = false;
        llvm::Function* function;
        VariablesScope variablesScope;
        FunctionsScope functionsScope;
        FunctionsScope privateFunctionsScope;

        explicit BlockContext() {
            block = nullptr;
            function = nullptr;
        }

        explicit BlockContext(
            llvm::BasicBlock* block,
            llvm::Function* function,
            VariablesScope variablesScope,
            FunctionsScope functionsScope,
            FunctionsScope privateFunctionsScope
        ) {
            this->block = block;
            this->function = function;
            this->variablesScope = std::move(variablesScope);
            this->functionsScope = std::move(functionsScope);
            this->privateFunctionsScope = std::move(privateFunctionsScope);
        }
    };
}
