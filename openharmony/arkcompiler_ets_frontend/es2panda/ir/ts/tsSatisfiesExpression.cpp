/**
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "tsSatisfiesExpression.h"

#include <typescript/checker.h>


namespace panda::es2panda::ir {

void TSSatisfiesExpression::Iterate(const NodeTraverser &cb) const
{
    cb(expression_);
    cb(typeAnnotation_);
}

void TSSatisfiesExpression::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSSatisfiesExpression"}, {"expression", expression_}, {"typeAnnotation", typeAnnotation_}});
}

void TSSatisfiesExpression::Compile(compiler::PandaGen *pg) const
{
    expression_->Compile(pg);
}

checker::Type *TSSatisfiesExpression::Check(checker::Checker *checker) const
{
    return nullptr;
}

void TSSatisfiesExpression::UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder)
{
    expression_ = std::get<ir::AstNode *>(cb(expression_))->AsExpression();
    typeAnnotation_ = std::get<ir::AstNode *>(cb(typeAnnotation_))->AsExpression();
}

}  // namespace panda::es2panda::ir
