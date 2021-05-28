#include <iostream>
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ManagedStatic.h"
#include "generated/tokens.cpp"
#include "generated/parser.cpp"
#include "codegen/CodeGenerator.cpp"
#include "library/library.cpp"

void handleError(std::string& message, int lineNumber) {
    message[0] = toupper(message[0]);

    std::cout << "Syntax error (line " << lineNumber << "): " << message << std::endl << std::endl;
}

int main() {
    yyparse();

    if (hasSyntaxError) {
        delete rootBlock;

        return 1;
    }

    LLVMLinkInMCJIT();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    auto generator = codegen::CodeGenerator::from(rootBlock);

    delete rootBlock;

    if (generator.has_value()) {
        generator.value().runCode();
        generator.value().shutdown();
    }

    llvm::llvm_shutdown();

    return 0;
}
