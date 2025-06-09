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

#include "etsNullishTypes.h"

#include "checker/ETSchecker.h"

namespace ark::es2panda::ir {
void ETSUndefinedType::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                         [[maybe_unused]] std::string_view const transformationName)
{
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void ETSUndefinedType::Iterate([[maybe_unused]] const NodeTraverser &cb) const
{
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void ETSUndefinedType::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "ETSUndefinedType"}, {"annotations", AstDumper::Optional(Annotations())}});
}

void ETSUndefinedType::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("undefined");
}

void ETSUndefinedType::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    ES2PANDA_UNREACHABLE();
}

checker::Type *ETSUndefinedType::Check([[maybe_unused]] checker::TSChecker *checker)
{
    ES2PANDA_UNREACHABLE();
}

checker::VerifiedType ETSUndefinedType::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

checker::Type *ETSUndefinedType::GetType([[maybe_unused]] checker::ETSChecker *checker)
{
    SetTsType(checker->GlobalETSUndefinedType());
    return TsType();
}

ETSUndefinedType *ETSUndefinedType::Clone(ArenaAllocator *allocator, AstNode *parent)
{
    auto *const clone = allocator->New<ir::ETSUndefinedType>(allocator);

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

    return clone;
}

void ETSNullType::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                    [[maybe_unused]] std::string_view const transformationName)
{
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void ETSNullType::Iterate([[maybe_unused]] const NodeTraverser &cb) const
{
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void ETSNullType::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "ETSNullType"}, {"annotations", AstDumper::Optional(Annotations())}});
}

void ETSNullType::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("null");
}

void ETSNullType::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    ES2PANDA_UNREACHABLE();
}

checker::Type *ETSNullType::Check([[maybe_unused]] checker::TSChecker *checker)
{
    ES2PANDA_UNREACHABLE();
}

checker::VerifiedType ETSNullType::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

checker::Type *ETSNullType::GetType([[maybe_unused]] checker::ETSChecker *checker)
{
    SetTsType(checker->GlobalETSNullType());
    return TsType();
}

ETSNullType *ETSNullType::Clone(ArenaAllocator *allocator, AstNode *parent)
{
    auto *const clone = allocator->New<ir::ETSNullType>(allocator);
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

    return clone;
}
}  // namespace ark::es2panda::ir
