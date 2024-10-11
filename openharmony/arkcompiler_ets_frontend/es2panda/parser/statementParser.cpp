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

#include <util/helpers.h>
#include <binder/tsBinding.h>
#include <ir/astNode.h>
#include <ir/base/annotation.h>
#include <ir/base/catchClause.h>
#include <ir/base/classDefinition.h>
#include <ir/base/decorator.h>
#include <ir/base/scriptFunction.h>
#include <ir/expression.h>
#include <ir/expressions/arrayExpression.h>
#include <ir/expressions/assignmentExpression.h>
#include <ir/expressions/binaryExpression.h>
#include <ir/expressions/conditionalExpression.h>
#include <ir/expressions/literals/stringLiteral.h>
#include <ir/expressions/objectExpression.h>
#include <ir/expressions/sequenceExpression.h>
#include <ir/module/exportAllDeclaration.h>
#include <ir/module/exportDefaultDeclaration.h>
#include <ir/module/exportNamedDeclaration.h>
#include <ir/module/exportSpecifier.h>
#include <ir/module/importDeclaration.h>
#include <ir/module/importDefaultSpecifier.h>
#include <ir/module/importNamespaceSpecifier.h>
#include <ir/module/importSpecifier.h>
#include <ir/statements/blockStatement.h>
#include <ir/statements/breakStatement.h>
#include <ir/statements/classDeclaration.h>
#include <ir/statements/continueStatement.h>
#include <ir/statements/debuggerStatement.h>
#include <ir/statements/doWhileStatement.h>
#include <ir/statements/emptyStatement.h>
#include <ir/statements/expressionStatement.h>
#include <ir/statements/forInStatement.h>
#include <ir/statements/forOfStatement.h>
#include <ir/statements/forUpdateStatement.h>
#include <ir/statements/functionDeclaration.h>
#include <ir/statements/ifStatement.h>
#include <ir/statements/labelledStatement.h>
#include <ir/statements/returnStatement.h>
#include <ir/statements/switchCaseStatement.h>
#include <ir/statements/switchStatement.h>
#include <ir/statements/throwStatement.h>
#include <ir/statements/tryStatement.h>
#include <ir/statements/variableDeclaration.h>
#include <ir/statements/variableDeclarator.h>
#include <ir/statements/whileStatement.h>
#include <ir/ts/tsEnumDeclaration.h>
#include <ir/ts/tsExternalModuleReference.h>
#include <ir/ts/tsImportEqualsDeclaration.h>
#include <ir/ts/tsInterfaceBody.h>
#include <ir/ts/tsInterfaceDeclaration.h>
#include <ir/ts/tsInterfaceHeritage.h>
#include <ir/ts/tsModuleBlock.h>
#include <ir/ts/tsModuleDeclaration.h>
#include <ir/ts/tsNamespaceExportDeclaration.h>
#include <ir/ts/tsTypeAliasDeclaration.h>
#include <ir/ts/tsTypeParameter.h>
#include <ir/ts/tsTypeParameterDeclaration.h>
#include <ir/ts/tsTypeParameterInstantiation.h>
#include <ir/ts/tsTypeReference.h>
#include <lexer/lexer.h>
#include <lexer/token/letters.h>
#include <lexer/token/sourceLocation.h>
#include <util/ustring.h>

#include <tuple>

#include "parserImpl.h"

namespace panda::es2panda::parser {

bool ParserImpl::CheckDeclare()
{
    ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE);

    const auto startPos = lexer_->Save();
    lexer_->NextToken();  // eat 'declare'
    if (lexer_->GetToken().NewLine()) {
        lexer_->Rewind(startPos);
        return false;
    }
    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_VAR:
        case lexer::TokenType::KEYW_LET:
        case lexer::TokenType::KEYW_CONST:
        case lexer::TokenType::KEYW_FUNCTION:
        case lexer::TokenType::KEYW_CLASS: {
            break;
        }
        case lexer::TokenType::LITERAL_IDENT: {
            if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_MODULE ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_NAMESPACE ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ENUM ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ABSTRACT ||
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_INTERFACE) {
                break;
            }

            [[fallthrough]];
        }
        default: {
            lexer_->Rewind(startPos);
            return false;
        }
    }

    if (context_.Status() & ParserStatus::IN_AMBIENT_CONTEXT) {
        lexer_->Rewind(startPos);
        ThrowSyntaxError("A 'declare' modifier cannot be used in an already ambient context.");
    }

    return true;
}

bool ParserImpl::IsLabelFollowedByIterationStatement()
{
    lexer_->NextToken();

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_DO:
        case lexer::TokenType::KEYW_FOR:
        case lexer::TokenType::KEYW_WHILE: {
            return true;
        }
        case lexer::TokenType::LITERAL_IDENT: {
            if (lexer_->Lookahead() == LEX_CHAR_COLON) {
                lexer_->NextToken();
                return IsLabelFollowedByIterationStatement();
            }

            [[fallthrough]];
        }
        default:
            return false;
    }
    return false;
}

bool ParserImpl::IsTsDeclarationStatement() const
{
    const auto startPos = lexer_->Save();
    bool isTsDeclarationStatement = false;

    auto keywordType = lexer_->GetToken().KeywordType();
    lexer_->NextToken();
    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT) {
        lexer_->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
    }
    switch (keywordType) {
        case lexer::TokenType::KEYW_MODULE:
        case lexer::TokenType::KEYW_NAMESPACE: {
            isTsDeclarationStatement = !lexer_->GetToken().NewLine() &&
                (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT ||
                lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING);
            break;
        }
        case lexer::TokenType::KEYW_GLOBAL: {
            isTsDeclarationStatement = lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT ||
                lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE ||
                lexer_->GetToken().Type() == lexer::TokenType::KEYW_EXPORT;
            break;
        }
        case lexer::TokenType::KEYW_INTERFACE:
        case lexer::TokenType::KEYW_TYPE: {
            isTsDeclarationStatement = !lexer_->GetToken().NewLine() &&
                lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT;
            break;
        }
        case lexer::TokenType::KEYW_ENUM: {
            isTsDeclarationStatement = true;
            break;
        }
        default: {
            break;
        }
    }

    lexer_->Rewind(startPos);
    return isTsDeclarationStatement;
}

ir::Statement *ParserImpl::ParseStatement(StatementParsingFlags flags)
{
    bool isDeclare = false;
    auto decoratorsAndAnnotations = ParseDecoratorsAndAnnotations();
    auto decorators = decoratorsAndAnnotations.first;
    auto annotations = decoratorsAndAnnotations.second;

    if (Extension() == ScriptExtension::TS) {
        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE) {
            isDeclare = CheckDeclare();
        }

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_AT) {
            lexer_->NextToken(); // eat @ symbol
            if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_INTERFACE) {
                ThrowSyntaxError("'interface' keyword expected.");
            }
            if (!decorators.empty()) {
                ThrowSyntaxError("Decorators are not valid here.", decorators.front()->Start());
            }
            if (!annotations.empty()) {
                ThrowSyntaxError("Annotations can not be used with annotation declaration",
                                 annotations.front()->Start());
            }
            return ParseClassDeclaration(true, std::move(decorators), std::move(annotations), isDeclare, false,
                                         false, true);
        }

        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ABSTRACT) {
            const auto startPos = lexer_->Save();
            lexer_->NextToken();  // eat abstract keyword

            if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE) {
                if (isDeclare) {
                    ThrowSyntaxError("'declare' modifier already seen.");
                }
                lexer_->NextToken();
                isDeclare = true;
            }

            if (!annotations.empty()) {
                ThrowSyntaxError("Annotations can not be used with abstract classes", annotations.front()->Start());
            }

            if (lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS) {
                lexer_->Rewind(startPos);
            } else {
                return ParseClassStatement(flags, isDeclare, std::move(decorators), std::move(annotations), true);
            }
        }

        if ((lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL ||
             lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_MODULE ||
             lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_NAMESPACE) &&
             IsTsDeclarationStatement()) {
            auto savedStatus = context_.Status();
            if (isDeclare) {
                context_.Status() |= ParserStatus::IN_AMBIENT_CONTEXT;
            }
            ir::TSModuleDeclaration *decl = ParseTsModuleDeclaration(isDeclare);
            context_.Status() = savedStatus;
            return decl;
        }
    }

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: {
            return ParseBlockStatement();
        }
        case lexer::TokenType::PUNCTUATOR_SEMI_COLON: {
            return ParseEmptyStatement();
        }
        case lexer::TokenType::KEYW_EXPORT: {
            return ParseExportDeclaration(flags, std::move(decorators), std::move(annotations));
        }
        case lexer::TokenType::KEYW_IMPORT: {
            return ParseImportDeclaration(flags);
        }
        case lexer::TokenType::KEYW_FUNCTION: {
            return ParseFunctionStatement(flags, isDeclare);
        }
        case lexer::TokenType::KEYW_CLASS: {
            return ParseClassStatement(flags, isDeclare, std::move(decorators), std::move(annotations));
        }
        case lexer::TokenType::KEYW_VAR: {
            return ParseVarStatement(isDeclare);
        }
        case lexer::TokenType::KEYW_LET: {
            return ParseLetStatement(flags, isDeclare);
        }
        case lexer::TokenType::KEYW_CONST: {
            return ParseConstStatement(flags, isDeclare);
        }
        case lexer::TokenType::KEYW_IF: {
            return ParseIfStatement();
        }
        case lexer::TokenType::KEYW_DO: {
            return ParseDoWhileStatement();
        }
        case lexer::TokenType::KEYW_FOR: {
            return ParseForStatement();
        }
        case lexer::TokenType::KEYW_TRY: {
            return ParseTryStatement();
        }
        case lexer::TokenType::KEYW_WHILE: {
            return ParseWhileStatement();
        }
        case lexer::TokenType::KEYW_BREAK: {
            return ParseBreakStatement();
        }
        case lexer::TokenType::KEYW_CONTINUE: {
            return ParseContinueStatement();
        }
        case lexer::TokenType::KEYW_THROW: {
            return ParseThrowStatement();
        }
        case lexer::TokenType::KEYW_RETURN: {
            return ParseReturnStatement();
        }
        case lexer::TokenType::KEYW_SWITCH: {
            return ParseSwitchStatement();
        }
        case lexer::TokenType::KEYW_DEBUGGER: {
            return ParseDebuggerStatement();
        }
        case lexer::TokenType::LITERAL_IDENT: {
            return ParsePotentialExpressionStatement(flags, isDeclare);
        }
        default: {
            break;
        }
    }

    return ParseExpressionStatement(flags);
}

ir::TSModuleDeclaration *ParserImpl::ParseTsModuleDeclaration(bool isDeclare, bool isExport)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    context_.Status() |= ParserStatus::TS_MODULE;

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL) {
        return ParseTsAmbientExternalModuleDeclaration(startLoc, isDeclare);
    }

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_NAMESPACE) {
        lexer_->NextToken();
        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT) {
            lexer_->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
        }
    } else {
        ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_MODULE);
        lexer_->NextToken();
        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT) {
            lexer_->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
        }
        if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING) {
            return ParseTsAmbientExternalModuleDeclaration(startLoc, isDeclare);
        }
    }

    return ParseTsModuleOrNamespaceDelaration(startLoc, isDeclare, isExport);
}

ir::TSModuleDeclaration *ParserImpl::ParseTsAmbientExternalModuleDeclaration(const lexer::SourcePosition &startLoc,
                                                                             bool isDeclare)
{
    bool isGlobal = false;
    ir::Expression *name = nullptr;

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL) {
        isGlobal = true;
        name = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
    } else {
        ASSERT(lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING);

        if (!isDeclare && !(context_.Status() & ParserStatus::IN_AMBIENT_CONTEXT)) {
            ThrowSyntaxError("Only ambient modules can use quoted names");
        }

        name = AllocNode<ir::StringLiteral>(lexer_->GetToken().String());
    }

    name->SetRange(lexer_->GetToken().Loc());

    lexer_->NextToken();

    binder::ExportBindings *exportBindings = Allocator()->New<binder::ExportBindings>(Allocator());
    auto localCtx = binder::LexicalScope<binder::TSModuleScope>(Binder(), exportBindings);

    ir::Statement *body = nullptr;
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        body = ParseTsModuleBlock();
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        lexer_->NextToken();
    } else if (!lexer_->GetToken().NewLine()) {
        ThrowSyntaxError("';' expected");
    }

    auto *moduleDecl = AllocNode<ir::TSModuleDeclaration>(localCtx.GetScope(), name, body, isDeclare, isGlobal);
    moduleDecl->SetRange({startLoc, lexer_->GetToken().End()});
    localCtx.GetScope()->BindNode(moduleDecl);

    return moduleDecl;
}

ir::TSModuleDeclaration *ParserImpl::ParseTsModuleOrNamespaceDelaration(const lexer::SourcePosition &startLoc,
                                                                        bool isDeclare, bool isExport)
{
    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Identifier expected");
    }

    auto name = lexer_->GetToken().Ident();
    auto *parentScope = Binder()->GetScope();
    binder::Variable *res = parentScope->FindLocalTSVariable<binder::TSBindingType::NAMESPACE>(name);
    if (!res && isExport && parentScope->IsTSModuleScope()) {
        res = parentScope->AsTSModuleScope()->FindExportTSVariable<binder::TSBindingType::NAMESPACE>(name);
        if (res != nullptr) {
            parentScope->AddLocalTSVariable<binder::TSBindingType::NAMESPACE>(name, res);
        }
    }
    if (res == nullptr) {
        Binder()->AddTsDecl<binder::NamespaceDecl>(lexer_->GetToken().Start(), isDeclare, Allocator(), name);
        res = parentScope->FindLocalTSVariable<binder::TSBindingType::NAMESPACE>(name);
        if (isExport && parentScope->IsTSModuleScope()) {
            parentScope->AsTSModuleScope()->AddExportTSVariable<binder::TSBindingType::NAMESPACE>(name, res);
        }
        res->AsNamespaceVariable()->SetExportBindings(Allocator()->New<binder::ExportBindings>(Allocator()));
    }
    binder::ExportBindings *exportBindings = res->AsNamespaceVariable()->GetExportBindings();

    auto *identNode = AllocNode<ir::Identifier>(name);
    identNode->SetRange(lexer_->GetToken().Loc());

    lexer_->NextToken();

    ir::Statement *body = nullptr;

    auto savedStatus = context_.Status();
    if (isDeclare) {
        context_.Status() |= ParserStatus::IN_AMBIENT_CONTEXT;
    }

    auto localCtx = binder::LexicalScope<binder::TSModuleScope>(Binder(), exportBindings);

    bool isInstantiated = IsInstantiatedInTsModuleBlock(&body);
    if (isDeclare) {
        isInstantiated = false;
    }

    context_.Status() = savedStatus;

    auto *moduleDecl = AllocNode<ir::TSModuleDeclaration>(localCtx.GetScope(), identNode, body,
                                                          isDeclare, false, isInstantiated);
    moduleDecl->SetRange({startLoc, lexer_->GetToken().End()});
    localCtx.GetScope()->BindNode(moduleDecl);
    res->Declaration()->AsNamespaceDecl()->Add(moduleDecl);

    return moduleDecl;
}

bool ParserImpl::IsInstantiatedInTsModuleBlock(ir::Statement **body)
{
    bool isInstantiated = false;
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD) {
        lexer_->NextToken();
        lexer::SourcePosition moduleStart = lexer_->GetToken().Start();
        *body = ParseTsModuleOrNamespaceDelaration(moduleStart, false, true);
        isInstantiated = (*body)->AsTSModuleDeclaration()->IsInstantiated();
    } else {
        *body = ParseTsModuleBlock();
        auto statements = (*body)->AsTSModuleBlock()->Statements();
        for (auto *it : statements) {
            auto statement = it;
            if (LIKELY(statement != nullptr) && statement->IsExportNamedDeclaration()) {
                statement = statement->AsExportNamedDeclaration()->Decl();
            }
            if (statement != nullptr && !statement->IsTSInterfaceDeclaration() &&
                !statement->IsTSTypeAliasDeclaration() &&
                (!statement->IsTSModuleDeclaration() || statement->AsTSModuleDeclaration()->IsInstantiated())) {
                isInstantiated = true;
                break;
            }
        }
    }
    return isInstantiated;
}

ir::TSImportEqualsDeclaration *ParserImpl::ParseTsImportEqualsDeclaration(const lexer::SourcePosition &startLoc,
                                                                          bool isExport)
{
    ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_IMPORT);
    lexer_->NextToken();
    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token");
    }

    auto *id = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
    id->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();  // eat id name

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        ThrowSyntaxError("'=' expected");
    }
    lexer_->NextToken();  // eat substitution

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("identifier expected");
    }

    if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_REQUIRE ||
        lexer_->Lookahead() != LEX_CHAR_LEFT_PAREN) {
        binder::DeclarationFlags declflag = binder::DeclarationFlags::NONE;
        auto *decl = Binder()->AddDecl<binder::ImportEqualsDecl>(id->Start(), declflag, false, id->Name());
        decl->BindNode(id);
        auto *scope = Binder()->GetScope();
        auto name = id->Name();
        auto *var = scope->FindLocalTSVariable<binder::TSBindingType::IMPORT_EQUALS>(name);
        CHECK_NOT_NULL(var);
        var->AsImportEqualsVariable()->SetScope(scope);
        if (isExport && scope->IsTSModuleScope()) {
            scope->AsTSModuleScope()->AddExportTSVariable<binder::TSBindingType::IMPORT_EQUALS>(name, var);
        }
    }

    auto *importEqualsDecl = AllocNode<ir::TSImportEqualsDeclaration>(id, ParseModuleReference(), isExport);
    importEqualsDecl->SetRange({startLoc, lexer_->GetToken().End()});

    ConsumeSemicolon(importEqualsDecl);

    return importEqualsDecl;
}

ir::TSNamespaceExportDeclaration *ParserImpl::ParseTsNamespaceExportDeclaration(const lexer::SourcePosition &startLoc)
{
    if (!IsDtsFile()) {
        ThrowSyntaxError("namespace export declaration is only supported in TypeScript '.d.ts'");
    }
    ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS);
    lexer_->NextToken();  // eat as keyword
    if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_NAMESPACE) {
        ThrowSyntaxError("'namespace' expected");
    }
    lexer_->NextToken();  // eat namespace keyword
    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("identifier expected");
    }

    auto *id = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
    id->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();  // eat identifier

    auto *namespaceExportDecl = AllocNode<ir::TSNamespaceExportDeclaration>(id);
    namespaceExportDecl->SetRange({startLoc, lexer_->GetToken().End()});

    ConsumeSemicolon(namespaceExportDecl);

    return namespaceExportDecl;
}

ir::TSModuleBlock *ParserImpl::ParseTsModuleBlock()
{
    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("'{' expected.");
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    auto statements = ParseStatementList();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ThrowSyntaxError("Expected a '}'");
    }

    auto *blockNode = AllocNode<ir::TSModuleBlock>(std::move(statements));
    blockNode->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    return blockNode;
}

ir::Statement *ParserImpl::ParseVarStatement(bool isDeclare)
{
    auto *variableDecl = ParseVariableDeclaration(VariableParsingFlags::VAR, isDeclare);
    ConsumeSemicolon(variableDecl);
    return variableDecl;
}

ir::Statement *ParserImpl::ParseLetStatement(StatementParsingFlags flags, bool isDeclare)
{
    if (!(flags & StatementParsingFlags::ALLOW_LEXICAL)) {
        ThrowSyntaxError("The 'let' declarations can only be declared at the top level or inside a block.");
    }

    auto *variableDecl = ParseVariableDeclaration(VariableParsingFlags::LET, isDeclare);
    ConsumeSemicolon(variableDecl);
    return variableDecl;
}

ir::Statement *ParserImpl::ParseConstStatement(StatementParsingFlags flags, bool isDeclare)
{
    lexer::SourcePosition constVarStar = lexer_->GetToken().Start();
    lexer_->NextToken();

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ENUM) {
        if (Extension() == ScriptExtension::TS) {
            return ParseEnumDeclaration(false, isDeclare, true);
        }
        ThrowSyntaxError("Unexpected token");
    }

    if (!(flags & StatementParsingFlags::ALLOW_LEXICAL)) {
        ThrowSyntaxError("The 'const' declarations can only be declared at the top level or inside a block.");
    }

    auto *variableDecl =
        ParseVariableDeclaration(VariableParsingFlags::CONST | VariableParsingFlags::NO_SKIP_VAR_KIND, isDeclare);
    variableDecl->SetStart(constVarStar);
    ConsumeSemicolon(variableDecl);

    return variableDecl;
}

ir::EmptyStatement *ParserImpl::ParseEmptyStatement()
{
    auto *empty = AllocNode<ir::EmptyStatement>();
    empty->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();
    return empty;
}

ir::DebuggerStatement *ParserImpl::ParseDebuggerStatement()
{
    auto *debuggerNode = AllocNode<ir::DebuggerStatement>();
    debuggerNode->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();
    ConsumeSemicolon(debuggerNode);
    return debuggerNode;
}

ir::Statement *ParserImpl::ParseFunctionStatement(StatementParsingFlags flags, bool isDeclare)
{
    CheckFunctionDeclaration(flags);

    if (!(flags & StatementParsingFlags::STMT_LEXICAL_SCOPE_NEEDED)) {
        return ParseFunctionDeclaration(false, ParserStatus::NO_OPTS, isDeclare);
    }

    auto localCtx = binder::LexicalScope<binder::LocalScope>(Binder());
    ArenaVector<ir::Statement *> stmts(Allocator()->Adapter());
    auto *funcDecl = ParseFunctionDeclaration(false, ParserStatus::NO_OPTS, isDeclare);
    stmts.push_back(funcDecl);

    auto *localBlockStmt = AllocNode<ir::BlockStatement>(localCtx.GetScope(), std::move(stmts));
    localBlockStmt->SetRange(funcDecl->Range());
    localCtx.GetScope()->BindNode(localBlockStmt);

    return funcDecl;
}

ir::Statement *ParserImpl::ParsePotentialExpressionStatement(StatementParsingFlags flags, bool isDeclare)
{
    if (lexer_->Lookahead() == LEX_CHAR_COLON) {
        const auto pos = lexer_->Save();
        lexer_->NextToken();
        return ParseLabelledStatement(pos);
    }

    if (Extension() == ScriptExtension::TS && IsTsDeclarationStatement()) {
        switch (lexer_->GetToken().KeywordType()) {
            case lexer::TokenType::KEYW_ENUM: {
                return ParseEnumDeclaration(false, isDeclare, false);
            }
            case lexer::TokenType::KEYW_TYPE: {
                return ParseTsTypeAliasDeclaration(isDeclare);
            }
            case lexer::TokenType::KEYW_INTERFACE: {
                return ParseTsInterfaceDeclaration(isDeclare);
            }
            default:
                break;
        }
    }

    return ParseExpressionStatement(flags);
}

ir::ClassDeclaration *ParserImpl::ParseClassStatement(StatementParsingFlags flags, bool isDeclare,
                                                      ArenaVector<ir::Decorator *> &&decorators,
                                                      ArenaVector<ir::Annotation *> &&annotations, bool isAbstract)
{
    if (!(flags & StatementParsingFlags::ALLOW_LEXICAL)) {
        ThrowSyntaxError("Lexical 'class' declaration is not allowed in single statement context");
    }

    return ParseClassDeclaration(true, std::move(decorators), std::move(annotations), isDeclare, isAbstract, false,
                                 false);
}

ir::ClassDeclaration *ParserImpl::ParseClassDeclaration(bool idRequired, ArenaVector<ir::Decorator *> &&decorators,
                                                        ArenaVector<ir::Annotation *> &&annotations, bool isDeclare,
                                                        bool isAbstract, bool isExported, bool isAnnotation)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    ir::ClassDefinition *classDefinition = ParseClassDefinition(true, idRequired, isDeclare, isAbstract);
    if (isExported && !idRequired) {
        classDefinition->SetAsExportDefault();
    }

    if (!decorators.empty()) {
        classDefinition->SetClassDecoratorPresent();
    }

    auto location = classDefinition->Ident() ? classDefinition->Ident()->Start() : startLoc;
    auto className = classDefinition->GetName();
    ASSERT(!className.Empty());

    binder::DeclarationFlags flag = isExported ? binder::DeclarationFlags::EXPORT : binder::DeclarationFlags::NONE;
    auto *decl = Binder()->AddDecl<binder::ClassDecl>(location, flag, classDefinition->Declare(), className);

    decl->BindNode(classDefinition);

    lexer::SourcePosition endLoc = classDefinition->End();
    auto *classDecl = AllocNode<ir::ClassDeclaration>(classDefinition, std::move(decorators), std::move(annotations),
                                                      isAnnotation);
    classDecl->SetRange({startLoc, endLoc});
    return classDecl;
}

ir::TSTypeAliasDeclaration *ParserImpl::ParseTsTypeAliasDeclaration(bool isDeclare)
{
    ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE);
    lexer::SourcePosition typeStart = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat type keyword

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT &&
        !(lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT && isDeclare)) {
        ThrowSyntaxError("Identifier expected");
    }

    if (lexer_->GetToken().IsReservedTypeName()) {
        std::string errMsg("Type alias name cannot be '");
        errMsg.append(TokenToString(lexer_->GetToken().KeywordType()));
        errMsg.append("'");
        ThrowSyntaxError(errMsg.c_str());
    }

    const util::StringView &ident = lexer_->GetToken().Ident();
    binder::TSBinding tsBinding(Allocator(), ident);
    auto *decl = Binder()->AddTsDecl<binder::TypeAliasDecl>(lexer_->GetToken().Start(), isDeclare, tsBinding.View());

    auto *id = AllocNode<ir::Identifier>(ident);
    id->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();

    ir::TSTypeParameterDeclaration *typeParamDecl = nullptr;
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        typeParamDecl = ParseTsTypeParameterDeclaration(true, true);
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        ThrowSyntaxError("'=' expected");
    }

    lexer_->NextToken();  // eat '='

    TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
    ir::Expression *typeAnnotation = ParseTsTypeAnnotation(&options);

    auto *typeAliasDecl = AllocNode<ir::TSTypeAliasDeclaration>(id, typeParamDecl, typeAnnotation, isDeclare);
    typeAliasDecl->SetRange({typeStart, lexer_->GetToken().End()});
    decl->BindNode(typeAliasDecl);
    ConsumeSemicolon(typeAliasDecl);

    return typeAliasDecl;
}

ir::TSInterfaceDeclaration *ParserImpl::ParseTsInterfaceDeclaration(bool isDeclare)
{
    ASSERT(lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_INTERFACE);
    context_.Status() |= ParserStatus::ALLOW_THIS_TYPE;
    lexer::SourcePosition interfaceStart = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat interface keyword

    ValidateTsInterfaceName(isDeclare);

    const util::StringView &ident = lexer_->GetToken().Ident();
    binder::TSBinding tsBinding(Allocator(), ident);

    const auto &bindings = Binder()->GetScope()->Bindings();
    auto res = bindings.find(tsBinding.View());
    binder::InterfaceDecl *decl {};

    if (res == bindings.end()) {
        decl = Binder()->AddTsDecl<binder::InterfaceDecl>(lexer_->GetToken().Start(), isDeclare,
                                                          Allocator(), tsBinding.View());
    } else if (!res->second->Declaration()->IsInterfaceDecl()) {
        Binder()->ThrowRedeclaration(lexer_->GetToken().Start(), ident);
    } else {
        decl = res->second->Declaration()->AsInterfaceDecl();
    }

    auto *id = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
    id->SetRange(lexer_->GetToken().Loc());
    id->SetReference();
    lexer_->NextToken();

    binder::LexicalScope<binder::LocalScope> localScope(Binder());

    ir::TSTypeParameterDeclaration *typeParamDecl = nullptr;
    if (Extension() == ScriptExtension::TS && lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        typeParamDecl = ParseTsTypeParameterDeclaration(true, true);
    }

    ArenaVector<ir::TSInterfaceHeritage *> extends = ParseTsInterfaceExtends();

    lexer::SourcePosition bodyStart = lexer_->GetToken().Start();
    auto members = ParseTsTypeLiteralOrInterface();

    auto *body = AllocNode<ir::TSInterfaceBody>(std::move(members));
    body->SetRange({bodyStart, lexer_->GetToken().End()});

    auto *interfaceDecl =
        AllocNode<ir::TSInterfaceDeclaration>(localScope.GetScope(), id, typeParamDecl, body, std::move(extends));
    interfaceDecl->SetRange({interfaceStart, lexer_->GetToken().End()});

    ASSERT(decl);

    if (res == bindings.end()) {
        decl->BindNode(interfaceDecl);
    }
    decl->AsInterfaceDecl()->Add(interfaceDecl);

    lexer_->NextToken();
    context_.Status() &= ~ParserStatus::ALLOW_THIS_TYPE;

    return interfaceDecl;
}

void ParserImpl::ValidateTsInterfaceName(bool isDeclare)
{
    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT &&
        !(lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT && isDeclare)) {
        ThrowSyntaxError("Identifier expected");
    }

    if (lexer_->GetToken().IsReservedTypeName()) {
        std::string errMsg("Interface name cannot be '");
        errMsg.append(TokenToString(lexer_->GetToken().KeywordType()));
        errMsg.append("'");
        ThrowSyntaxError(errMsg.c_str());
    }
}

ArenaVector<ir::TSInterfaceHeritage *> ParserImpl::ParseTsInterfaceExtends()
{
    ArenaVector<ir::TSInterfaceHeritage *> extends(Allocator()->Adapter());

    if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_EXTENDS) {
        return extends;
    }

    lexer_->NextToken();  // eat extends keyword
    while (true) {
        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            ThrowSyntaxError("Identifier expected");
        }
        const lexer::SourcePosition &heritageStart = lexer_->GetToken().Start();
        lexer::SourcePosition heritageEnd = lexer_->GetToken().End();
        ir::Expression *expr = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
        expr->AsIdentifier()->SetReference();
        expr->SetRange(lexer_->GetToken().Loc());
        if (lexer_->Lookahead() == LEX_CHAR_LESS_THAN) {
            lexer_->ForwardToken(lexer::TokenType::PUNCTUATOR_LESS_THAN, 1);
        } else {
            lexer_->NextToken();
        }
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD) {
            expr = ParseTsQualifiedReference(expr);
        }
        ir::TSTypeParameterInstantiation *typeParamInst = nullptr;
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
            typeParamInst = ParseTsTypeParameterInstantiation();
            heritageEnd = typeParamInst->End();
        }
        auto *typeReference = AllocNode<ir::TSTypeReference>(expr, typeParamInst);
        typeReference->SetRange({heritageStart, heritageEnd});
        auto *heritage = AllocNode<ir::TSInterfaceHeritage>(typeReference);
        heritage->SetRange(typeReference->Range());
        extends.push_back(heritage);
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            break;
        }
        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA) {
            ThrowSyntaxError("',' expected");
        }
        lexer_->NextToken();
    }

    return extends;
}

void ParserImpl::CheckFunctionDeclaration(StatementParsingFlags flags)
{
    if (flags & StatementParsingFlags::ALLOW_LEXICAL) {
        return;
    }

    if (lexer_->Lookahead() == LEX_CHAR_ASTERISK) {
        ThrowSyntaxError("Generators can only be declared at the top level or inside a block.");
    }

    ThrowSyntaxError(
        "In strict mode code, functions can only be "
        "declared at top level, inside a block, "
        "or "
        "as the body of an if statement");
}

void ParserImpl::ConsumeSemicolon(ir::Statement *statement)
{
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        statement->SetEnd(lexer_->GetToken().End());
        lexer_->NextToken();
        return;
    }

    if (!lexer_->GetToken().NewLine()) {
        if (lexer_->GetToken().Type() != lexer::TokenType::EOS &&
            lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
            ThrowSyntaxError("Unexpected token");
        }
    }
}

ArenaVector<ir::Statement *> ParserImpl::ParseStatementList(StatementParsingFlags flags)
{
    ArenaVector<ir::Statement *> statements(Allocator()->Adapter());
    ParseDirectivePrologue(&statements);

    auto endType =
        (flags & StatementParsingFlags::GLOBAL) ? lexer::TokenType::EOS : lexer::TokenType::PUNCTUATOR_RIGHT_BRACE;

    while (lexer_->GetToken().Type() != endType) {
        statements.push_back(ParseStatement(flags));
    }

    return statements;
}

bool ParserImpl::ParseDirective(ArenaVector<ir::Statement *> *statements)
{
    ASSERT(lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING);

    const util::StringView &str = lexer_->GetToken().String();

    const auto status = static_cast<ParserStatus>(
        context_.Status() & (ParserStatus::CONSTRUCTOR_FUNCTION | ParserStatus::HAS_COMPLEX_PARAM));
    if (status == ParserStatus::HAS_COMPLEX_PARAM && str.Is("use strict")) {
        ThrowSyntaxError(
            "Illegal 'use strict' directive in function with "
            "non-simple parameter list");
    }

    ir::Expression *exprNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    bool isDirective = exprNode->IsStringLiteral();

    auto *exprStatement = AllocNode<ir::ExpressionStatement>(exprNode);
    exprStatement->SetRange(exprNode->Range());

    ConsumeSemicolon(exprStatement);
    statements->push_back(exprStatement);

    return isDirective;
}

void ParserImpl::ParseDirectivePrologue(ArenaVector<ir::Statement *> *statements)
{
    while (true) {
        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING || !ParseDirective(statements)) {
            break;
        }
    }
}

ir::BlockStatement *ParserImpl::ParseBlockStatement(binder::Scope *scope)
{
    ASSERT(lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE);

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    auto statements = ParseStatementList();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ThrowSyntaxError("Expected a '}'");
    }

    auto *blockNode = AllocNode<ir::BlockStatement>(scope, std::move(statements));
    blockNode->SetRange({startLoc, lexer_->GetToken().End()});
    scope->BindNode(blockNode);

    return blockNode;
}

ir::BlockStatement *ParserImpl::ParseBlockStatement()
{
    auto localCtx = binder::LexicalScope<binder::LocalScope>(Binder());
    auto *blockNode = ParseBlockStatement(localCtx.GetScope());
    lexer_->NextToken();
    return blockNode;
}

ir::BreakStatement *ParserImpl::ParseBreakStatement()
{
    bool allowBreak = (context_.Status() & (ParserStatus::IN_ITERATION | ParserStatus::IN_SWITCH));

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON ||
        lexer_->GetToken().Type() == lexer::TokenType::EOS || lexer_->GetToken().NewLine() ||
        lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {

        if (!allowBreak && Extension() == ScriptExtension::JS) {
            ThrowSyntaxError("Illegal break statement");
        }

        if (!allowBreak && Extension() == ScriptExtension::TS) {
            if (context_.Status() & ParserStatus::FUNCTION) {
                ThrowSyntaxError("Jump target cannot cross function boundary");
            } else {
                ThrowSyntaxError(
                    "A 'break' statement can only be used within an "
                    "enclosing iteration or switch statement");
            }
        }

        auto *breakStatement = AllocNode<ir::BreakStatement>();
        breakStatement->SetRange({startLoc, lexer_->GetToken().End()});
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            lexer_->NextToken();
        }

        return breakStatement;
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token.");
    }

    const auto &label = lexer_->GetToken().Ident();

    if (!context_.FindLabel(label)) {
        ThrowSyntaxError("Undefined label");
    }

    auto *identNode = AllocNode<ir::Identifier>(label);
    identNode->SetRange(lexer_->GetToken().Loc());

    auto *breakStatement = AllocNode<ir::BreakStatement>(identNode);
    breakStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    ConsumeSemicolon(breakStatement);

    return breakStatement;
}

ir::ContinueStatement *ParserImpl::ParseContinueStatement()
{
    if (Extension() == ScriptExtension::TS &&
        (static_cast<ParserStatus>(context_.Status() & (ParserStatus::FUNCTION | ParserStatus::IN_ITERATION |
                                                        ParserStatus::IN_SWITCH)) == ParserStatus::FUNCTION)) {
        ThrowSyntaxError("Jump target cannot cross function boundary");
    }

    if (!(context_.Status() & ParserStatus::IN_ITERATION)) {
        if (Extension() == ScriptExtension::JS) {
            ThrowSyntaxError("Illegal continue statement");
        }
        if (Extension() == ScriptExtension::TS) {
            ThrowSyntaxError(
                "A 'continue' statement can only be used within an "
                "enclosing iteration statement");
        }
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer::SourcePosition endLoc = lexer_->GetToken().End();
    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        auto *continueStatement = AllocNode<ir::ContinueStatement>();
        continueStatement->SetRange({startLoc, lexer_->GetToken().End()});
        lexer_->NextToken();
        return continueStatement;
    }

    if (lexer_->GetToken().NewLine() || lexer_->GetToken().Type() == lexer::TokenType::EOS ||
        lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        auto *continueStatement = AllocNode<ir::ContinueStatement>();
        continueStatement->SetRange({startLoc, endLoc});
        return continueStatement;
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token.");
    }

    const auto &label = lexer_->GetToken().Ident();
    const ParserContext *labelCtx = context_.FindLabel(label);

    if (!labelCtx || !(labelCtx->Status() & (ParserStatus::IN_ITERATION | ParserStatus::IN_LABELED)) ||
       (labelCtx->Status() & ParserStatus::DISALLOW_CONTINUE)) {
        ThrowSyntaxError("Undefined label");
    }

    auto *identNode = AllocNode<ir::Identifier>(label);
    identNode->SetRange(lexer_->GetToken().Loc());

    auto *continueStatement = AllocNode<ir::ContinueStatement>(identNode);
    continueStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    ConsumeSemicolon(continueStatement);

    return continueStatement;
}

ir::DoWhileStatement *ParserImpl::ParseDoWhileStatement()
{
    auto *savedScope = Binder()->GetScope();
    IterationContext<binder::LoopScope> iterCtx(&context_, Binder());

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    ir::Statement *body = ParseStatement();

    if (lexer_->GetToken().Type() != lexer::TokenType::KEYW_WHILE) {
        ThrowSyntaxError("Missing 'while' keyword in a 'DoWhileStatement'");
    }

    lexer_->NextToken();
    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        ThrowSyntaxError("Missing left parenthesis in a 'DoWhileStatement'");
    }

    lexer_->NextToken();
    ir::Expression *test = nullptr;

    // The while expression should be included in the outer scope
    {
        auto outerScope = binder::LexicalScope<binder::Scope>::Enter(Binder(), savedScope);
        test = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Missing right parenthesis in a 'DoWhileStatement'");
    }

    auto *doWhileStatement = AllocNode<ir::DoWhileStatement>(iterCtx.LexicalScope().GetScope(), body, test);
    doWhileStatement->SetRange({startLoc, lexer_->GetToken().End()});
    iterCtx.LexicalScope().GetScope()->BindNode(doWhileStatement);

    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        doWhileStatement->SetEnd(lexer_->GetToken().End());
        lexer_->NextToken();
    }

    return doWhileStatement;
}

ir::FunctionDeclaration *ParserImpl::ParseFunctionDeclaration(bool canBeAnonymous, ParserStatus newStatus,
                                                              bool isDeclare)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();

    ASSERT(lexer_->GetToken().Type() == lexer::TokenType::KEYW_FUNCTION);
    ParserStatus savedStatus = context_.Status();

    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_MULTIPLY) {
        newStatus |= ParserStatus::GENERATOR_FUNCTION;
        lexer_->NextToken();
    }

    context_.Status() = savedStatus;

    // e.g. export default function () {}
    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT &&
        lexer_->GetToken().Type() != lexer::TokenType::KEYW_AWAIT) {
        if (canBeAnonymous) {
            ir::ScriptFunction *func = ParseFunction(newStatus, isDeclare);
            if (func->Body() != nullptr) {
                lexer_->NextToken();
            }
            func->SetStart(startLoc);
            func->SetAsExportDefault();

            auto *funcDecl = AllocNode<ir::FunctionDeclaration>(func);
            funcDecl->SetRange(func->Range());

            binder::DeclarationFlags declflag = (newStatus & ParserStatus::EXPORT_REACHED) ?
                                                binder::DeclarationFlags::EXPORT : binder::DeclarationFlags::NONE;
            Binder()->AddDecl<binder::FunctionDecl>(startLoc, declflag, isDeclare, Allocator(),
                                                    parser::SourceTextModuleRecord::DEFAULT_LOCAL_NAME, func);

            return funcDecl;
        }

        ThrowSyntaxError("Unexpected token, expected identifier after 'function' keyword");
    }

    if (!isDeclare) {
        CheckStrictReservedWord();
    }

    util::StringView ident = lexer_->GetToken().Ident();

    auto *identNode = AllocNode<ir::Identifier>(ident);
    identNode->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();

    newStatus |= ParserStatus::FUNCTION_DECLARATION;
    ir::ScriptFunction *func = ParseFunction(newStatus, isDeclare);
    if (func->Body() != nullptr) {
        lexer_->NextToken();
    }

    func->SetIdent(identNode);
    func->SetStart(startLoc);
    auto *funcDecl = AllocNode<ir::FunctionDeclaration>(func);
    funcDecl->SetRange(func->Range());

    AddFunctionToBinder(func, newStatus);

    if (func->IsOverload() && lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        lexer_->NextToken();
    }

    CheckOptionalBindingPatternParameter(func);

    return funcDecl;
}

void ParserImpl::AddFunctionToBinder(ir::ScriptFunction *func, ParserStatus newStatus)
{
    binder::DeclarationFlags declflag = (newStatus & ParserStatus::EXPORT_REACHED) ?
                                        binder::DeclarationFlags::EXPORT : binder::DeclarationFlags::NONE;
    const ir::Identifier *identNode = func->Id();
    const lexer::SourcePosition &startLoc = func->Start();
    const util::StringView ident = identNode->Name();
    if (Extension() == ScriptExtension::TS) {
        const auto &bindings = Binder()->GetScope()->Bindings();
        auto res = bindings.find(ident);
        binder::Decl *currentDecl = res == bindings.end() ? nullptr : res->second->Declaration();
        binder::FunctionDecl *decl {};

        if (res == bindings.end() ||
            (currentDecl->IsClassDecl() && currentDecl->AsClassDecl()->IsDeclare())) {
            decl = Binder()->AddDecl<binder::FunctionDecl>(identNode->Start(), declflag, func->Declare(),
                                                           Allocator(), ident, func);
        } else {
            if (!currentDecl->IsFunctionDecl()) {
                Binder()->ThrowRedeclaration(startLoc, currentDecl->Name());
            }

            decl = currentDecl->AsFunctionDecl();

            if (!decl->Node()->AsScriptFunction()->IsOverload()) {
                Binder()->ThrowRedeclaration(startLoc, currentDecl->Name());
            }
            if (!func->IsOverload()) {
                decl->BindNode(func);
            }
        }

        decl->Add(func);
    } else {
        Binder()->AddDecl<binder::FunctionDecl>(identNode->Start(), declflag, func->Declare(),
                                                Allocator(), ident, func);
    }
}

void ParserImpl::CheckOptionalBindingPatternParameter(ir::ScriptFunction *func) const
{
    if (func->Declare() || func->IsOverload()) {
        return;
    }
    for (auto *it : func->Params()) {
        if ((it->IsObjectPattern() && it->AsObjectPattern()->Optional()) ||
            (it->IsArrayPattern() && it->AsArrayPattern()->Optional())) {
            ThrowSyntaxError(
                "A binding pattern parameter cannot be optional in an "
                "implementation signature", it->Start());
        }
    }
}

ir::Statement *ParserImpl::ParseExpressionStatement(StatementParsingFlags flags)
{
    const auto startPos = lexer_->Save();
    ParserStatus savedStatus = context_.Status();

    if (lexer_->GetToken().IsAsyncModifier()) {
        context_.Status() |= ParserStatus::ASYNC_FUNCTION;
        lexer_->NextToken();

        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_FUNCTION && !lexer_->GetToken().NewLine()) {
            if (!(flags & StatementParsingFlags::ALLOW_LEXICAL)) {
                ThrowSyntaxError("Async functions can only be declared at the top level or inside a block.");
            }

            ir::FunctionDeclaration *functionDecl = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION);
            functionDecl->SetStart(startPos.token.Start());

            return functionDecl;
        }

        lexer_->Rewind(startPos);
    }

    ir::Expression *exprNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    context_.Status() = savedStatus;
    lexer::SourcePosition endPos = exprNode->End();

    auto *exprStatementNode = AllocNode<ir::ExpressionStatement>(exprNode);
    exprStatementNode->SetRange({startPos.token.Start(), endPos});
    ConsumeSemicolon(exprStatementNode);

    return exprStatementNode;
}

std::tuple<ForStatementKind, ir::Expression *, ir::Expression *> ParserImpl::ParseForInOf(
    ir::AstNode *initNode, ExpressionParseFlags exprFlags, bool isAwait)
{
    ForStatementKind forKind = ForStatementKind::UPDATE;
    ir::Expression *updateNode = nullptr;
    ir::Expression *rightNode = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN ||
        lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_OF) {
        const ir::VariableDeclarator *varDecl = initNode->AsVariableDeclaration()->Declarators().front();

        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN) {
            if (varDecl->Init()) {
                ThrowSyntaxError("for-in loop variable declaration may not have an initializer");
            }
            forKind = ForStatementKind::IN;
            exprFlags = ExpressionParseFlags::ACCEPT_COMMA;
        } else {
            if (varDecl->Init()) {
                ThrowSyntaxError("for-of loop variable declaration may not have an initializer");
            }

            forKind = ForStatementKind::OF;
        }

        lexer_->NextToken();
        rightNode = ParseExpression(exprFlags);
    } else {
        if (isAwait) {
            ThrowSyntaxError("Unexpected token");
        }

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
            ThrowSyntaxError("Invalid left-hand side in 'For[In/Of]Statement'");
        } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            lexer_->NextToken();
        } else {
            rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
            if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
                ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
            }
            lexer_->NextToken();
        }

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
            updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
        }
    }

    return {forKind, rightNode, updateNode};
}

std::tuple<ForStatementKind, ir::AstNode *, ir::Expression *, ir::Expression *> ParserImpl::ParseForInOf(
    ir::Expression *leftNode, ExpressionParseFlags exprFlags, bool isAwait)
{
    ForStatementKind forKind = ForStatementKind::UPDATE;
    ir::AstNode *initNode = nullptr;
    ir::Expression *updateNode = nullptr;
    ir::Expression *rightNode = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN ||
        (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_OF)) {
        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN) {
            forKind = ForStatementKind::IN;
            exprFlags = ExpressionParseFlags::ACCEPT_COMMA;
        } else {
            forKind = ForStatementKind::OF;
        }

        bool isValid = true;
        switch (leftNode->Type()) {
            case ir::AstNodeType::IDENTIFIER: {
                constexpr std::string_view ASYNC = "async";
                if (isAwait || !(forKind == ForStatementKind::OF && leftNode->AsIdentifier()->Name().Is(ASYNC) &&
                    leftNode->End().index - leftNode->Start().index == ASYNC.length())) {
                    break;
                }
                ThrowSyntaxError(" The left-hand side of a for-of loop may not be 'async'", leftNode->Start());
            }
            case ir::AstNodeType::MEMBER_EXPRESSION: {
                break;
            }
            case ir::AstNodeType::ARRAY_EXPRESSION: {
                isValid = leftNode->AsArrayExpression()->ConvertibleToArrayPattern();
                break;
            }
            case ir::AstNodeType::OBJECT_EXPRESSION: {
                isValid = leftNode->AsObjectExpression()->ConvertibleToObjectPattern();
                break;
            }
            default: {
                isValid = false;
            }
        }

        if (!isValid) {
            ValidateLvalueAssignmentTarget(leftNode);
        }

        initNode = leftNode;
        lexer_->NextToken();
        rightNode = ParseExpression(exprFlags);

        return {forKind, initNode, rightNode, updateNode};
    }

    if (isAwait) {
        ThrowSyntaxError("Unexpected token");
    }

    exprFlags &= ExpressionParseFlags::POTENTIALLY_IN_PATTERN;
    ir::Expression *expr = ParseAssignmentExpression(leftNode, exprFlags);

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
        initNode = ParseSequenceExpression(expr);
    } else {
        initNode = expr;
    }

    if (initNode->IsConditionalExpression()) {
        ir::ConditionalExpression *condExpr = initNode->AsConditionalExpression();
        if (condExpr->Alternate()->IsBinaryExpression()) {
            const auto *binaryExpr = condExpr->Alternate()->AsBinaryExpression();
            if (binaryExpr->OperatorType() == lexer::TokenType::KEYW_IN) {
                ThrowSyntaxError("Invalid left-hand side in for-in statement");
            }
        }
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Invalid left-hand side in 'For[In/Of]Statement'");
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
    }

    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        lexer_->NextToken();
    } else {
        rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
        }
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    }

    return {forKind, initNode, rightNode, updateNode};
}

std::tuple<ir::Expression *, ir::Expression *> ParserImpl::ParseForUpdate(bool isAwait)
{
    if (isAwait) {
        ThrowSyntaxError("Unexpected token");
    }

    ir::Expression *updateNode = nullptr;
    ir::Expression *rightNode = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        lexer_->NextToken();
    } else {
        rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
        }
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    }

    return {rightNode, updateNode};
}

ir::Statement *ParserImpl::ParseForStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    ForStatementKind forKind = ForStatementKind::UPDATE;
    ir::AstNode *initNode = nullptr;
    ir::Expression *updateNode = nullptr;
    ir::Expression *leftNode = nullptr;
    ir::Expression *rightNode = nullptr;
    bool canBeForInOf = true;
    bool isAwait = false;
    lexer_->NextToken();
    VariableParsingFlags varFlags = VariableParsingFlags::STOP_AT_IN | VariableParsingFlags::IN_FOR;
    ExpressionParseFlags exprFlags = ExpressionParseFlags::NO_OPTS;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT) {
        isAwait = true;
        varFlags |= VariableParsingFlags::DISALLOW_INIT;
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        ThrowSyntaxError("Missing left parenthesis in a 'ForStatement'");
    }
    lexer_->NextToken();

    IterationContext<binder::LoopScope> iterCtx(&context_, Binder());

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_VAR: {
            initNode = ParseVariableDeclaration(varFlags | VariableParsingFlags::VAR);
            break;
        }
        case lexer::TokenType::KEYW_LET: {
            initNode = ParseVariableDeclaration(varFlags | VariableParsingFlags::LET);
            break;
        }
        case lexer::TokenType::KEYW_CONST: {
            initNode = ParseVariableDeclaration(varFlags | VariableParsingFlags::CONST |
                                                VariableParsingFlags::ACCEPT_CONST_NO_INIT);
            break;
        }
        case lexer::TokenType::PUNCTUATOR_SEMI_COLON: {
            if (isAwait) {
                ThrowSyntaxError("Unexpected token");
            }

            canBeForInOf = false;
            lexer_->NextToken();
            break;
        }
        default: {
            leftNode = ParseUnaryOrPrefixUpdateExpression(ExpressionParseFlags::POTENTIALLY_IN_PATTERN);

            break;
        }
    }

    if (initNode != nullptr) {
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            lexer_->NextToken();
            canBeForInOf = false;
        } else {
            canBeForInOf = initNode->AsVariableDeclaration()->Declarators().size() == 1;
        }
    }

    // VariableDeclaration->DeclarationSize > 1 or seen semi_colon
    if (!canBeForInOf) {
        std::tie(rightNode, updateNode) = ParseForUpdate(isAwait);
    } else if (leftNode) {
        // initNode was parsed as LHS
        if (leftNode->IsArrayExpression() || leftNode->IsObjectExpression()) {
            exprFlags |= ExpressionParseFlags::POTENTIALLY_IN_PATTERN;
        }
        std::tie(forKind, initNode, rightNode, updateNode) = ParseForInOf(leftNode, exprFlags, isAwait);
    } else if (initNode) {
        // initNode was parsed as VariableDeclaration and declaration size = 1
        std::tie(forKind, rightNode, updateNode) = ParseForInOf(initNode, exprFlags, isAwait);
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Unexpected token, expected ')' in 'ForStatement'.");
    }
    lexer_->NextToken();

    ir::Statement *bodyNode = ParseStatement();
    lexer::SourcePosition endLoc = bodyNode->End();

    ir::Statement *forStatement = nullptr;
    auto *loopScope = iterCtx.LexicalScope().GetScope();

    if (forKind == ForStatementKind::UPDATE) {
        forStatement = AllocNode<ir::ForUpdateStatement>(loopScope, initNode, rightNode, updateNode, bodyNode);
    } else if (forKind == ForStatementKind::IN) {
        forStatement = AllocNode<ir::ForInStatement>(loopScope, initNode, rightNode, bodyNode);
    } else {
        forStatement = AllocNode<ir::ForOfStatement>(loopScope, initNode, rightNode, bodyNode, isAwait);
    }

    forStatement->SetRange({startLoc, endLoc});
    loopScope->BindNode(forStatement);

    return forStatement;
}

ir::IfStatement *ParserImpl::ParseIfStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer::SourcePosition endLoc = lexer_->GetToken().End();
    lexer_->NextToken();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        ThrowSyntaxError("Missing left parenthesis in an 'IfStatement'");
    }

    lexer_->NextToken();
    ir::Expression *test = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Missing right parenthesis in an 'IfStatement'");
    }

    lexer_->NextToken();
    ir::Statement *consequent = ParseStatement(StatementParsingFlags::IF_ELSE);

    if (Extension() == ScriptExtension::TS && consequent->IsEmptyStatement()) {
        ThrowSyntaxError("The body of an if statement cannot be the empty statement");
    }

    endLoc = consequent->End();
    ir::Statement *alternate = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_ELSE) {
        lexer_->NextToken();  // eat ELSE keyword
        alternate = ParseStatement(StatementParsingFlags::IF_ELSE);
        endLoc = alternate->End();
    }

    auto *ifStatement = AllocNode<ir::IfStatement>(test, consequent, alternate);
    ifStatement->SetRange({startLoc, endLoc});
    return ifStatement;
}

ir::LabelledStatement *ParserImpl::ParseLabelledStatement(const lexer::LexerPosition &pos)
{
    const auto savedPos = lexer_->Save();
    bool isLabelFollowedByIterationStatement = IsLabelFollowedByIterationStatement();
    lexer_->Rewind(savedPos);

    const util::StringView &actualLabel = pos.token.Ident();

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AWAIT && context_.IsModule()) {
        ThrowSyntaxError("'await' is a reserved identifier in module code", pos.token.Start());
    }

    if (context_.FindLabel(actualLabel)) {
        ThrowSyntaxError("Label already declared", pos.token.Start());
    }

    SavedParserContext newCtx(this, ParserStatus::IN_LABELED | context_.Status(), actualLabel);
    if (isLabelFollowedByIterationStatement) {
        context_.Status() &= ~ParserStatus::DISALLOW_CONTINUE;
    } else {
        context_.Status() |= ParserStatus::DISALLOW_CONTINUE;
    }

    auto *identNode = AllocNode<ir::Identifier>(actualLabel);
    identNode->SetRange(pos.token.Loc());

    lexer_->NextToken();

    ir::Statement *body = ParseStatement(StatementParsingFlags::LABELLED);

    auto *labeledStatement = AllocNode<ir::LabelledStatement>(identNode, body);
    labeledStatement->SetRange({pos.token.Start(), body->End()});

    return labeledStatement;
}

ir::ReturnStatement *ParserImpl::ParseReturnStatement()
{
    if (!(context_.Status() & ParserStatus::FUNCTION)) {
        ThrowSyntaxError("return keyword should be used in function body");
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer::SourcePosition endLoc = lexer_->GetToken().End();
    lexer_->NextToken();

    bool hasArgument = (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON &&
                        lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE &&
                        lexer_->GetToken().Type() != lexer::TokenType::EOS && !lexer_->GetToken().NewLine());

    ir::ReturnStatement *returnStatement = nullptr;

    if (hasArgument) {
        ir::Expression *expression = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
        endLoc = expression->End();
        returnStatement = AllocNode<ir::ReturnStatement>(expression);
    } else {
        returnStatement = AllocNode<ir::ReturnStatement>();
    }

    returnStatement->SetRange({startLoc, endLoc});
    ConsumeSemicolon(returnStatement);

    return returnStatement;
}

ir::SwitchCaseStatement *ParserImpl::ParseSwitchCaseStatement(bool *seenDefault)
{
    lexer::SourcePosition caseStartLoc = lexer_->GetToken().Start();
    ir::Expression *testExpr = nullptr;

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_CASE: {
            lexer_->NextToken();
            testExpr = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
            break;
        }
        case lexer::TokenType::KEYW_DEFAULT: {
            if (*seenDefault) {
                if (Extension() == ScriptExtension::TS) {
                    ThrowSyntaxError(
                        "A 'default' clause cannot appear more than once in a "
                        "'switch' statement");
                } else {
                    ThrowSyntaxError("Multiple default clauses.");
                }
            }
            *seenDefault = true;
            lexer_->NextToken();
            break;
        }
        default: {
            ThrowSyntaxError("Unexpected token, expected 'case' or 'default'.");
        }
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COLON) {
        ThrowSyntaxError("Unexpected token, expected ':'");
    }

    ArenaVector<ir::Statement *> consequents(Allocator()->Adapter());
    lexer::SourcePosition caseEndLoc = lexer_->GetToken().End();

    lexer_->NextToken();

    while (lexer_->GetToken().Type() != lexer::TokenType::KEYW_CASE &&
           lexer_->GetToken().Type() != lexer::TokenType::KEYW_DEFAULT &&
           lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ir::Statement *consequent = ParseStatement(StatementParsingFlags::ALLOW_LEXICAL);
        caseEndLoc = consequent->End();
        consequents.push_back(consequent);
    }

    auto *caseNode = AllocNode<ir::SwitchCaseStatement>(testExpr, std::move(consequents));
    caseNode->SetRange({caseStartLoc, caseEndLoc});
    return caseNode;
}

ir::SwitchStatement *ParserImpl::ParseSwitchStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    if (!(lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS)) {
        ThrowSyntaxError("Unexpected token, expected '('");
    }

    lexer_->NextToken();
    ir::Expression *discriminant = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);

    if (!(lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS)) {
        ThrowSyntaxError("Unexpected token, expected ')'");
    }

    lexer_->NextToken();
    SwitchContext switchContext(&context_);

    if (!(lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE)) {
        ThrowSyntaxError("Unexpected token, expected '{'");
    }

    lexer_->NextToken();
    bool seenDefault = false;
    auto localCtx = binder::LexicalScope<binder::LocalScope>(Binder());
    ArenaVector<ir::SwitchCaseStatement *> cases(Allocator()->Adapter());

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        cases.push_back(ParseSwitchCaseStatement(&seenDefault));
    }

    auto *switchStatement = AllocNode<ir::SwitchStatement>(localCtx.GetScope(), discriminant, std::move(cases));
    switchStatement->SetRange({startLoc, lexer_->GetToken().End()});
    localCtx.GetScope()->BindNode(switchStatement);

    lexer_->NextToken();

    return switchStatement;
}

ir::ThrowStatement *ParserImpl::ParseThrowStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();

    if (lexer_->GetToken().NewLine()) {
        if (Extension() == ScriptExtension::JS) {
            ThrowSyntaxError("Illegal newline after throw");
        }

        if (Extension() == ScriptExtension::TS) {
            ThrowSyntaxError("Line break not permitted here");
        }
    }

    ir::Expression *expression = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    lexer::SourcePosition endLoc = expression->End();

    auto *throwStatement = AllocNode<ir::ThrowStatement>(expression);
    throwStatement->SetRange({startLoc, endLoc});
    ConsumeSemicolon(throwStatement);

    return throwStatement;
}

ir::Expression *ParserImpl::ParseCatchParam()
{
    ir::Expression *param = nullptr;

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        return param;
    }

    lexer_->NextToken();  // eat left paren

    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        switch (lexer_->GetToken().KeywordType()) {
            case lexer::TokenType::KEYW_EVAL: {
                ThrowSyntaxError("Binding 'eval' in strict mode is invalid");
            }
            case lexer::TokenType::KEYW_ARGUMENTS: {
                ThrowSyntaxError("Binding 'arguments' in strict mode is invalid");
            }
            default: {
                break;
            }
        }

        param = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
        param->SetRange(lexer_->GetToken().Loc());

        lexer_->NextToken();
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET) {
        param = ParseArrayExpression(ExpressionParseFlags::MUST_BE_PATTERN);
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        param = ParseObjectExpression(ExpressionParseFlags::MUST_BE_PATTERN);
    } else {
        ThrowSyntaxError("Unexpected token in catch parameter");
    }

    Binder()->AddParamDecl(param);

    if (Extension() == ScriptExtension::TS) {
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
            lexer_->NextToken();  // eat ':'

            TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
            param->SetTsTypeAnnotation(ParseTsTypeAnnotation(&options));
        }

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
            ThrowSyntaxError("Catch clause variable cannot have an initializer");
        }
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Unexpected token, expected ')'");
    }

    lexer_->NextToken();

    return param;
}

ir::CatchClause *ParserImpl::ParseCatchClause()
{
    lexer::SourcePosition catchStartLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat 'catch' keyword

    auto catchParamCtx = binder::LexicalScope<binder::CatchParamScope>(Binder());
    auto *catchParamScope = catchParamCtx.GetScope();

    ir::Expression *param = ParseCatchParam();
    catchParamScope->BindNode(param);

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("Unexpected token, expected '{'");
    }

    auto catchCtx = binder::LexicalScope<binder::CatchScope>(Binder());
    auto *catchScope = catchCtx.GetScope();
    catchScope->AssignParamScope(catchParamScope);

    ir::BlockStatement *catchBlock = ParseBlockStatement(catchScope);
    lexer_->NextToken();
    lexer::SourcePosition endLoc = catchBlock->End();

    auto *catchClause = AllocNode<ir::CatchClause>(catchScope, param, catchBlock);
    catchClause->SetRange({catchStartLoc, endLoc});
    catchScope->BindNode(catchClause);

    return catchClause;
}

ir::TryStatement *ParserImpl::ParseTryStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer::SourcePosition endLoc = lexer_->GetToken().End();

    lexer_->NextToken();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("Unexpected token, expected '{'");
    }

    ir::BlockStatement *body = ParseBlockStatement();

    if (lexer_->GetToken().Type() != lexer::TokenType::KEYW_CATCH &&
        lexer_->GetToken().Type() != lexer::TokenType::KEYW_FINALLY) {
        ThrowSyntaxError("Missing catch or finally clause");
    }

    ir::CatchClause *catchClause = nullptr;
    ir::BlockStatement *finnalyClause = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_CATCH) {
        catchClause = ParseCatchClause();
        endLoc = catchClause->End();
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_FINALLY) {
        lexer_->NextToken();  // eat 'finally' keyword

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            ThrowSyntaxError("Unexpected token, expected '{'");
        }

        finnalyClause = ParseBlockStatement();
        endLoc = finnalyClause->End();
    }

    auto *tryStatement = AllocNode<ir::TryStatement>(body, catchClause, finnalyClause);
    tryStatement->SetRange({startLoc, endLoc});
    return tryStatement;
}

void ParserImpl::ValidateDeclaratorId(bool isDeclare)
{
    if (context_.Status() & ParserStatus::IN_AMBIENT_CONTEXT) {
        return;
    }

    switch (lexer_->GetToken().KeywordType()) {
        case lexer::TokenType::KEYW_AWAIT: {
            if (context_.IsModule() && !program_.IsDtsFile() && !context_.IsTsModule() && !isDeclare) {
                ThrowSyntaxError("'await' is not permitted as an identifier in module code");
            }
            break;
        }
        case lexer::TokenType::KEYW_EVAL: {
            ThrowSyntaxError("Binding 'eval' in strict mode is invalid");
        }
        case lexer::TokenType::KEYW_ARGUMENTS: {
            ThrowSyntaxError("Binding 'arguments' in strict mode is invalid");
        }
        case lexer::TokenType::KEYW_LET: {
            ThrowSyntaxError("let is disallowed as a lexically bound name");
            break;
        }
        case lexer::TokenType::KEYW_STATIC:
        case lexer::TokenType::KEYW_IMPLEMENTS:
        case lexer::TokenType::KEYW_INTERFACE:
        case lexer::TokenType::KEYW_PACKAGE:
        case lexer::TokenType::KEYW_PRIVATE:
        case lexer::TokenType::KEYW_PROTECTED:
        case lexer::TokenType::KEYW_PUBLIC:
        case lexer::TokenType::KEYW_YIELD: {
            ThrowSyntaxError("Unexpected reserved word");
        }
        default: {
            break;
        }
    }
}

ir::VariableDeclarator *ParserImpl::ParseVariableDeclaratorInitializer(ir::Expression *init, VariableParsingFlags flags,
                                                                       const lexer::SourcePosition &startLoc,
                                                                       bool isDeclare)
{
    if (flags & VariableParsingFlags::DISALLOW_INIT) {
        ThrowSyntaxError("for-await-of loop variable declaration may not have an initializer");
    }

    lexer_->NextToken();

    if (isDeclare && !(flags & VariableParsingFlags::CONST)) {
        ThrowSyntaxError("Initializers are not allowed in ambient contexts.");
    }

    auto exprFlags =
        ((flags & VariableParsingFlags::STOP_AT_IN) ? ExpressionParseFlags::STOP_AT_IN : ExpressionParseFlags::NO_OPTS);

    ir::Expression *initializer = ParseExpression(exprFlags);
    lexer::SourcePosition endLoc = initializer->End();

    auto *declarator = AllocNode<ir::VariableDeclarator>(init, initializer);
    declarator->SetRange({startLoc, endLoc});

    return declarator;
}

ir::VariableDeclarator *ParserImpl::ParseVariableDeclarator(VariableParsingFlags flags, bool isDeclare)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();

    bool isDefinite = false;
    ir::Expression *init = ParseVariableDeclaratorKey(flags, isDeclare, &isDefinite);

    ir::VariableDeclarator *declarator {};

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        declarator = ParseVariableDeclaratorInitializer(init, flags, startLoc, isDeclare);
    } else {
        if (!isDeclare && (flags & VariableParsingFlags::CONST) &&
            !(flags & VariableParsingFlags::ACCEPT_CONST_NO_INIT) &&
            !(context_.Status() & ParserStatus::IN_AMBIENT_CONTEXT)) {
            ThrowSyntaxError("Missing initializer in const declaration");
        }

        if (!isDeclare && !(flags & VariableParsingFlags::IN_FOR) &&
            (init->IsArrayPattern() || init->IsObjectPattern())) {
            ThrowSyntaxError("Missing initializer in destructuring declaration");
        }

        lexer::SourcePosition endLoc = init->End();
        declarator = AllocNode<ir::VariableDeclarator>(init);
        declarator->SetRange({startLoc, endLoc});
    }

    declarator->SetDefinite(isDefinite);

    std::vector<const ir::Identifier *> bindings = util::Helpers::CollectBindingNames(init);

    for (const auto *binding : bindings) {
        binder::Decl *decl = nullptr;
        binder::DeclarationFlags declflag = (flags & VariableParsingFlags::EXPORTED) ?
                                            binder::DeclarationFlags::EXPORT : binder::DeclarationFlags::NONE;
        if (flags & VariableParsingFlags::EXPORTED_IN_TSMODULE) {
            declflag |= binder::DeclarationFlags::EXPORT_IN_TSMODULE;
        }

        if (flags & VariableParsingFlags::VAR) {
            decl = Binder()->AddDecl<binder::VarDecl>(startLoc, declflag, isDeclare, binding->Name());
        } else if (flags & VariableParsingFlags::LET) {
            decl = Binder()->AddDecl<binder::LetDecl>(startLoc, declflag, isDeclare, binding->Name());
        } else {
            decl = Binder()->AddDecl<binder::ConstDecl>(startLoc, declflag, isDeclare, binding->Name());
        }

        decl->BindNode(init);
    }

    return declarator;
}

ir::Expression *ParserImpl::ParseVariableDeclaratorKey(VariableParsingFlags flags, bool isDeclare, bool *isDefinite)
{
    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT && isDeclare) {
        lexer_->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
    }

    ir::Expression *init = nullptr;
    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::LITERAL_IDENT: {
            ValidateDeclaratorId(isDeclare);

            if (!(flags & VariableParsingFlags::VAR) &&
                lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_LET) {
                ThrowSyntaxError("let is disallowed as a lexically bound name");
            }

            const util::StringView &identStr = lexer_->GetToken().Ident();
            init = AllocNode<ir::Identifier>(identStr);
            init->SetRange(lexer_->GetToken().Loc());

            if (Extension() == ScriptExtension::TS) {
                init->AsIdentifier()->SetReference();
            }

            lexer_->NextToken();
            break;
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET: {
            init = ParseArrayExpression(ExpressionParseFlags::MUST_BE_PATTERN);
            break;
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: {
            init = ParseObjectExpression(ExpressionParseFlags::MUST_BE_PATTERN);
            break;
        }
        default: {
            ThrowSyntaxError("Unexpected token in variable declaration");
        }
    }

    if (Extension() == ScriptExtension::TS) {
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK) {
            lexer_->NextToken();  // eat '!'
            *isDefinite = true;
        }
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
            lexer_->NextToken();  // eat ':'
            TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
            init->SetTsTypeAnnotation(ParseTsTypeAnnotation(&options));
        }
    }

    return init;
}

ir::Statement *ParserImpl::ParseVariableDeclaration(VariableParsingFlags flags, bool isDeclare, bool isExport)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    if (!(flags & VariableParsingFlags::NO_SKIP_VAR_KIND)) {
        lexer_->NextToken();
    }

    if ((flags & VariableParsingFlags::LET) && util::Helpers::IsGlobalIdentifier(lexer_->GetToken().Ident())) {
        ThrowSyntaxError("Declaration name conflicts with built-in global identifier '"
                        + lexer_->GetToken().Ident().Mutf8() + "'.");
    }

    if (Extension() == ScriptExtension::TS && lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ENUM) {
        if (!(flags & VariableParsingFlags::CONST)) {
            ThrowSyntaxError("Variable declaration expected.");
        }
        return ParseEnumDeclaration(isExport, isDeclare, true);
    }

    ArenaVector<ir::VariableDeclarator *> declarators(Allocator()->Adapter());

    while (true) {
        ir::VariableDeclarator *declarator = ParseVariableDeclarator(flags, isDeclare);

        declarators.push_back(declarator);

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA) {
            break;
        }
        lexer_->NextToken();
    }

    auto varKind = ir::VariableDeclaration::VariableDeclarationKind::VAR;

    if (flags & VariableParsingFlags::LET) {
        varKind = ir::VariableDeclaration::VariableDeclarationKind::LET;
    } else if (flags & VariableParsingFlags::CONST) {
        varKind = ir::VariableDeclaration::VariableDeclarationKind::CONST;
    }

    lexer::SourcePosition endLoc = declarators.back()->End();
    auto *declaration = AllocNode<ir::VariableDeclaration>(varKind, std::move(declarators), isDeclare);
    declaration->SetRange({startLoc, endLoc});

    return declaration;
}

ir::WhileStatement *ParserImpl::ParseWhileStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        ThrowSyntaxError("Unexpected token, expected '('");
    }

    lexer_->NextToken();
    ir::Expression *test = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Unexpected token, expected ')'");
    }

    lexer_->NextToken();
    IterationContext<binder::LoopScope> iterCtx(&context_, Binder());
    ir::Statement *body = ParseStatement();

    lexer::SourcePosition endLoc = body->End();
    auto *whileStatement = AllocNode<ir::WhileStatement>(iterCtx.LexicalScope().GetScope(), test, body);
    whileStatement->SetRange({startLoc, endLoc});
    iterCtx.LexicalScope().GetScope()->BindNode(whileStatement);

    return whileStatement;
}

void ParserImpl::AddImportEntryItem(const ir::StringLiteral *source,
                                    const ArenaVector<ir::AstNode *> *specifiers, bool isType, bool isLazy)
{
    if (context_.IsTsModule()) {
        return;
    }

    ASSERT(source != nullptr);

    if (specifiers == nullptr) {
        if (isType) {
            ThrowSyntaxError("Unexpected import type syntax", source->Start());
        }
        auto *moduleRecord = GetSourceTextModuleRecord();
        ASSERT(moduleRecord != nullptr);
        moduleRecord->AddModuleRequest(source->Str());
        return;
    }

    for (auto *it : *specifiers) {
        if (!it->IsImportDefaultSpecifier() && !it->IsImportNamespaceSpecifier() && !it->IsImportSpecifier()) {
            ThrowSyntaxError("Unexpected astNode type", it->Start());
        }
        if (it->IsImportSpecifier()) {
            AddImportEntryItemForImportSpecifier(source, it, isLazy);
        } else {
            AddImportEntryItemForImportDefaultOrNamespaceSpecifier(source, it, isType);
        }
    }
}

void ParserImpl::AddImportEntryItemForImportSpecifier(const ir::StringLiteral *source, const ir::AstNode *specifier,
    bool isLazy)
{
    auto *moduleRecord = specifier->AsImportSpecifier()->IsType() ?
        GetSourceTextTypeModuleRecord() : GetSourceTextModuleRecord();
    ASSERT(moduleRecord != nullptr);
    int moduleRequestIdx = moduleRecord->AddModuleRequest(source->Str(), isLazy);

    auto localName = specifier->AsImportSpecifier()->Local()->Name();
    auto importName = specifier->AsImportSpecifier()->Imported()->Name();
    auto localId = specifier->AsImportSpecifier()->Local();
    auto importId = specifier->AsImportSpecifier()->Imported();
    auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ImportEntry>(
        localName, importName, moduleRequestIdx, localId, importId);
    moduleRecord->AddImportEntry(entry);
}

void ParserImpl::AddImportEntryItemForImportDefaultOrNamespaceSpecifier(const ir::StringLiteral *source,
                                                                        const ir::AstNode *specifier, bool isType)
{
    auto *moduleRecord = isType ? GetSourceTextTypeModuleRecord() : GetSourceTextModuleRecord();
    ASSERT(moduleRecord != nullptr);
    auto moduleRequestIdx = moduleRecord->AddModuleRequest(source->Str());

    if (specifier->IsImportDefaultSpecifier()) {
        auto localName = specifier->AsImportDefaultSpecifier()->Local()->Name();
        auto importName = parser::SourceTextModuleRecord::DEFAULT_EXTERNAL_NAME;
        auto localId = specifier->AsImportDefaultSpecifier()->Local();
        auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ImportEntry>(
            localName, importName, moduleRequestIdx, localId, nullptr);
        moduleRecord->AddImportEntry(entry);
    } else if (specifier->IsImportNamespaceSpecifier()) {
        auto localName = specifier->AsImportNamespaceSpecifier()->Local()->Name();
        auto localId = specifier->AsImportNamespaceSpecifier()->Local();
        auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ImportEntry>(
            localName, moduleRequestIdx, localId);
        moduleRecord->AddStarImportEntry(entry);
    }
}

void ParserImpl::AddExportNamedEntryItem(const ArenaVector<ir::ExportSpecifier *> &specifiers,
                                         const ir::StringLiteral *source, bool isType)
{
    // The exported objects in the TSModuleScope do not need to be allocated index.
    if (context_.IsTsModule()) {
        ASSERT(Binder()->GetScope()->IsTSModuleScope());
        return;
    }

    if (source) {
        for (auto *it : specifiers) {
            auto exportSpecifier = it->AsExportSpecifier();
            auto moduleRecord =
                exportSpecifier->IsType() ? GetSourceTextTypeModuleRecord() : GetSourceTextModuleRecord();
            ASSERT(moduleRecord != nullptr);
            auto moduleRequestIdx = moduleRecord->AddModuleRequest(source->Str());

            auto importName = exportSpecifier->Local()->Name();
            auto exportName = exportSpecifier->Exported()->Name();
            auto importId = exportSpecifier->Local();
            auto exportId = exportSpecifier->Exported();
            auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
                exportName, importName, moduleRequestIdx, exportId, importId);
            if (!moduleRecord->AddIndirectExportEntry(entry)) {
                ThrowSyntaxError("Duplicate export name of '" + exportName.Mutf8() + "'",
                                 exportSpecifier->Start());
            }
        }
    } else {
        for (auto *it : specifiers) {
            auto exportSpecifier = it->AsExportSpecifier();
            auto moduleRecord =
                exportSpecifier->IsType() ? GetSourceTextTypeModuleRecord() : GetSourceTextModuleRecord();
            ASSERT(moduleRecord != nullptr);

            auto exportName = exportSpecifier->Exported()->Name();
            auto localName = exportSpecifier->Local()->Name();
            auto exportId = exportSpecifier->Exported();
            auto localId = exportSpecifier->Local();
            auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
                exportName, localName, exportId, localId);
            if (!moduleRecord->AddLocalExportEntry(entry)) {
                ThrowSyntaxError("Duplicate export name of '" + exportName.Mutf8() + "'",
                                 exportSpecifier->Start());
            }
        }
    }
}

void ParserImpl::AddExportStarEntryItem(const lexer::SourcePosition &startLoc, const ir::StringLiteral *source,
                                        const ir::Identifier *exported)
{
    auto moduleRecord = GetSourceTextModuleRecord();
    ASSERT(moduleRecord != nullptr);
    ASSERT(source != nullptr);
    auto moduleRequestIdx = moduleRecord->AddModuleRequest(source->Str());

    if (exported != nullptr) {
        /* Transform [NamespaceExport] into [NamespaceImport] & [LocalExport]
         * e.g. export * as ns from 'test.js'
         *      --->
         *      import * as [internalName] from 'test.js'
         *      export { [internalName] as ns }
         */
        auto namespaceExportInternalName = GetNamespaceExportInternalName();
        auto *decl = Binder()->AddDecl<binder::ConstDecl>(startLoc, binder::DeclarationFlags::EXPORT,
                                                          false, namespaceExportInternalName);
        decl->BindNode(exported);

        auto *importEntry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ImportEntry>(
            namespaceExportInternalName, moduleRequestIdx, nullptr);
        auto *exportEntry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
            exported->Name(), namespaceExportInternalName, exported, nullptr);
        moduleRecord->AddStarImportEntry(importEntry);
        if (!moduleRecord->AddLocalExportEntry(exportEntry)) {
            ThrowSyntaxError("Duplicate export name of '" + exported->Name().Mutf8() + "'", exported->Start());
        }
        return;
    }

    auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(moduleRequestIdx);
    moduleRecord->AddStarExportEntry(entry);
}

void ParserImpl::AddExportDefaultEntryItem(const ir::AstNode *declNode)
{
    if (context_.IsTsModule()) {
        return;
    }

    ASSERT(declNode != nullptr);
    if (declNode->IsTSInterfaceDeclaration() ||
        (declNode->IsFunctionDeclaration() && declNode->AsFunctionDeclaration()->Function()->IsOverload())) {
        return;
    }

    auto moduleRecord = GetSourceTextModuleRecord();
    ASSERT(moduleRecord != nullptr);
    util::StringView exportName = parser::SourceTextModuleRecord::DEFAULT_EXTERNAL_NAME;
    util::StringView localName = parser::SourceTextModuleRecord::DEFAULT_LOCAL_NAME;
    if (declNode->IsFunctionDeclaration() || declNode->IsClassDeclaration()) {
        localName = declNode->IsFunctionDeclaration() ? declNode->AsFunctionDeclaration()->Function()->GetName() :
                                                        declNode->AsClassDeclaration()->Definition()->GetName();
    }

    ASSERT(!localName.Empty());
    auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
        exportName, localName, nullptr, nullptr);
    if (!moduleRecord->AddLocalExportEntry(entry)) {
        ThrowSyntaxError("Duplicate export name of '" + exportName.Mutf8() + "'", declNode->Start());
    }
}

void ParserImpl::AddExportLocalEntryItem(const ir::Statement *declNode, bool isTsModule)
{
    ASSERT(declNode != nullptr);
    auto moduleRecord = GetSourceTextModuleRecord();
    ASSERT(isTsModule || moduleRecord != nullptr);
    binder::TSModuleScope *tsModuleScope = nullptr;
    if (isTsModule) {
        ASSERT(Binder()->GetScope()->IsTSModuleScope());
        tsModuleScope = Binder()->GetScope()->AsTSModuleScope();
    }
    if (declNode->IsVariableDeclaration()) {
        auto declarators = declNode->AsVariableDeclaration()->Declarators();
        for (auto *decl : declarators) {
            std::vector<const ir::Identifier *> bindings = util::Helpers::CollectBindingNames(decl->Id());
            for (const auto *binding : bindings) {
                if (isTsModule) {
                    tsModuleScope->AddExportVariable(binding->Name());
                } else {
                    auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
                        binding->Name(), binding->Name(), binding, binding);
                    if (!moduleRecord->AddLocalExportEntry(entry)) {
                        ThrowSyntaxError("Duplicate export name of '" + binding->Name().Mutf8()
                                         + "'", binding->Start());
                    }
                }
            }
        }
    }
    if (declNode->IsFunctionDeclaration() || declNode->IsClassDeclaration()) {
        auto name = declNode->IsFunctionDeclaration() ?
                    declNode->AsFunctionDeclaration()->Function()->Id() :
                    declNode->AsClassDeclaration()->Definition()->Ident();
        if (name == nullptr) {
            ThrowSyntaxError("A class or function declaration without the default modifier must have a name.",
                             declNode->Start());
        }
        if (declNode->IsFunctionDeclaration() && declNode->AsFunctionDeclaration()->Function()->IsOverload()) {
            return;
        }
        if (isTsModule) {
            tsModuleScope->AddExportVariable(name->Name());
        } else {
            auto *entry = moduleRecord->NewEntry<parser::SourceTextModuleRecord::ExportEntry>(
                name->Name(), name->Name(), name, name);
            if (!moduleRecord->AddLocalExportEntry(entry)) {
                ThrowSyntaxError("Duplicate export name of '" + name->Name().Mutf8() + "'", name->Start());
            }
        }
    }
    AddTsTypeExportLocalEntryItem(declNode, isTsModule, tsModuleScope);
}

void ParserImpl::AddTsTypeExportLocalEntryItem(const ir::Statement *declNode, bool isTsModule,
                                               binder::TSModuleScope *tsModuleScope)
{
    if (!declNode->IsTSInterfaceDeclaration() && !declNode->IsTSTypeAliasDeclaration()) {
        return;
    }
    auto name = declNode->IsTSInterfaceDeclaration() ?
                declNode->AsTSInterfaceDeclaration()->Id() : declNode->AsTSTypeAliasDeclaration()->Id();
    if (name == nullptr) {
        ThrowSyntaxError("An interface or type alias declaration must have a name.");
    }
    if (isTsModule) {
        binder::TSBinding tsBinding(Allocator(), name->Name());
        tsModuleScope->AddExportVariable(tsBinding.View());
    }
}

ir::ExportDefaultDeclaration *ParserImpl::ParseExportDefaultDeclaration(const lexer::SourcePosition &startLoc,
                                                                        ArenaVector<ir::Decorator *> decorators,
                                                                        ArenaVector<ir::Annotation *> annotations,
                                                                        bool isExportEquals)
{
    lexer_->NextToken();  // eat `default` keyword or `=`

    ir::AstNode *declNode = nullptr;
    bool eatSemicolon = false;

    if (!decorators.empty() && lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS &&
        (context_.Status() & ParserStatus::IN_CLASS_BODY) == 0) {
        ThrowSyntaxError("Decorators are not valid here.", decorators.front()->Start());
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_FUNCTION) {
        declNode = ParseFunctionDeclaration(true, ParserStatus::EXPORT_REACHED);
    } else if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_CLASS) {
        declNode = ParseClassDeclaration(false, std::move(decorators), std::move(annotations), false, false, true);
    } else if (lexer_->GetToken().IsAsyncModifier()) {
        lexer_->NextToken();  // eat `async` keyword
        declNode = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION | ParserStatus::EXPORT_REACHED);
    } else if (Extension() == ScriptExtension::TS &&
               lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_INTERFACE) {
        declNode = ParseTsInterfaceDeclaration(false);
    } else if (Extension() == ScriptExtension::TS &&
               lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ABSTRACT) {
        lexer_->NextToken();
        if (lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS) {
            ThrowSyntaxError("Unexpected token, expected 'class'.");
        }

        if (!annotations.empty()) {
            ThrowSyntaxError("Annotations can not be used with abstract classes", annotations.front()->Start());
        }

        declNode = ParseClassDeclaration(false, std::move(decorators), std::move(annotations), false, true, true);
    } else {
        declNode = ParseExpression();
        Binder()->AddDecl<binder::LetDecl>(declNode->Start(), binder::DeclarationFlags::EXPORT,
                                           false, parser::SourceTextModuleRecord::DEFAULT_LOCAL_NAME);
        eatSemicolon = true;
    }

    // record default export entry
    if (!isExportEquals) {
        AddExportDefaultEntryItem(declNode);
    }

    lexer::SourcePosition endLoc = declNode->End();
    auto *exportDeclaration = AllocNode<ir::ExportDefaultDeclaration>(declNode, isExportEquals);
    exportDeclaration->SetRange({startLoc, endLoc});

    if (eatSemicolon) {
        ConsumeSemicolon(exportDeclaration);
    }

    return exportDeclaration;
}

ir::Identifier *ParserImpl::ParseNamedExport(const lexer::Token &exportedToken)
{
    if (exportedToken.Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token, expected an identifier.");
    }

    const util::StringView &exportedString = exportedToken.Ident();

    auto *exported = AllocNode<ir::Identifier>(exportedString);
    exported->SetRange(exportedToken.Loc());

    return exported;
}

ir::ExportAllDeclaration *ParserImpl::ParseExportAllDeclaration(const lexer::SourcePosition &startLoc)
{
    lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat `*` character

    ir::Identifier *exported = nullptr;

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
        lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);
        exported = ParseNamedExport(lexer_->GetToken());
        lexer_->NextToken();  // eat exported name
    }

    ir::StringLiteral *source = ParseFromClause();
    lexer::SourcePosition endLoc = source->End();

    // record export star entry
    AddExportStarEntryItem(startLoc, source, exported);

    ir::AssertClause *assertClause = nullptr;
    if (Extension() == ScriptExtension::TS && !lexer_->GetToken().NewLine() &&
        lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
        lexer_->GetToken().Ident().Is("assert")) {
        assertClause = ParseAssertClause();
    }

    auto *exportDeclaration = AllocNode<ir::ExportAllDeclaration>(source, exported, assertClause);
    exportDeclaration->SetRange({startLoc, endLoc});

    ConsumeSemicolon(exportDeclaration);

    return exportDeclaration;
}

ir::ExportNamedDeclaration *ParserImpl::ParseExportNamedSpecifiers(const lexer::SourcePosition &startLoc,
                                                                   bool isType)
{
    if (Extension() == ScriptExtension::TS && context_.IsTsModule() &&
        !(context_.Status() & ParserStatus::IN_AMBIENT_CONTEXT)) {
        ThrowSyntaxError("Export declarations are not permitted in a namespace.");
    }

    lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat `{` character

    ArenaVector<ir::ExportSpecifier *> specifiers(Allocator()->Adapter());

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        bool isTypeOfExportSpecifier = isType;
        if (Extension() == ScriptExtension::TS && lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE) {
            auto isTypeUsedAsKeyword = HandleTypeImportOrExportSpecifier();
            if (isTypeUsedAsKeyword) {
                lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat type
                if (isType) {
                    ThrowSyntaxError("The type modifier cannot be used on a named export "
                                     "when 'export type' is used on its export statement.");
                }
                isTypeOfExportSpecifier = true;
            }
        }

        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            ThrowSyntaxError("Unexpected token");
        }

        lexer::Token localToken = lexer_->GetToken();
        auto *local = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
        local->SetRange(lexer_->GetToken().Loc());

        if (Extension() == ScriptExtension::TS) {
            local->SetReference();
        }

        lexer_->NextToken();  // eat local name

        ir::Identifier *exported = nullptr;

        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
            lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat `as` literal
            exported = ParseNamedExport(lexer_->GetToken());
            lexer_->NextToken();  // eat exported name
        } else {
            exported = ParseNamedExport(localToken);
        }

        auto *specifier = AllocNode<ir::ExportSpecifier>(local, exported, isTypeOfExportSpecifier);
        specifier->SetRange({local->Start(), exported->End()});

        specifiers.push_back(specifier);

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat comma
        }
    }

    lexer::SourcePosition endPos = lexer_->GetToken().End();
    lexer_->NextToken();  // eat right brace

    ir::StringLiteral *source = nullptr;

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_FROM) {
        source = ParseFromClause();
    }

    // record ExportEntry
    AddExportNamedEntryItem(specifiers, source, isType);

    ir::AssertClause *assertClause = nullptr;
    if (Extension() == ScriptExtension::TS && !lexer_->GetToken().NewLine() &&
        lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
        lexer_->GetToken().Ident().Is("assert")) {
        if (source == nullptr) {
            ThrowSyntaxError("';' expected.");
        }
        if (isType) {
            ThrowSyntaxError("Import assertions cannot be used with type-only imports or exports.");
        }
        assertClause = ParseAssertClause();
    }

    auto *exportDeclaration = AllocNode<ir::ExportNamedDeclaration>(source, std::move(specifiers),
                                                                    assertClause, isType);
    exportDeclaration->SetRange({startLoc, endPos});
    ConsumeSemicolon(exportDeclaration);

    return exportDeclaration;
}

ir::ExportNamedDeclaration *ParserImpl::ParseNamedExportDeclaration(const lexer::SourcePosition &startLoc,
                                                                    ArenaVector<ir::Decorator *> &&decorators,
                                                                    ArenaVector<ir::Annotation *> &&annotations)
{
    ir::Statement *decl = nullptr;

    bool isDeclare = false;
    bool isTsModule = context_.IsTsModule();

    if (Extension() == ScriptExtension::TS && lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE) {
        isDeclare = CheckDeclare();
    }

    if (!decorators.empty() && lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS &&
        (context_.Status() & ParserStatus::IN_CLASS_BODY) == 0 &&
        !(lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
          (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_ABSTRACT))) {
        ThrowSyntaxError("Decorators are not valid here.", decorators.front()->Start());
    }

    if (!annotations.empty() && lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS &&
        (context_.Status() & ParserStatus::IN_CLASS_BODY) == 0) {
        ThrowSyntaxError("Annotations can be used only with classes and class methods", annotations.front()->Start());
    }

    VariableParsingFlags flag = isTsModule ?
        VariableParsingFlags::EXPORTED_IN_TSMODULE : VariableParsingFlags::EXPORTED;
    ParserStatus status = isTsModule ? ParserStatus::NO_OPTS : ParserStatus::EXPORT_REACHED;

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_VAR: {
            decl = ParseVariableDeclaration(flag | VariableParsingFlags::VAR, isDeclare, true);
            break;
        }
        case lexer::TokenType::KEYW_CONST: {
            decl = ParseVariableDeclaration(flag | VariableParsingFlags::CONST, isDeclare, true);
            break;
        }
        case lexer::TokenType::KEYW_LET: {
            decl = ParseVariableDeclaration(flag | VariableParsingFlags::LET, isDeclare, true);
            break;
        }
        case lexer::TokenType::KEYW_FUNCTION: {
            decl = ParseFunctionDeclaration(false, status, isDeclare);
            break;
        }
        case lexer::TokenType::KEYW_CLASS: {
            decl = ParseClassDeclaration(true, std::move(decorators), std::move(annotations), isDeclare || IsDtsFile(),
                                         false, !isTsModule);
            break;
        }
        case lexer::TokenType::PUNCTUATOR_AT: {
            lexer_->NextToken(); // eat @ symbol
            if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_INTERFACE) {
                ThrowSyntaxError("'interface' keyword expected.");
            }

            if (!decorators.empty()) {
                ThrowSyntaxError("Decorators are not valid here.", decorators.front()->Start());
            }

            if (!annotations.empty()) {
                ThrowSyntaxError("Annotations can not be used with annotation declaration",
                                 annotations.front()->Start());
            }
            decl = ParseClassDeclaration(true, std::move(decorators), std::move(annotations), isDeclare || IsDtsFile(),
                                         false, !isTsModule, true);
            break;
        }
        case lexer::TokenType::LITERAL_IDENT: {
            if (Extension() == ScriptExtension::TS) {
                switch (lexer_->GetToken().KeywordType()) {
                    case lexer::TokenType::KEYW_LET: {
                        decl = ParseVariableDeclaration(VariableParsingFlags::LET, isDeclare);
                        break;
                    }
                    case lexer::TokenType::KEYW_ENUM: {
                        decl = ParseEnumDeclaration(true, isDeclare, false);
                        break;
                    }
                    case lexer::TokenType::KEYW_INTERFACE: {
                        decl = ParseTsInterfaceDeclaration(isDeclare);
                        break;
                    }
                    case lexer::TokenType::KEYW_TYPE: {
                        decl = ParseTsTypeAliasDeclaration(isDeclare);
                        break;
                    }
                    case lexer::TokenType::KEYW_GLOBAL:
                    case lexer::TokenType::KEYW_MODULE:
                    case lexer::TokenType::KEYW_NAMESPACE: {
                        auto savedStatus = context_.Status();
                        if (isDeclare) {
                            context_.Status() |= ParserStatus::IN_AMBIENT_CONTEXT;
                        }
                        decl = ParseTsModuleDeclaration(isDeclare, true);
                        context_.Status() = savedStatus;
                        break;
                    }
                    case lexer::TokenType::KEYW_ABSTRACT: {
                        lexer_->NextToken();  // eat abstract keyword

                        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE) {
                            if (isDeclare) {
                                ThrowSyntaxError("'declare' modifier already seen.");
                            }
                            lexer_->NextToken();
                            isDeclare = true;
                        }

                        if (lexer_->GetToken().Type() != lexer::TokenType::KEYW_CLASS) {
                            ThrowSyntaxError("Unexpected token, expected 'class'.");
                        }
                        if (!annotations.empty()) {
                            ThrowSyntaxError("Annotations can not be used with abstract classes",
                                             annotations.front()->Start());
                        }
                        decl = ParseClassDeclaration(true, std::move(decorators), std::move(annotations),
                                                     isDeclare || IsDtsFile(), true, !isTsModule);
                        break;
                    }
                    default: {
                        break;
                    }
                }

                if (decl) {
                    break;
                }
            }

            [[fallthrough]];
        }
        default: {
            if (!lexer_->GetToken().IsAsyncModifier()) {
                ThrowSyntaxError("Unexpected token");
            }

            lexer_->NextToken();  // eat `async` keyword
            decl = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION | status);
        }
    }

    if (decl->IsVariableDeclaration()) {
        ConsumeSemicolon(decl);
    }

    AddExportLocalEntryItem(decl, isTsModule);

    lexer::SourcePosition endLoc = decl->End();
    ArenaVector<ir::ExportSpecifier *> specifiers(Allocator()->Adapter());
    auto *exportDeclaration = AllocNode<ir::ExportNamedDeclaration>(decl, std::move(specifiers));
    exportDeclaration->SetRange({startLoc, endLoc});

    return exportDeclaration;
}

ir::Statement *ParserImpl::ParseExportDeclaration(StatementParsingFlags flags,
                                                  ArenaVector<ir::Decorator *> &&decorators,
                                                  ArenaVector<ir::Annotation *> &&annotations)
{
    if (Extension() == ScriptExtension::JS || !context_.IsTsModule()) {
        if (!(flags & StatementParsingFlags::GLOBAL)) {
            ThrowSyntaxError("'import' and 'export' may only appear at the top level");
        }

        if (!context_.IsModule()) {
            ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
        }
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat `export` keyword

    // won't set `isType` for type alias
    bool isType = false;
    if (Extension() == ScriptExtension::TS &&
        lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE) {
        const auto savedPos = lexer_->Save();
        lexer_->NextToken();  // eat type
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            isType = true;
        } else {
            lexer_->Rewind(savedPos);
        }
    }

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_DEFAULT: { // export default Id
            return ParseExportDefaultDeclaration(startLoc, std::move(decorators), std::move(annotations));
        }
        case lexer::TokenType::PUNCTUATOR_MULTIPLY: { // export * ...
            return ParseExportAllDeclaration(startLoc);
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: { // export { ... } ...
            return ParseExportNamedSpecifiers(startLoc, isType);
        }
        case lexer::TokenType::KEYW_IMPORT: {
            return ParseTsImportEqualsDeclaration(startLoc, true);
        }
        case lexer::TokenType::PUNCTUATOR_SUBSTITUTION: {
            if (!annotations.empty()) {
                ThrowSyntaxError("Annotations can not be used with assignment expression",
                                 annotations.front()->Start());
            }

            if (Extension() == ScriptExtension::TS) {
                return ParseExportDefaultDeclaration(startLoc, std::move(decorators), std::move(annotations), true);
            }

            [[fallthrough]];
        }
        case lexer::TokenType::LITERAL_IDENT: {
            if (Extension() == ScriptExtension::TS && lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
                return ParseTsNamespaceExportDeclaration(startLoc);
            }

            [[fallthrough]];
        }
        default: { // export [var] id
            ir::ExportNamedDeclaration *exportDecl = ParseNamedExportDeclaration(startLoc, std::move(decorators),
                                                                                 std::move(annotations));

            if (Extension() == ScriptExtension::TS && exportDecl->Decl()->IsVariableDeclaration() &&
                !(flags & StatementParsingFlags::GLOBAL) && exportDecl->Parent() &&
                !exportDecl->Parent()->IsTSModuleBlock() && !context_.IsModule()) {
                ThrowSyntaxError("Modifiers cannot appear here'");
            }

            return exportDecl;
        }
    }
}

void ParserImpl::ParseNameSpaceImport(ArenaVector<ir::AstNode *> *specifiers, bool isType)
{
    lexer::SourcePosition namespaceStart = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat `*` character

    if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_AS) {
        ThrowSyntaxError("Unexpected token.");
    }

    lexer_->NextToken();  // eat `as` literal

    ir::Identifier *local = ParseNamedImport(lexer_->GetToken());

    auto *specifier = AllocNode<ir::ImportNamespaceSpecifier>(local);
    specifier->SetRange({namespaceStart, lexer_->GetToken().End()});
    specifiers->push_back(specifier);

    binder::Decl *decl = AddImportDecl(isType, local->Name(), namespaceStart,
        binder::DeclarationFlags::NAMESPACE_IMPORT);
    decl->BindNode(specifier);

    lexer_->NextToken();  // eat local name
}

binder::Decl *ParserImpl::AddImportDecl(bool isType,
                                        util::StringView name,
                                        lexer::SourcePosition startPos,
                                        binder::DeclarationFlags flag)
{
    if (isType) {
        binder::TSBinding tsBinding(Allocator(), name);
        return Binder()->AddTsDecl<binder::TypeAliasDecl>(startPos, flag, false, tsBinding.View());
    }
    return Binder()->AddDecl<binder::ConstDecl>(startPos, flag, false, name);
}

ir::Identifier *ParserImpl::ParseNamedImport(const lexer::Token &importedToken)
{
    if (importedToken.Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token");
    }

    switch (importedToken.KeywordType()) {
        case lexer::TokenType::KEYW_EVAL: {
            ThrowSyntaxError("Binding 'eval' in strict mode is invalid");
        }
        case lexer::TokenType::KEYW_ARGUMENTS: {
            ThrowSyntaxError("Binding 'arguments' in strict mode is invalid");
        }
        default: {
            break;
        }
    }

    auto *local = AllocNode<ir::Identifier>(importedToken.Ident());
    local->SetRange(importedToken.Loc());

    return local;
}

void ParserImpl::ParseNamedImportSpecifiers(ArenaVector<ir::AstNode *> *specifiers, bool isType, bool isLazy)
{
    lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat `{` character

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        bool isTypeOfImportSpecifier = isType;
        if (Extension() == ScriptExtension::TS && lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE) {
            auto isTypeUsedAsKeyword = HandleTypeImportOrExportSpecifier();
            if (isTypeUsedAsKeyword) {
                lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat type
                isTypeOfImportSpecifier = true;
            }
            if (isTypeUsedAsKeyword && isType) {
                ThrowSyntaxError("The type modifier cannot be used on a named import "
                                 "when 'import type' is used on its import statement.");
            }
        }

        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT ||
            (Extension() == ScriptExtension::TS && lexer_->GetToken().Ident().Is("await"))) {
            ThrowSyntaxError("Unexpected token");
        }

        lexer::Token importedToken = lexer_->GetToken();
        auto *imported = AllocNode<ir::Identifier>(importedToken.Ident());
        ir::Identifier *local = nullptr;
        imported->SetRange(lexer_->GetToken().Loc());

        lexer_->NextToken();  // eat import name

        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
            lexer_->NextToken();  // eat `as` literal
            local = ParseNamedImport(lexer_->GetToken());
            lexer_->NextToken();  // eat local name
        } else {
            local = ParseNamedImport(importedToken);
        }

        auto *specifier = AllocNode<ir::ImportSpecifier>(imported, local, isTypeOfImportSpecifier, isLazy);
        specifier->SetRange({imported->Start(), local->End()});
        specifiers->push_back(specifier);

        binder::Decl *decl = AddImportDecl(isTypeOfImportSpecifier, local->Name(), local->Start(),
                                           binder::DeclarationFlags::IMPORT);
        decl->BindNode(specifier);

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat comma
        } else if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
            ThrowSyntaxError("Unexpected token");
        }
    }

    lexer_->NextToken();  // eat right brace
}

bool ParserImpl::HandleTypeImportOrExportSpecifier()
{
    bool isTypeUsedAsKeyword = false;
    const auto savedPos = lexer_->Save();
    
    lexer_->NextToken(lexer::LexerNextTokenFlags::KEYWORD_TO_IDENT);  // eat type

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
        // {type as ...? }
        lexer_->NextToken(); // eat first as
        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
            // { type as as ...? }
            lexer_->NextToken(); // eat second as
            if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
                // { type as as something }
                isTypeUsedAsKeyword = true;
            } else {
                // { type as as }
                isTypeUsedAsKeyword = false;
            }
        } else if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
            // { type as something }
            isTypeUsedAsKeyword = false;
        } else {
            // { type as }
            isTypeUsedAsKeyword = true;
        }
    } else if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        // { type something ...? }
        isTypeUsedAsKeyword = true;
    } else {
        // { type }
        isTypeUsedAsKeyword = false;
    }

    lexer_->Rewind(savedPos);
    return isTypeUsedAsKeyword;
}

ir::Expression *ParserImpl::ParseModuleReference()
{
    ir::Expression *result = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
        lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_REQUIRE &&
        lexer_->Lookahead() == LEX_CHAR_LEFT_PAREN) {
        lexer::SourcePosition start = lexer_->GetToken().Start();
        lexer_->NextToken();  // eat 'require'
        lexer_->NextToken();  // eat '('

        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING) {
            ThrowSyntaxError("String literal expected.");
        }

        result = AllocNode<ir::StringLiteral>(lexer_->GetToken().String());
        result->SetRange(lexer_->GetToken().Loc());
        lexer_->NextToken();

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
            ThrowSyntaxError("')' expected.");
        }

        result = AllocNode<ir::TSExternalModuleReference>(result);
        result->SetRange({start, lexer_->GetToken().End()});
        lexer_->NextToken();  // eat ')'
    } else {
        result = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
        result->SetRange(lexer_->GetToken().Loc());
        lexer_->NextToken();

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD) {
            result = ParseTsQualifiedReference(result);
        }
    }

    return result;
}

ir::AstNode *ParserImpl::ParseImportDefaultSpecifier(ArenaVector<ir::AstNode *> *specifiers, bool isType)
{
    ir::Identifier *local = ParseNamedImport(lexer_->GetToken());
    lexer_->NextToken();  // eat local name

    if (Extension() == ScriptExtension::TS && lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        lexer_->NextToken();  // eat substitution
        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            ThrowSyntaxError("identifier expected");
        }
        if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_REQUIRE ||
            lexer_->Lookahead() != LEX_CHAR_LEFT_PAREN) {
            auto *decl = Binder()->AddDecl<binder::ImportEqualsDecl>(local->Start(), binder::DeclarationFlags::NONE,
                                                                     false, local->Name());
            decl->BindNode(local);
            auto *scope = Binder()->GetScope();
            auto *var = scope->FindLocalTSVariable<binder::TSBindingType::IMPORT_EQUALS>(local->Name());
            CHECK_NOT_NULL(var);
            var->AsImportEqualsVariable()->SetScope(scope);
        }

        auto *importEqualsDecl = AllocNode<ir::TSImportEqualsDeclaration>(local, ParseModuleReference(), false);

        return importEqualsDecl;
    }

    auto *specifier = AllocNode<ir::ImportDefaultSpecifier>(local);
    specifier->SetRange(specifier->Local()->Range());
    specifiers->push_back(specifier);

    binder::Decl *decl = AddImportDecl(isType, local->Name(), local->Start(), binder::DeclarationFlags::IMPORT);
    decl->BindNode(specifier);

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
        lexer_->NextToken();  // eat comma
    } else if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_FROM) {
        ThrowSyntaxError("Unexpected token.");
    }

    return nullptr;
}

ir::StringLiteral *ParserImpl::ParseFromClause(bool requireFrom)
{
    if (lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_FROM) {
        if (requireFrom) {
            ThrowSyntaxError("Unexpected token.");
        }
    } else {
        lexer_->NextToken();  // eat `from` literal
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING) {
        ThrowSyntaxError("Unexpected token.");
    }

    ASSERT(lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING);

    auto *source = AllocNode<ir::StringLiteral>(lexer_->GetToken().String());
    source->SetRange(lexer_->GetToken().Loc());

    lexer_->NextToken();

    return source;
}

ir::AstNode *ParserImpl::ParseImportSpecifiers(ArenaVector<ir::AstNode *> *specifiers, bool isType, bool isLazy)
{
    ASSERT(specifiers->empty());

    // import [default] from 'source'
    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        ir::AstNode *astNode = ParseImportDefaultSpecifier(specifiers, isType);
        if (astNode != nullptr) {
            return astNode;
        }
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_MULTIPLY) {
        ParseNameSpaceImport(specifiers, isType);
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ParseNamedImportSpecifiers(specifiers, isType, isLazy);
    }
    return nullptr;
}

void ParserImpl::VerifySupportLazyImportVersion()
{
    if (!util::Helpers::IsSupportLazyImportVersion(program_.TargetApiVersion(), program_.GetTargetApiSubVersion())) {
        std::string errMessage = "Current configuration does not support using lazy import. Lazy import can be "
            "used in the beta3 version of API 12 or higher versions.\n"
            "Solutions: > Check the compatibleSdkVersion and compatibleSdkVersionStage in build-profile.json5."
            "> If compatibleSdkVersion is set to API 12, then compatibleSdkVersionStage needs to be configured "
            "as beta3."
            "> If you're running es2abc in commandline without IDE, please check whether target-api-version and "
            "target-api-sub-version options are correctly configured.";
        ThrowSyntaxError(errMessage);
    }
}

ir::Statement *ParserImpl::ParseImportDeclaration(StatementParsingFlags flags)
{
    char32_t nextChar = lexer_->Lookahead();
    // dynamic import || import.meta
    if (nextChar == LEX_CHAR_LEFT_PAREN || nextChar == LEX_CHAR_DOT) {
        return ParseExpressionStatement();
    }

    if (Extension() == ScriptExtension::JS) {
        if (!(flags & StatementParsingFlags::GLOBAL)) {
            ThrowSyntaxError("'import' and 'export' may only appear at the top level");
        }

        if (!context_.IsModule()) {
            ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
        }
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat import

    bool isLazy = false;
    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_LAZY) {
        const auto savedPos = lexer_->Save();
        lexer_->NextToken();  // eat lazy
        // only support import lazy {xxx} from '...'
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            VerifySupportLazyImportVersion();
            isLazy = true;
        } else {
            lexer_->Rewind(savedPos);
        }
    }

    bool isType = false;
    if (Extension() == ScriptExtension::TS &&
        lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_TYPE) {
        const auto savedPos = lexer_->Save();
        lexer_->NextToken();  // eat type
        if ((lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
            lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_FROM) ||
            lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE ||
            lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_MULTIPLY) {
            isType = true;
        } else {
            lexer_->Rewind(savedPos);
        }
    }

    ArenaVector<ir::AstNode *> specifiers(Allocator()->Adapter());

    ir::StringLiteral *source = nullptr;

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING) {
        ir::AstNode *astNode = ParseImportSpecifiers(&specifiers, isType, isLazy);
        if (astNode != nullptr) {
            ASSERT(astNode->IsTSImportEqualsDeclaration());
            astNode->SetRange({startLoc, lexer_->GetToken().End()});
            ConsumeSemicolon(astNode->AsTSImportEqualsDeclaration());
            return astNode->AsTSImportEqualsDeclaration();
        }
        if (Extension() == ScriptExtension::TS && !context_.IsModule()) {
            ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
        }
        source = ParseFromClause(true);
        AddImportEntryItem(source, &specifiers, isType, isLazy);
    } else {
        if (Extension() == ScriptExtension::TS && !context_.IsModule()) {
            ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
        }
        // import 'source'
        source = ParseFromClause(false);
        AddImportEntryItem(source, nullptr, isType, isLazy);
    }

    ir::AssertClause *assertClause = nullptr;
    if (Extension() == ScriptExtension::TS && !lexer_->GetToken().NewLine() &&
        lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT &&
        lexer_->GetToken().Ident().Is("assert")) {
        if (isType) {
            ThrowSyntaxError("Import assertions cannot be used with type-only imports or exports.");
        }
        assertClause = ParseAssertClause();
    }

    lexer::SourcePosition endLoc = source->End();
    auto *importDeclaration = AllocNode<ir::ImportDeclaration>(source, std::move(specifiers),
        assertClause, isType, isLazy);
    if (importDeclaration->Specifiers().empty() && program_.IsShared()) {
        ThrowSyntaxError("Arkts-no-side-effects-import is not allowed in shared module.");
    }
    importDeclaration->SetRange({startLoc, endLoc});

    ConsumeSemicolon(importDeclaration);

    return importDeclaration;
}

ir::AssertClause *ParserImpl::ParseAssertClause()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat assert

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("expected left brace.");
    }
    lexer_->NextToken();  // eat '{'

    ArenaVector<ir::AssertEntry *> elements(Allocator()->Adapter());
    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        elements.push_back(ParseAssertEntry());
    }

    lexer_->NextToken();  // eat '}'

    auto *assertClause = AllocNode<ir::AssertClause>(std::move(elements));
    assertClause->SetRange({startLoc, lexer_->GetToken().End()});

    return assertClause;
}

ir::AssertEntry *ParserImpl::ParseAssertEntry()
{
    ir::Expression *key = nullptr;
    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        key = AllocNode<ir::Identifier>(lexer_->GetToken().Ident());
        key->SetRange(lexer_->GetToken().Loc());
    } else if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_STRING) {
        key = AllocNode<ir::StringLiteral>(lexer_->GetToken().String());
        key->SetRange(lexer_->GetToken().Loc());
    } else {
        ThrowSyntaxError("Identifier or string literal expected.");
    }

    lexer_->NextToken();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COLON) {
        ThrowSyntaxError("':' expected.");
    }
    lexer_->NextToken();  // eat :

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING) {
        ThrowSyntaxError("Import assertion values must be string literal expressions.");
    }

    ir::StringLiteral *value = AllocNode<ir::StringLiteral>(lexer_->GetToken().String());
    value->SetRange(lexer_->GetToken().Loc());
    lexer_->NextToken();

    ASSERT(key);
    ASSERT(value);

    auto *assertEntry = AllocNode<ir::AssertEntry>(key, value);
    assertEntry->SetRange({key->Start(), value->End()});

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
        lexer_->NextToken();  // eat comma
    }

    return assertEntry;
}

}  // namespace panda::es2panda::parser
