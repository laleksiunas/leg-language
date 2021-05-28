#pragma once

#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "FunctionCallNode.h"
#include "codegen/context/BlockContext.h"
#include "shared/KickResult.h"

namespace ast {
    class KickNode : public ExpressionNode {
    public:
        FunctionCallNode* functionCall;

        explicit KickNode(FunctionCallNode* functionCall) : functionCall(functionCall) {}

        ~KickNode() override {
            delete functionCall;
        }

        llvm::Value* codegen(codegen::BlockContext& blockContext) override;

        shared::KickResult codegenInCatch(codegen::BlockContext& blockContext);

    private:
        shared::KickResult codegen(codegen::BlockContext& blockContext, bool isCatchBlock);

        llvm::Type* createThreadDataStructType(std::vector<llvm::Value*>& routineFunctionArgs) const;

        static std::vector<llvm::GetElementPtrInst*> createThreadDataStructMemberAccessPointers(
            llvm::BasicBlock* block,
            llvm::Value* threadDataPointer,
            size_t memberCount
        );

        static llvm::Value* allocateMemoryForType(
            codegen::BlockContext& blockContext,
            llvm::Type* dataType,
            llvm::BasicBlock* wrapperBlock
        );

        static void addDeallocateThreadDataInstruction(
            codegen::BlockContext& blockContext,
            llvm::Value* threadDataPointer,
            llvm::BasicBlock* routineWrapperFunctionBlock
        );

        static llvm::Value* addParameterlessRoutineCall(
            llvm::Function* routineFunction,
            llvm::BasicBlock* routineWrapperFunctionBlock
        );

        static llvm::Value* addParameterRoutineCall(
            codegen::BlockContext& blockContext,
            llvm::Function* routineFunction,
            llvm::BasicBlock* routineWrapperFunctionBlock,
            llvm::Value* threadDataPointer,
            llvm::Type* threadDataType
        );

        static llvm::Value* addRoutineFunctionCall(
            codegen::BlockContext& blockContext,
            llvm::Type* threadDataType,
            llvm::Function* wrapperFunction,
            llvm::Function* routineFunction,
            llvm::BasicBlock* implementationBlock
        );

        static llvm::Value* allocateRoutineResult(
            codegen::BlockContext& blockContext,
            llvm::Value* routineResult,
            llvm::BasicBlock* implementationBlock
        );

        static llvm::Value* loadThreadId(codegen::BlockContext& blockContext, llvm::Value* threadIdPointer);

        static void addSpawnThreadInstruction(
            codegen::BlockContext& blockContext,
            std::vector<llvm::Value*>& arguments
        );

        llvm::Value* allocateThreadData(
            codegen::BlockContext& blockContext,
            std::vector<llvm::Value*>& routineFunctionArgs
        );

        llvm::Value* allocateThreadId(codegen::BlockContext& blockContext) const;

        llvm::Function* createRoutineWrapperFunction(
            codegen::BlockContext& blockContext,
            llvm::Function* routineFunction,
            llvm::Type* threadDataType,
            bool canAllocateReturnValue
        ) const;

        static llvm::Value* castToVoidPointer(llvm::Value* threadData, llvm::BasicBlock* block);
    };
}
