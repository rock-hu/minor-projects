/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_IR_STATEMENT_CLASS_DECLARATION_H
#define ES2PANDA_IR_STATEMENT_CLASS_DECLARATION_H

#include <ir/base/classDefinition.h>
#include <ir/statement.h>

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class ClassDeclaration : public Statement {
public:
    explicit ClassDeclaration(ClassDefinition *def, ArenaVector<Decorator *> &&decorators,
                              ArenaVector<Annotation *> &&annotations, bool isAnnoDecl)
        : Statement(AstNodeType::CLASS_DECLARATION), def_(def), decorators_(std::move(decorators)),
          isAnnotationDecl_(isAnnoDecl)
    {
        def_->Ctor()->SetAnnotations(std::move(annotations));
    }

    const ClassDefinition *Definition() const
    {
        return def_;
    }

    ClassDefinition *Definition()
    {
        return def_;
    }

    const ArenaVector<Decorator *> &Decorators() const
    {
        return decorators_;
    }

    bool HasDecorators() const
    {
        return !decorators_.empty();
    }

    bool IsAnnotationDecl() const
    {
        return isAnnotationDecl_;
    }

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    checker::Type *Check([[maybe_unused]] checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder) override;

private:
    ClassDefinition *def_;
    ArenaVector<Decorator *> decorators_;
    bool isAnnotationDecl_ = false;
};

}  // namespace panda::es2panda::ir

#endif
