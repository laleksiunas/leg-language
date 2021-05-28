#pragma once

#include <map>
#include <string>
#include "fmt/format.h"
#include "codegen/types/abstracts/Type.h"
#include "codegen/keywords.h"

namespace codegen {
    class TypesContainer {
    public:
        ~TypesContainer() {
            for (auto& [_, value] : typeByName) {
                delete value;
            }
        }

        std::string getName(llvm::Type* type) {
            return type->isVoidTy() ? keywords::Void : getByType(type)->getName();
        }

        codegen::Type* getByName(const std::string& name) {
            return typeByName[name];
        }

        codegen::Type* getByType(llvm::Type* type) {
            return typeByLlvmType[type];
        }

        codegen::Type* add(codegen::Type* type) {
            if (has(type->getName())) {
                throw std::invalid_argument(fmt::format("Type {} already exists", type->getName()));
            }

            typeByName[type->getName()] = type;
            typeByLlvmType[type->getType()] = type;

            return type;
        }

        bool has(const std::string& name) {
            return typeByName.find(name) != typeByName.end();
        }

        bool has(llvm::Type* type) {
            return typeByLlvmType.find(type) != typeByLlvmType.end();
        }

    private:
        std::map<std::string, codegen::Type*> typeByName;
        std::map<llvm::Type*, codegen::Type*> typeByLlvmType;
    };
}
