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

#ifndef ES2PANDA_IR_TS_TYPE_REFERENCE_H
#define ES2PANDA_IR_TS_TYPE_REFERENCE_H

#include <ir/typeNode.h>

namespace panda::es2panda::binder {
class ScopeFindResult;
class Variable;
}  // namespace panda::es2panda::binder

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class TSTypeParameterInstantiation;

class TSTypeReference : public TypeNode {
public:
    explicit TSTypeReference(Expression *typeName, TSTypeParameterInstantiation *typeParams)
        : TypeNode(AstNodeType::TS_TYPE_REFERENCE), typeName_(typeName), typeParams_(typeParams)
    {
    }

    const TSTypeParameterInstantiation *TypeParams() const
    {
        return typeParams_;
    }

    const Expression *TypeName() const
    {
        return typeName_;
    }

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    checker::Type *Check(checker::Checker *checker) const override;
    checker::Type *GetType(checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder) override;

private:
    Expression *typeName_;
    TSTypeParameterInstantiation *typeParams_;
};
}  // namespace panda::es2panda::ir

#endif
