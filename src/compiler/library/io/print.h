#pragma once

#include <map>
#include "llvm/IR/Function.h"
#include "codegen/types/primitive/string.h"
#include "codegen/context/BlockContext.h"

namespace library {
    const auto PRINTF = "printf";
    const auto PRINTLN = "println";
    const auto PRINT = "print";

    namespace print {
        llvm::Function* createNativePrintfFunction() {
            auto printfArguments = std::vector<llvm::Type*> { codegen::createStringType() };

            return codegen::createNativeFunction(PRINTF, codegen::createIntType(), printfArguments, true);
        }

        llvm::Function* createPrintFunctionDefinition(const std::string& name, llvm::Type* argumentType) {
            auto printArguments = argumentType == nullptr
                ? std::vector<llvm::Type*>()
                : std::vector<llvm::Type*> { argumentType };

            auto* printFunctionType = llvm::FunctionType::get(
                codegen::createVoidType(),
                llvm::makeArrayRef(printArguments),
                false
            );

            return llvm::Function::Create(
                printFunctionType,
                llvm::Function::InternalLinkage,
                name,
                codegen::Module().get()
            );
        }

        llvm::Function* createPrintFunction(
            llvm::Function* nativePrintfFunction,
            const std::string& name,
            const std::string& printFormat,
            llvm::Type* argumentType
        ) {
            auto* printFunction = createPrintFunctionDefinition(name, argumentType);

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", printFunction);

            auto nativePrintfCallArgs = std::vector<llvm::Value*> {
                codegen::types::createConstString(printFormat),
                printFunction->getArg(0)
            };

            llvm::CallInst::Create(
                nativePrintfFunction,
                llvm::makeArrayRef(nativePrintfCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(codegen::Context(), implementationBlock);

            return printFunction;
        }

        llvm::Function* createEmptyLinePrintFunction(llvm::Function* nativePrintfFunction, const std::string& name) {
            auto* printFunction = createPrintFunctionDefinition(name, nullptr);

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", printFunction);

            auto nativePrintfCallArgs = std::vector<llvm::Value*> {
                codegen::types::createConstString("\n"),
            };

            llvm::CallInst::Create(
                nativePrintfFunction,
                llvm::makeArrayRef(nativePrintfCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(codegen::Context(), implementationBlock);

            return printFunction;
        }
    }

    void registerPrintFunctions(codegen::BlockContext& rootContext) {
        auto* nativePrintfFunction = print::createNativePrintfFunction();

        rootContext.addFunction(PRINTLN, print::createEmptyLinePrintFunction(nativePrintfFunction, PRINTLN));

        auto formatByArgType = std::map<llvm::Type*, std::string> {
            { codegen::createStringType(), "%s" },
            { codegen::createIntType(), "%d" },
            { codegen::createFloatType(), "%f" },
            { codegen::createBooleanType(), "%d" },
        };

        for (auto [argType, format] : formatByArgType) {
            rootContext.addFunction(
                PRINTLN,
                print::createPrintFunction(nativePrintfFunction, PRINTLN, fmt::format("{}\n", format), argType)
            );

            rootContext.addFunction(
                PRINT,
                print::createPrintFunction(nativePrintfFunction, PRINT, format, argType)
            );
        }
    }
}
