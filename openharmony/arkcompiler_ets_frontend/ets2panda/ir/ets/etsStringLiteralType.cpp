/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "etsStringLiteralType.h"

#include "checker/ETSchecker.h"

namespace ark::es2panda::ir {
void ETSStringLiteralType::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                             [[maybe_unused]] std::string_view const transformationName)
{
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void ETSStringLiteralType::Iterate([[maybe_unused]] const NodeTraverser &cb) const
{
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void ETSStringLiteralType::Dump(ir::AstDumper *dumper) const
{
    dumper->Add(
        {{"type", "ETSStringLiteralType"}, {"value", value_}, {"annotations", AstDumper::Optional(Annotations())}});
}

void ETSStringLiteralType::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("\"" + value_.Mutf8() + "\"");
}

void ETSStringLiteralType::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    ES2PANDA_UNREACHABLE();
}

checker::Type *ETSStringLiteralType::Check([[maybe_unused]] checker::TSChecker *checker)
{
    ES2PANDA_UNREACHABLE();
}

checker::VerifiedType ETSStringLiteralType::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

checker::Type *ETSStringLiteralType::GetType([[maybe_unused]] checker::ETSChecker *checker)
{
    SetTsType(checker->CreateETSStringLiteralType(value_));
    return TsType();
}

ETSStringLiteralType *ETSStringLiteralType::Clone(ArenaAllocator *allocator, AstNode *parent)
{
    auto *const clone = allocator->New<ir::ETSStringLiteralType>(value_, allocator);

    if (parent != nullptr) {
        clone->SetParent(parent);
    }

    if (!Annotations().empty()) {
        ArenaVector<AnnotationUsage *> annotationUsages {allocator->Adapter()};
        for (auto *annotationUsage : Annotations()) {
            annotationUsages.push_back(annotationUsage->Clone(allocator, clone)->AsAnnotationUsage());
        }
        clone->SetAnnotations(std::move(annotationUsages));
    }

    clone->SetRange(Range());
    return clone;
}
}  // namespace ark::es2panda::ir
