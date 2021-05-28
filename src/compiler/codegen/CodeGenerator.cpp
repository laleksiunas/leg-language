#include <optional>
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "codegen/context/context.h"
#include "context/BlockContext.cpp"
#include "codegen/types/primitive/primitiveTypes.h"
#include "ast/Node.h"
#include "library/library.h"
#include "CodeGenerator.h"

namespace codegen {
    namespace {
        void addOptimizationPasses(llvm::legacy::PassManager& passManager) {
            passManager.add(llvm::createDeadCodeEliminationPass());
            passManager.add(llvm::createStripDeadPrototypesPass());
        }

        void addDebugPasses(llvm::legacy::PassManager& passManager) {
            passManager.add(llvm::createPrintModulePass(llvm::outs()));
            passManager.add(llvm::createVerifierPass());
        }
    }

    std::optional<CodeGenerator> CodeGenerator::from(ast::Node* node) {
        auto rootBlockContext = BlockContext::root();

        library::registerCoreLibraryFunctions(rootBlockContext);
        codegen::registerPrimitiveTypes();

        node->codegen(rootBlockContext);

        auto* mainFunction = getMainFunction();

        if (Errors().hasErrors()) {
            return std::nullopt;
        }

        auto passManager = llvm::legacy::PassManager();

#ifdef NDEBUG
        addOptimizationPasses(passManager);
#else
        addDebugPasses(passManager);
#endif

        passManager.run(*codegen::Module());

        auto* engine = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(codegen::Module().get()))
            .setEngineKind(llvm::EngineKind::JIT)
            .create();

        engine->finalizeObject();

        return std::optional(CodeGenerator(engine, mainFunction));
    }

    void CodeGenerator::runCode() {
        executionEngine->runFunction(mainFunction, std::vector<llvm::GenericValue>());
    }

    void CodeGenerator::shutdown() {
        executionEngine->removeModule(codegen::Module().get());
        delete executionEngine;
    }

    llvm::Function* CodeGenerator::getMainFunction() {
        auto* function = codegen::Module()->getFunction("main");

        if (function == nullptr || !function->arg_empty()) {
            codegen::Errors().reportGlobal("Cannot find entry function 'fn main(): void'");
        }

        return function;
    }
}
