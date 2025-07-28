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

#ifndef ES2PANDA_PARSER_INCLUDE_AST_CLASS_PROPERTY_H
#define ES2PANDA_PARSER_INCLUDE_AST_CLASS_PROPERTY_H

#include "ir/base/classElement.h"
#include "ir/statements/annotationUsage.h"
#include "ir/annotationAllowed.h"
#include "ir/jsDocAllowed.h"

namespace ark::es2panda::checker {
class ETSAnalyzer;
}  // namespace ark::es2panda::checker

namespace ark::es2panda::ir {
class Expression;
class TypeNode;

class ClassProperty : public JsDocAllowed<AnnotationAllowed<ClassElement>> {
public:
    ClassProperty() = delete;
    ~ClassProperty() override = default;

    NO_COPY_SEMANTIC(ClassProperty);
    NO_MOVE_SEMANTIC(ClassProperty);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    explicit ClassProperty(Expression *const key, Expression *const value, TypeNode *const typeAnnotation,
                           ModifierFlags const modifiers, ArenaAllocator *const allocator, bool const isComputed)
        : JsDocAllowed<AnnotationAllowed<ClassElement>>(AstNodeType::CLASS_PROPERTY, key, value, modifiers, allocator,
                                                        isComputed),
          typeAnnotation_(typeAnnotation)
    {
    }

    [[nodiscard]] bool IsDefaultAccessModifier() const noexcept
    {
        return isDefault_;
    }

    void SetDefaultAccessModifier(bool isDefault)
    {
        isDefault_ = isDefault;
    }

    [[nodiscard]] TypeNode *TypeAnnotation() const noexcept
    {
        return typeAnnotation_;
    }

    void SetTypeAnnotation(TypeNode *typeAnnotation) noexcept
    {
        typeAnnotation_ = typeAnnotation;
    }

    [[nodiscard]] PrivateFieldKind ToPrivateFieldKind(bool const isStatic) const override
    {
        return isStatic ? PrivateFieldKind::STATIC_FIELD : PrivateFieldKind::FIELD;
    }

    [[nodiscard]] ClassProperty *Clone(ArenaAllocator *allocator, AstNode *parent) override;

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;

    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check(checker::TSChecker *checker) override;
    checker::VerifiedType Check(checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

    [[nodiscard]] bool NeedInitInStaticBlock() const
    {
        return needInitInStaticBlock_;
    }

    void SetInitInStaticBlock(bool needInitInStaticBlock)
    {
        needInitInStaticBlock_ = needInitInStaticBlock;
    }

protected:
    ClassProperty *Construct(ArenaAllocator *allocator) override;
    void CopyTo(AstNode *other) const override;

private:
    void DumpPrefix(ir::SrcDumper *dumper) const;
    void DumpModifiers(ir::SrcDumper *dumper) const;
    bool RegisterUnexportedForDeclGen(ir::SrcDumper *dumper) const;
    bool DumpNamespaceForDeclGen(ir::SrcDumper *dumper) const;
    void DumpCheckerTypeForDeclGen(ir::SrcDumper *dumper) const;

    friend class SizeOfNodeTest;
    TypeNode *typeAnnotation_;
    bool isDefault_ = false;
    bool needInitInStaticBlock_ = false;
};
}  // namespace ark::es2panda::ir

#endif
