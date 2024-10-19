/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "parser/parserFlags.h"
#include "parser/parserStatusContext.h"
#include "util/helpers.h"
#include "ir/astNode.h"
#include "ir/base/catchClause.h"
#include "ir/base/classDefinition.h"
#include "ir/base/scriptFunction.h"
#include "ir/expression.h"
#include "ir/expressions/arrayExpression.h"
#include "ir/expressions/binaryExpression.h"
#include "ir/expressions/conditionalExpression.h"
#include "ir/expressions/literals/stringLiteral.h"
#include "ir/expressions/objectExpression.h"
#include "ir/expressions/sequenceExpression.h"
#include "ir/module/exportAllDeclaration.h"
#include "ir/module/exportDefaultDeclaration.h"
#include "ir/module/exportNamedDeclaration.h"
#include "ir/module/exportSpecifier.h"
#include "ir/module/importDeclaration.h"
#include "ir/module/importDefaultSpecifier.h"
#include "ir/module/importNamespaceSpecifier.h"
#include "ir/module/importSpecifier.h"
#include "ir/statements/assertStatement.h"
#include "ir/statements/blockStatement.h"
#include "ir/statements/breakStatement.h"
#include "ir/statements/classDeclaration.h"
#include "ir/statements/continueStatement.h"
#include "ir/statements/debuggerStatement.h"
#include "ir/statements/doWhileStatement.h"
#include "ir/statements/emptyStatement.h"
#include "ir/statements/expressionStatement.h"
#include "ir/statements/forInStatement.h"
#include "ir/statements/forOfStatement.h"
#include "ir/statements/forUpdateStatement.h"
#include "ir/statements/functionDeclaration.h"
#include "ir/statements/ifStatement.h"
#include "ir/statements/labelledStatement.h"
#include "ir/statements/returnStatement.h"
#include "ir/statements/switchCaseStatement.h"
#include "ir/statements/switchStatement.h"
#include "ir/statements/throwStatement.h"
#include "ir/statements/tryStatement.h"
#include "ir/statements/variableDeclaration.h"
#include "ir/statements/variableDeclarator.h"
#include "ir/statements/whileStatement.h"
#include "ir/ets/etsStructDeclaration.h"
#include "lexer/lexer.h"
#include "lexer/token/letters.h"
#include "lexer/token/sourceLocation.h"
#include "util/ustring.h"

#include "parserImpl.h"

namespace ark::es2panda::parser {

using namespace std::literals::string_literals;

// NOLINTNEXTLINE(google-default-arguments)
ir::Statement *ParserImpl::ParseStatement(StatementParsingFlags flags)
{
    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: {
            return ParseBlockStatement();
        }
        case lexer::TokenType::PUNCTUATOR_SEMI_COLON: {
            return ParseEmptyStatement();
        }
        case lexer::TokenType::KEYW_ASSERT: {
            return ParseAssertStatement();
        }
        case lexer::TokenType::KEYW_EXPORT: {
            return ParseExportDeclaration(flags);
        }
        case lexer::TokenType::KEYW_IMPORT: {
            return ParseImportDeclaration(flags);
        }
        case lexer::TokenType::KEYW_FUNCTION: {
            return ParseFunctionStatement(flags);
        }
        case lexer::TokenType::KEYW_CLASS: {
            return ParseClassStatement(flags, ir::ClassDefinitionModifiers::NONE);
        }
        case lexer::TokenType::KEYW_VAR: {
            return ParseVarStatement();
        }
        case lexer::TokenType::KEYW_LET: {
            return ParseLetStatement(flags);
        }
        case lexer::TokenType::KEYW_CONST: {
            return ParseConstStatement(flags);
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
            if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_STRUCT) {
                return ParseStructStatement(flags, ir::ClassDefinitionModifiers::NONE);
            }

            if (lexer_->Lookahead() == lexer::LEX_CHAR_COLON) {
                const auto pos = lexer_->Save();
                lexer_->NextToken();
                return ParseLabelledStatement(pos);
            }

            return ParsePotentialExpressionStatement(flags);
        }
        case lexer::TokenType::KEYW_WITH: {
            ThrowSyntaxError("'With' is deprecated and not supported any more.");
        }
        case lexer::TokenType::KEYW_ENUM: {
            return ParseEnumDeclaration();
        }
        case lexer::TokenType::KEYW_INTERFACE: {
            return ParseInterfaceDeclaration(false);
        }
        case lexer::TokenType::PUNCTUATOR_FORMAT: {
            if (lexer_->Lookahead() == static_cast<char32_t>(STATEMENT_FORMAT_NODE)) {
                return ParseStatementFormatPlaceholder();
            }
            [[fallthrough]];
        }
        default: {
            return ParseExpressionStatement(flags);
        }
    }
}

ir::Statement *ParserImpl::ParseVarStatement()
{
    auto *variableDecl = ParseVariableDeclaration(VariableParsingFlags::VAR);
    ConsumeSemicolon(variableDecl);
    return variableDecl;
}

ir::Statement *ParserImpl::ParseLetStatement(StatementParsingFlags flags)
{
    if ((flags & StatementParsingFlags::ALLOW_LEXICAL) == 0) {
        ThrowSyntaxError("Lexical declaration is not allowed in single statement context");
    }

    auto *variableDecl = ParseVariableDeclaration(VariableParsingFlags::LET);
    ConsumeSemicolon(variableDecl);
    return variableDecl;
}

ir::Statement *ParserImpl::ParseConstStatement(StatementParsingFlags flags)
{
    if ((flags & StatementParsingFlags::ALLOW_LEXICAL) == 0) {
        ThrowSyntaxError("Lexical declaration is not allowed in single statement context");
    }

    lexer::SourcePosition constVarStar = lexer_->GetToken().Start();
    lexer_->NextToken();

    auto *variableDecl = ParseVariableDeclaration(VariableParsingFlags::CONST | VariableParsingFlags::NO_SKIP_VAR_KIND);
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

ir::Statement *ParserImpl::ParseFunctionStatement(StatementParsingFlags flags)
{
    CheckFunctionDeclaration(flags);

    if ((flags & StatementParsingFlags::STMT_LEXICAL_SCOPE_NEEDED) == 0) {
        return ParseFunctionDeclaration(false, ParserStatus::NO_OPTS);
    }

    auto *funcDecl = ParseFunctionDeclaration(false, ParserStatus::NO_OPTS);
    if (funcDecl == nullptr) {  // Error processing.
        return nullptr;
    }

    ArenaVector<ir::Statement *> stmts(Allocator()->Adapter());
    stmts.push_back(funcDecl);

    auto *localBlockStmt = AllocNode<ir::BlockStatement>(Allocator(), std::move(stmts));
    localBlockStmt->SetRange(funcDecl->Range());

    return funcDecl;
}

ir::Statement *ParserImpl::ParsePotentialExpressionStatement(StatementParsingFlags flags)
{
    return ParseExpressionStatement(flags);
}

// NOLINTNEXTLINE(google-default-arguments)
ir::ETSStructDeclaration *ParserImpl::ParseStructStatement([[maybe_unused]] StatementParsingFlags flags,
                                                           [[maybe_unused]] ir::ClassDefinitionModifiers modifiers,
                                                           [[maybe_unused]] ir::ModifierFlags modFlags)
{
    ThrowSyntaxError("Illegal start of expression", Lexer()->GetToken().Start());
}

// NOLINTNEXTLINE(google-default-arguments)
ir::ClassDeclaration *ParserImpl::ParseClassStatement(StatementParsingFlags flags,
                                                      ir::ClassDefinitionModifiers modifiers,
                                                      ir::ModifierFlags modFlags)
{
    if ((flags & StatementParsingFlags::ALLOW_LEXICAL) == 0) {
        ThrowSyntaxError("Lexical declaration is not allowed in single statement context");
    }

    return ParseClassDeclaration(modifiers, modFlags);
}

ir::ETSStructDeclaration *ParserImpl::ParseStructDeclaration(ir::ClassDefinitionModifiers modifiers,
                                                             ir::ModifierFlags flags)
{
    const lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    modifiers |= ir::ClassDefinitionModifiers::DECLARATION;
    if (IsExternal()) {
        modifiers |= ir::ClassDefinitionModifiers::FROM_EXTERNAL;
    }

    if ((flags & ir::ModifierFlags::ABSTRACT) != 0U) {
        ThrowSyntaxError("struct declaration is not allowed to use 'abstract' modifiers.");
    }

    ir::ClassDefinition *classDefinition = ParseClassDefinition(modifiers, flags);

    if ((classDefinition->Modifiers() & ir::ClassDefinitionModifiers::HAS_SUPER) != 0U) {
        ThrowSyntaxError("struct declaration cannot extends form other class");
    }

    lexer::SourcePosition endLoc = classDefinition->End();
    auto *structDecl = AllocNode<ir::ETSStructDeclaration>(classDefinition, Allocator());
    structDecl->SetRange({startLoc, endLoc});
    return structDecl;
}

ir::ClassDeclaration *ParserImpl::ParseClassDeclaration(ir::ClassDefinitionModifiers modifiers, ir::ModifierFlags flags)
{
    const lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    modifiers |= ir::ClassDefinitionModifiers::DECLARATION;
    if (IsExternal()) {
        modifiers |= ir::ClassDefinitionModifiers::FROM_EXTERNAL;
    }

    ir::ClassDefinition *classDefinition = ParseClassDefinition(modifiers, flags);

    lexer::SourcePosition endLoc = classDefinition->End();
    auto *classDecl = AllocNode<ir::ClassDeclaration>(classDefinition, Allocator());
    classDecl->SetRange({startLoc, endLoc});
    return classDecl;
}

void ParserImpl::CheckFunctionDeclaration(StatementParsingFlags flags)
{
    if ((flags & StatementParsingFlags::LABELLED) != 0) {
        ThrowSyntaxError(
            "In strict mode code, functions can only be "
            "declared at top level, inside a block, "
            "or "
            "as the body of an if statement");
    }

    if ((flags & StatementParsingFlags::ALLOW_LEXICAL) == 0) {
        if ((flags & (StatementParsingFlags::IF_ELSE | StatementParsingFlags::LABELLED)) == 0) {
            ThrowSyntaxError("Lexical declaration is not allowed in single statement context");
        }

        if (lexer_->Lookahead() == lexer::LEX_CHAR_ASTERISK) {
            ThrowSyntaxError("Generators can only be declared at the top level or inside a block");
        }
    }
}

void ParserImpl::ConsumeSemicolon(ir::Statement *statement)
{
    auto tokenType = lexer_->GetToken().Type();
    if (tokenType == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        statement->SetEnd(lexer_->GetToken().End());
        lexer_->NextToken();
        return;
    }

    if (!lexer_->GetToken().NewLine() && tokenType != lexer::TokenType::EOS &&
        tokenType != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        LogSyntaxError({"Unexpected token '", TokenToString(tokenType), "'."});
    }
}

ArenaVector<ir::Statement *> ParserImpl::ParseStatementList(StatementParsingFlags flags)
{
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT &&
        lexer_->Lookahead() == static_cast<char32_t>(ARRAY_FORMAT_NODE)) {
        return std::move(ParseStatementsArrayFormatPlaceholder());
    }

    ArenaVector<ir::Statement *> statements(Allocator()->Adapter());
    ParseDirectivePrologue(&statements);

    auto endType =
        (flags & StatementParsingFlags::GLOBAL) != 0 ? lexer::TokenType::EOS : lexer::TokenType::PUNCTUATOR_RIGHT_BRACE;

    while (lexer_->GetToken().Type() != endType) {
        if (auto statement = ParseStatement(flags); statement != nullptr) {  // Error processing.
            statements.push_back(statement);
        }
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

ir::Statement *ParserImpl::ParseAssertStatement()
{
    return nullptr;
}

void ParserImpl::ValidateLabeledStatement([[maybe_unused]] lexer::TokenType type) {}

ir::BlockStatement *ParserImpl::ParseBlockStatement()
{
    ASSERT(lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE);

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();
    auto statements = ParseStatementList();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ThrowSyntaxError("Expected a '}'");
    }

    auto *blockNode = AllocNode<ir::BlockStatement>(Allocator(), std::move(statements));
    blockNode->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    return blockNode;
}

void ParserImpl::ThrowPossibleOutOfBoundaryJumpError([[maybe_unused]] bool allowBreak) {}

void ParserImpl::ThrowIllegalBreakError()
{
    ThrowSyntaxError("Illegal break statement");
}

ir::BreakStatement *ParserImpl::ParseBreakStatement()
{
    bool allowBreak = (context_.Status() & (ParserStatus::IN_ITERATION | ParserStatus::IN_SWITCH)) != 0;

    ThrowPossibleOutOfBoundaryJumpError(allowBreak);

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON ||
        lexer_->GetToken().Type() == lexer::TokenType::EOS || lexer_->GetToken().NewLine() ||
        lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        if (!allowBreak) {
            ThrowIllegalBreakError();
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
    if (!ValidateBreakLabel(label)) {
        ThrowSyntaxError("Undefined label");
    }

    auto *identNode = AllocNode<ir::Identifier>(label, Allocator());
    identNode->SetReference();
    identNode->SetRange(lexer_->GetToken().Loc());

    auto *breakStatement = AllocNode<ir::BreakStatement>(identNode);
    breakStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    ConsumeSemicolon(breakStatement);

    return breakStatement;
}

void ParserImpl::ThrowIllegalContinueError()
{
    ThrowSyntaxError("Illegal continue statement");
}

ir::ContinueStatement *ParserImpl::ParseContinueStatement()
{
    ThrowPossibleOutOfBoundaryJumpError((context_.Status() & (ParserStatus::IN_ITERATION | ParserStatus::IN_SWITCH)) !=
                                        0U);

    if ((context_.Status() & ParserStatus::IN_ITERATION) == 0) {
        ThrowIllegalContinueError();
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
    if (!ValidateContinueLabel(label)) {
        ThrowSyntaxError("Undefined label");
    }

    auto *identNode = AllocNode<ir::Identifier>(label, Allocator());
    identNode->SetReference();
    identNode->SetRange(lexer_->GetToken().Loc());

    auto *continueStatement = AllocNode<ir::ContinueStatement>(identNode);
    continueStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();
    ConsumeSemicolon(continueStatement);

    return continueStatement;
}

ir::DoWhileStatement *ParserImpl::ParseDoWhileStatement()
{
    IterationContext iterCtx(&context_);

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

    ir::Expression *test = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError("Missing right parenthesis in a 'DoWhileStatement'");
    }

    auto *doWhileStatement = AllocNode<ir::DoWhileStatement>(body, test);
    doWhileStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        doWhileStatement->SetEnd(lexer_->GetToken().End());
        lexer_->NextToken();
    }

    return doWhileStatement;
}

ir::FunctionDeclaration *ParserImpl::ParseFunctionDeclaration(bool canBeAnonymous, ParserStatus newStatus)
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

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        if (canBeAnonymous) {
            ir::ScriptFunction *func = ParseFunction(newStatus | ParserStatus::NEED_RETURN_TYPE);
            func->SetStart(startLoc);

            auto *funcDecl = AllocNode<ir::FunctionDeclaration>(Allocator(), func, true);
            funcDecl->SetRange(func->Range());
            return funcDecl;
        }

        ThrowSyntaxError("Unexpected token, expected identifier after 'function' keyword");
    }

    CheckRestrictedBinding();
    auto *identNode = ExpectIdentifier();

    newStatus |= ParserStatus::FUNCTION_DECLARATION;
    ir::ScriptFunction *func = ParseFunction(newStatus | ParserStatus::NEED_RETURN_TYPE);

    func->SetIdent(identNode);
    func->SetStart(startLoc);
    auto *funcDecl = AllocNode<ir::FunctionDeclaration>(Allocator(), func);
    funcDecl->SetRange(func->Range());

    if (func->IsOverload() && lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        lexer_->NextToken();
    }

    return funcDecl;
}

ir::Statement *ParserImpl::ParseExpressionStatement(StatementParsingFlags flags)
{
    const auto startPos = lexer_->Save();
    ParserStatus savedStatus = context_.Status();

    auto tokenType = lexer_->GetToken().Type();
    if (tokenType == lexer::TokenType::KEYW_PUBLIC || tokenType == lexer::TokenType::KEYW_PRIVATE ||
        tokenType == lexer::TokenType::KEYW_PROTECTED) {
        lexer_->NextToken();
        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_CLASS ||
            lexer_->GetToken().Type() == lexer::TokenType::KEYW_INTERFACE) {
            ThrowSyntaxError("A local class or interface declaration can not have access modifier",
                             startPos.GetToken().Start());
        }
        lexer_->Rewind(startPos);
    }

    if (lexer_->GetToken().IsAsyncModifier()) {
        lexer_->NextToken();

        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_FUNCTION && !lexer_->GetToken().NewLine()) {
            if ((flags & StatementParsingFlags::ALLOW_LEXICAL) == 0) {
                ThrowSyntaxError("Lexical declaration is not allowed in single statement context");
            }

            ir::FunctionDeclaration *functionDecl = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION);
            functionDecl->SetStart(startPos.GetToken().Start());

            return functionDecl;
        }

        lexer_->Rewind(startPos);
    }

    ir::Expression *exprNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    context_.Status() = savedStatus;
    lexer::SourcePosition endPos = exprNode->End();

    auto *exprStatementNode = AllocNode<ir::ExpressionStatement>(exprNode);
    exprStatementNode->SetRange({startPos.GetToken().Start(), endPos});
    ConsumeSemicolon(exprStatementNode);

    return exprStatementNode;
}

// NOLINTBEGIN(cert-err58-cpp)
static std::string const INVALID_LEFT_HAND_IN_FOR_OF =
    "Invalid left-hand side in 'for' statement: must have a single binding."s;
static std::string const UNEXPECTED_TOKEN = "Unexpected token"s;
static std::string const MISSING_LEFT_IN_FOR = "Missing left parenthesis in 'for' statement."s;
static std::string const MISSING_RIGHT_IN_FOR = "Missing right parenthesis in 'for' statement."s;
static std::string const INVALID_TYPE_ANNOTATION_IN_FOR =
    "Type annotation is not allowed when existing variable is used as loop iterator in 'for' statement."s;
// NOLINTEND(cert-err58-cpp)

std::tuple<ForStatementKind, ir::Expression *, ir::Expression *> ParserImpl::ParseForInOf(
    ir::AstNode *initNode, ExpressionParseFlags exprFlags, bool isAwait)
{
    ForStatementKind forKind = ForStatementKind::UPDATE;
    ir::Expression *updateNode = nullptr;
    ir::Expression *rightNode = nullptr;

    if (lexer_->GetToken().IsForInOf()) {
        const ir::VariableDeclarator *varDecl = initNode->AsVariableDeclaration()->Declarators().front();

        if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN) {
            if (varDecl->Init() != nullptr) {
                ThrowSyntaxError("for-in loop variable declaration may not have an initializer");
            }
            forKind = ForStatementKind::IN;
            exprFlags = ExpressionParseFlags::ACCEPT_COMMA;
            ValidateForInStatement();
        } else {
            if (varDecl->Init() != nullptr) {
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
            rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
            if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
                ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
            }
            lexer_->NextToken();
        }

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
            updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
        }
    }

    return {forKind, rightNode, updateNode};
}

std::tuple<ForStatementKind, ir::AstNode *, ir::Expression *, ir::Expression *> ParserImpl::ParseIsForInOf(
    ir::Expression *leftNode, ExpressionParseFlags exprFlags)
{
    ASSERT(lexer_->GetToken().IsForInOf());
    ForStatementKind forKind = ForStatementKind::OF;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_IN) {
        forKind = ForStatementKind::IN;
        exprFlags = ExpressionParseFlags::ACCEPT_COMMA;
        ValidateForInStatement();
    }

    bool isValid = true;
    switch (leftNode->Type()) {
        case ir::AstNodeType::IDENTIFIER:
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

    lexer_->NextToken();

    return {forKind, leftNode, ParseExpression(exprFlags), nullptr};
}

std::tuple<ForStatementKind, ir::AstNode *, ir::Expression *, ir::Expression *> ParserImpl::ParseForInOf(
    ir::Expression *leftNode, ExpressionParseFlags exprFlags, bool isAwait)
{
    ir::Expression *updateNode = nullptr;
    ir::Expression *rightNode = nullptr;
    if (lexer_->GetToken().IsForInOf()) {
        return ParseIsForInOf(leftNode, exprFlags);
    }

    if (isAwait) {
        ThrowSyntaxError("Unexpected token");
    }

    ir::Expression *expr = ParseAssignmentExpression(leftNode);

    ir::AstNode *initNode =
        lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA ? ParseSequenceExpression(expr) : expr;

    if (initNode->IsConditionalExpression()) {
        ir::ConditionalExpression *condExpr = initNode->AsConditionalExpression();
        if (condExpr->Alternate()->IsBinaryExpression() &&
            condExpr->Alternate()->AsBinaryExpression()->OperatorType() == lexer::TokenType::KEYW_IN) {
            ThrowSyntaxError("Invalid left-hand side in for-in statement");
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
        rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
        }
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
    }

    return {ForStatementKind::UPDATE, initNode, rightNode, updateNode};
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
        rightNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            ThrowSyntaxError("Unexpected token, expected ';' in 'ForStatement'.");
        }
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        updateNode = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA | ExpressionParseFlags::IN_FOR);
    }

    return {rightNode, updateNode};
}

std::tuple<ir::Expression *, ir::AstNode *> ParserImpl::ParseForLoopInitializer()
{
    VariableParsingFlags varFlags = VariableParsingFlags::IN_FOR;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT) {
        varFlags |= VariableParsingFlags::DISALLOW_INIT;
        lexer_->NextToken();
    }

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        ThrowSyntaxError(MISSING_LEFT_IN_FOR, lexer_->GetToken().Start());
    }
    lexer_->NextToken();

    lexer::TokenType tokenType;
    auto const currentPosition = lexer_->Save();
    do {
        tokenType = lexer_->GetToken().Type();
        if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_OF) {
            varFlags |= VariableParsingFlags::FOR_OF;
            break;
        }
        if (tokenType == lexer::TokenType::KEYW_IN) {
            varFlags |= VariableParsingFlags::STOP_AT_IN;
            break;
        }
        lexer_->NextToken();
    } while (tokenType != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS &&
             tokenType != lexer::TokenType::PUNCTUATOR_LEFT_BRACE &&
             tokenType != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS && tokenType != lexer::TokenType::EOS);
    lexer_->Rewind(currentPosition);

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_VAR: {
            return {nullptr, ParseVariableDeclaration(varFlags | VariableParsingFlags::VAR)};
        }
        case lexer::TokenType::KEYW_LET: {
            return {nullptr, ParseVariableDeclaration(varFlags | VariableParsingFlags::LET)};
        }
        case lexer::TokenType::KEYW_CONST: {
            return {nullptr, ParseVariableDeclaration(varFlags | VariableParsingFlags::CONST |
                                                      VariableParsingFlags::ACCEPT_CONST_NO_INIT)};
        }
        case lexer::TokenType::PUNCTUATOR_SEMI_COLON: {
            if ((varFlags & VariableParsingFlags::DISALLOW_INIT) != 0 /*isAsync*/) {
                ThrowSyntaxError(UNEXPECTED_TOKEN, lexer_->GetToken().Start());
            }

            lexer_->NextToken();
            return {nullptr, nullptr};
        }
        default: {
            return {ParseUnaryOrPrefixUpdateExpression(ExpressionParseFlags::POTENTIALLY_IN_PATTERN), nullptr};
        }
    }
}

ir::Statement *ParserImpl::ParseForStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    ForStatementKind forKind = ForStatementKind::UPDATE;
    ir::AstNode *initNode = nullptr;
    ir::Expression *updateNode = nullptr;
    ir::Expression *leftNode = nullptr;
    ir::Expression *rightNode = nullptr;

    lexer_->NextToken();
    bool isAwait = lexer_->GetToken().Type() == lexer::TokenType::KEYW_AWAIT;
    std::tie(leftNode, initNode) = ParseForLoopInitializer();
    bool canBeForInOf = (leftNode != nullptr) || (initNode != nullptr);

    IterationContext iterCtx(&context_);

    if (initNode != nullptr) {
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            lexer_->NextToken();
            canBeForInOf = false;
        } else if (initNode->AsVariableDeclaration()->Declarators().size() > 1 && lexer_->GetToken().IsForInOf()) {
            ThrowSyntaxError(INVALID_LEFT_HAND_IN_FOR_OF, initNode->AsVariableDeclaration()->Declarators()[1]->Start());
        }
    }

    // VariableDeclaration->DeclarationSize > 1 or seen semi_colon
    if (!canBeForInOf) {
        std::tie(rightNode, updateNode) = ParseForUpdate(isAwait);
    } else if (leftNode != nullptr) {
        // initNode was parsed as LHS
        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
            ThrowSyntaxError(INVALID_TYPE_ANNOTATION_IN_FOR, lexer_->GetToken().Start());
        }
        std::tie(forKind, initNode, rightNode, updateNode) =
            ParseForInOf(leftNode, ExpressionParseFlags::NO_OPTS, isAwait);
    } else if (initNode != nullptr) {
        // initNode was parsed as VariableDeclaration and declaration size = 1
        std::tie(forKind, rightNode, updateNode) = ParseForInOf(initNode, ExpressionParseFlags::NO_OPTS, isAwait);
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK) {
        lexer_->NextToken();
    }
    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_PARENTHESIS) {
        ThrowSyntaxError(MISSING_RIGHT_IN_FOR, lexer_->GetToken().Start());
    }
    lexer_->NextToken();

    ir::Statement *bodyNode = ParseStatement();
    ir::Statement *forStatement = nullptr;

    if (forKind == ForStatementKind::UPDATE) {
        forStatement = AllocNode<ir::ForUpdateStatement>(initNode, rightNode, updateNode, bodyNode);
    } else if (forKind == ForStatementKind::IN) {
        forStatement = AllocNode<ir::ForInStatement>(initNode, rightNode, bodyNode);
    } else {
        forStatement = AllocNode<ir::ForOfStatement>(initNode, rightNode, bodyNode, isAwait);
    }

    forStatement->SetRange({startLoc, bodyNode->End()});

    return forStatement;
}

void ParserImpl::ThrowIfBodyEmptyError([[maybe_unused]] ir::Statement *consequent) {}

ir::IfStatement *ParserImpl::ParseIfStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer::SourcePosition endLoc;
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
    ir::Statement *consequent = ParseStatement(StatementParsingFlags::IF_ELSE | StatementParsingFlags::ALLOW_LEXICAL);

    ThrowIfBodyEmptyError(consequent);

    endLoc = consequent->End();
    ir::Statement *alternate = nullptr;

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_ELSE) {
        lexer_->NextToken();  // eat ELSE keyword
        alternate = ParseStatement(StatementParsingFlags::IF_ELSE | StatementParsingFlags::ALLOW_LEXICAL);
        endLoc = alternate->End();
    }

    auto *ifStatement = AllocNode<ir::IfStatement>(test, consequent, alternate);
    ifStatement->SetRange({startLoc, endLoc});
    return ifStatement;
}

ir::LabelledStatement *ParserImpl::ParseLabelledStatement(const lexer::LexerPosition &pos)
{
    const util::StringView &actualLabel = pos.GetToken().Ident();

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_AWAIT && context_.IsModule()) {
        ThrowSyntaxError("'await' is a reserved identifier in module code", pos.GetToken().Start());
    }

    if (context_.FindLabel(actualLabel) != nullptr) {
        ThrowSyntaxError("Label already declared", pos.GetToken().Start());
    }

    SavedParserContext newCtx(this, ParserStatus::IN_LABELED, actualLabel);

    auto *identNode = AllocNode<ir::Identifier>(actualLabel, Allocator());
    identNode->SetReference();
    identNode->SetRange(pos.GetToken().Loc());

    lexer_->NextToken();

    ValidateLabeledStatement(Lexer()->GetToken().Type());

    ir::Statement *body = ParseStatement(StatementParsingFlags::LABELLED);

    auto *labeledStatement = AllocNode<ir::LabelledStatement>(identNode, body);
    labeledStatement->SetRange({pos.GetToken().Start(), body->End()});

    return labeledStatement;
}

ir::ReturnStatement *ParserImpl::ParseReturnStatement()
{
    if ((context_.Status() & ParserStatus::FUNCTION) == 0) {
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

    context_.Status() |= ParserStatus::FUNCTION_HAS_RETURN_STATEMENT;
    return returnStatement;
}

void ParserImpl::ThrowMultipleDefaultError()
{
    ThrowSyntaxError("Multiple default clauses.");
}

ir::SwitchCaseStatement *ParserImpl::ParseSwitchCaseStatement(bool *seenDefault)
{
    lexer::SourcePosition caseStartLoc = lexer_->GetToken().Start();
    ir::Expression *testExpr = nullptr;

    switch (lexer_->GetToken().KeywordType()) {
        case lexer::TokenType::KEYW_CASE: {
            lexer_->NextToken();
            testExpr = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
            break;
        }
        case lexer::TokenType::KEYW_DEFAULT: {
            if (*seenDefault) {
                ThrowMultipleDefaultError();
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
           lexer_->GetToken().KeywordType() != lexer::TokenType::KEYW_DEFAULT &&
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
    ArenaVector<ir::SwitchCaseStatement *> cases(Allocator()->Adapter());

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        cases.push_back(ParseSwitchCaseStatement(&seenDefault));
    }

    auto *switchStatement = AllocNode<ir::SwitchStatement>(discriminant, std::move(cases));
    switchStatement->SetRange({startLoc, lexer_->GetToken().End()});

    lexer_->NextToken();

    return switchStatement;
}

void ParserImpl::ThrowIllegalNewLineErrorAfterThrow()
{
    ThrowSyntaxError("Illegal newline after throw");
}

ir::ThrowStatement *ParserImpl::ParseThrowStatement()
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();

    if (lexer_->GetToken().NewLine()) {
        ThrowIllegalNewLineErrorAfterThrow();
    }

    ir::Expression *expression = ParseExpression(ExpressionParseFlags::ACCEPT_COMMA);
    lexer::SourcePosition endLoc = expression->End();

    auto *throwStatement = AllocNode<ir::ThrowStatement>(expression);
    throwStatement->SetRange({startLoc, endLoc});
    ConsumeSemicolon(throwStatement);

    return throwStatement;
}

void ParserImpl::ParseCatchParamTypeAnnotation([[maybe_unused]] ir::AnnotatedExpression *param) {}

ir::Expression *ParserImpl::ParseCatchParam()
{
    ir::AnnotatedExpression *param = nullptr;

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        return param;
    }

    lexer_->NextToken();  // eat left paren

    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        CheckRestrictedBinding();
        param = ExpectIdentifier();
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET) {
        param = ParseArrayExpression(ExpressionParseFlags::MUST_BE_PATTERN);
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        param = ParseObjectExpression(ExpressionParseFlags::MUST_BE_PATTERN);
    } else {
        ThrowSyntaxError("Unexpected token in catch parameter");
    }

    ParseCatchParamTypeAnnotation(param);

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

    ir::Expression *param = ParseCatchParam();

    if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("Unexpected token, expected '{'");
    }

    ir::BlockStatement *catchBlock = ParseBlockStatement();
    lexer::SourcePosition endLoc = catchBlock->End();

    auto *catchClause = AllocNode<ir::CatchClause>(param, catchBlock);
    catchClause->SetRange({catchStartLoc, endLoc});

    return catchClause;
}

ir::Statement *ParserImpl::ParseTryStatement()
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
    ir::BlockStatement *finallyClause = nullptr;
    ArenaVector<ir::CatchClause *> catchClauses(Allocator()->Adapter());

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_CATCH) {
        catchClause = ParseCatchClause();
        endLoc = catchClause->End();
        catchClauses.push_back(catchClause);
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_FINALLY) {
        lexer_->NextToken();  // eat 'finally' keyword

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            ThrowSyntaxError("Unexpected token, expected '{'");
        }

        finallyClause = ParseBlockStatement();
        endLoc = finallyClause->End();
    }

    ArenaVector<std::pair<compiler::LabelPair, const ir::Statement *>> finalizerInsertions(Allocator()->Adapter());

    auto *tryStatement = AllocNode<ir::TryStatement>(body, std::move(catchClauses), finallyClause, finalizerInsertions);
    tryStatement->SetRange({startLoc, endLoc});
    return tryStatement;
}

void ParserImpl::ValidateDeclaratorId()
{
    if (InAmbientContext()) {
        return;
    }

    CheckRestrictedBinding();
}

ir::VariableDeclarator *ParserImpl::ParseVariableDeclaratorInitializer(ir::Expression *init, VariableParsingFlags flags,
                                                                       const lexer::SourcePosition &startLoc)
{
    if ((flags & VariableParsingFlags::DISALLOW_INIT) != 0) {
        ThrowSyntaxError("for-await-of loop variable declaration may not have an initializer");
    }

    lexer_->NextToken();

    if (InAmbientContext() && (flags & VariableParsingFlags::CONST) == 0) {
        ThrowSyntaxError("Initializers are not allowed in ambient contexts.");
    }

    auto exprFlags = ((flags & VariableParsingFlags::STOP_AT_IN) != 0 ? ExpressionParseFlags::STOP_AT_IN
                                                                      : ExpressionParseFlags::NO_OPTS);

    ir::Expression *initializer = ParseExpression(exprFlags);
    lexer::SourcePosition endLoc = initializer->End();

    auto *declarator = AllocNode<ir::VariableDeclarator>(GetFlag(flags), init, initializer);
    declarator->SetRange({startLoc, endLoc});

    return declarator;
}

ir::AnnotatedExpression *ParserImpl::ParseVariableDeclaratorKey([[maybe_unused]] VariableParsingFlags flags)
{
    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::LITERAL_IDENT: {
            ValidateDeclaratorId();
            return ExpectIdentifier(true);
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET: {
            return ParseArrayExpression(ExpressionParseFlags::MUST_BE_PATTERN);
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: {
            return ParseObjectExpression(ExpressionParseFlags::MUST_BE_PATTERN);
        }
        default: {
            break;
        }
    }

    ThrowSyntaxError("Unexpected token in variable declaration");
    return nullptr;
}

ir::VariableDeclaratorFlag ParserImpl::GetFlag(VariableParsingFlags flags)
{
    constexpr auto VARIABLE_FLAGS =
        (VariableParsingFlags::VAR | VariableParsingFlags::LET | VariableParsingFlags::CONST);
    switch (static_cast<VariableParsingFlags>(flags & VARIABLE_FLAGS)) {
        case VariableParsingFlags::CONST:
            return ir::VariableDeclaratorFlag::CONST;
        case VariableParsingFlags::VAR:
            return ir::VariableDeclaratorFlag::VAR;
        case VariableParsingFlags::LET:
            return ir::VariableDeclaratorFlag::LET;
        default:
            UNREACHABLE();
    }
}

ir::VariableDeclarator *ParserImpl::ParseVariableDeclarator(ir::Expression *init, lexer::SourcePosition startLoc,
                                                            VariableParsingFlags flags)
{
    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        return ParseVariableDeclaratorInitializer(init, flags, startLoc);
    }

    if ((flags & VariableParsingFlags::CONST) != 0U && (flags & VariableParsingFlags::ACCEPT_CONST_NO_INIT) == 0U) {
        ThrowSyntaxError("Missing initializer in const declaration");
    }

    if ((flags & VariableParsingFlags::IN_FOR) == 0U && (init->IsArrayPattern() || init->IsObjectPattern())) {
        ThrowSyntaxError("Missing initializer in destructuring declaration");
    }

    lexer::SourcePosition endLoc = init->End();
    auto declarator = AllocNode<ir::VariableDeclarator>(GetFlag(flags), init);
    declarator->SetRange({startLoc, endLoc});

    return declarator;
}

ir::VariableDeclarator *ParserImpl::ParseVariableDeclarator(VariableParsingFlags flags)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    ir::Expression *init = ParseVariableDeclaratorKey(flags);
    ir::VariableDeclarator *declarator = ParseVariableDeclarator(init, startLoc, flags);

    return declarator;
}

ir::Statement *ParserImpl::ParsePotentialConstEnum([[maybe_unused]] VariableParsingFlags flags)
{
    ThrowSyntaxError("Variable declaration expected.");
}

void ParserImpl::ThrowIfVarDeclaration([[maybe_unused]] VariableParsingFlags flags) {}

ir::Statement *ParserImpl::ParseVariableDeclaration(VariableParsingFlags flags)
{
    lexer::SourcePosition startLoc = lexer_->GetToken().Start();

    if ((flags & VariableParsingFlags::NO_SKIP_VAR_KIND) == 0) {
        lexer_->NextToken();
    }

    ThrowIfVarDeclaration(flags);

    if (lexer_->GetToken().Type() == lexer::TokenType::KEYW_ENUM) {
        return ParsePotentialConstEnum(flags);
    }

    ArenaVector<ir::VariableDeclarator *> declarators(Allocator()->Adapter());

    while (true) {
        ir::VariableDeclarator *declarator = ParseVariableDeclarator(flags);

        declarators.push_back(declarator);

        if (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA) {
            break;
        }
        lexer_->NextToken();
    }

    auto varKind = ir::VariableDeclaration::VariableDeclarationKind::VAR;

    if ((flags & VariableParsingFlags::LET) != 0) {
        varKind = ir::VariableDeclaration::VariableDeclarationKind::LET;
    } else if ((flags & VariableParsingFlags::CONST) != 0) {
        varKind = ir::VariableDeclaration::VariableDeclarationKind::CONST;
    }

    lexer::SourcePosition endLoc = declarators.back()->End();
    auto *declaration =
        AllocNode<ir::VariableDeclaration>(varKind, Allocator(), std::move(declarators), InAmbientContext());
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
    IterationContext iterCtx(&context_);
    ir::Statement *body = ParseStatement();

    lexer::SourcePosition endLoc = body->End();
    auto *whileStatement = AllocNode<ir::WhileStatement>(test, body);
    whileStatement->SetRange({startLoc, endLoc});

    return whileStatement;
}

// NOLINTNEXTLINE(google-default-arguments)
ir::ExportDefaultDeclaration *ParserImpl::ParseExportDefaultDeclaration(const lexer::SourcePosition &startLoc,
                                                                        bool isExportEquals)
{
    lexer_->NextToken();  // eat `default` keyword or `=`

    ir::AstNode *declNode = nullptr;
    bool eatSemicolon = false;

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_FUNCTION: {
            declNode = ParseFunctionDeclaration(true);
            break;
        }
        case lexer::TokenType::KEYW_CLASS: {
            declNode = ParseClassDeclaration(ir::ClassDefinitionModifiers::NONE);
            break;
        }
        case lexer::TokenType::LITERAL_IDENT: {
            switch (lexer_->GetToken().KeywordType()) {
                case lexer::TokenType::KEYW_STRUCT: {
                    declNode = ParseStructDeclaration(ir::ClassDefinitionModifiers::NONE);
                    break;
                }
                case lexer::TokenType::KEYW_ASYNC: {
                    if ((lexer_->GetToken().Flags() & lexer::TokenFlags::HAS_ESCAPE) == 0) {
                        lexer_->NextToken();  // eat `async`
                        declNode = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION);
                        break;
                    }

                    [[fallthrough]];
                }
                default: {
                    declNode = ParseExpression();
                    eatSemicolon = true;
                    break;
                }
            }

            break;
        }
        default: {
            declNode = ParseExpression();
            eatSemicolon = true;
            break;
        }
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

    CheckRestrictedBinding(exportedToken.KeywordType());

    const util::StringView &exportedString = exportedToken.Ident();

    auto *exported = AllocNode<ir::Identifier>(exportedString, Allocator());
    exported->SetRange(exportedToken.Loc());

    return exported;
}

ir::ExportAllDeclaration *ParserImpl::ParseExportAllDeclaration(const lexer::SourcePosition &startLoc)
{
    lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat `*` character

    ir::Identifier *exported = nullptr;

    if (CheckModuleAsModifier()) {
        lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);
        exported = ParseNamedExport(lexer_->GetToken());
        lexer_->NextToken();  // eat exported name
    }

    ir::StringLiteral *source = ParseFromClause();
    lexer::SourcePosition endLoc = source->End();

    auto *exportDeclaration = AllocNode<ir::ExportAllDeclaration>(source, exported);
    exportDeclaration->SetRange({startLoc, endLoc});

    ConsumeSemicolon(exportDeclaration);

    return exportDeclaration;
}

ir::ExportNamedDeclaration *ParserImpl::ParseExportNamedSpecifiers(const lexer::SourcePosition &startLoc)
{
    lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat `{` character

    ArenaVector<ir::ExportSpecifier *> specifiers(Allocator()->Adapter());

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            ThrowSyntaxError("Unexpected token");
        }

        lexer::Token localToken = lexer_->GetToken();
        auto *local = AllocNode<ir::Identifier>(lexer_->GetToken().Ident(), Allocator());
        local->SetRange(lexer_->GetToken().Loc());

        lexer_->NextToken();  // eat local name

        ir::Identifier *exported = nullptr;

        if (CheckModuleAsModifier()) {
            lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat `as` literal
            exported = ParseNamedExport(lexer_->GetToken());
            lexer_->NextToken();  // eat exported name
        } else {
            exported = ParseNamedExport(localToken);
        }

        auto *specifier = AllocNode<ir::ExportSpecifier>(local, exported);
        specifier->SetRange({local->Start(), exported->End()});

        specifiers.push_back(specifier);

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat comma
        }
    }

    lexer::SourcePosition endPos = lexer_->GetToken().End();
    lexer_->NextToken();  // eat right brace

    ir::StringLiteral *source = nullptr;

    if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_FROM) {
        source = ParseFromClause();
    }

    auto *exportDeclaration = AllocNode<ir::ExportNamedDeclaration>(Allocator(), source, std::move(specifiers));
    exportDeclaration->SetRange({startLoc, endPos});
    ConsumeSemicolon(exportDeclaration);

    return exportDeclaration;
}

ir::ExportNamedDeclaration *ParserImpl::ParseNamedExportDeclaration(const lexer::SourcePosition &startLoc)
{
    ir::Statement *decl = nullptr;

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::KEYW_VAR: {
            decl = ParseVariableDeclaration(VariableParsingFlags::VAR);
            break;
        }
        case lexer::TokenType::KEYW_CONST: {
            decl = ParseVariableDeclaration(VariableParsingFlags::CONST);
            break;
        }
        case lexer::TokenType::KEYW_LET: {
            decl = ParseVariableDeclaration(VariableParsingFlags::LET);
            break;
        }
        case lexer::TokenType::KEYW_FUNCTION: {
            decl = ParseFunctionDeclaration(false, ParserStatus::NO_OPTS);
            break;
        }
        case lexer::TokenType::KEYW_CLASS: {
            decl = ParseClassDeclaration(ir::ClassDefinitionModifiers::ID_REQUIRED);
            break;
        }
        case lexer::TokenType::LITERAL_IDENT: {
            if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_STRUCT) {
                decl = ParseStructDeclaration(ir::ClassDefinitionModifiers::NONE);
                break;
            }
            [[fallthrough]];
        }
        default: {
            if (!lexer_->GetToken().IsAsyncModifier()) {
                ThrowSyntaxError("Unexpected token");
            }

            lexer_->NextToken();  // eat `async` keyword
            decl = ParseFunctionDeclaration(false, ParserStatus::ASYNC_FUNCTION);
        }
    }

    if (decl->IsVariableDeclaration()) {
        ConsumeSemicolon(decl);
    }

    lexer::SourcePosition endLoc = decl->End();
    ArenaVector<ir::ExportSpecifier *> specifiers(Allocator()->Adapter());
    auto *exportDeclaration = AllocNode<ir::ExportNamedDeclaration>(Allocator(), decl, std::move(specifiers));
    exportDeclaration->SetRange({startLoc, endLoc});

    return exportDeclaration;
}

ir::Statement *ParserImpl::ParseExportDeclaration(StatementParsingFlags flags)
{
    if ((flags & StatementParsingFlags::GLOBAL) == 0) {
        ThrowSyntaxError("'import' and 'export' may only appear at the top level");
    }

    if (!context_.IsModule()) {
        ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat `export` keyword

    switch (lexer_->GetToken().Type()) {
        case lexer::TokenType::PUNCTUATOR_MULTIPLY: {
            return ParseExportAllDeclaration(startLoc);
        }
        case lexer::TokenType::PUNCTUATOR_LEFT_BRACE: {
            return ParseExportNamedSpecifiers(startLoc);
        }
        default: {
            if (lexer_->GetToken().KeywordType() == lexer::TokenType::KEYW_DEFAULT) {
                return ParseExportDefaultDeclaration(startLoc);
            }
            return ParseNamedExportDeclaration(startLoc);
        }
    }
}

void ParserImpl::ParseNameSpaceImport(ArenaVector<ir::AstNode *> *specifiers)
{
    lexer::SourcePosition namespaceStart = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat `*` character

    if (!CheckModuleAsModifier()) {
        ThrowSyntaxError("Unexpected token.");
    }

    lexer_->NextToken();  // eat `as` literal

    ir::Identifier *local = ParseNamedImport(lexer_->GetToken());

    auto *specifier = AllocNode<ir::ImportNamespaceSpecifier>(local);
    specifier->SetRange({namespaceStart, lexer_->GetToken().End()});
    specifiers->push_back(specifier);

    lexer_->NextToken();  // eat local name
}

ir::Identifier *ParserImpl::ParseNamedImport(const lexer::Token &importedToken)
{
    if (importedToken.Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Unexpected token");
    }

    CheckRestrictedBinding(importedToken.KeywordType());

    auto *local = AllocNode<ir::Identifier>(importedToken.Ident(), Allocator());
    local->SetReference();
    local->SetRange(importedToken.Loc());

    return local;
}

void ParserImpl::ParseNamedImportSpecifiers(ArenaVector<ir::AstNode *> *specifiers)
{
    lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat `{` character

    while (lexer_->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            ThrowSyntaxError("Unexpected token");
        }

        lexer::Token importedToken = lexer_->GetToken();
        auto *imported = AllocNode<ir::Identifier>(importedToken.Ident(), Allocator());
        ir::Identifier *local = nullptr;
        imported->SetReference();
        imported->SetRange(lexer_->GetToken().Loc());

        lexer_->NextToken();  // eat import name

        if (CheckModuleAsModifier()) {
            lexer_->NextToken();  // eat `as` literal
            local = ParseNamedImport(lexer_->GetToken());
            lexer_->NextToken();  // eat local name
        } else {
            local = ParseNamedImport(importedToken);
        }

        auto *specifier = AllocNode<ir::ImportSpecifier>(imported, local);
        specifier->SetRange({imported->Start(), local->End()});
        specifiers->push_back(specifier);

        if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            lexer_->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat comma
        }
    }

    lexer_->NextToken();  // eat right brace
}

ir::AstNode *ParserImpl::ParseImportDefaultSpecifier(ArenaVector<ir::AstNode *> *specifiers)
{
    ir::Identifier *local = ParseNamedImport(lexer_->GetToken());
    lexer_->NextToken();  // eat local name

    auto *specifier = AllocNode<ir::ImportDefaultSpecifier>(local);
    specifier->SetRange(specifier->Local()->Range());
    specifiers->push_back(specifier);

    lexer_->NextToken();  // eat specifier name

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
        lexer_->NextToken();  // eat comma
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

ir::AstNode *ParserImpl::ParseImportSpecifiers(ArenaVector<ir::AstNode *> *specifiers)
{
    ASSERT(specifiers->empty());

    if (lexer_->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
        ir::AstNode *astNode = ParseImportDefaultSpecifier(specifiers);

        if (astNode != nullptr) {
            return astNode;
        }
    }

    if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_MULTIPLY) {
        ParseNameSpaceImport(specifiers);
    } else if (lexer_->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ParseNamedImportSpecifiers(specifiers);
    }
    return nullptr;
}

ir::Statement *ParserImpl::ParseImportDeclaration(StatementParsingFlags flags)
{
    if ((flags & StatementParsingFlags::GLOBAL) == 0) {
        ThrowSyntaxError("'import' and 'export' may only appear at the top level");
    }

    if (!context_.IsModule()) {
        ThrowSyntaxError("'import' and 'export' may appear only with 'sourceType: module'");
    }

    char32_t nextChar = lexer_->Lookahead();
    if (nextChar == lexer::LEX_CHAR_LEFT_PAREN || nextChar == lexer::LEX_CHAR_DOT) {
        return ParseExpressionStatement();
    }

    lexer::SourcePosition startLoc = lexer_->GetToken().Start();
    lexer_->NextToken();  // eat import

    ArenaVector<ir::AstNode *> specifiers(Allocator()->Adapter());

    ir::StringLiteral *source = nullptr;

    if (lexer_->GetToken().Type() != lexer::TokenType::LITERAL_STRING) {
        ParseImportSpecifiers(&specifiers);
        source = ParseFromClause(true);
    } else {
        source = ParseFromClause(false);
    }

    lexer::SourcePosition endLoc = source->End();
    auto *importDeclaration = AllocNode<ir::ImportDeclaration>(source, std::move(specifiers));
    importDeclaration->SetRange({startLoc, endLoc});

    ConsumeSemicolon(importDeclaration);

    return importDeclaration;
}
}  // namespace ark::es2panda::parser
