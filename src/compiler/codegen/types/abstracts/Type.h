#pragma once

#include <string>
#include <utility>
#include <map>
#include <optional>
#include <functional>
#include "fmt/format.h"
#include "llvm/IR/Type.h"
#include "Indexer.h"

namespace codegen {
    typedef std::function<llvm::Value*(llvm::BasicBlock*)> DefaultValueCreator;
    typedef std::function<llvm::Value*(llvm::BasicBlock*, llvm::Value*, llvm::Value*)> Operator;
    typedef std::function<llvm::Value*(llvm::BasicBlock*, llvm::Value*, llvm::Value*)> GetterIndexer;
    typedef std::function<llvm::Value*(llvm::BasicBlock*, llvm::Value*, llvm::Value*, llvm::Value*)> SetterIndexer;
    typedef std::function<llvm::Value*(llvm::BasicBlock*, llvm::Value*)> GetterProperty;

    class Type {
    public:
        explicit Type(std::string name, llvm::Type* type) : name(std::move(name)), type(type) {}

        std::string getName() {
            return name;
        }

        llvm::Type* getType() {
            return type;
        }

        std::optional<DefaultValueCreator> getDefaultValueCreator() {
            return defaultValueCreator;
        }

        void setDefaultValueCreator(DefaultValueCreator creator) {
            defaultValueCreator = std::move(creator);
        }

        bool hasOperator(const std::string& operatorName, llvm::Type* rightType) {
            if (!operators.contains(operatorName)) {
                return false;
            }

            auto operatorHandlers = operators[operatorName];

            if (!operatorHandlers.contains(rightType)) {
                return false;
            }

            return true;
        }

        std::optional<Operator> getOperator(const std::string& operatorName, llvm::Type* rightType) {
            if (hasOperator(operatorName, rightType)) {
                return operators[operatorName][rightType];
            }

            return std::nullopt;
        }

        void addOperator(const std::string& operatorName, llvm::Type* rightType, Operator operatorHandler) {
            if (hasOperator(operatorName, rightType)) {
                throw std::invalid_argument(fmt::format("Operator {} already exists", operatorName));
            }

            if (!operators.contains(operatorName)) {
                operators[operatorName] = std::map<llvm::Type*, Operator>();
            }

            operators[operatorName][rightType] = std::move(operatorHandler);
        }

        bool hasGetterIndexer(llvm::Type* indexerType) {
            return hasIndexer(getterIndexers, indexerType);
        }

        void addGetterIndexer(llvm::Type* indexerType, Indexer<GetterIndexer> indexer) {
            return addIndexer(getterIndexers, indexerType, std::move(indexer));
        }

        std::optional<Indexer<GetterIndexer>> getGetterIndexer(llvm::Type* indexerType) {
            return getIndexer(getterIndexers, indexerType);
        }

        bool hasSetterIndexer(llvm::Type* indexerType) {
            return hasIndexer(setterIndexers, indexerType);
        }

        void addSetterIndexer(llvm::Type* indexerType, Indexer<SetterIndexer> indexer) {
            return addIndexer(setterIndexers, indexerType, std::move(indexer));
        }

        std::optional<Indexer<SetterIndexer>> getSetterIndexer(llvm::Type* indexerType) {
            return getIndexer(setterIndexers, indexerType);
        }

        bool hasGetterProperty(const std::string& propertyName) {
            return hasProperty(getterProperties, propertyName);
        }

        void addGetterProperty(const std::string& propertyName, GetterProperty property) {
            return addProperty(getterProperties, propertyName, std::move(property));
        }

        std::optional<GetterProperty> getGetterProperty(const std::string& propertyName) {
            return getProperty(getterProperties, propertyName);
        }

    private:
        std::string name;
        llvm::Type* type;
        std::optional<DefaultValueCreator> defaultValueCreator;
        std::map<std::string, std::map<llvm::Type*, Operator>> operators;
        std::map<llvm::Type*, Indexer<GetterIndexer>> getterIndexers;
        std::map<llvm::Type*, Indexer<SetterIndexer>> setterIndexers;
        std::map<std::string, GetterProperty> getterProperties;

        template<typename TIndexer>
        bool hasIndexer(std::map<llvm::Type*, TIndexer>& indexers, llvm::Type* indexerType) {
            return indexers.contains(indexerType);
        }

        template<typename TIndexer>
        void addIndexer(std::map<llvm::Type*, TIndexer>& indexers, llvm::Type* indexerType, TIndexer handler) {
            if (hasIndexer(indexers, indexerType)) {
                throw std::invalid_argument("Indexer already exists");
            }

            indexers[indexerType] = std::move(handler);
        }

        template<typename TIndexer>
        std::optional<TIndexer> getIndexer(std::map<llvm::Type*, TIndexer>& indexers, llvm::Type* indexerType) {
            if (hasIndexer(indexers, indexerType)) {
                return std::optional(indexers[indexerType]);
            }

            return std::nullopt;
        }

        template<typename TProperty>
        bool hasProperty(std::map<std::string, TProperty>& properties, const std::string& propertyName) {
            return properties.contains(propertyName);
        }

        template<typename TProperty>
        void addProperty(
            std::map<std::string, TProperty>& properties,
            const std::string& propertyName,
            TProperty handler
        ) {
            if (hasProperty(properties, propertyName)) {
                throw std::invalid_argument(fmt::format("Property '{}' already exists", propertyName));
            }

            properties[propertyName] = std::move(handler);
        }

        template<typename TProperty>
        std::optional<TProperty> getProperty(
            std::map<std::string, TProperty>& properties,
            const std::string& propertyName
        ) {
            if (hasProperty(properties, propertyName)) {
                return std::optional(properties[propertyName]);
            }

            return std::nullopt;
        }
    };
}
