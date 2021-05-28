#include <vector>
#include <set>
#include "fmt/format.h"
#include "llvm/IR/Value.h"
#include "ast/statements/CatchNode.h"
#include "library/multithreading/multithreading.h"
#include "library/memory/memory.h"
#include "library/io/print.h"
#include "shared/KickResult.h"

void ast::CatchNode::validateDestructuringIdentifiers(codegen::BlockContext& blockContext) {
    std::set<std::string> existingIdentifiers;

    for (auto* identifier : *destructuringIdentifiers) {
        if (identifier->isDiscard()) {
            continue;
        }

        if (codegen::keywords::isReservedKeyword(identifier->value)) {
            codegen::Errors().reportUseOfKeywordForIdentifier(identifier->value, lineNumber);
        }

        if (blockContext.hasIdentifier(identifier->value) || existingIdentifiers.contains(identifier->value)) {
            codegen::Errors().reportIdentifierRedeclaration(identifier->value, lineNumber);
        }

        existingIdentifiers.insert(identifier->value);
    }
}

std::vector<shared::KickResult> ast::CatchNode::spawnThreads(codegen::BlockContext& blockContext) const {
    std::vector<shared::KickResult> kickResults;

    kickResults.reserve(arguments->size());

    for (auto& kickArgument : *arguments) {
        kickResults.push_back(kickArgument->codegenInCatch(blockContext));
    }

    return kickResults;
}

void ast::CatchNode::joinVoidThread(codegen::BlockContext& blockContext, shared::KickResult& kickResult) {
    auto* pthreadJoinFunction = blockContext.getPrivateFunction(library::PTHREAD_JOIN);

    auto joinArguments = std::vector<llvm::Value*> {
        kickResult.threadId,
        llvm::ConstantPointerNull::get(codegen::createVoidType()->getPointerTo()->getPointerTo())
    };

    llvm::CallInst::Create(pthreadJoinFunction, llvm::makeArrayRef(joinArguments), "", blockContext.getBlock());
}

llvm::Value* ast::CatchNode::joinResultThread(codegen::BlockContext& blockContext, shared::KickResult& kickResult) {
    auto* pthreadJoinFunction = blockContext.getPrivateFunction(library::PTHREAD_JOIN);

    auto* rawResultPointer = new llvm::AllocaInst(
        codegen::createVoidType()->getPointerTo(),
        0,
        kickResult.routineFunction->getName() + "_result",
        blockContext.getBlock()
    );

    auto joinArguments = std::vector<llvm::Value*> {
        kickResult.threadId,
        rawResultPointer
    };

    llvm::CallInst::Create(pthreadJoinFunction, llvm::makeArrayRef(joinArguments), "", blockContext.getBlock());

    auto* resultPointer = new llvm::BitCastInst(
        rawResultPointer,
        kickResult.routineFunction->getReturnType()->getPointerTo()->getPointerTo(),
        "",
        blockContext.getBlock()
    );

    return new llvm::LoadInst(
        resultPointer->getType()->getPointerElementType(),
        resultPointer,
        "",
        blockContext.getBlock()
    );
}

std::vector<llvm::Value*> ast::CatchNode::joinThreads(
    codegen::BlockContext& blockContext,
    std::vector<shared::KickResult>& kickResults
) {
    std::vector<llvm::Value*> resultsPointers;

    resultsPointers.reserve(kickResults.size());

    for (auto& kickResult : kickResults) {
        if (kickResult.routineFunction->getReturnType()->isVoidTy()) {
            joinVoidThread(blockContext, kickResult);

            resultsPointers.push_back(nullptr);
        } else {
            resultsPointers.push_back(joinResultThread(blockContext, kickResult));
        }
    }

    return resultsPointers;
}

std::vector<llvm::Value*> ast::CatchNode::loadReturnValues(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Value*>& returnValuePointers
) {
    std::vector<llvm::Value*> returnValues;

    returnValues.reserve(returnValuePointers.size());

    for (auto valuePointer : returnValuePointers) {
        if (valuePointer == nullptr) {
            returnValues.push_back(nullptr);
            continue;
        }

        auto* value = new llvm::LoadInst(
            valuePointer->getType()->getPointerElementType(),
            valuePointer,
            "",
            blockContext.getBlock()
        );

        returnValues.push_back(value);
    }

    return returnValues;
}

void ast::CatchNode::deallocateReturnValuePointers(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Value*>& returnValuePointers
) {
    auto* freeFunction = blockContext.getPrivateFunction(library::FREE);

    for (auto valuePointer : returnValuePointers) {
        if (valuePointer == nullptr) {
            continue;
        }

        auto* rawValuePointer = new llvm::BitCastInst(
            valuePointer,
            codegen::createVoidType()->getPointerTo(),
            "",
            blockContext.getBlock()
        );

        auto freeFunctionArgs = std::vector<llvm::Value*> { rawValuePointer };

        llvm::CallInst::Create(freeFunction, llvm::makeArrayRef(freeFunctionArgs), "", blockContext.getBlock());
    }
}

void ast::CatchNode::assignValuesToVariables(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Value*>& values
) const {
    for (int i = 0; i < destructuringIdentifiers->size(); i++) {
        auto* value = values[i];
        auto* identifier = destructuringIdentifiers->at(i);

        if (!identifier->isDiscard()) {
            if (value == nullptr) {
                auto errorText = fmt::format(
                    "Cannot assign {} return type to variable {}",
                    codegen::keywords::Void,
                    identifier->value
                );

                codegen::Errors().report(errorText, lineNumber);
            }

            declareValue(blockContext, identifier->value, value);
        }
    }
}

llvm::Value* ast::CatchNode::codegen(codegen::BlockContext& blockContext) {
    validateDestructuringIdentifiers(blockContext);

    auto kickResults = spawnThreads(blockContext);
    auto returnValuePointers = joinThreads(blockContext, kickResults);
    auto returnValues = loadReturnValues(blockContext, returnValuePointers);

    deallocateReturnValuePointers(blockContext, returnValuePointers);

    assignValuesToVariables(blockContext, returnValues);

    return nullptr;
}
