#include "llvm/IR/Value.h"
#include "fmt/format.h"
#include "ast/statements/ForNode.h"
#include "ast/statements/DeclarationNode.h"
#include "codegen/context/BlockContext.h"
#include "shared/Range.h"

namespace codegen::for_loop {
    auto IncrementOperatorName = operators::Plus;
    auto DecrementOperatorName = operators::Minus;
    auto InRangeLowerOperatorName = operators::GreaterEqual;
    auto InRangeUpperOperatorName = operators::LessThanEqual;

    struct ForLoopBlocks {
        llvm::BasicBlock* body;
        llvm::BasicBlock* bodyEnd;
        llvm::BasicBlock* increment;
        llvm::BasicBlock* decrement;
        llvm::BasicBlock* condition;
        llvm::BasicBlock* end;
    };

    struct RangeOperators {
        codegen::Operator increment;
        codegen::Operator decrement;
        codegen::Operator increasingInRange;
        codegen::Operator decreasingInRange;
    };

    ForLoopBlocks createForLoopBlocks(llvm::Function* wrapperFunction) {
        return ForLoopBlocks {
            llvm::BasicBlock::Create(codegen::Context(), "loop", wrapperFunction),
            nullptr,
            llvm::BasicBlock::Create(codegen::Context(), "loop_increment", wrapperFunction),
            llvm::BasicBlock::Create(codegen::Context(), "loop_decrement", wrapperFunction),
            llvm::BasicBlock::Create(codegen::Context(), "loop_condition", wrapperFunction),
            llvm::BasicBlock::Create(codegen::Context(), "loop_end", wrapperFunction)
        };
    }

    RangeOperators getRangeOperators(shared::Range& range) {
        auto rangeType = codegen::Types().getByType(range.type);

        return RangeOperators {
            rangeType->getOperator(IncrementOperatorName, range.step->getType()).value(),
            rangeType->getOperator(DecrementOperatorName, range.step->getType()).value(),
            rangeType->getOperator(InRangeUpperOperatorName, range.type).value(),
            rangeType->getOperator(InRangeLowerOperatorName, range.type).value()
        };
    }

    llvm::Value* updateRangeValue(
        llvm::BasicBlock* rangeUpdateBlock,
        llvm::BasicBlock* nextBlock,
        llvm::Value* loopVariable,
        codegen::Operator& updateOperator,
        shared::Range& range
    ) {
        codegen::Builder().SetInsertPoint(rangeUpdateBlock);

        auto* currentLoopValue = new llvm::LoadInst(
            range.type,
            loopVariable,
            "",
            rangeUpdateBlock
        );
        auto* loopValueAfterIncrement = updateOperator(rangeUpdateBlock, currentLoopValue, range.step);

        new llvm::StoreInst(loopValueAfterIncrement, loopVariable, rangeUpdateBlock);

        codegen::Builder().CreateBr(nextBlock);

        return loopValueAfterIncrement;
    }

    void updateLoopVariable(
        ForLoopBlocks& blocks,
        shared::Range& range,
        RangeOperators& rangeOperators,
        llvm::Value* loopVariable
    ) {
        codegen::Builder().SetInsertPoint(blocks.bodyEnd);
        codegen::Builder().CreateCondBr(
            rangeOperators.increasingInRange(blocks.bodyEnd, range.begin, range.end),
            blocks.increment,
            blocks.decrement
        );

        updateRangeValue(blocks.increment, blocks.condition, loopVariable, rangeOperators.increment, range);
        updateRangeValue(blocks.decrement, blocks.condition, loopVariable, rangeOperators.decrement, range);
    }

    void checkLoopTerminationCondition(
        ForLoopBlocks& blocks,
        shared::Range& range,
        RangeOperators& rangeOperators,
        llvm::Value* loopVariable
    ) {
        codegen::Builder().SetInsertPoint(blocks.condition);

        auto* currentLoopValue = new llvm::LoadInst(
            range.type,
            loopVariable,
            "",
            blocks.condition
        );

        auto* loopCondition = codegen::Builder().CreateOr(
            codegen::Builder().CreateAnd(
                rangeOperators.increasingInRange(blocks.condition, range.begin, range.end),
                rangeOperators.increasingInRange(blocks.condition, currentLoopValue, range.end)
            ),
            codegen::Builder().CreateAnd(
                rangeOperators.decreasingInRange(blocks.condition, range.begin, range.end),
                rangeOperators.decreasingInRange(blocks.condition, currentLoopValue, range.end)
            )
        );

        codegen::Builder().CreateCondBr(loopCondition, blocks.body, blocks.end);
    }
}

void ast::ForNode::validateForVoidRangeType(llvm::Type* rangeType) {
    if (rangeType->isVoidTy()) {
        codegen::Errors().report(
            fmt::format("Unexpected '{}' type in a range construct", codegen::keywords::Void),
            lineNumber
        );
    }
}

void ast::ForNode::validateRangeStepOperator(
    codegen::Type* rangeType,
    codegen::Type* stepType,
    const std::string& operatorName
) {
    if (!rangeType->hasOperator(operatorName, stepType->getType())) {
        codegen::Errors().report(
            fmt::format(
                "'{0}' cannot be used as a range step for the range of '{1}'. '{1}' is missing an operator '{1} {2} {0}'",
                stepType->getName(),
                rangeType->getName(),
                operatorName
            ),
            lineNumber
        );
    }
}

void ast::ForNode::validateInRangeOperators(codegen::Type* rangeType, const std::string& operatorName) {
    if (!rangeType->hasOperator(operatorName, rangeType->getType())) {
        codegen::Errors().report(
            fmt::format(
                "'{0}' cannot be used as a range type because it is missing an operator '{0} {1} {0}'",
                rangeType->getName(),
                operatorName
            ),
            lineNumber
        );
    }
}

llvm::Value* ast::ForNode::getDefaultStepValue(llvm::Type* rangeType) {
    if (rangeType == codegen::createIntType()) {
        return llvm::ConstantInt::get(codegen::createIntType(), 1, true);
    }

    if (rangeType == codegen::createFloatType()) {
        return llvm::ConstantFP::get(codegen::createFloatType(), 1);
    }

    return nullptr;
}

llvm::Value* ast::ForNode::resolveRangeStepValue(codegen::Type* rangeType, codegen::BlockContext& blockContext) {
    if (step == nullptr) {
        auto* defaultValue = getDefaultStepValue(rangeType->getType());

        if (defaultValue == nullptr) {
            codegen::Errors().report(
                fmt::format("Cannot resolve implicit default range step value for type '{}'", rangeType->getName()),
                lineNumber
            );
        }

        return defaultValue;
    }

    auto* stepValue = step->codegen(blockContext);
    auto* stepType = codegen::Types().getByType(stepValue->getType());

    validateForVoidRangeType(stepValue->getType());
    validateRangeStepOperator(rangeType, stepType, codegen::for_loop::IncrementOperatorName);
    validateRangeStepOperator(rangeType, stepType, codegen::for_loop::DecrementOperatorName);

    return stepValue;
}

shared::Range ast::ForNode::resolveRange(codegen::BlockContext& blockContext) {
    auto* beginValue = rangeBegin->codegen(blockContext);
    auto* endValue = rangeEnd->codegen(blockContext);

    validateForVoidRangeType(beginValue->getType());
    validateForVoidRangeType(endValue->getType());

    auto beginValueTypeName = codegen::Types().getName(beginValue->getType());
    auto endValueTypeName = codegen::Types().getName(endValue->getType());
    auto* rangeType = codegen::Types().getByType(beginValue->getType());

    if (beginValue->getType() != endValue->getType()) {
        codegen::Errors().report(
            fmt::format(
                "Expected range boundaries to be of the same type, got '{}' and '{}'",
                beginValueTypeName,
                endValueTypeName
            ),
            lineNumber
        );
    }

    validateInRangeOperators(rangeType, codegen::for_loop::InRangeUpperOperatorName);
    validateInRangeOperators(rangeType, codegen::for_loop::InRangeLowerOperatorName);

    return shared::Range {
        beginValue,
        endValue,
        resolveRangeStepValue(rangeType, blockContext),
        beginValue->getType()
    };
}

llvm::Value* ast::ForNode::declareLoopVariable(codegen::BlockContext& blockContext, llvm::Value* initialValue) const {
    auto variable = codegen::Variable {
        initialValue,
        true,
        false
    };

    return codegen::declareValue(blockContext, variableName->value, variable);
}

llvm::BasicBlock* ast::ForNode::createLoopBody(
    codegen::BlockContext& blockContext,
    llvm::BasicBlock* loopEntryBlock
) const {
    codegen::Builder().SetInsertPoint(loopEntryBlock);
    codegen::Builder().CreateBr(blockContext.getBlock());

    codegen::Builder().SetInsertPoint(blockContext.getBlock());

    block->codegen(blockContext);

    return blockContext.getBlock();
}

llvm::Value* ast::ForNode::codegen(codegen::BlockContext& blockContext) {
    DeclarationNode::validateIdentifier(variableName->value, blockContext, lineNumber);

    auto blocks = codegen::for_loop::createForLoopBlocks(blockContext.getCurrentFunction());

    auto range = resolveRange(blockContext);
    auto rangeOperators = codegen::for_loop::getRangeOperators(range);

    auto* loopVariable = declareLoopVariable(blockContext, range.begin);
    auto loopBlockContext = blockContext.createNestedBlock(blocks.body);

    blocks.bodyEnd = createLoopBody(loopBlockContext, blockContext.getBlock());

    if (!loopBlockContext.hasReturnValue()) {
        codegen::for_loop::updateLoopVariable(
            blocks,
            range,
            rangeOperators,
            loopVariable
        );

        codegen::for_loop::checkLoopTerminationCondition(blocks, range, rangeOperators, loopVariable);
    }

    codegen::Builder().SetInsertPoint(blocks.end);
    blockContext.setBlock(blocks.end);
    blockContext.removeVariable(variableName->value);

    return nullptr;
}
