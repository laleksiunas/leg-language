#pragma once

#include <string>
#include <iostream>
#include <numeric>
#include "fmt/format.h"
#include "llvm/IR/Type.h"
#include "CompilationException.h"
#include "codegen/keywords.h"

namespace codegen {
    class ErrorsContainer {
    public:
        explicit ErrorsContainer(TypesContainer& types) : types(types) {}

        bool hasErrors() const {
            return errorsReported;
        }

        void reportUndeclaredVariable(const std::string& variableName, int lineNumber) {
            report(fmt::format("Use of undeclared variable '{}'", variableName), lineNumber);
        }

        void reportUndeclaredType(const std::string& typeName, int lineNumber) {
            report(fmt::format("Use of undeclared type '{}'", typeName), lineNumber);
        }

        void reportAssignmentToImmutableVariable(const std::string& variableName, int lineNumber) {
            report(fmt::format("Cannot assign to immutable variable '{}'", variableName), lineNumber);
        }

        void reportIdentifierRedeclaration(const std::string& identifierName, int lineNumber) {
            report(
                fmt::format("Declaration of identifier '{}' which is already declared in the scope", identifierName),
                lineNumber
            );
        }

        void reportUseOfKeywordForIdentifier(const std::string& keyword, int lineNumber) {
            report(
                fmt::format("Cannot use reserved keyword '{}' as an identifier", keyword),
                lineNumber
            );
        }

        void reportAssignmentTypesMismatch(
            const std::string& variableName,
            llvm::Type* expectedType,
            llvm::Type* receivedType,
            int lineNumber
        ) {
            auto expectedTypeName = types.getName(expectedType);
            auto receivedTypeName = types.getName(receivedType);

            auto message = fmt::format(
                "Cannot assign value of type '{}' to variable '{}' of type '{}'",
                receivedTypeName,
                variableName,
                expectedTypeName
            );

            report(message, lineNumber);
        }

        void reportUndeclaredFunctionCall(
            const std::string& functionName,
            std::vector<llvm::Type*>& callArgumentTypes,
            int lineNumber
        ) {
            auto functionCallText = functionToString(functionName, callArgumentTypes);

            report(fmt::format("Call to undeclared function '{}'", functionCallText), lineNumber);
        }

        void reportUndeclaredFunctionCall(
            const std::string& functionName,
            std::vector<llvm::Type*>& callArgumentTypes,
            std::vector<llvm::Function*>& availableFunctions,
            int lineNumber
        ) {
            auto functionCallText = functionToString(functionName, callArgumentTypes);
            auto availableFunctionsText = std::accumulate(
                availableFunctions.begin(),
                availableFunctions.end(),
                std::string(),
                [&](std::string& acc, llvm::Function* function)
                {
                    return acc.append("\n\t").append(functionToString(functionName, function));
                }
            );

            auto errorText = fmt::format(
                "Call to undeclared function '{}'. Possible candidates: {}",
                functionCallText,
                availableFunctionsText
            );

            report(errorText, lineNumber);
        }

        void reportFunctionRedeclaration(
            const std::string& functionName,
            std::vector<llvm::Type*>& argumentTypes,
            int lineNumber
        ) {
            auto functionText = functionToString(functionName, argumentTypes);

            report(
                fmt::format("Declaration of function '{}' which is already declared in the scope", functionText),
                lineNumber
            );
        }

        void reportInvalidOperatorUsage(
            const std::string& operatorName,
            llvm::Type* left,
            llvm::Type* right,
            int lineNumber
        ) {
            auto errorText = fmt::format(
                "Operator '{}' cannot be applied to operands of type '{}' and '{}'",
                operatorName,
                types.getName(left),
                types.getName(right)
            );

            report(errorText, lineNumber);
        }

        void report(const std::string& error, int lineNumber) {
            errorsReported = true;

            printError(error, lineNumber);

            throw CompilationException();
        }

        void reportGlobal(const std::string& error) {
            errorsReported = true;

            printError(error);
        }

    private:
        bool errorsReported = false;
        TypesContainer& types;

        static void printError(const std::string& error) {
            std::cout << "Compilation error: " << error << std::endl << std::endl;
        }

        static void printError(const std::string& error, int lineNumber) {
            std::cout << "Compilation error (line " << lineNumber << "): " << error << std::endl << std::endl;
        }

        std::string functionToString(const std::string& name, std::vector<llvm::Type*>& argumentTypes) {
            auto argumentTypesText = std::accumulate(
                argumentTypes.begin(),
                argumentTypes.end(),
                std::string(),
                [&](std::string& acc, llvm::Type* argumentType)
                {
                    if (argumentType->isVoidTy()) {
                        return acc;
                    }

                    auto typeName = types.getName(argumentType);

                    return acc.empty() ? acc.append(typeName) : acc.append(", ").append(typeName);
                }
            );

            return fmt::format("{}({})", name, argumentTypesText);
        }

        std::string functionToString(const std::string& name, llvm::Function* function) {
            auto returnType = types.getName(function->getReturnType());

            auto argumentTypes = std::accumulate(
                function->arg_begin(),
                function->arg_end(),
                std::string(),
                [&](std::string& acc, llvm::Argument& arg)
                {
                    if (arg.getType()->isVoidTy()) {
                        return acc;
                    }

                    auto typeName = types.getName(arg.getType());

                    return acc.empty() ? acc.append(typeName) : acc.append(", ").append(typeName);
                }
            );

            return fmt::format("{} {}({})", returnType, name, argumentTypes);
        }
    };
}
