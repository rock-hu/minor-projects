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

#ifndef ES2PANDA_PARSER_INCLUDE_AST_CLASS_DEFINITION_H
#define ES2PANDA_PARSER_INCLUDE_AST_CLASS_DEFINITION_H

#include "varbinder/scope.h"
#include "varbinder/variable.h"
#include "ir/srcDump.h"
#include "ir/annotationAllowed.h"
#include "ir/astNode.h"
#include "ir/expressions/identifier.h"
#include "ir/jsDocAllowed.h"
#include "ir/statements/annotationUsage.h"
#include "ir/statements/classDeclaration.h"
#include "util/language.h"

namespace ark::es2panda::ir {
class ClassElement;
class Identifier;
class MethodDefinition;
class TSTypeParameterDeclaration;
class TSTypeParameterInstantiation;
class TSClassImplements;
class TSIndexSignature;

using ENUMBITOPS_OPERATORS;

enum class ClassDefinitionModifiers : uint32_t {
    NONE = 0,
    DECLARATION = 1U << 0U,
    ID_REQUIRED = 1U << 1U,
    GLOBAL = 1U << 2U,
    HAS_SUPER = 1U << 3U,
    SET_CTOR_ID = 1U << 4U,
    EXTERN = 1U << 5U,
    ANONYMOUS = 1U << 6U,
    GLOBAL_INITIALIZED = 1U << 7U,
    CLASS_DECL = 1U << 8U,
    INNER = 1U << 9U,
    FROM_EXTERNAL = 1U << 10U,
    LOCAL = 1U << 11U,
    CLASSDEFINITION_CHECKED = 1U << 12U,
    NAMESPACE_TRANSFORMED = 1U << 13U,
    STRING_ENUM_TRANSFORMED = 1U << 14U,
    INT_ENUM_TRANSFORMED = 1U << 15U,
    FROM_STRUCT = 1U << 16U,
    FUNCTIONAL_REFERENCE = 1U << 17U,
    DECLARATION_ID_REQUIRED = DECLARATION | ID_REQUIRED,
    ETS_MODULE = NAMESPACE_TRANSFORMED | GLOBAL
};

}  // namespace ark::es2panda::ir

template <>
struct enumbitops::IsAllowedType<ark::es2panda::ir::ClassDefinitionModifiers> : std::true_type {
};

namespace ark::es2panda::ir {

class ClassDefinition : public JsDocAllowed<AnnotationAllowed<TypedAstNode>> {
public:
    ClassDefinition() = delete;
    ~ClassDefinition() override = default;

    NO_COPY_SEMANTIC(ClassDefinition);
    NO_MOVE_SEMANTIC(ClassDefinition);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    explicit ClassDefinition(Identifier *ident, TSTypeParameterDeclaration *typeParams,
                             TSTypeParameterInstantiation *superTypeParams,
                             ArenaVector<TSClassImplements *> &&implements, MethodDefinition *ctor,
                             Expression *superClass, ArenaVector<AstNode *> &&body, ClassDefinitionModifiers modifiers,
                             ModifierFlags flags, Language lang)
        : JsDocAllowed<AnnotationAllowed<TypedAstNode>>(AstNodeType::CLASS_DEFINITION, flags,
                                                        ArenaVector<AnnotationUsage *>(body.get_allocator()),
                                                        ArenaVector<JsDocInfo>(body.get_allocator())),
          ident_(ident),
          typeParams_(typeParams),
          superTypeParams_(superTypeParams),
          implements_(std::move(implements)),
          ctor_(ctor),
          superClass_(superClass),
          body_(std::move(body)),
          modifiers_(modifiers),
          lang_(lang),
          capturedVars_(body_.get_allocator()),
          localVariableIsNeeded_(body_.get_allocator()),
          localIndex_(classCounter_++),
          localPrefix_("$" + std::to_string(localIndex_)),
          exportedClasses_(body_.get_allocator())
    {
    }
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    explicit ClassDefinition(ArenaAllocator *allocator, Identifier *ident, ArenaVector<AstNode *> &&body,
                             ClassDefinitionModifiers modifiers, ModifierFlags flags, Language lang)
        : JsDocAllowed<AnnotationAllowed<TypedAstNode>>(AstNodeType::CLASS_DEFINITION, flags, allocator),
          ident_(ident),
          implements_(allocator->Adapter()),
          body_(std::move(body)),
          modifiers_(modifiers),
          lang_(lang),
          capturedVars_(allocator->Adapter()),
          localVariableIsNeeded_(allocator->Adapter()),
          localIndex_(classCounter_++),
          localPrefix_("$" + std::to_string(localIndex_)),
          exportedClasses_(body_.get_allocator())
    {
    }

    explicit ClassDefinition(ArenaAllocator *allocator, Identifier *ident, ClassDefinitionModifiers modifiers,
                             ModifierFlags flags, Language lang)
        : JsDocAllowed<AnnotationAllowed<TypedAstNode>>(AstNodeType::CLASS_DEFINITION, flags, allocator),
          ident_(ident),
          implements_(allocator->Adapter()),
          body_(allocator->Adapter()),
          modifiers_(modifiers),
          lang_(lang),
          capturedVars_(allocator->Adapter()),
          localVariableIsNeeded_(allocator->Adapter()),
          localIndex_(classCounter_++),
          localPrefix_("$" + std::to_string(localIndex_)),
          exportedClasses_(body_.get_allocator())
    {
    }

    [[nodiscard]] bool IsScopeBearer() const noexcept override
    {
        return true;
    }

    [[nodiscard]] varbinder::LocalScope *Scope() const noexcept override
    {
        return scope_;
    }

    void SetScope(varbinder::LocalScope *scope)
    {
        ES2PANDA_ASSERT(scope_ == nullptr);
        scope_ = scope;
    }

    void ClearScope() noexcept override
    {
        scope_ = nullptr;
    }

    [[nodiscard]] const Identifier *Ident() const noexcept
    {
        return ident_;
    }

    [[nodiscard]] Identifier *Ident() noexcept
    {
        return ident_;
    }

    void SetIdent(ir::Identifier *ident) noexcept;

    [[nodiscard]] const util::StringView &InternalName() const noexcept
    {
        return internalName_;
    }

    void SetInternalName(util::StringView internalName) noexcept
    {
        internalName_ = internalName;
    }

    [[nodiscard]] Expression *Super() noexcept
    {
        return superClass_;
    }

    [[nodiscard]] const Expression *Super() const noexcept
    {
        return superClass_;
    }

    void SetSuper(Expression *superClass)
    {
        superClass_ = superClass;
        if (superClass_ != nullptr) {
            superClass_->SetParent(this);
        }
    }

    [[nodiscard]] bool IsGlobal() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::GLOBAL) != 0;
    }

    [[nodiscard]] bool IsLocal() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::LOCAL) != 0;
    }

    [[nodiscard]] bool IsExtern() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::EXTERN) != 0;
    }

    [[nodiscard]] bool IsFromExternal() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::FROM_EXTERNAL) != 0;
    }
    [[nodiscard]] bool IsInner() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::INNER) != 0;
    }

    [[nodiscard]] bool IsGlobalInitialized() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::GLOBAL_INITIALIZED) != 0;
    }

    [[nodiscard]] bool IsClassDefinitionChecked() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::CLASSDEFINITION_CHECKED) != 0;
    }

    [[nodiscard]] bool IsAnonymous() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::ANONYMOUS) != 0;
    }

    [[nodiscard]] bool IsIntEnumTransformed() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::INT_ENUM_TRANSFORMED) != 0;
    }

    [[nodiscard]] bool IsStringEnumTransformed() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::STRING_ENUM_TRANSFORMED) != 0;
    }

    [[nodiscard]] bool IsEnumTransformed() const noexcept
    {
        return IsIntEnumTransformed() || IsStringEnumTransformed();
    }

    [[nodiscard]] bool IsNamespaceTransformed() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::NAMESPACE_TRANSFORMED) != 0;
    }

    [[nodiscard]] bool IsFromStruct() const noexcept
    {
        return (modifiers_ & ClassDefinitionModifiers::FROM_STRUCT) != 0;
    }

    [[nodiscard]] bool IsModule() const noexcept
    {
        return IsGlobal() || IsNamespaceTransformed();
    }

    [[nodiscard]] es2panda::Language Language() const noexcept
    {
        return lang_;
    }

    void SetGlobalInitialized() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::GLOBAL_INITIALIZED;
    }

    void SetInnerModifier() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::INNER;
    }

    void SetClassDefinitionChecked() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::CLASSDEFINITION_CHECKED;
    }

    void SetAnonymousModifier() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::ANONYMOUS;
    }

    void SetNamespaceTransformed() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::NAMESPACE_TRANSFORMED;
    }

    void SetFromStructModifier() noexcept
    {
        modifiers_ |= ClassDefinitionModifiers::FROM_STRUCT;
    }

    [[nodiscard]] ClassDefinitionModifiers Modifiers() const noexcept
    {
        return modifiers_;
    }

    void SetModifiers(ClassDefinitionModifiers modifiers) noexcept
    {
        modifiers_ = modifiers;
    }

    void AddProperties(ArenaVector<AstNode *> &&body)
    {
        for (auto *prop : body) {
            prop->SetParent(this);
        }

        body_.insert(body_.end(), body.begin(), body.end());
    }

    [[nodiscard]] ArenaVector<AstNode *> &Body() noexcept
    {
        return body_;
    }

    [[nodiscard]] const ArenaVector<AstNode *> &Body() const noexcept
    {
        return body_;
    }

    [[nodiscard]] MethodDefinition *Ctor() noexcept
    {
        return ctor_;
    }

    void SetCtor(MethodDefinition *ctor)
    {
        ctor_ = ctor;
    }

    [[nodiscard]] ArenaVector<ir::TSClassImplements *> &Implements() noexcept
    {
        return implements_;
    }

    [[nodiscard]] const ArenaVector<ir::TSClassImplements *> &Implements() const noexcept
    {
        return implements_;
    }

    [[nodiscard]] const ir::TSTypeParameterDeclaration *TypeParams() const noexcept
    {
        return typeParams_;
    }

    [[nodiscard]] ir::TSTypeParameterDeclaration *TypeParams() noexcept
    {
        return typeParams_;
    }

    void SetTypeParams(ir::TSTypeParameterDeclaration *typeParams)
    {
        typeParams_ = typeParams;
    }

    const TSTypeParameterInstantiation *SuperTypeParams() const
    {
        return superTypeParams_;
    }

    TSTypeParameterInstantiation *SuperTypeParams()
    {
        return superTypeParams_;
    }

    [[nodiscard]] static int LocalTypeCounter() noexcept
    {
        return classCounter_;
    }

    [[nodiscard]] int LocalIndex() const noexcept
    {
        return localIndex_;
    }

    [[nodiscard]] MethodDefinition *FunctionalReferenceReferencedMethod() const noexcept
    {
        return functionalReferenceReferencedMethod_;
    }

    void SetFunctionalReferenceReferencedMethod(MethodDefinition *functionalReferenceReferencedMethod)
    {
        functionalReferenceReferencedMethod_ = functionalReferenceReferencedMethod;
    }

    [[nodiscard]] const std::string &LocalPrefix() const noexcept
    {
        return localPrefix_;
    }

    bool CaptureVariable(varbinder::Variable *var)
    {
        return capturedVars_.insert(var).second;
    }

    bool AddToLocalVariableIsNeeded(varbinder::Variable *var)
    {
        return localVariableIsNeeded_.insert(var).second;
    }

    bool IsLocalVariableNeeded(varbinder::Variable *var) const
    {
        return localVariableIsNeeded_.find(var) != localVariableIsNeeded_.end();
    }

    [[nodiscard]] const ArenaSet<varbinder::Variable *> &CapturedVariables() const noexcept
    {
        return capturedVars_;
    }

    bool EraseCapturedVariable(varbinder::Variable *var)
    {
        return capturedVars_.erase(var) != 0;
    }

    void SetOrigEnumDecl(ir::TSEnumDeclaration *enumDecl)
    {
        origEnumDecl_ = enumDecl;
    }

    ir::TSEnumDeclaration *OrigEnumDecl() const
    {
        return origEnumDecl_;
    }

    ClassDeclaration *GetAnonClass() noexcept
    {
        return anonClass_;
    }

    void SetAnonClass(ClassDeclaration *anonClass) noexcept
    {
        anonClass_ = anonClass;
    }

    const FunctionExpression *Ctor() const;
    bool HasPrivateMethod() const;
    bool HasNativeMethod() const;
    bool HasComputedInstanceField() const;
    bool HasMatchingPrivateKey(const util::StringView &name) const;

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

    template <typename T>
    static void DumpItems(ir::SrcDumper *dumper, const std::string &prefix, const ArenaVector<T *> &items)
    {
        if (items.empty()) {
            return;
        }
        dumper->Add(prefix);
        for (size_t i = 0; i < items.size(); ++i) {
            items[i]->Dump(dumper);
            if (i < items.size() - 1) {
                dumper->Add(", ");
            }
        }
    }

    void CleanUp() override
    {
        AstNode::CleanUp();
        modifiers_ &= ~(ClassDefinitionModifiers::CLASSDEFINITION_CHECKED);
    }

    void AddToExportedClasses(const ir::ClassDeclaration *cls)
    {
        ES2PANDA_ASSERT(cls->IsExported() || cls->Definition()->IsGlobal());
        exportedClasses_.push_back(cls);
    }

    void BatchAddToExportedClasses(const ArenaVector<const ir::ClassDeclaration *> &classes)
    {
        for (const auto cls : classes) {
            AddToExportedClasses(cls);
        }
    }

    [[nodiscard]] const ArenaVector<const ir::ClassDeclaration *> &ExportedClasses() const noexcept
    {
        return exportedClasses_;
    }

protected:
    ClassDefinition *Construct(ArenaAllocator *allocator) override;

    void CopyTo(AstNode *other) const override;

private:
    void CompileStaticFieldInitializers(compiler::PandaGen *pg, compiler::VReg classReg,
                                        const std::vector<compiler::VReg> &staticComputedFieldKeys) const;

    // This method is needed by OHOS CI code checker
    void DumpBody(ir::SrcDumper *dumper) const;
    void DumpGlobalClass(ir::SrcDumper *dumper) const;
    void DumpPrefix(ir::SrcDumper *dumper) const;
    bool RegisterUnexportedForDeclGen(ir::SrcDumper *dumper) const;

    friend class SizeOfNodeTest;
    varbinder::LocalScope *scope_ {nullptr};
    util::StringView internalName_ {};
    Identifier *ident_ {};
    TSTypeParameterDeclaration *typeParams_ {};
    TSTypeParameterInstantiation *superTypeParams_ {};
    ArenaVector<TSClassImplements *> implements_;
    MethodDefinition *ctor_ {};
    Expression *superClass_ {};
    ArenaVector<AstNode *> body_;
    ClassDefinitionModifiers modifiers_;
    es2panda::Language lang_;
    ArenaSet<varbinder::Variable *> capturedVars_;
    ArenaSet<varbinder::Variable *> localVariableIsNeeded_;
    TSEnumDeclaration *origEnumDecl_ {};
    ClassDeclaration *anonClass_ {nullptr};
    static int classCounter_;
    int localIndex_ {};
    std::string localPrefix_ {};
    MethodDefinition *functionalReferenceReferencedMethod_ {};
    ArenaVector<const ir::ClassDeclaration *> exportedClasses_;
};
}  // namespace ark::es2panda::ir

#endif
