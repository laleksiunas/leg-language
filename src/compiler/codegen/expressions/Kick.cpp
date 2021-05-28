#include <vector>
#include <string>
#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "ast/expressions/KickNode.h"
#include "codegen/context/BlockContext.h"
#include "codegen/utils.h"
#include "library/multithreading/multithreading.h"
#include "library/memory/memory.h"
#include "shared/KickResult.h"

llvm::Type* ast::KickNode::createThreadDataStructType(std::vector<llvm::Value*>& routineFunctionArgs) const {
    auto memberTypes = std::vector<llvm::Type*>();

    memberTypes.reserve(routineFunctionArgs.size());

    for (auto argument : routineFunctionArgs) {
        memberTypes.push_back(argument->getType());
    }

    return llvm::StructType::create(
        codegen::Context(),
        llvm::makeArrayRef(memberTypes),
        functionCall->functionName->value + "_thread_data"
    );
}

std::vector<llvm::GetElementPtrInst*> ast::KickNode::createThreadDataStructMemberAccessPointers(
    llvm::BasicBlock* block,
    llvm::Value* threadDataPointer,
    size_t memberCount
) {
    std::vector<llvm::GetElementPtrInst*> pointers;

    pointers.reserve(memberCount);

    for (int i = 0; i < memberCount; i++) {
        auto pointerOffsets = std::vector<llvm::Value*> {
            llvm::ConstantInt::get(codegen::createIntType(), 0),
            llvm::ConstantInt::get(codegen::createIntType(), i)
        };

        pointers.push_back(
            llvm::GetElementPtrInst::Create(
                threadDataPointer->getType()->getPointerElementType(),
                threadDataPointer,
                llvm::makeArrayRef(pointerOffsets),
                "",
                block
            )
        );
    }

    return pointers;
}

llvm::Value* ast::KickNode::allocateMemoryForType(
    codegen::BlockContext& blockContext,
    llvm::Type* dataType,
    llvm::BasicBlock* wrapperBlock
) {
    auto typeSize = codegen::Module()->getDataLayout().getTypeAllocSize(dataType);

    auto* mallocFunction = blockContext.getPrivateFunction(library::MALLOC);

    auto mallocArgs = std::vector<llvm::Value*> {
        llvm::ConstantInt::get(codegen::createIntType(), typeSize)
    };

    auto* dataPointer = llvm::CallInst::Create(
        mallocFunction,
        llvm::makeArrayRef(mallocArgs),
        "",
        wrapperBlock
    );

    return llvm::BitCastInst::Create(
        llvm::Instruction::BitCast,
        dataPointer,
        dataType->getPointerTo(),
        "",
        wrapperBlock
    );
}

llvm::Value* ast::KickNode::allocateThreadData(
    codegen::BlockContext& blockContext,
    std::vector<llvm::Value*>& routineFunctionArgs
) {
    if (routineFunctionArgs.empty()) {
        return nullptr;
    }

    auto* threadDataType = createThreadDataStructType(routineFunctionArgs);
    auto* threadDataPointer = allocateMemoryForType(blockContext, threadDataType, blockContext.getBlock());
    auto threadDataMemberAccessPointers = createThreadDataStructMemberAccessPointers(
        blockContext.getBlock(),
        threadDataPointer,
        routineFunctionArgs.size()
    );

    for (int i = 0; i < routineFunctionArgs.size(); i++) {
        new llvm::StoreInst(
            routineFunctionArgs[i],
            threadDataMemberAccessPointers[i],
            blockContext.getBlock()
        );
    }

    return threadDataPointer;
}

void ast::KickNode::addDeallocateThreadDataInstruction(
    codegen::BlockContext& blockContext,
    llvm::Value* threadDataPointer,
    llvm::BasicBlock* routineWrapperFunctionBlock
) {
    auto* freeFunction = blockContext.getPrivateFunction(library::FREE);

    auto freeFunctionArgs = std::vector<llvm::Value*> { threadDataPointer };

    llvm::CallInst::Create(freeFunction, llvm::makeArrayRef(freeFunctionArgs), "", routineWrapperFunctionBlock);
}

llvm::Value* ast::KickNode::addParameterlessRoutineCall(llvm::Function* routineFunction, llvm::BasicBlock* routineWrapperFunctionBlock) {
    auto arguments = std::vector<llvm::Value*>();

    return llvm::CallInst::Create(routineFunction, llvm::makeArrayRef(arguments), "", routineWrapperFunctionBlock);
}

llvm::Value* ast::KickNode::addParameterRoutineCall(
    codegen::BlockContext& blockContext,
    llvm::Function* routineFunction,
    llvm::BasicBlock* routineWrapperFunctionBlock,
    llvm::Value* rawThreadDataPointer,
    llvm::Type* threadDataType
) {
    auto argumentsSize = routineFunction->arg_size();
    auto arguments = std::vector<llvm::Value*>();

    arguments.reserve(argumentsSize);

    auto* threadDataPointer = new llvm::BitCastInst(
        rawThreadDataPointer,
        threadDataType,
        "",
        routineWrapperFunctionBlock
    );

    auto threadDataMemberAccessPointers = createThreadDataStructMemberAccessPointers(
        routineWrapperFunctionBlock,
        threadDataPointer,
        argumentsSize
    );

    for (int i = 0; i < argumentsSize; i++) {
        auto routineFunctionArgumentType = routineFunction->getArg(i)->getType();

        arguments.push_back(
            new llvm::LoadInst(
                routineFunctionArgumentType,
                threadDataMemberAccessPointers[i],
                "",
                routineWrapperFunctionBlock
            )
        );
    }

    return llvm::CallInst::Create(routineFunction, llvm::makeArrayRef(arguments), "", routineWrapperFunctionBlock);
}

llvm::Value* ast::KickNode::addRoutineFunctionCall(
    codegen::BlockContext& blockContext,
    llvm::Type* threadDataType,
    llvm::Function* wrapperFunction,
    llvm::Function* routineFunction,
    llvm::BasicBlock* implementationBlock
) {
    if (routineFunction->arg_empty()) {
        return addParameterlessRoutineCall(routineFunction, implementationBlock);
    }

    auto* threadDataPointer = wrapperFunction->getArg(0);

    auto* routineResult = addParameterRoutineCall(
        blockContext,
        routineFunction,
        implementationBlock,
        threadDataPointer,
        threadDataType
    );
    addDeallocateThreadDataInstruction(blockContext, threadDataPointer, implementationBlock);

    return routineResult;
}

llvm::Value* ast::KickNode::allocateRoutineResult(
    codegen::BlockContext& blockContext,
    llvm::Value* routineResult,
    llvm::BasicBlock* implementationBlock
) {
    auto* resultPointer = allocateMemoryForType(blockContext, routineResult->getType(), implementationBlock);

    new llvm::StoreInst(routineResult, resultPointer, implementationBlock);

    return resultPointer;
}

llvm::Function* ast::KickNode::createRoutineWrapperFunction(
    codegen::BlockContext& blockContext,
    llvm::Function* routineFunction,
    llvm::Type* threadDataType,
    bool canAllocateReturnValue
) const {
    auto isReturningValue = canAllocateReturnValue && !routineFunction->getReturnType()->isVoidTy();

    auto wrapperFunctionArgs = std::vector<llvm::Type*> { codegen::createVoidType()->getPointerTo() };

    auto* wrapperFunctionType = llvm::FunctionType::get(
        codegen::createVoidType()->getPointerTo(),
        wrapperFunctionArgs,
        false
    );

    auto* wrapperFunction = llvm::Function::Create(
        wrapperFunctionType,
        llvm::GlobalValue::InternalLinkage,
        functionCall->functionName->value + "_thread_wrapper",
        codegen::Module().get()
    );

    auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", wrapperFunction);

    auto* routineResult = addRoutineFunctionCall(
        blockContext,
        threadDataType,
        wrapperFunction,
        routineFunction,
        implementationBlock
    );

    if (isReturningValue) {
        auto* resultPointer = allocateRoutineResult(blockContext, routineResult, implementationBlock);

        llvm::ReturnInst::Create(
            codegen::Context(),
            castToVoidPointer(resultPointer, implementationBlock),
            implementationBlock
        );
    } else {
        llvm::ReturnInst::Create(codegen::Context(), codegen::createNullptr(), implementationBlock);
    }

    return wrapperFunction;
}

llvm::Value* ast::KickNode::allocateThreadId(codegen::BlockContext& blockContext) const {
    return new llvm::AllocaInst(
        codegen::createArchIntegerType()->getPointerTo(),
        0,
        functionCall->functionName->value + "_thread_id",
        blockContext.getBlock()
    );
}

llvm::Value* ast::KickNode::loadThreadId(codegen::BlockContext& blockContext, llvm::Value* threadIdPointer) {
    return new llvm::LoadInst(
        threadIdPointer->getType()->getPointerElementType(),
        threadIdPointer,
        "",
        blockContext.getBlock()
    );
}

void ast::KickNode::addSpawnThreadInstruction(codegen::BlockContext& blockContext, std::vector<llvm::Value*>& arguments) {
    auto* pthreadCreateFunction = blockContext.getPrivateFunction(library::PTHREAD_CREATE);

    llvm::CallInst::Create(pthreadCreateFunction, llvm::makeArrayRef(arguments), "", blockContext.getBlock());
}

llvm::Value* ast::KickNode::castToVoidPointer(llvm::Value* value, llvm::BasicBlock* block) {
    return new llvm::BitCastInst(
        value,
        codegen::createVoidType()->getPointerTo(),
        "",
        block
    );
}

shared::KickResult ast::KickNode::codegen(codegen::BlockContext& blockContext, bool isCatchBlock) {
    auto routineFunctionArgs = generateFunctionArguments(blockContext, *functionCall->arguments);

    functionCall->validateFunction(blockContext, routineFunctionArgs);

    auto* routineFunction = blockContext.getFunction(functionCall->functionName->value, routineFunctionArgs);
    auto* threadDataPointer = allocateThreadData(blockContext, routineFunctionArgs);

    auto* threadIdPointer = allocateThreadId(blockContext);
    auto* threadAttributesPointer = codegen::createNullptr();
    auto* routineWrapperFunction = createRoutineWrapperFunction(
        blockContext,
        routineFunction,
        threadDataPointer == nullptr ? nullptr : threadDataPointer->getType(),
        isCatchBlock
    );

    auto pthreadArguments = std::vector<llvm::Value*> {
        threadIdPointer,
        threadAttributesPointer,
        routineWrapperFunction,
        threadDataPointer == nullptr
            ? codegen::createNullptr()
            : castToVoidPointer(threadDataPointer, blockContext.getBlock()),
    };

    addSpawnThreadInstruction(blockContext, pthreadArguments);

    return shared::KickResult {
        loadThreadId(blockContext, threadIdPointer),
        routineFunction
    };
}

llvm::Value* ast::KickNode::codegen(codegen::BlockContext& blockContext) {
    return codegen(blockContext, false).threadId;
}

shared::KickResult ast::KickNode::codegenInCatch(codegen::BlockContext& blockContext) {
    return codegen(blockContext, true);
}
