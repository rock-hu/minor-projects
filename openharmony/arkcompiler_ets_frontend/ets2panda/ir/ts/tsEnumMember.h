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

#ifndef ES2PANDA_IR_TS_ENUM_MEMBER_H
#define ES2PANDA_IR_TS_ENUM_MEMBER_H

#include "ir/statement.h"
#include "ir/jsDocAllowed.h"

namespace ark::es2panda::ir {
class Expression;

class TSEnumMember : public Statement {
public:
    explicit TSEnumMember(Expression *key, Expression *init)
        : Statement(AstNodeType::TS_ENUM_MEMBER), key_(key), init_(init)
    {
    }

    explicit TSEnumMember(Expression *key, Expression *init, bool isGenerated)
        : Statement(AstNodeType::TS_ENUM_MEMBER), key_(key), init_(init), isGenerated_(isGenerated)
    {
    }

    const Expression *Key() const
    {
        return key_;
    }

    Expression *Key()
    {
        return key_;
    }

    const Expression *Init() const
    {
        return init_;
    }

    Expression *Init()
    {
        return init_;
    }

    bool IsGenerated() const
    {
        return isGenerated_;
    }

    [[nodiscard]] util::StringView Name() const;

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

private:
    Expression *key_;
    Expression *init_;
    bool isGenerated_ {false};
};
}  // namespace ark::es2panda::ir

#endif
