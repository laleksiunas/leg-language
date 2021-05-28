#pragma once

#include <map>
#include "llvm/IR/Function.h"
#include "codegen/context/BlockContext.h"

namespace library {
    const auto FOPEN = "fopen";
    const auto FCLOSE = "fclose";
    const auto FPRINTF = "fprintf";

    const auto FILE_OPEN = "fileOpen";
    const auto FILE_CLOSE = "fileClose";
    const auto FILE_WRITE = "fileWrite";
    const auto FILE_WRITELN = "fileWriteln";

    namespace files {
        llvm::Type* registerFileHandleType() {
            auto name = "File";
            auto memberTypes = std::vector<llvm::Type*> { codegen::createVoidType()->getPointerTo() };

            auto* fileHandleType = llvm::StructType::create(
                codegen::Context(),
                llvm::makeArrayRef(memberTypes),
                name
            );

            codegen::Types().add(new codegen::Type(name, fileHandleType));

            return fileHandleType;
        }

        llvm::Function* createNativeFileOpenFunction() {
            auto arguments = std::vector<llvm::Type*> { codegen::createStringType(), codegen::createStringType() };

            return codegen::createNativeFunction(FOPEN, codegen::createVoidType()->getPointerTo(), arguments);
        }

        llvm::Function* createNativeFileCloseFunction() {
            auto arguments = std::vector<llvm::Type*> { codegen::createVoidType()->getPointerTo() };

            return codegen::createNativeFunction(FCLOSE, codegen::createIntType(), arguments);
        }

        llvm::Function* createNativeFileWriteFunction() {
            auto arguments = std::vector<llvm::Type*> {
                codegen::createVoidType()->getPointerTo(),
                codegen::createStringType()
            };

            return codegen::createNativeFunction(FPRINTF, codegen::createIntType(), arguments, true);
        }

        llvm::Value* getNativeFileHandlePointer(llvm::Value* fileHandle, llvm::BasicBlock* block) {
            auto pointerOffsets = std::vector<llvm::Value*> {
                llvm::ConstantInt::get(codegen::createIntType(), 0),
                llvm::ConstantInt::get(codegen::createIntType(), 0)
            };

            return llvm::GetElementPtrInst::Create(
                fileHandle->getType()->getPointerElementType(),
                fileHandle,
                llvm::makeArrayRef(pointerOffsets),
                "",
                block
            );
        }

        llvm::Value* allocateFileHandle(
            llvm::Type* fileHandleType,
            llvm::Value* nativeHandle,
            llvm::BasicBlock* block
        ) {
            auto* handleVariable = new llvm::AllocaInst(fileHandleType, 0, "", block);

            new llvm::StoreInst(nativeHandle, getNativeFileHandlePointer(handleVariable, block), block);

            return new llvm::LoadInst(fileHandleType, handleVariable, "", block);
        }

        llvm::Value* loadNativeFileHandle(llvm::Value* fileHandle, llvm::BasicBlock* block) {
            auto* handleVariable = new llvm::AllocaInst(fileHandle->getType(), 0, "", block);

            new llvm::StoreInst(fileHandle, handleVariable, block);

            return new llvm::LoadInst(
                codegen::createVoidType()->getPointerTo(),
                getNativeFileHandlePointer(handleVariable, block),
                "",
                block
            );
        }

        llvm::Function* createFileOpenFunction(llvm::Type* fileHandleType) {
            auto arguments = std::vector<llvm::Type*> { codegen::createStringType(), codegen::createStringType() };

            auto* fileOpenFunction = llvm::Function::Create(
                llvm::FunctionType::get(fileHandleType, llvm::makeArrayRef(arguments), false),
                llvm::Function::InternalLinkage,
                FILE_OPEN,
                codegen::Module().get()
            );

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", fileOpenFunction);

            auto nativeFileOpenFunctionCallArgs = std::vector<llvm::Value*> {
                fileOpenFunction->getArg(0),
                fileOpenFunction->getArg(1)
            };

            auto* fileHandle = llvm::CallInst::Create(
                createNativeFileOpenFunction(),
                llvm::makeArrayRef(nativeFileOpenFunctionCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(
                codegen::Context(),
                allocateFileHandle(fileHandleType, fileHandle, implementationBlock),
                implementationBlock
            );

            return fileOpenFunction;
        }

        llvm::Function* createFileCloseFunction(llvm::Type* fileHandleType) {
            auto arguments = std::vector<llvm::Type*> { fileHandleType };

            auto* fileCloseFunctionType = llvm::FunctionType::get(
                codegen::createVoidType(),
                llvm::makeArrayRef(arguments),
                false
            );

            auto* fileCloseFunction = llvm::Function::Create(
                fileCloseFunctionType,
                llvm::Function::InternalLinkage,
                FILE_CLOSE,
                codegen::Module().get()
            );

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", fileCloseFunction);

            auto nativeFileCloseFunctionCallArgs = std::vector<llvm::Value*> {
                loadNativeFileHandle(fileCloseFunction->getArg(0), implementationBlock),
            };

            llvm::CallInst::Create(
                createNativeFileCloseFunction(),
                llvm::makeArrayRef(nativeFileCloseFunctionCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(codegen::Context(), implementationBlock);

            return fileCloseFunction;
        }

        llvm::Function* createFileWriteFunctionDefinition(
            const std::string& name,
            llvm::Type* fileHandleType,
            llvm::Type* argumentType
        ) {
            auto writeArguments = argumentType == nullptr
                ? std::vector<llvm::Type*> { fileHandleType }
                : std::vector<llvm::Type*> { fileHandleType, argumentType };

            auto* writeFileFunctionType = llvm::FunctionType::get(
                codegen::createVoidType(),
                llvm::makeArrayRef(writeArguments),
                false
            );

            return llvm::Function::Create(
                writeFileFunctionType,
                llvm::Function::InternalLinkage,
                name,
                codegen::Module().get()
            );
        }

        llvm::Function* createFileWriteFunction(
            llvm::Function* nativeFunction,
            const std::string& name,
            const std::string& printFormat,
            llvm::Type* argumentType,
            llvm::Type* fileHandleType
        ) {
            auto* writeFileFunction = createFileWriteFunctionDefinition(name, fileHandleType, argumentType);

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", writeFileFunction);

            auto nativeWriteFileCallArgs = std::vector<llvm::Value*> {
                loadNativeFileHandle(writeFileFunction->getArg(0), implementationBlock),
                codegen::types::createConstString(printFormat),
                writeFileFunction->getArg(1)
            };

            llvm::CallInst::Create(
                nativeFunction,
                llvm::makeArrayRef(nativeWriteFileCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(codegen::Context(), implementationBlock);

            return writeFileFunction;
        }

        llvm::Function* createEmptyFileWriteFunction(
            llvm::Function* nativeFunction,
            const std::string& name,
            llvm::Type* fileHandleType
        ) {
            auto* writeFileFunction = createFileWriteFunctionDefinition(name, fileHandleType, nullptr);

            auto* implementationBlock = llvm::BasicBlock::Create(codegen::Context(), "", writeFileFunction);

            auto nativeWriteFileCallArgs = std::vector<llvm::Value*> {
                loadNativeFileHandle(writeFileFunction->getArg(0), implementationBlock),
                codegen::types::createConstString("\n"),
            };

            llvm::CallInst::Create(
                nativeFunction,
                llvm::makeArrayRef(nativeWriteFileCallArgs),
                "",
                implementationBlock
            );

            llvm::ReturnInst::Create(codegen::Context(), implementationBlock);

            return writeFileFunction;
        }
    }

    void registerFilesFunctions(codegen::BlockContext& rootContext) {
        auto* fileHandleType = files::registerFileHandleType();
        auto* nativeFileWriteFunction = files::createNativeFileWriteFunction();

        rootContext.addFunction(FILE_OPEN, files::createFileOpenFunction(fileHandleType));
        rootContext.addFunction(FILE_CLOSE, files::createFileCloseFunction(fileHandleType));

        rootContext.addFunction(
            FILE_WRITELN, files::createEmptyFileWriteFunction(
                nativeFileWriteFunction,
                FILE_WRITELN,
                fileHandleType
            )
        );

        auto formatByArgType = std::map<llvm::Type*, std::string> {
            { codegen::createStringType(), "%s" },
            { codegen::createIntType(), "%d" },
            { codegen::createFloatType(), "%f" },
            { codegen::createBooleanType(), "%d" },
        };

        for (auto [argType, format] : formatByArgType) {
            rootContext.addFunction(
                FILE_WRITELN,
                files::createFileWriteFunction(
                    nativeFileWriteFunction,
                    FILE_WRITELN,
                    fmt::format("{}\n", format),
                    argType,
                    fileHandleType
                )
            );

            rootContext.addFunction(
                FILE_WRITE,
                files::createFileWriteFunction(
                    nativeFileWriteFunction,
                    FILE_WRITE,
                    format,
                    argType,
                    fileHandleType
                )
            );
        }
    }
}
