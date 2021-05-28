#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include "fmt/format.h"

namespace fs = std::filesystem;

struct SystemCallArgs {
    std::string preprocessorPath;
    std::string compilerPath;
    std::string fileNamePath;
};

SystemCallArgs formatSystemCallArgs(char* executablePath, char* fileName) {
    fs::path currentDirectory = fs::canonical(executablePath / fs::path(".."));
    fs::path filePath(fileName);

    fs::path preprocessorPath = fs::canonical(currentDirectory / "./leg_preprocessor");
    fs::path compilerPath = fs::canonical(currentDirectory / "./leg_compiler");

    return SystemCallArgs {
        preprocessorPath,
        compilerPath,
        filePath
    };
}

int main(int argsCount, char* args[]) {
    if (argsCount == 1) {
        std::cerr << "File path is not specified" << std::endl;

        return 1;
    }

    if (!fs::exists(args[1])) {
        std::cerr << "File does not exist" << std::endl;

        return 2;
    }

    auto systemCallArgs = formatSystemCallArgs(args[0], args[1]);

    std::system(
        fmt::format("{} < {} | {}",
            systemCallArgs.preprocessorPath,
            systemCallArgs.fileNamePath,
            systemCallArgs.compilerPath
        ).c_str()
    );

    return 0;
}
