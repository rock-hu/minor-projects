/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ETSchecker.h"

namespace ark::es2panda::checker {

CheckerContext::CheckerContext(Checker *checker, CheckerStatus newStatus, ETSObjectType const *containingClass,
                               Signature *containingSignature)
    : parent_(checker),
      status_(newStatus),
      capturedVars_(parent_->Allocator()->Adapter()),
      smartCasts_(parent_->Allocator()->Adapter()),
      containingClass_(containingClass),
      containingSignature_(containingSignature),
      testSmartCasts_(parent_->Allocator()->Adapter()),
      breakSmartCasts_(parent_->Allocator()->Adapter())
{
}

void CheckerContext::SetSmartCast(varbinder::Variable const *const variable, checker::Type *const smartType) noexcept
{
    // Just block captured and modified variables here instead of finding all their usage occurrences.
    if (!variable->HasFlag(varbinder::VariableFlags::CAPTURED_MODIFIED)) {
        smartCasts_.insert_or_assign(variable, smartType);
    }
}

SmartCastTypes CheckerContext::CloneTestSmartCasts(bool const clearData) noexcept
{
    if (testSmartCasts_.empty()) {
        return std::nullopt;
    }

    SmartCastTestArray smartCasts {};
    smartCasts.reserve(testSmartCasts_.size());

    for (auto [variable, types] : testSmartCasts_) {
        if (types.first != nullptr || types.second != nullptr) {
            smartCasts.emplace_back(variable, types.first, types.second);
        }
    }

    if (clearData) {
        ClearTestSmartCasts();
    }

    return std::make_optional(smartCasts);
}

SmartCastArray CheckerContext::CloneSmartCasts(bool const clearData) noexcept
{
    SmartCastArray smartCasts {};

    if (!smartCasts_.empty()) {
        smartCasts.reserve(smartCasts_.size());

        for (auto const [variable, type] : smartCasts_) {
            smartCasts.emplace_back(variable, type);
        }
    }

    if (clearData) {
        ClearSmartCasts();
    }

    return smartCasts;
}

void CheckerContext::RestoreSmartCasts(SmartCastArray const &otherSmartCasts)
{
    smartCasts_.clear();
    if (!otherSmartCasts.empty()) {
        for (auto [variable, type] : otherSmartCasts) {
            smartCasts_.emplace(variable, type);
        }
    }
}

void CheckerContext::RemoveSmartCasts(SmartCastArray const &otherSmartCasts) noexcept
{
    if (!smartCasts_.empty()) {
        auto it = smartCasts_.begin();
        while (it != smartCasts_.end()) {
            if (std::find_if(otherSmartCasts.begin(), otherSmartCasts.end(), [&it](auto const &item) -> bool {
                    return item.first == it->first;
                }) == otherSmartCasts.end()) {
                it = smartCasts_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

//  Auxiliary private method returning combined type (if types differ) or 'nullptr' if types are identical
//  and no smart cast change is required.
checker::Type *CheckerContext::CombineTypes(checker::Type *const typeOne, checker::Type *const typeTwo) const noexcept
{
    ASSERT(typeOne != nullptr && typeTwo != nullptr);
    auto *const checker = parent_->AsETSChecker();

    if (checker->Relation()->IsIdenticalTo(typeOne, typeTwo)) {
        return nullptr;
    }

    return checker->CreateETSUnionType({typeOne, typeTwo});
}

void CheckerContext::CombineSmartCasts(SmartCastArray const &otherSmartCasts)
{
    auto *const checker = parent_->AsETSChecker();

    for (auto [variable, type] : otherSmartCasts) {
        auto const it = smartCasts_.find(variable);
        if (it == smartCasts_.end()) {
            continue;
        }
        // Smart cast presents in both sets
        if (auto *const smartType = CombineTypes(it->second, type); smartType != nullptr) {
            // Remove it or set to new combined value
            if (checker->Relation()->IsIdenticalTo(it->first->TsType(), smartType)) {
                smartCasts_.erase(it);
            } else {
                it->second = smartType;
            }
        }
    }

    // Remove smart casts that don't present in the other set.
    RemoveSmartCasts(otherSmartCasts);
}

// Second return value shows if the 'IN_LOOP' flag should be cleared on exit from the loop (case of nested loops).
std::pair<SmartCastArray, bool> CheckerContext::EnterLoop(ir::LoopStatement const &loop) noexcept
{
    bool const clearFlag = !IsInLoop();
    if (clearFlag) {
        status_ |= CheckerStatus::IN_LOOP;
    }

    auto smartCasts = CloneSmartCasts();

    SmartVariables changedVariables {};
    loop.Iterate([this, &changedVariables](ir::AstNode *childNode) { CheckAssignments(childNode, changedVariables); });

    if (!changedVariables.empty()) {
        for (auto const *variable : changedVariables) {
            smartCasts_.erase(variable);
        }
    }

    return {std::move(smartCasts), clearFlag};
}

void CheckerContext::ExitLoop(SmartCastArray &prevSmartCasts, bool const clearFlag,
                              ir::LoopStatement *loopStatement) noexcept
{
    if (clearFlag) {
        status_ &= ~CheckerStatus::IN_LOOP;
    }

    if (!breakSmartCasts_.empty()) {
        auto it = breakSmartCasts_.begin();

        while (it != breakSmartCasts_.end()) {
            if (it->first != loopStatement) {
                ++it;
            } else {
                CombineSmartCasts(it->second);
                it = breakSmartCasts_.erase(it);
            }
        }
    }

    //  Now we don't process smart casts inside the loops correctly, thus just combine them on exit from the loop.
    CombineSmartCasts(prevSmartCasts);
}

void CheckerContext::CheckAssignments(ir::AstNode const *node, SmartVariables &changedVariables) noexcept
{
    if (node == nullptr) {  //  Just in case!
        return;
    }

    if (!node->IsAssignmentExpression()) {
        node->Iterate(
            [this, &changedVariables](ir::AstNode *childNode) { CheckAssignments(childNode, changedVariables); });
        return;
    }

    auto const *assignment = node->AsAssignmentExpression();
    if (assignment->Left()->IsIdentifier()) {
        auto const *const ident = assignment->Left()->AsIdentifier();

        auto const *variable = ident->Variable();
        if (variable == nullptr) {
            //  NOTE: we're interesting in the local variables ONLY!
            variable = parent_->AsETSChecker()->FindVariableInFunctionScope(
                ident->Name(), varbinder::ResolveBindingOptions::ALL_NON_TYPE);
        }

        if (variable != nullptr) {
            changedVariables.insert(variable);
        }
    }

    assignment->Right()->Iterate(
        [this, &changedVariables](ir::AstNode *childNode) { CheckAssignments(childNode, changedVariables); });
}

SmartCastArray CheckerContext::CheckTryBlock(ir::BlockStatement const &tryBlock) noexcept
{
    SmartVariables changedVariables {};
    tryBlock.Iterate(
        [this, &changedVariables](ir::AstNode *childNode) { CheckAssignments(childNode, changedVariables); });

    SmartCastArray smartCasts {};
    if (!smartCasts_.empty()) {
        smartCasts.reserve(smartCasts_.size());

        for (auto const [variable, type] : smartCasts_) {
            if (changedVariables.find(variable) == changedVariables.end()) {
                smartCasts.emplace_back(variable, type);
            }
        }
    }

    return smartCasts;
}

//  Check that the expression is a part of logical OR/AND or unary negation operators chain
//  (other cases are not interested)
bool CheckerContext::IsInValidChain(ir::AstNode const *parent) noexcept
{
    while (parent != nullptr && !parent->IsIfStatement() && !parent->IsConditionalExpression()) {
        if (parent->IsBinaryExpression()) {
            auto const operation = parent->AsBinaryExpression()->OperatorType();
            if (operation != lexer::TokenType::PUNCTUATOR_LOGICAL_OR &&
                operation != lexer::TokenType::PUNCTUATOR_LOGICAL_AND) {
                return false;
            }
        } else if (parent->IsUnaryExpression()) {
            if (parent->AsUnaryExpression()->OperatorType() != lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK) {
                return false;
            }
        } else {
            return false;
        }
        parent = parent->Parent();
    }
    return parent != nullptr;
}

void CheckerContext::CheckIdentifierSmartCastCondition(ir::Identifier const *const identifier) noexcept
{
    if (!IsInTestExpression()) {
        return;
    }

    auto const *const variable = identifier->Variable();
    ASSERT(variable != nullptr);

    //  Smart cast for extended conditional check can be applied only to the variables of reference types.
    if (auto const *const variableType = variable->TsType(); !variableType->IsETSReferenceType()) {
        return;
    }

    if (!IsInValidChain(identifier->Parent())) {
        return;
    }

    ASSERT(testCondition_.variable == nullptr);
    if (identifier->TsType()->PossiblyETSNullish()) {
        testCondition_ = {variable, parent_->AsETSChecker()->GlobalETSNullType(), true, false};
    }
}

void CheckerContext::CheckUnarySmartCastCondition(ir::UnaryExpression const *const unaryExpression) noexcept
{
    if (!IsInTestExpression() || unaryExpression->OperatorType() != lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK) {
        return;
    }

    auto const *const argument = unaryExpression->Argument();
    if (argument == nullptr || (!argument->IsIdentifier() && !argument->IsBinaryExpression())) {
        return;
    }

    if (!IsInValidChain(unaryExpression->Parent())) {
        return;
    }

    if (testCondition_.variable != nullptr) {
        testCondition_.negate = !testCondition_.negate;
    }
}

void CheckerContext::CheckBinarySmartCastCondition(ir::BinaryExpression *const binaryExpression) noexcept
{
    if (!IsInTestExpression() || !IsInValidChain(binaryExpression->Parent())) {
        return;
    }

    if (auto const operatorType = binaryExpression->OperatorType(); operatorType == lexer::TokenType::KEYW_INSTANCEOF) {
        ASSERT(testCondition_.variable == nullptr);
        if (binaryExpression->Left()->IsIdentifier()) {
            testCondition_ = {binaryExpression->Left()->AsIdentifier()->Variable(),
                              binaryExpression->Right()->TsType()};
        }
    } else if (operatorType == lexer::TokenType::PUNCTUATOR_STRICT_EQUAL ||
               operatorType == lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL ||
               operatorType == lexer::TokenType::PUNCTUATOR_EQUAL ||
               operatorType == lexer::TokenType::PUNCTUATOR_NOT_EQUAL) {
        ASSERT(testCondition_.variable == nullptr);
        CheckSmartCastEqualityCondition(binaryExpression);
    }
}

//  Extracted just to avoid large length and depth of method 'CheckBinarySmartCastCondition()'.
void CheckerContext::CheckSmartCastEqualityCondition(ir::BinaryExpression *const binaryExpression) noexcept
{
    varbinder::Variable const *variable = nullptr;
    checker::Type *testedType = nullptr;
    auto const operatorType = binaryExpression->OperatorType();

    bool strict = operatorType == lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL ||
                  operatorType == lexer::TokenType::PUNCTUATOR_STRICT_EQUAL;

    // extracted just to avoid extra nested level
    auto const getTestedType = [&variable, &testedType, &strict](ir::Identifier const *const identifier,
                                                                 ir::Expression *const expression) -> void {
        ASSERT(identifier != nullptr && expression != nullptr);
        variable = identifier->Variable();
        if (expression->IsLiteral()) {
            testedType = expression->TsType();
            if (!expression->IsNullLiteral() && !expression->IsUndefinedLiteral()) {
                strict = false;
            }
        }
    };

    if (binaryExpression->Left()->IsIdentifier()) {
        getTestedType(binaryExpression->Left()->AsIdentifier(), binaryExpression->Right());
    }

    if (testedType == nullptr && binaryExpression->Right()->IsIdentifier()) {
        getTestedType(binaryExpression->Right()->AsIdentifier(), binaryExpression->Left());
    }

    if (testedType != nullptr) {
        bool const negate = operatorType == lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL ||
                            operatorType == lexer::TokenType::PUNCTUATOR_NOT_EQUAL;

        if (testedType->DefinitelyETSNullish()) {
            testCondition_ = {variable, testedType, negate, strict};
        }
    }
}

void CheckerContext::ClearTestSmartCasts() noexcept
{
    testCondition_ = {};
    testSmartCasts_.clear();
    operatorType_ = lexer::TokenType::EOS;
}

checker::Type *CheckerContext::GetSmartCast(varbinder::Variable const *const variable) const noexcept
{
    if (IsInTestExpression()) {
        if (operatorType_ == lexer::TokenType::PUNCTUATOR_LOGICAL_AND) {
            if (auto const it = testSmartCasts_.find(variable);
                it != testSmartCasts_.end() && it->second.first != nullptr) {
                return it->second.first;
            }
        } else if (operatorType_ == lexer::TokenType::PUNCTUATOR_LOGICAL_OR) {
            if (auto const it = testSmartCasts_.find(variable);
                it != testSmartCasts_.end() && it->second.second != nullptr) {
                return it->second.second;
            }
        }
    }

    auto const it = smartCasts_.find(variable);
    return it == smartCasts_.end() ? nullptr : it->second;
}

void CheckerContext::OnBreakStatement(ir::BreakStatement const *breakStatement)
{
    ir::Statement const *targetStatement = breakStatement->Target()->AsStatement();
    ASSERT(targetStatement != nullptr);
    if (targetStatement->IsLabelledStatement()) {
        targetStatement = targetStatement->AsLabelledStatement()->Body();
    }
    ASSERT(targetStatement != nullptr);

    auto const inInnerScope = [targetStatement](varbinder::Scope const *scope, ir::AstNode const *parent) -> bool {
        do {
            parent = parent->Parent();
            if (parent->IsScopeBearer() && parent->Scope() == scope) {
                return true;
            }
        } while (parent != targetStatement);
        return false;
    };

    status_ |= CheckerStatus::MEET_BREAK;

    if (smartCasts_.empty()) {
        return;
    }

    SmartCastArray smartCasts {};
    smartCasts.reserve(smartCasts_.size());

    for (auto const [variable, type] : smartCasts_) {
        if (!inInnerScope(variable->AsLocalVariable()->GetScope(), breakStatement)) {
            smartCasts.emplace_back(variable, type);
        }
    }

    if (!smartCasts.empty()) {
        AddBreakSmartCasts(targetStatement, std::move(smartCasts));
    }

    ClearSmartCasts();
}

void CheckerContext::AddBreakSmartCasts(ir::Statement const *targetStatement, SmartCastArray &&smartCasts)
{
    breakSmartCasts_.emplace(targetStatement, std::move(smartCasts));
}

void CheckerContext::CombineBreakSmartCasts(ir::Statement const *targetStatement)
{
    ASSERT(smartCasts_.empty());

    if (!breakSmartCasts_.empty()) {
        bool firstCase = true;
        auto it = breakSmartCasts_.begin();

        while (it != breakSmartCasts_.end()) {
            if (it->first != targetStatement) {
                ++it;
                continue;
            }

            if (firstCase) {
                firstCase = false;
                RestoreSmartCasts(it->second);
            } else {
                CombineSmartCasts(it->second);
            }

            it = breakSmartCasts_.erase(it);
        }
    }
}
}  // namespace ark::es2panda::checker
