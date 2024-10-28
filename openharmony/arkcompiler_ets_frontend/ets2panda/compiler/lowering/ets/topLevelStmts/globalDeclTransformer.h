/*
 * Copyright (c) 2023 - 2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_GLOBALDECLTRANSFORMER_H
#define PANDA_GLOBALDECLTRANSFORMER_H

#include "util/helpers.h"
#include "compiler/lowering/phase.h"
#include "ir/visitor/IterateAstVisitor.h"

namespace ark::es2panda::compiler {

class GlobalDeclTransformer : public ir::visitor::CustomAstVisitor {
    const std::unordered_set<ir::AstNodeType> typeDecl_ = {
        ir::AstNodeType::CLASS_DECLARATION,         ir::AstNodeType::STRUCT_DECLARATION,
        ir::AstNodeType::TS_ENUM_DECLARATION,       ir::AstNodeType::TS_INTERFACE_DECLARATION,
        ir::AstNodeType::ETS_PACKAGE_DECLARATION,   ir::AstNodeType::ETS_IMPORT_DECLARATION,
        ir::AstNodeType::TS_TYPE_ALIAS_DECLARATION, ir::AstNodeType::EXPORT_ALL_DECLARATION,
        ir::AstNodeType::EXPORT_NAMED_DECLARATION,  ir::AstNodeType::REEXPORT_STATEMENT,
        ir::AstNodeType::NAMESPACE_DECLARATION,     ir::AstNodeType::ANNOTATION_DECLARATION,
    };

    const std::unordered_set<ir::AstNodeType> propertiesDecl_ = {
        ir::AstNodeType::FUNCTION_DECLARATION,
        ir::AstNodeType::VARIABLE_DECLARATION,
    };

public:
    struct ResultT {
        explicit ResultT(ArenaAllocator *alloc) : classProperties(alloc->Adapter()), initStatements(alloc->Adapter()) {}

        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        ArenaVector<ir::Statement *> classProperties;
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        ArenaVector<ir::Statement *> initStatements;
    };

    explicit GlobalDeclTransformer(ArenaAllocator *allocator) : allocator_(allocator), result_(allocator) {}

    /**
     * Removes top level statements, global variable declarations, global function declarations
     * @param stmts
     */
    void FilterDeclarations(ArenaVector<ir::Statement *> &stmts);

    /**
     * Creates ClassProperty for global variables and MethodFunction for global functions.
     * Copy top level statements to vector.
     * @param stmts top level statements
     * @param addInitializer $init$ should contain global variable initializers
     * @return pair (class properties, init statements)
     */
    ResultT TransformStatements(const ArenaVector<ir::Statement *> &stmts, bool addInitializer);

    void VisitFunctionDeclaration(ir::FunctionDeclaration *funcDecl) override;
    void VisitVariableDeclaration(ir::VariableDeclaration *varDecl) override;
    void HandleNode(ir::AstNode *node) override;

    ir::Identifier *RefIdent(const util::StringView &name);

    ir::ExpressionStatement *InitTopLevelProperty(ir::ClassProperty *classProperty);

private:
    ArenaAllocator *allocator_;
    ResultT result_;
    bool addInitializer_ = true;
};

}  // namespace ark::es2panda::compiler
#endif  // PANDA_GLOBALDECLTRANSFORMER_H
