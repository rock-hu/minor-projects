/**
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

#ifndef ES2PANDA_IR_TS_TYPE_ALIAS_DECLARATION_H
#define ES2PANDA_IR_TS_TYPE_ALIAS_DECLARATION_H

#include "ir/statement.h"
#include "ir/typed.h"
#include "ir/jsDocAllowed.h"
#include "ir/statements/annotationUsage.h"

namespace ark::es2panda::varbinder {
class Variable;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::ir {
class Identifier;
class TSTypeParameterDeclaration;

class TSTypeAliasDeclaration : public JsDocAllowed<AnnotatedStatement> {
public:
    explicit TSTypeAliasDeclaration(ArenaAllocator *allocator, Identifier *id, TSTypeParameterDeclaration *typeParams,
                                    TypeNode *typeAnnotation)
        : JsDocAllowed<AnnotatedStatement>(AstNodeType::TS_TYPE_ALIAS_DECLARATION, typeAnnotation, allocator),
          decorators_(allocator->Adapter()),
          annotations_(allocator->Adapter()),
          id_(id),
          typeParams_(typeParams),
          typeParamTypes_(allocator->Adapter())
    {
    }

    explicit TSTypeAliasDeclaration(ArenaAllocator *allocator, Identifier *id)
        : JsDocAllowed<AnnotatedStatement>(allocator, AstNodeType::TS_TYPE_ALIAS_DECLARATION),
          decorators_(allocator->Adapter()),
          annotations_(allocator->Adapter()),
          id_(id),
          typeParams_(nullptr),
          typeParamTypes_(allocator->Adapter())
    {
    }

    Identifier *Id()
    {
        return id_;
    }

    const Identifier *Id() const
    {
        return id_;
    }

    TSTypeParameterDeclaration *TypeParams() const
    {
        return typeParams_;
    }

    const ArenaVector<Decorator *> &Decorators() const
    {
        return decorators_;
    }

    const ArenaVector<Decorator *> *DecoratorsPtr() const override
    {
        return &Decorators();
    }

    void SetTypeParameters(ir::TSTypeParameterDeclaration *typeParams)
    {
        typeParams_ = typeParams;
    }

    void AddDecorators([[maybe_unused]] ArenaVector<ir::Decorator *> &&decorators) override
    {
        decorators_ = std::move(decorators);
    }

    bool CanHaveDecorator([[maybe_unused]] bool inTs) const override
    {
        return !inTs;
    }

    void SetTypeParameterTypes(ArenaVector<checker::Type *> &&typeParamTypes)
    {
        typeParamTypes_ = std::move(typeParamTypes);
    }

    ArenaVector<checker::Type *> const &TypeParameterTypes() const
    {
        return typeParamTypes_;
    }

    [[nodiscard]] ArenaVector<ir::AnnotationUsage *> &Annotations() noexcept
    {
        return annotations_;
    }

    [[nodiscard]] const ArenaVector<ir::AnnotationUsage *> &Annotations() const noexcept
    {
        return annotations_;
    }

    void SetAnnotations(ArenaVector<ir::AnnotationUsage *> &&annotations)
    {
        annotations_ = std::move(annotations);
        for (AnnotationUsage *anno : annotations_) {
            anno->SetParent(this);
        }
    }

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::VerifiedType Check([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

    void CleanUp() override
    {
        AstNode::CleanUp();
        typeParamTypes_.clear();
    }

    TSTypeAliasDeclaration *Construct(ArenaAllocator *allocator) override;
    void CopyTo(AstNode *other) const override;

private:
    bool RegisterUnexportedForDeclGen(ir::SrcDumper *dumper) const;
    friend class SizeOfNodeTest;
    ArenaVector<Decorator *> decorators_;
    ArenaVector<AnnotationUsage *> annotations_;
    Identifier *id_;
    TSTypeParameterDeclaration *typeParams_;
    ArenaVector<checker::Type *> typeParamTypes_;
};
}  // namespace ark::es2panda::ir

#endif
