/*
 * Copyright (c) 2021 - 2023 Huawei Device Co., Ltd.
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

#include "generatorFunctionBuilder.h"

#include "compiler/core/pandagen.h"
#include "compiler/base/catchTable.h"
#include "ir/base/scriptFunction.h"

namespace ark::es2panda::compiler {
void GeneratorFunctionBuilder::Prepare(const ir::ScriptFunction *node) const
{
    VReg callee = FunctionReg(node);

    pg_->CreateGeneratorObj(node, callee);
    pg_->StoreAccumulator(node, funcObj_);
    pg_->SuspendGenerator(node, funcObj_);
    pg_->SetLabel(node, catchTable_->LabelSet().TryBegin());
}

void GeneratorFunctionBuilder::CleanUp(const ir::ScriptFunction *node) const
{
    const auto &labelSet = catchTable_->LabelSet();

    pg_->SetLabel(node, labelSet.TryEnd());
    pg_->SetLabel(node, labelSet.CatchBegin());
    pg_->GeneratorComplete(node, funcObj_);
    pg_->EmitThrow(node);
    pg_->SetLabel(node, labelSet.CatchEnd());
}

void GeneratorFunctionBuilder::DirectReturn(const ir::AstNode *node) const
{
    pg_->GeneratorComplete(node, funcObj_);
    pg_->CreateIterResultObject(node, true);
    pg_->EmitReturn(node);
}

void GeneratorFunctionBuilder::ImplicitReturn(const ir::AstNode *node) const
{
    pg_->LoadConst(node, Constant::JS_UNDEFINED);
    DirectReturn(node);
}

void GeneratorFunctionBuilder::Yield(const ir::AstNode *node)
{
    RegScope rs(pg_);
    VReg completionType = pg_->AllocReg();
    VReg completionValue = pg_->AllocReg();

    pg_->CreateIterResultObject(node, false);
    pg_->GeneratorYield(node, funcObj_);
    SuspendResumeExecution(node, completionType, completionValue);

    HandleCompletion(node, completionType, completionValue);
}
}  // namespace ark::es2panda::compiler
