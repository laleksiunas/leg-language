#include <vector>
#include "BlockContext.h"
#include "Variable.h"

namespace codegen {
    namespace {
        bool areArgumentsEqual(
            llvm::iterator_range<llvm::Function::arg_iterator>& definitionArgs,
            std::vector<llvm::Type*>& functionArguments
        ) {
            int functionArgumentsIndex = 0;

            for (auto& arg : definitionArgs) {
                if (arg.getType()->isVoidTy()) {
                    return functionArguments.empty();
                }

                if (
                    functionArgumentsIndex == functionArguments.size() ||
                    arg.getType() != functionArguments[functionArgumentsIndex]
                ) {
                    return false;
                }

                functionArgumentsIndex++;
            }

            return functionArgumentsIndex == functionArguments.size();
        }

        llvm::Function* getFunction(
            FunctionsScope& scope,
            const std::string& name,
            std::vector<llvm::Type*>& functionArguments
        ) {
            auto availableFunctionsIt = scope.equal_range(name);

            for (auto it = availableFunctionsIt.first; it != availableFunctionsIt.second; it++) {
                auto functionDefinitionArgs = it->second->args();

                if (areArgumentsEqual(functionDefinitionArgs, functionArguments)) {
                    return it->second;
                }
            }

            return nullptr;
        }

        std::vector<llvm::Type*> mapValuesToTypes(std::vector<llvm::Value*>& values) {
            std::vector<llvm::Type*> types;

            types.reserve(values.size());

            for (auto value : values) {
                types.push_back(value->getType());
            }

            return types;
        }
    }

    bool BlockContext::hasIdentifier(const std::string& name) {
        return hasVariable(name) || hasFunction(name);
    }

    bool BlockContext::hasVariable(const std::string& name) {
        return variablesScope.find(name) != variablesScope.end();
    }

    Variable BlockContext::getVariable(const std::string& name) {
        auto entry = variablesScope.find(name);

        return entry == variablesScope.end() ? Variable::empty() : entry->second;
    }

    Variable BlockContext::setVariable(const std::string& name, llvm::Value* value) {
        auto variable = Variable {
            value,
            true,
            true
        };

        return setVariable(name, variable);
    }

    Variable BlockContext::setVariable(const std::string& name, Variable& variable) {
        variablesScope.erase(name);
        variablesScope.insert(VariablesScope::value_type(name, variable));

        return variable;
    }

    void BlockContext::removeVariable(const std::string& name) {
        variablesScope.erase(name);
    }

    bool BlockContext::hasFunction(const std::string& name) {
        return functionsScope.find(name) != functionsScope.end();
    }

    llvm::Function* BlockContext::getFunction(const std::string& name, std::vector<llvm::Type*>& functionArguments) {
        return codegen::getFunction(functionsScope, name, functionArguments);
    }

    llvm::Function* BlockContext::getFunction(const std::string& name, std::vector<llvm::Value*>& functionArguments) {
        auto arguments = codegen::mapValuesToTypes(functionArguments);

        return getFunction(name, arguments);
    }

    std::vector<llvm::Function*> BlockContext::getFunctions(const std::string& name) {
        auto from = functionsScope.lower_bound(name);
        auto to = functionsScope.upper_bound(name);

        std::vector<llvm::Function*> result;

        transform(from, to, back_inserter(result), [](auto pair) { return pair.second; });

        return result;
    }

    llvm::Function* BlockContext::addFunction(const std::string& name, llvm::Function* function) {
        functionsScope.insert(FunctionsScope::value_type(name, function));

        return function;
    }

    llvm::Function* BlockContext::getPrivateFunction(const std::string& name, std::vector<llvm::Type*>& functionArguments) {
        return codegen::getFunction(privateFunctionsScope, name, functionArguments);
    }

    llvm::Function* BlockContext::getPrivateFunction(const std::string& name, std::vector<llvm::Value*>& functionArguments) {
        auto arguments = codegen::mapValuesToTypes(functionArguments);

        return getPrivateFunction(name, arguments);
    }

    llvm::Function* BlockContext::getPrivateFunction(const std::string& name) {
        return privateFunctionsScope.equal_range(name).first->second;
    }

    llvm::Function* BlockContext::addPrivateFunction(const std::string& name, llvm::Function* function) {
        privateFunctionsScope.insert(FunctionsScope::value_type(name, function));

        return function;
    }

    BlockContext BlockContext::createNestedBlock(llvm::BasicBlock* nestedBlock) {
        return BlockContext(
            nestedBlock,
            function,
            VariablesScope(variablesScope.begin(), variablesScope.end()),
            FunctionsScope(functionsScope.begin(), functionsScope.end()),
            FunctionsScope(privateFunctionsScope.begin(), privateFunctionsScope.end())
        );
    }
}
