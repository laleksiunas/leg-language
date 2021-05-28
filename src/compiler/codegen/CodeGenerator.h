#pragma once

#include "llvm/IR/Function.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include "codegen/expressions/Block.cpp"
#include "codegen/expressions/Integer.cpp"
#include "codegen/expressions/Float.cpp"
#include "codegen/expressions/String.cpp"
#include "codegen/expressions/FunctionCall.cpp"
#include "codegen/expressions/Variable.cpp"
#include "codegen/expressions/Kick.cpp"
#include "codegen/expressions/Arithmetics.cpp"
#include "codegen/expressions/Boolean.cpp"
#include "codegen/expressions/CreateArray.cpp"
#include "codegen/expressions/GetterIndexer.cpp"
#include "codegen/expressions/GetterProperty.cpp"
#include "codegen/expressions//InitializeArray.cpp"
#include "codegen/statements/FunctionDeclaration.cpp"
#include "codegen/statements/ExpressionStatement.cpp"
#include "codegen/statements/FunctionReturnStatement.cpp"
#include "codegen/statements/AssignmentStatement.cpp"
#include "codegen/statements/DeclarationStatement.cpp"
#include "codegen/statements/CatchStatement.cpp"
#include "codegen/statements/SetterIndexer.cpp"
#include "codegen/statements/CompoundAssignmentStatement.cpp"
#include "codegen/statements/IfStatement.cpp"
#include "codegen/statements/ForStatement.cpp"
#include "codegen/statements/WhileStatement.cpp"
#include "codegen/misc/TypeNode.cpp"
#include "codegen/misc/PrimitiveTypeNode.cpp"
#include "codegen/misc/ArrayTypeNode.cpp"

namespace codegen {
    class CodeGenerator {
    public:
        static std::optional<CodeGenerator> from(ast::Node* node);

        void runCode();

        void shutdown();

    private:
        static llvm::Function* getMainFunction();

        llvm::ExecutionEngine* executionEngine;

        llvm::Function* mainFunction;

        explicit CodeGenerator(llvm::ExecutionEngine* executionEngine, llvm::Function* mainFunction)
            : executionEngine(executionEngine), mainFunction(mainFunction) {}
    };
}
