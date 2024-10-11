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

#include "TypedParser.h"

#include "parser/parserImpl.h"
#include "parser/parserStatusContext.h"
#include "varbinder/privateBinding.h"
#include "lexer/lexer.h"
#include "ir/base/classDefinition.h"
#include "ir/base/decorator.h"
#include "ir/base/spreadElement.h"
#include "ir/base/tsPropertySignature.h"
#include "ir/base/tsMethodSignature.h"
#include "ir/base/methodDefinition.h"
#include "ir/base/tsIndexSignature.h"
#include "ir/base/scriptFunction.h"
#include "ir/expressions/identifier.h"
#include "ir/expressions/yieldExpression.h"
#include "ir/expressions/objectExpression.h"
#include "ir/expressions/arrayExpression.h"
#include "ir/expressions/assignmentExpression.h"
#include "ir/expressions/arrowFunctionExpression.h"
#include "ir/expressions/sequenceExpression.h"
#include "ir/expressions/callExpression.h"
#include "ir/expressions/literals/stringLiteral.h"
#include "ir/expressions/literals/numberLiteral.h"
#include "ir/expressions/functionExpression.h"
#include "ir/statement.h"
#include "ir/statements/classDeclaration.h"
#include "ir/statements/variableDeclarator.h"
#include "ir/ts/tsModuleDeclaration.h"
#include "ir/ts/tsModuleBlock.h"
#include "ir/ts/tsInterfaceBody.h"
#include "ir/ts/tsInterfaceDeclaration.h"
#include "ir/ts/tsEnumMember.h"
#include "ir/ts/tsEnumDeclaration.h"
#include "ir/ts/tsTypeParameterInstantiation.h"
#include "ir/ts/tsTypeParameterDeclaration.h"
#include "ir/ts/tsTypeParameter.h"
#include "ir/ts/tsQualifiedName.h"
#include "ir/ts/tsTypeReference.h"
#include "ir/ts/tsTypeAliasDeclaration.h"
#include "ir/ts/tsInterfaceHeritage.h"
#include "ir/ts/tsClassImplements.h"
#include "ir/ts/tsTypeAssertion.h"

namespace ark::es2panda::parser {

ir::Expression *TypedParser::ParsePotentialAsExpression([[maybe_unused]] ir::Expression *primaryExpression)
{
    return nullptr;
}

// NOLINTNEXTLINE(google-default-arguments)
ir::Expression *TypedParser::ParseExpression(ExpressionParseFlags flags)
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_YIELD &&
        ((flags & ExpressionParseFlags::DISALLOW_YIELD) == 0)) {
        ir::YieldExpression *yieldExpr = ParseYieldExpression();

        return ParsePotentialExpressionSequence(yieldExpr, flags);
    }

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        const auto startPos = Lexer()->Save();

        // NOTE(rsipka): ParseTsGenericArrowFunction and ParseTsTypeAssertion might be in a common function
        ir::Expression *expr = ParseGenericArrowFunction();
        // NOTE(rsipka): negative cases are not covered, probably this is not a complete solution yet
        if (expr == nullptr) {
            Lexer()->Rewind(startPos);
            expr = ParseTypeAssertion();
        }

        return expr;
    }

    ir::Expression *unaryExpressionNode = ParseUnaryOrPrefixUpdateExpression(flags);

    if (unaryExpressionNode->IsArrowFunctionExpression()) {
        return unaryExpressionNode;
    }

    ir::Expression *assignmentExpression = ParseAssignmentExpression(unaryExpressionNode, flags);

    if (Lexer()->GetToken().NewLine()) {
        return assignmentExpression;
    }

    switch (Lexer()->GetToken().Type()) {
        case lexer::TokenType::LITERAL_IDENT: {
            if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_AS) {
                return ParsePotentialAsExpression(assignmentExpression);
            }
            break;
        }
        case lexer::TokenType::PUNCTUATOR_COMMA: {
            if ((flags & ExpressionParseFlags::ACCEPT_COMMA) != 0) {
                return ParseSequenceExpression(assignmentExpression,
                                               ((flags & ExpressionParseFlags::ACCEPT_REST) != 0));
            }
            break;
        }
        default: {
            break;
        }
    }

    return assignmentExpression;
}

ir::Statement *TypedParser::ParsePotentialExpressionStatement(StatementParsingFlags flags)
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_IDENT);

    switch (Lexer()->GetToken().KeywordType()) {
        case lexer::TokenType::KEYW_TYPE: {
            return ParseTypeAliasDeclaration();
        }
        case lexer::TokenType::KEYW_ABSTRACT: {
            Lexer()->NextToken();  // eat abstract keyword

            if (Lexer()->GetToken().Type() != lexer::TokenType::KEYW_CLASS) {
                ThrowSyntaxError(
                    "abstract modifier can only appear on a class, struct, method, or property declaration.");
            }

            return ParseClassStatement(flags, ir::ClassDefinitionModifiers::NONE, ir::ModifierFlags::ABSTRACT);
        }
        case lexer::TokenType::KEYW_GLOBAL:
        case lexer::TokenType::KEYW_MODULE:
        case lexer::TokenType::KEYW_NAMESPACE: {
            return ParseModuleDeclaration();
        }
        default: {
            break;
        }
    }
    return ParseExpressionStatement(flags);
}

ir::TSTypeAssertion *TypedParser::ParseTypeAssertion()
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN);
    lexer::SourcePosition start = Lexer()->GetToken().Start();
    Lexer()->NextToken();  // eat '<'

    TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
    ir::TypeNode *typeAnnotation = ParseTypeAnnotation(&options);

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_GREATER_THAN) {
        return nullptr;
    }

    Lexer()->NextToken();  // eat '>'
    ir::Expression *expression = ParseExpression();
    auto *typeAssertion = AllocNode<ir::TSTypeAssertion>(typeAnnotation, expression);
    typeAssertion->SetRange({start, Lexer()->GetToken().End()});

    return typeAssertion;
}

// NOLINTNEXTLINE(google-default-arguments)
ir::Statement *TypedParser::ParseModuleDeclaration([[maybe_unused]] StatementParsingFlags flags)
{
    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();
    GetContext().Status() |= ParserStatus::MODULE;

    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL) {
        return ParseAmbientExternalModuleDeclaration(startLoc);
    }

    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_NAMESPACE) {
        Lexer()->NextToken();
    } else {
        ASSERT(Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_MODULE);
        Lexer()->NextToken();
        if (Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_STRING) {
            return ParseAmbientExternalModuleDeclaration(startLoc);
        }
    }

    return ParseModuleOrNamespaceDeclaration(startLoc);
}

ir::ArrowFunctionExpression *TypedParser::ParseGenericArrowFunction()
{
    ArrowFunctionContext arrowFunctionContext(this, false);

    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN);
    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();

    auto typeParamDeclOptions = TypeAnnotationParsingOptions::NO_OPTS;
    ir::TSTypeParameterDeclaration *typeParamDecl = ParseTypeParameterDeclaration(&typeParamDeclOptions);

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_PARENTHESIS) {
        return nullptr;
    }

    FunctionParameterContext funcParamContext(&GetContext());
    auto params = ParseFunctionParams();

    ParserStatus arrowStatus = ParserStatus::NO_OPTS;

    if (std::any_of(params.begin(), params.end(), [](const auto *param) { return !param->IsIdentifier(); })) {
        arrowStatus = ParserStatus::HAS_COMPLEX_PARAM;
    }

    ir::TypeNode *returnTypeAnnotation = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
        Lexer()->NextToken();  // eat ':'
        TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
        returnTypeAnnotation = ParseTypeAnnotation(&options);
    }

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_ARROW) {
        return nullptr;
    }

    ArrowFunctionDescriptor desc(std::move(params), startLoc, arrowStatus);

    return ParseArrowFunctionExpressionBody(&arrowFunctionContext, &desc, typeParamDecl, returnTypeAnnotation);
}

ir::TSModuleDeclaration *TypedParser::ParseAmbientExternalModuleDeclaration(const lexer::SourcePosition &startLoc)
{
    bool isGlobal = false;
    ir::Expression *name = nullptr;

    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_GLOBAL) {
        isGlobal = true;
        name = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
    } else {
        ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_STRING);

        if (!InAmbientContext()) {
            ThrowSyntaxError("Only ambient modules can use quoted names");
        }

        name = AllocNode<ir::StringLiteral>(Lexer()->GetToken().String());
    }

    name->SetRange(Lexer()->GetToken().Loc());

    Lexer()->NextToken();

    ir::Statement *body = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        body = ParseTsModuleBlock();
    } else if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        Lexer()->NextToken();
    } else {
        ThrowSyntaxError("';' expected");
    }

    auto *moduleDecl = AllocNode<ir::TSModuleDeclaration>(
        Allocator(), name, body, ir::TSModuleDeclaration::ConstructorFlags {InAmbientContext(), isGlobal, true});
    moduleDecl->SetRange({startLoc, Lexer()->GetToken().End()});

    return moduleDecl;
}

ir::TSModuleDeclaration *TypedParser::ParseModuleOrNamespaceDeclaration(const lexer::SourcePosition &startLoc)
{
    if (Lexer()->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
        ThrowSyntaxError("Identifier expected");
    }

    auto *identNode = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
    identNode->SetRange(Lexer()->GetToken().Loc());

    Lexer()->NextToken();

    ir::Statement *body = nullptr;

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD) {
        Lexer()->NextToken();
        lexer::SourcePosition moduleStart = Lexer()->GetToken().Start();
        body = ParseModuleOrNamespaceDeclaration(moduleStart);
    } else {
        body = ParseTsModuleBlock();
    }

    auto *moduleDecl = AllocNode<ir::TSModuleDeclaration>(
        Allocator(), identNode, body, ir::TSModuleDeclaration::ConstructorFlags {InAmbientContext(), false, false});
    moduleDecl->SetRange({startLoc, Lexer()->GetToken().End()});

    return moduleDecl;
}

ir::TSModuleBlock *TypedParser::ParseTsModuleBlock()
{
    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("'{' expected.");
    }

    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();
    Lexer()->NextToken();
    auto statements = ParseStatementList();

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ThrowSyntaxError("Expected a '}'");
    }

    auto *blockNode = AllocNode<ir::TSModuleBlock>(std::move(statements));
    blockNode->SetRange({startLoc, Lexer()->GetToken().End()});

    Lexer()->NextToken();
    return blockNode;
}

void TypedParser::CheckDeclare()
{
    ASSERT(Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE);

    if (InAmbientContext()) {
        ThrowSyntaxError("A 'declare' modifier cannot be used in an already ambient context.");
    }

    GetContext().Status() |= ParserStatus::IN_AMBIENT_CONTEXT;

    Lexer()->NextToken();  // eat 'declare'

    switch (Lexer()->GetToken().KeywordType()) {
        case lexer::TokenType::KEYW_VAR:
        case lexer::TokenType::KEYW_LET:
        case lexer::TokenType::KEYW_CONST:
        case lexer::TokenType::KEYW_FUNCTION:
        case lexer::TokenType::KEYW_CLASS:
        case lexer::TokenType::KEYW_TYPE:
        case lexer::TokenType::KEYW_MODULE:
        case lexer::TokenType::KEYW_GLOBAL:
        case lexer::TokenType::KEYW_NAMESPACE:
        case lexer::TokenType::KEYW_ENUM:
        case lexer::TokenType::KEYW_ABSTRACT:
        case lexer::TokenType::KEYW_INTERFACE: {
            return;
        }
        default: {
            ThrowSyntaxError("Unexpected token.");
        }
    }
}

void TypedParser::ParseDecorators(ArenaVector<ir::Decorator *> &decorators)
{
    while (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_AT) {
        decorators.push_back(ParseDecorator());
    }
}

ir::TypeNode *TypedParser::ParseFunctionReturnType(ParserStatus status)
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
        Lexer()->NextToken();  // eat ':'
        TypeAnnotationParsingOptions options =
            TypeAnnotationParsingOptions::THROW_ERROR | TypeAnnotationParsingOptions::RETURN_TYPE;
        return ParseTypeAnnotation(&options);
    }

    if ((status & ParserStatus::NEED_RETURN_TYPE) != 0) {
        ThrowSyntaxError("Type expected");
    }

    return nullptr;
}

ir::TypeNode *TypedParser::ParseInterfaceExtendsElement()
{
    const lexer::SourcePosition &heritageStart = Lexer()->GetToken().Start();
    lexer::SourcePosition heritageEnd = Lexer()->GetToken().End();
    ir::Expression *expr = ParseQualifiedName();

    if (Lexer()->Lookahead() == lexer::LEX_CHAR_LESS_THAN) {
        Lexer()->ForwardToken(lexer::TokenType::PUNCTUATOR_LESS_THAN, 1);
    }

    ir::TSTypeParameterInstantiation *typeParamInst = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
        typeParamInst = ParseTypeParameterInstantiation(&options);
        heritageEnd = typeParamInst->End();
    }

    auto *typeReference = AllocNode<ir::TSTypeReference>(expr, typeParamInst);
    typeReference->SetRange({heritageStart, heritageEnd});
    return typeReference;
}

ArenaVector<ir::TSInterfaceHeritage *> TypedParser::ParseInterfaceExtendsClause()
{
    Lexer()->NextToken();  // eat extends keyword

    ArenaVector<ir::TSInterfaceHeritage *> extends(Allocator()->Adapter());

    while (true) {
        auto *typeReference = ParseInterfaceExtendsElement();
        auto *heritage = AllocNode<ir::TSInterfaceHeritage>(typeReference);
        heritage->SetRange(typeReference->Range());
        extends.push_back(heritage);

        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            break;
        }

        if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA) {
            ThrowSyntaxError("',' expected");
        }

        Lexer()->NextToken();
    }

    return extends;
}

ir::TSTypeParameterDeclaration *TypedParser::ParseFunctionTypeParameters()
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        auto options = TypeAnnotationParsingOptions::THROW_ERROR;
        return ParseTypeParameterDeclaration(&options);
    }

    return nullptr;
}

ir::Statement *TypedParser::ParseInterfaceDeclaration(bool isStatic)
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::KEYW_INTERFACE);
    GetContext().Status() |= ParserStatus::ALLOW_THIS_TYPE;
    lexer::SourcePosition interfaceStart = Lexer()->GetToken().Start();
    Lexer()->NextToken();  // eat interface keyword

    auto id = ExpectIdentifier(true);

    ir::TSTypeParameterDeclaration *typeParamDecl = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        auto options = TypeAnnotationParsingOptions::THROW_ERROR;
        typeParamDecl = ParseTypeParameterDeclaration(&options);
    }

    ArenaVector<ir::TSInterfaceHeritage *> extends(Allocator()->Adapter());
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_EXTENDS) {
        extends = ParseInterfaceExtendsClause();
    }

    lexer::SourcePosition bodyStart = Lexer()->GetToken().Start();
    auto members = ParseTypeLiteralOrInterface();

    auto *body = AllocNode<ir::TSInterfaceBody>(std::move(members));
    body->SetRange({bodyStart, Lexer()->GetToken().End()});

    const auto isExternal = IsExternal();
    auto *interfaceDecl = AllocNode<ir::TSInterfaceDeclaration>(
        Allocator(), std::move(extends),
        ir::TSInterfaceDeclaration::ConstructorData {id, typeParamDecl, body, isStatic, isExternal,
                                                     GetContext().GetLanguage()});
    interfaceDecl->SetRange({interfaceStart, Lexer()->GetToken().End()});

    Lexer()->NextToken();
    GetContext().Status() &= ~ParserStatus::ALLOW_THIS_TYPE;

    return interfaceDecl;
}

static util::StringView GetTSPropertyName(ir::Expression *key)
{
    switch (key->Type()) {
        case ir::AstNodeType::IDENTIFIER: {
            return key->AsIdentifier()->Name();
        }
        case ir::AstNodeType::NUMBER_LITERAL: {
            return key->AsNumberLiteral()->Str();
        }
        case ir::AstNodeType::STRING_LITERAL: {
            return key->AsStringLiteral()->Str();
        }
        default: {
            UNREACHABLE();
        }
    }
}

void TypedParser::CheckObjectTypeForDuplicatedProperties(ir::Expression *key, ArenaVector<ir::AstNode *> &members)
{
    if (!key->IsIdentifier() && !key->IsNumberLiteral() && !key->IsStringLiteral()) {
        return;
    }

    for (auto *it : members) {
        ir::Expression *compare = nullptr;

        switch (it->Type()) {
            case ir::AstNodeType::TS_PROPERTY_SIGNATURE: {
                compare = it->AsTSPropertySignature()->Key();
                break;
            }
            case ir::AstNodeType::TS_METHOD_SIGNATURE: {
                compare = it->AsTSMethodSignature()->Key();
                break;
            }
            default: {
                continue;
            }
        }

        if (!compare->IsIdentifier() && !compare->IsNumberLiteral() && !compare->IsStringLiteral()) {
            continue;
        }

        if (GetTSPropertyName(key) == GetTSPropertyName(compare)) {
            ThrowSyntaxError("Duplicated identifier", key->Start());
        }
    }
}

ArenaVector<ir::AstNode *> TypedParser::ParseTypeLiteralOrInterfaceBody()
{
    ArenaVector<ir::AstNode *> members(Allocator()->Adapter());

    while (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ir::AstNode *member = ParseTypeLiteralOrInterfaceMember();
        if (member->IsMethodDefinition() && member->AsMethodDefinition()->Function() != nullptr &&
            member->AsMethodDefinition()->Function()->IsOverload() &&
            member->AsMethodDefinition()->Function()->Body() != nullptr) {
            continue;
        }

        if (member->IsTSPropertySignature()) {
            CheckObjectTypeForDuplicatedProperties(member->AsTSPropertySignature()->Key(), members);
        } else if (member->IsTSMethodSignature()) {
            CheckObjectTypeForDuplicatedProperties(member->AsTSMethodSignature()->Key(), members);
        }

        members.push_back(member);

        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
            break;
        }

        if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA &&
            Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
            if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
                ThrowSyntaxError("Interface member initialization is prohibited");
            }
            if (!Lexer()->GetToken().NewLine()) {
                ThrowSyntaxError("',' expected");
            }

            if (Lexer()->GetToken().IsKeyword() && ((Lexer()->GetToken().Type() != lexer::TokenType::KEYW_STATIC) &&
                                                    (Lexer()->GetToken().Type() != lexer::TokenType::KEYW_PRIVATE))) {
                Lexer()->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
            }

            continue;
        }

        Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);
    }

    return members;
}

ArenaVector<ir::AstNode *> TypedParser::ParseTypeLiteralOrInterface()
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_IMPLEMENTS) {
        ThrowSyntaxError("Interface declaration cannot have 'implements' clause.");
    }

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("Unexpected token, expected '{'");
    }

    Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat '{'

    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_OVERRIDE) {
        ThrowSyntaxError("'override' modifier cannot appear in interfaces");
    }

    bool const formattedParsing = Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT &&
                                  Lexer()->Lookahead() == static_cast<char32_t>(ARRAY_FORMAT_NODE);

    ArenaVector<ir::AstNode *> members =
        !formattedParsing ? ParseTypeLiteralOrInterfaceBody() : std::move(ParseAstNodesArrayFormatPlaceholder());

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ThrowSyntaxError("Expected a '}'");
    }

    return members;
}

ir::TSEnumDeclaration *TypedParser::ParseEnumMembers(ir::Identifier *key, const lexer::SourcePosition &enumStart,
                                                     bool isConst, [[maybe_unused]] bool isStatic)
{
    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        ThrowSyntaxError("'{' expected");
    }

    ArenaVector<ir::AstNode *> members(Allocator()->Adapter());
    Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat '{'

    while (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE) {
        ir::Expression *memberKey = nullptr;

        if (Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
            memberKey = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
            memberKey->SetRange(Lexer()->GetToken().Loc());
            Lexer()->NextToken();
        } else if (Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_STRING) {
            memberKey = AllocNode<ir::StringLiteral>(Lexer()->GetToken().String());
            memberKey->SetRange(Lexer()->GetToken().Loc());
            Lexer()->NextToken();
        } else {
            ThrowSyntaxError("Unexpected token in enum member");
        }

        ir::Expression *memberInit = nullptr;
        lexer::SourcePosition initStart = Lexer()->GetToken().Start();

        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
            Lexer()->NextToken();  // eat '='
            initStart = Lexer()->GetToken().Start();
            memberInit = ParseExpression();
        }

        auto *member = AllocNode<ir::TSEnumMember>(memberKey, memberInit);
        member->SetRange({initStart, Lexer()->GetToken().End()});
        members.push_back(member);

        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);  // eat ','
        }
    }

    auto *enumDeclaration = AllocNode<ir::TSEnumDeclaration>(Allocator(), key, std::move(members),
                                                             ir::TSEnumDeclaration::ConstructorFlags {isConst});
    enumDeclaration->SetRange({enumStart, Lexer()->GetToken().End()});
    Lexer()->NextToken();  // eat '}'

    return enumDeclaration;
}

// NOLINTNEXTLINE(google-default-arguments)
ir::Statement *TypedParser::ParseEnumDeclaration(bool isConst, [[maybe_unused]] bool isStatic)
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::KEYW_ENUM);
    lexer::SourcePosition enumStart = Lexer()->GetToken().Start();
    Lexer()->NextToken();  // eat enum keyword
    auto *key = ExpectIdentifier(true);
    auto *declNode = ParseEnumMembers(key, enumStart, isConst, false);
    return declNode;
}

ir::TSTypeParameter *TypedParser::ParseTypeParameter(TypeAnnotationParsingOptions *options)
{
    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();
    bool throwError = ((*options) & TypeAnnotationParsingOptions::THROW_ERROR) != 0;

    if (Lexer()->GetToken().Type() != lexer::TokenType::LITERAL_IDENT || Lexer()->GetToken().IsDefinableTypeName()) {
        if (!throwError) {
            return nullptr;
        }

        ThrowSyntaxError("Type parameter declaration expected");
    }

    if (throwError) {
        CheckIfTypeParameterNameIsReserved();
    }

    const auto &ident = Lexer()->GetToken().Ident();
    auto *paramIdent = AllocNode<ir::Identifier>(ident, Allocator());

    paramIdent->SetRange({Lexer()->GetToken().Start(), Lexer()->GetToken().End()});

    Lexer()->NextToken();

    TypeAnnotationParsingOptions newOptions = TypeAnnotationParsingOptions::NO_OPTS;

    if (throwError) {
        newOptions |= TypeAnnotationParsingOptions::THROW_ERROR;
    }

    ir::TypeNode *constraint = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_EXTENDS) {
        Lexer()->NextToken();
        constraint = ParseTypeAnnotation(&newOptions);
    }

    ir::TypeNode *defaultType = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        Lexer()->NextToken();
        defaultType = ParseTypeAnnotation(&newOptions);
    }

    auto *typeParam = AllocNode<ir::TSTypeParameter>(paramIdent, constraint, defaultType);

    typeParam->SetRange({startLoc, Lexer()->GetToken().End()});

    return typeParam;
}

//  Auxiliary method to reduce the size of functions.
ir::AstNode *TypedParser::ParseTypeParameterDeclarationImpl(TypeAnnotationParsingOptions *options)
{
    ArenaVector<ir::TSTypeParameter *> params(Allocator()->Adapter());
    bool seenDefault = false;
    size_t requiredParams = 0U;

    while (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_GREATER_THAN) {
        auto newOptions = *options | TypeAnnotationParsingOptions::ADD_TYPE_PARAMETER_BINDING;
        ir::TSTypeParameter *currentParam = ParseTypeParameter(&newOptions);

        if (currentParam == nullptr) {
            ASSERT((newOptions & TypeAnnotationParsingOptions::THROW_ERROR) == 0);
            return nullptr;
        }

        if (currentParam->DefaultType() != nullptr) {
            seenDefault = true;
        } else if (seenDefault) {
            ThrowSyntaxError("Required type parameters may not follow optional type parameters.");
        } else {
            requiredParams++;
        }

        params.push_back(currentParam);

        if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_COMMA) {
            break;
        }

        Lexer()->NextToken();
    }

    if (params.empty()) {
        ThrowSyntaxError("Type parameter list cannot be empty.");
    }

    return AllocNode<ir::TSTypeParameterDeclaration>(std::move(params), requiredParams);
}

ir::TSTypeParameterDeclaration *TypedParser::ParseTypeParameterDeclaration(TypeAnnotationParsingOptions *options)
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN);

    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();
    Lexer()->NextToken();  // eat '<'

    ir::AstNode *typeParamDecl;

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT &&
        Lexer()->Lookahead() == static_cast<char32_t>(EXPRESSION_FORMAT_NODE)) {
        typeParamDecl = ParseTypeParametersFormatPlaceholder();
    } else {
        typeParamDecl = ParseTypeParameterDeclarationImpl(options);
    }

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_GREATER_THAN) {
        if ((*options & TypeAnnotationParsingOptions::THROW_ERROR) == 0) {
            return nullptr;
        }
        ThrowSyntaxError("Expected closing '>'.");
    }

    lexer::SourcePosition endLoc = Lexer()->GetToken().End();
    Lexer()->NextToken();  // eat '>'

    if (typeParamDecl != nullptr) {
        typeParamDecl->SetRange({startLoc, endLoc});
        return typeParamDecl->AsTSTypeParameterDeclaration();
    }

    return nullptr;
}

ir::Expression *TypedParser::ParseSuperClassReference()
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_EXTENDS) {
        Lexer()->NextToken();

        return ParseLeftHandSideExpression();
    }

    return nullptr;
}

std::tuple<ir::Expression *, ir::TSTypeParameterInstantiation *> TypedParser::ParseSuperClass()
{
    ir::Expression *superClass = ParseSuperClassReference();

    ir::TSTypeParameterInstantiation *superTypeParams = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN ||
        Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SHIFT) {
        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SHIFT) {
            Lexer()->BackwardToken(lexer::TokenType::PUNCTUATOR_LESS_THAN, 1);
        }

        TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
        superTypeParams = ParseTypeParameterInstantiation(&options);
    }

    return {superClass, superTypeParams};
}

std::tuple<ir::Expression *, ir::TSTypeParameterInstantiation *> TypedParser::ParseClassImplementsElement()
{
    ir::Expression *expr = ParseQualifiedName();

    ir::TSTypeParameterInstantiation *implTypeParams = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN ||
        Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SHIFT) {
        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SHIFT) {
            Lexer()->BackwardToken(lexer::TokenType::PUNCTUATOR_LESS_THAN, 1);
        }

        TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
        implTypeParams = ParseTypeParameterInstantiation(&options);
    }

    return {expr, implTypeParams};
}

ArenaVector<ir::TSClassImplements *> TypedParser::ParseClassImplementClause()
{
    ArenaVector<ir::TSClassImplements *> implements(Allocator()->Adapter());

    while (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
        lexer::SourcePosition implStart = Lexer()->GetToken().Start();
        auto [expr, implTypeParams] = ParseClassImplementsElement();
        auto *impl = AllocNode<ir::TSClassImplements>(expr, implTypeParams);
        impl->SetRange({implStart, Lexer()->GetToken().End()});
        implements.push_back(impl);

        if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COMMA) {
            Lexer()->NextToken();
            continue;
        }

        if (InAmbientContext()) {
            break;
        }

        if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_LEFT_BRACE) {
            ThrowSyntaxError("',' expected");
        }
    }

    if (implements.empty()) {
        ThrowSyntaxError("Implements clause can not be empty");
    }

    return implements;
}

bool TypedParser::CheckClassElementInterfaceBody(ir::AstNode *property, ArenaVector<ir::AstNode *> &properties)
{
    for (auto *it : property->AsTSInterfaceBody()->Body()) {
        properties.push_back(it);
    }

    return true;
}

bool TypedParser::CheckClassElement(ir::AstNode *property, ir::MethodDefinition *&ctor,
                                    [[maybe_unused]] ArenaVector<ir::AstNode *> &properties)
{
    if (property->IsTSInterfaceBody()) {
        return CheckClassElementInterfaceBody(property, properties);
    }

    return ParserImpl::CheckClassElement(property, ctor, properties);
}

// NOLINTNEXTLINE(google-default-arguments)
ir::ClassDefinition *TypedParser::ParseClassDefinition(ir::ClassDefinitionModifiers modifiers, ir::ModifierFlags flags)
{
    Lexer()->NextToken();

    ir::Identifier *identNode = ParseClassIdent(modifiers);

    ir::TSTypeParameterDeclaration *typeParamDecl = nullptr;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN) {
        auto options = TypeAnnotationParsingOptions::THROW_ERROR;
        typeParamDecl = ParseTypeParameterDeclaration(&options);
    }

    varbinder::PrivateBinding privateBinding(Allocator(), ClassId()++);

    // Parse SuperClass
    auto [superClass, superTypeParams] = ParseSuperClass();

    if (superClass != nullptr) {
        modifiers |= ir::ClassDefinitionModifiers::HAS_SUPER;
    }

    // Parse implements clause
    ArenaVector<ir::TSClassImplements *> implements(Allocator()->Adapter());
    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_IMPLEMENTS) {
        Lexer()->NextToken();
        implements = ParseClassImplementClause();
    }

    ExpectToken(lexer::TokenType::PUNCTUATOR_LEFT_BRACE, false);

    // Parse ClassBody
    auto [ctor, properties, bodyRange] = ParseClassBody(modifiers, flags);

    if (InAmbientContext()) {
        flags |= ir::ModifierFlags::DECLARE;
    }

    auto *classDefinition = AllocNode<ir::ClassDefinition>(
        privateBinding.View(), identNode, typeParamDecl, superTypeParams, std::move(implements), ctor, superClass,
        std::move(properties), modifiers, flags, GetContext().GetLanguage());

    classDefinition->SetRange(bodyRange);

    return classDefinition;
}

ir::AstNode *TypedParser::ParseProperty(const ArenaVector<ir::AstNode *> &properties, ClassElementDescriptor &desc,
                                        ir::Expression *propName)
{
    ir::AstNode *property = nullptr;
    ir::TypeNode *typeAnnotation = ParseClassKeyAnnotation();

    if (desc.isIndexSignature) {
        if (!desc.decorators.empty()) {
            ThrowSyntaxError("Decorators are not valid here.", desc.decorators.front()->Start());
        }

        ValidateIndexSignatureTypeAnnotation(typeAnnotation);

        if (typeAnnotation == nullptr) {
            ThrowSyntaxError("An index signature must have a type annotation");
        }

        if ((desc.modifiers & ir::ModifierFlags::DECLARE) != 0) {
            ThrowSyntaxError("'declare' modifier cannot appear on an index signature.");
        }

        property =
            AllocNode<ir::TSIndexSignature>(propName, typeAnnotation, desc.modifiers & ir::ModifierFlags::READONLY);

        property->SetRange({property->AsTSIndexSignature()->Param()->Start(),
                            property->AsTSIndexSignature()->TypeAnnotation()->End()});
    } else {
        ValidateClassMethodStart(&desc, typeAnnotation);
        property = ParseClassProperty(&desc, properties, propName, typeAnnotation);

        if (!desc.decorators.empty()) {
            if (desc.isPrivateIdent) {
                ThrowSyntaxError("Decorators are not valid here");
            }

            property->AddDecorators(std::move(desc.decorators));
        }
    }

    ASSERT(property != nullptr);
    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_SEMI_COLON &&
        Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_RIGHT_BRACE &&
        ((Lexer()->GetToken().Flags() & lexer::TokenFlags::NEW_LINE) == 0) &&
        !(property->IsMethodDefinition() &&
          property->AsMethodDefinition()->Value()->AsFunctionExpression()->Function()->Body() != nullptr)) {
        ThrowSyntaxError("';' expected.");
    }

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SEMI_COLON) {
        Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);
    }

    GetContext().Status() &= ~ParserStatus::ALLOW_THIS_TYPE;

    if (desc.isPrivateIdent) {
        AddPrivateElement(property->AsClassElement());
    }

    return property;
}

ir::AstNode *TypedParser::ParseClassElement(const ArenaVector<ir::AstNode *> &properties,
                                            ir::ClassDefinitionModifiers modifiers, ir::ModifierFlags flags)
{
    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_STATIC &&
        Lexer()->Lookahead() == lexer::LEX_CHAR_LEFT_BRACE) {
        return ParseClassStaticBlock();
    }

    ClassElementDescriptor desc(Allocator());

    desc.methodKind = ir::MethodDefinitionKind::METHOD;
    desc.newStatus = ParserStatus::ALLOW_SUPER;
    desc.hasSuperClass = (modifiers & ir::ClassDefinitionModifiers::HAS_SUPER) != 0;
    desc.propStart = Lexer()->GetToken().Start();

    ParseDecorators(desc.decorators);

    desc.modifiers = ParseModifiers();

    if (((desc.modifiers & ir::ModifierFlags::ABSTRACT) != 0) && ((flags & ir::ModifierFlags::ABSTRACT) == 0)) {
        ThrowSyntaxError("Abstract methods can only appear within an abstract class.");
    }

    char32_t nextCp = Lexer()->Lookahead();
    CheckClassGeneratorMethod(&desc, &nextCp);
    ParseClassAccessor(&desc, &nextCp);

    if ((desc.modifiers & ir::ModifierFlags::STATIC) == 0) {
        GetContext().Status() |= ParserStatus::ALLOW_THIS_TYPE;
    }

    ir::Expression *propName = ParseClassKey(&desc);

    if (desc.methodKind == ir::MethodDefinitionKind::CONSTRUCTOR && !desc.decorators.empty()) {
        ThrowSyntaxError("Decorators are not valid here.", desc.decorators.front()->Start());
    }

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_QUESTION_MARK) {
        ParseOptionalClassElement(&desc);
    } else if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK) {
        if (desc.isIndexSignature || Lexer()->Lookahead() != lexer::LEX_CHAR_COLON) {
            ThrowSyntaxError("';' expected");
        }

        desc.modifiers |= ir::ModifierFlags::DEFINITE;
        Lexer()->NextToken();
    }

    return ParseProperty(properties, desc, propName);
}

void TypedParser::ParseOptionalClassElement(ClassElementDescriptor *desc)
{
    if (desc->isIndexSignature) {
        ThrowSyntaxError("';' expected");
    }

    if (desc->methodKind == ir::MethodDefinitionKind::CONSTRUCTOR) {
        ThrowSyntaxError("'(' expected");
    }

    desc->modifiers |= ir::ModifierFlags::OPTIONAL;
    Lexer()->NextToken();
}

static std::pair<ir::ModifierFlags, ir::ModifierFlags> ParseActualNextStatus(lexer::TokenType keywordType)
{
    constexpr auto ASYNC_STATIC_READONLY =
        ir::ModifierFlags::ASYNC | ir::ModifierFlags::STATIC | ir::ModifierFlags::READONLY;
    constexpr auto ASYNC_STATIC_READONLY_DECLARE_ABSTRACT =
        ASYNC_STATIC_READONLY | ir::ModifierFlags::DECLARE | ir::ModifierFlags::ABSTRACT;

    switch (keywordType) {
        case lexer::TokenType::KEYW_PUBLIC:
            return {ir::ModifierFlags::PUBLIC, ASYNC_STATIC_READONLY_DECLARE_ABSTRACT};
        case lexer::TokenType::KEYW_PRIVATE:
            return {ir::ModifierFlags::PRIVATE, ASYNC_STATIC_READONLY_DECLARE_ABSTRACT};
        case lexer::TokenType::KEYW_PROTECTED:
            return {ir::ModifierFlags::PROTECTED, ASYNC_STATIC_READONLY_DECLARE_ABSTRACT};
        case lexer::TokenType::KEYW_INTERNAL:
            return {ir::ModifierFlags::INTERNAL, ASYNC_STATIC_READONLY_DECLARE_ABSTRACT | ir::ModifierFlags::PROTECTED};
        case lexer::TokenType::KEYW_STATIC:
            return {ir::ModifierFlags::STATIC, ir::ModifierFlags::ASYNC | ir::ModifierFlags::READONLY |
                                                   ir::ModifierFlags::DECLARE | ir::ModifierFlags::ABSTRACT};
        case lexer::TokenType::KEYW_ASYNC:
            return {ir::ModifierFlags::ASYNC,
                    ir::ModifierFlags::READONLY | ir::ModifierFlags::DECLARE | ir::ModifierFlags::ABSTRACT};
        case lexer::TokenType::KEYW_ABSTRACT:
            return {ir::ModifierFlags::ABSTRACT,
                    ASYNC_STATIC_READONLY | ir::ModifierFlags::ACCESS | ir::ModifierFlags::DECLARE};
        case lexer::TokenType::KEYW_DECLARE:
            return {ir::ModifierFlags::DECLARE, ASYNC_STATIC_READONLY | ir::ModifierFlags::ACCESS};
        case lexer::TokenType::KEYW_READONLY:
            return {ir::ModifierFlags::READONLY,
                    ir::ModifierFlags::ASYNC | ir::ModifierFlags::DECLARE | ir::ModifierFlags::ABSTRACT};
        default:
            UNREACHABLE();
    }
}

ir::ModifierFlags TypedParser::ParseModifiers()
{
    ir::ModifierFlags resultStatus = ir::ModifierFlags::NONE;
    ir::ModifierFlags prevStatus = ir::ModifierFlags::ALL;

    while (IsModifierKind(Lexer()->GetToken())) {
        char32_t nextCp = Lexer()->Lookahead();
        if (!(nextCp != lexer::LEX_CHAR_EQUALS && nextCp != lexer::LEX_CHAR_SEMICOLON &&
              nextCp != lexer::LEX_CHAR_COMMA && nextCp != lexer::LEX_CHAR_LEFT_PAREN)) {
            break;
        }

        lexer::TokenFlags tokenFlags = Lexer()->GetToken().Flags();
        if ((tokenFlags & lexer::TokenFlags::HAS_ESCAPE) != 0) {
            ThrowSyntaxError("Keyword must not contain escaped characters");
        }

        auto [actualStatus, nextStatus] = ParseActualNextStatus(Lexer()->GetToken().KeywordType());

        nextCp = Lexer()->Lookahead();
        if (nextCp == lexer::LEX_CHAR_COLON || nextCp == lexer::LEX_CHAR_COMMA ||
            nextCp == lexer::LEX_CHAR_RIGHT_PAREN || nextCp == lexer::LEX_CHAR_QUESTION ||
            nextCp == lexer::LEX_CHAR_RIGHT_BRACE || nextCp == lexer::LEX_CHAR_LESS_THAN) {
            break;
        }

        auto pos = Lexer()->Save();
        Lexer()->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);

        if ((prevStatus & actualStatus) == 0) {
            Lexer()->Rewind(pos);
            ThrowSyntaxError("Unexpected modifier");
        }

        if ((resultStatus & actualStatus) != 0) {
            Lexer()->Rewind(pos);
            ThrowSyntaxError("Duplicated modifier is not allowed");
        }

        if ((GetContext().Status() & ParserStatus::CONSTRUCTOR_FUNCTION) != 0 &&
            (actualStatus & ~ir::ModifierFlags::ALLOWED_IN_CTOR_PARAMETER) != 0) {
            Lexer()->Rewind(pos);
            ThrowParameterModifierError(actualStatus);
        }

        resultStatus |= actualStatus;
        prevStatus = nextStatus;
    }

    return resultStatus;
}

ir::Expression *TypedParser::ParseQualifiedName(ExpressionParseFlags flags)
{
    ir::Expression *expr = nullptr;

    switch (Lexer()->GetToken().Type()) {
        case lexer::TokenType::PUNCTUATOR_FORMAT:
            expr = ParseIdentifierFormatPlaceholder(std::nullopt);
            break;
        case lexer::TokenType::LITERAL_IDENT:
            expr = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
            expr->SetRange(Lexer()->GetToken().Loc());
            Lexer()->NextToken();
            break;
        default:
            if ((flags & ExpressionParseFlags::POTENTIAL_NEW_ARRAY) != 0) {
                return expr;
            }
            ThrowSyntaxError("Identifier expected");
    }

    expr->AsIdentifier()->SetReference();

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD) {
        expr = ParseQualifiedReference(expr, flags);
    }

    return expr;
}

ir::Expression *TypedParser::ParseLiteralIndent(ir::Expression *typeName, ExpressionParseFlags flags,
                                                const lexer::SourcePosition &startLoc)
{
    if ((flags & ExpressionParseFlags::POTENTIAL_CLASS_LITERAL) != 0) {
        if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_CLASS) {
            typeName->SetRange({startLoc, Lexer()->GetToken().End()});
            return typeName;
        }
        if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_THIS) {
            return static_cast<ir::Expression *>(nullptr);
        }
    }

    ThrowSyntaxError("Identifier expected");
}

ir::Expression *TypedParser::ParseQualifiedReference(ir::Expression *typeName, ExpressionParseFlags flags)
{
    lexer::SourcePosition startLoc = typeName->Start();

    do {
        Lexer()->NextToken();  // eat '.'

        ir::Identifier *propName {};
        if ((flags & ExpressionParseFlags::IMPORT) != 0 &&
            Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_MULTIPLY) {
            Lexer()->NextToken();  // eat '*'
            propName = AllocNode<ir::Identifier>(varbinder::VarBinder::STAR_IMPORT, Allocator());
        } else if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT) {
            propName = ParseIdentifierFormatPlaceholder(std::nullopt);
        } else if (Lexer()->GetToken().Type() != lexer::TokenType::LITERAL_IDENT) {
            return ParseLiteralIndent(typeName, flags, startLoc);
        } else {
            propName = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
        }

        propName->SetReference();
        propName->SetRange(Lexer()->GetToken().Loc());

        typeName = AllocNode<ir::TSQualifiedName>(typeName, propName);
        typeName->SetRange({typeName->AsTSQualifiedName()->Left()->Start(), Lexer()->GetToken().End()});

        if (Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_IDENT) {
            Lexer()->NextToken();
        }
    } while (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_PERIOD);

    typeName->SetRange({startLoc, Lexer()->GetToken().End()});

    return typeName;
}

//  Auxiliary method to reduce the size of functions.
ir::AstNode *TypedParser::ParseTypeParameterInstantiationImpl(TypeAnnotationParsingOptions *options)
{
    ArenaVector<ir::TypeNode *> params(Allocator()->Adapter());

    while (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_GREATER_THAN) {
        TypeAnnotationParsingOptions tmpOptions = *options &= ~TypeAnnotationParsingOptions::IGNORE_FUNCTION_TYPE;
        // Need to parse correctly the cases like `x: T|C<T|U>`
        tmpOptions &= ~TypeAnnotationParsingOptions::DISALLOW_UNION;
        ir::TypeNode *currentParam = ParseTypeAnnotation(&tmpOptions);

        if (currentParam == nullptr) {
            return nullptr;
        }

        params.push_back(currentParam);

        switch (Lexer()->GetToken().Type()) {
            case lexer::TokenType::PUNCTUATOR_COMMA: {
                Lexer()->NextToken();
                continue;
            }
            case lexer::TokenType::PUNCTUATOR_RIGHT_SHIFT: {
                Lexer()->BackwardToken(lexer::TokenType::PUNCTUATOR_GREATER_THAN, 1);
                break;
            }
            case lexer::TokenType::PUNCTUATOR_UNSIGNED_RIGHT_SHIFT: {
                Lexer()->BackwardToken(lexer::TokenType::PUNCTUATOR_GREATER_THAN, 2U);
                break;
            }
            case lexer::TokenType::PUNCTUATOR_GREATER_THAN: {
                break;
            }
            default: {
                return nullptr;
            }
        }
    }

    return AllocNode<ir::TSTypeParameterInstantiation>(std::move(params));
}

ir::TSTypeParameterInstantiation *TypedParser::ParseTypeParameterInstantiation(TypeAnnotationParsingOptions *options)
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LESS_THAN);

    const bool inPotentialNewArray = (*options & TypeAnnotationParsingOptions::POTENTIAL_NEW_ARRAY) != 0;
    *options &= ~TypeAnnotationParsingOptions::POTENTIAL_NEW_ARRAY;
    lexer::SourcePosition startLoc = Lexer()->GetToken().Start();
    Lexer()->NextToken();  // eat '<'

    ir::AstNode *typeParamInst;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT &&
        Lexer()->Lookahead() == static_cast<char32_t>(EXPRESSION_FORMAT_NODE)) {
        typeParamInst = ParseTypeParametersFormatPlaceholder();
    } else {
        typeParamInst = ParseTypeParameterInstantiationImpl(options);
    }

    if (inPotentialNewArray) {
        *options |= TypeAnnotationParsingOptions::POTENTIAL_NEW_ARRAY;
    }

    if (Lexer()->GetToken().Type() != lexer::TokenType::PUNCTUATOR_GREATER_THAN) {
        if ((*options & TypeAnnotationParsingOptions::THROW_ERROR) == 0) {
            return nullptr;
        }
        ThrowSyntaxError("Expected closing '>'.");
    }

    lexer::SourcePosition endLoc = Lexer()->GetToken().End();
    Lexer()->NextToken();  // eat '>'

    if (typeParamInst != nullptr) {
        typeParamInst->SetRange({startLoc, endLoc});
        return typeParamInst->AsTSTypeParameterInstantiation();
    }

    return nullptr;
}

ir::Statement *TypedParser::ParseDeclareAndDecorators(StatementParsingFlags flags)
{
    ArenaVector<ir::Decorator *> decorators(Allocator()->Adapter());

    ParseDecorators(decorators);

    if (Lexer()->GetToken().KeywordType() == lexer::TokenType::KEYW_DECLARE) {
        CheckDeclare();
    }

    ir::Statement *stmt = TypedParser::ParseStatement(flags);

    GetContext().Status() &= ~ParserStatus::IN_AMBIENT_CONTEXT;
    AddDecorators(stmt, decorators);

    return stmt;
}

void TypedParser::ConvertThisKeywordToIdentIfNecessary()
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::KEYW_THIS) {
        Lexer()->GetToken().SetTokenType(lexer::TokenType::LITERAL_IDENT);
    }
}

ir::VariableDeclarator *TypedParser::ParseVariableDeclarator(ir::Expression *init, lexer::SourcePosition startLoc,
                                                             VariableParsingFlags flags)
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        return ParseVariableDeclaratorInitializer(init, flags, startLoc);
    }

    if (((flags & VariableParsingFlags::CONST) != 0) && ((flags & VariableParsingFlags::ACCEPT_CONST_NO_INIT) == 0) &&
        !InAmbientContext()) {
        ThrowSyntaxError("Missing initializer in const declaration");
    }

    if (((flags & VariableParsingFlags::IN_FOR) == 0) && (init->IsArrayPattern() || init->IsObjectPattern())) {
        ThrowSyntaxError("Missing initializer in destructuring declaration");
    }

    lexer::SourcePosition endLoc = init->End();
    auto declarator = AllocNode<ir::VariableDeclarator>(GetFlag(flags), init);
    declarator->SetRange({startLoc, endLoc});

    return declarator;
}

void TypedParser::ParsePotentialOptionalFunctionParameter(ir::AnnotatedExpression *returnNode)
{
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_QUESTION_MARK) {
        ASSERT(returnNode->IsIdentifier() || returnNode->IsObjectPattern() || returnNode->IsArrayPattern() ||
               returnNode->IsRestElement());

        switch (returnNode->Type()) {
            case ir::AstNodeType::IDENTIFIER: {
                returnNode->AsIdentifier()->SetOptional(true);
                break;
            }
            case ir::AstNodeType::OBJECT_PATTERN: {
                returnNode->AsObjectPattern()->SetOptional(true);
                break;
            }
            case ir::AstNodeType::ARRAY_PATTERN: {
                returnNode->AsArrayPattern()->SetOptional(true);
                break;
            }
            case ir::AstNodeType::REST_ELEMENT: {
                returnNode->AsRestElement()->SetOptional(true);
                break;
            }
            default: {
                UNREACHABLE();
            }
        }

        Lexer()->NextToken();  // eat '?'
    }

    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_COLON) {
        Lexer()->NextToken();  // eat ':'
        TypeAnnotationParsingOptions options = TypeAnnotationParsingOptions::THROW_ERROR;
        returnNode->SetTsTypeAnnotation(ParseTypeAnnotation(&options));
    }
}

ParserStatus TypedParser::ValidateArrowParameterAssignment(ir::AssignmentExpression *assignmentExpr)
{
    if (assignmentExpr->Right()->IsYieldExpression()) {
        ThrowSyntaxError("yield is not allowed in arrow function parameters");
    }

    if (assignmentExpr->Right()->IsAwaitExpression()) {
        ThrowSyntaxError("await is not allowed in arrow function parameters");
    }

    if (!assignmentExpr->ConvertibleToAssignmentPattern()) {
        ThrowSyntaxError("Invalid destructuring assignment target");
    }

    if (assignmentExpr->Left()->IsIdentifier() && assignmentExpr->Left()->AsIdentifier()->IsOptional()) {
        ThrowSyntaxError("Parameter cannot have question mark and initializer.", assignmentExpr->Start());
    }

    ValidateArrowParameterBindings(assignmentExpr);
    return ParserStatus::HAS_COMPLEX_PARAM;
}

ParserStatus TypedParser::ValidateArrowParameterArray(ir::ArrayExpression *arrayPattern)
{
    if (!arrayPattern->ConvertibleToArrayPattern()) {
        ThrowSyntaxError("Invalid destructuring assignment target");
    }

    if (!InAmbientContext() && ((GetContext().Status() & ParserStatus::FUNCTION) != 0) && arrayPattern->IsOptional()) {
        ThrowSyntaxError("A binding pattern parameter cannot be optional in an implementation signature.",
                         arrayPattern->Start());
    }

    ValidateArrowParameterBindings(arrayPattern);
    return ParserStatus::HAS_COMPLEX_PARAM;
}

ParserStatus TypedParser::ValidateArrowParameterObject(ir::ObjectExpression *objectPattern)
{
    if (!objectPattern->ConvertibleToObjectPattern()) {
        ThrowSyntaxError("Invalid destructuring assignment target");
    }

    if (!InAmbientContext() && ((GetContext().Status() & ParserStatus::FUNCTION) != 0) && objectPattern->IsOptional()) {
        ThrowSyntaxError("A binding pattern parameter cannot be optional in an implementation signature.",
                         objectPattern->Start());
    }

    ValidateArrowParameterBindings(objectPattern);
    return ParserStatus::HAS_COMPLEX_PARAM;
}

ParserStatus TypedParser::ValidateArrowParameter(ir::Expression *expr, bool *seenOptional)
{
    switch (expr->Type()) {
        case ir::AstNodeType::SPREAD_ELEMENT: {
            if (!expr->AsSpreadElement()->ConvertibleToRest(true)) {
                ThrowSyntaxError("Invalid rest element.");
            }

            [[fallthrough]];
        }
        case ir::AstNodeType::REST_ELEMENT: {
            if (expr->AsRestElement()->IsOptional()) {
                ThrowSyntaxError("A rest parameter cannot be optional.", expr->Start());
            }

            ValidateArrowParameterBindings(expr->AsRestElement()->Argument());
            return ParserStatus::HAS_COMPLEX_PARAM;
        }
        case ir::AstNodeType::IDENTIFIER: {
            const util::StringView &identifier = expr->AsIdentifier()->Name();
            bool isOptional = expr->AsIdentifier()->IsOptional();
            if ((*seenOptional) && !isOptional) {
                ThrowSyntaxError("A required parameter cannot follow an optional parameter.", expr->Start());
            }

            (*seenOptional) |= isOptional;

            if (identifier.Is("arguments")) {
                ThrowSyntaxError("Binding 'arguments' in strict mode is invalid");
            } else if (identifier.Is("eval")) {
                ThrowSyntaxError("Binding 'eval' in strict mode is invalid");
            }

            ValidateArrowParameterBindings(expr);
            return ParserStatus::NO_OPTS;
        }
        case ir::AstNodeType::OBJECT_EXPRESSION:
            return ValidateArrowParameterObject(expr->AsObjectExpression());
        case ir::AstNodeType::ARRAY_EXPRESSION:
            return ValidateArrowParameterArray(expr->AsArrayExpression());
        case ir::AstNodeType::ASSIGNMENT_EXPRESSION:
            return ValidateArrowParameterAssignment(expr->AsAssignmentExpression());
        default:
            break;
    }
    ThrowSyntaxError("Insufficient formal parameter in arrow function.");
    return ParserStatus::NO_OPTS;
}

}  // namespace ark::es2panda::parser
