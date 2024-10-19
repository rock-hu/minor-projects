/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ETSparser.h"

#include "lexer/lexer.h"
#include "ir/typeNode.h"
#include "ir/expressions/identifier.h"
#include "ir/expressions/blockExpression.h"
#include "ir/expressions/sequenceExpression.h"
#include "ir/statements/blockStatement.h"
#include "ir/statements/expressionStatement.h"
#include "ir/base/methodDefinition.h"
#include "ir/ts/tsInterfaceBody.h"
#include "parser/parserStatusContext.h"

namespace ark::es2panda::parser {
//================================================================================================//
//  Methods to create AST node(s) from the specified string (part of valid ETS-code!)
//================================================================================================//

// NOLINTBEGIN(modernize-avoid-c-arrays)
inline constexpr char const FORMAT_SIGNATURE = '@';
inline constexpr char const TYPE_FORMAT_NODE = 'T';
inline constexpr char const GENERAL_FORMAT_NODE = 'N';
inline constexpr char const IDENTIFIER_FORMAT_NODE = 'I';

static constexpr char const INVALID_NUMBER_NODE[] = "Invalid node number in format expression.";
static constexpr char const INVALID_FORMAT_NODE[] = "Invalid node type in format expression.";
static constexpr char const INSERT_NODE_ABSENT[] = "There is no any node to insert at the placeholder position.";
static constexpr char const INVALID_INSERT_NODE[] =
    "Inserting node type differs from that required by format specification.";
static constexpr char const INVALID_CLASS_FIELD[] = "Cannot parse class field definition property.";
static constexpr char const INVALID_CLASS_METHOD[] = "Cannot parse class method definition property.";
// NOLINTEND(modernize-avoid-c-arrays)

ParserImpl::NodeFormatType ETSParser::GetFormatPlaceholderType() const
{
    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_FORMAT);
    Lexer()->NextToken();

    bool isArray = false;
    if (Lexer()->GetToken().Type() == lexer::TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET) {
        isArray = true;
        Lexer()->NextToken();
    }

    ASSERT(Lexer()->GetToken().Type() == lexer::TokenType::LITERAL_IDENT);
    char const *const identData = Lexer()->GetToken().Ident().Bytes();

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic, cert-err34-c)
    auto identNumber = std::atoi(identData + 1U);
    if (identNumber <= 0) {
        ThrowSyntaxError(INVALID_NUMBER_NODE, Lexer()->GetToken().Start());
    }

    return {isArray, *identData,
            static_cast<decltype(std::get<2>(std::declval<ParserImpl::NodeFormatType>()))>(identNumber - 1)};
}

ir::Expression *ETSParser::ParseExpressionFormatPlaceholder()
{
    if (insertingNodes_.empty()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (std::get<0>(nodeFormat)) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    if (auto const placeholderType = std::get<1>(nodeFormat); placeholderType == TYPE_FORMAT_NODE) {
        return ParseTypeFormatPlaceholder(std::make_optional(std::move(nodeFormat)));
    } else if (placeholderType == IDENTIFIER_FORMAT_NODE) {  // NOLINT(readability-else-after-return)
        return ParseIdentifierFormatPlaceholder(std::make_optional(std::move(nodeFormat)));
    } else if (placeholderType != EXPRESSION_FORMAT_NODE) {  // NOLINT(readability-else-after-return)
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsExpression()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    auto *const insertExpression = insertingNode->AsExpression();
    Lexer()->NextToken();
    return insertExpression;
}

ir::TypeNode *ETSParser::ParseTypeFormatPlaceholder(std::optional<ParserImpl::NodeFormatType> nodeFormat)
{
    if (!nodeFormat.has_value()) {
        if (insertingNodes_.empty()) {
            ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
        }

        nodeFormat = GetFormatPlaceholderType();
        if (std::get<0>(*nodeFormat) || std::get<1>(*nodeFormat) != TYPE_FORMAT_NODE) {
            ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
        }
    }

    auto const placeholderNumber = std::get<2>(*nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsExpression() || !insertingNode->AsExpression()->IsTypeNode()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    auto *const insertType = insertingNode->AsExpression()->AsTypeNode();
    Lexer()->NextToken();
    return insertType;
}

ir::Identifier *ETSParser::ParseIdentifierFormatPlaceholder(std::optional<ParserImpl::NodeFormatType> nodeFormat) const
{
    if (!nodeFormat.has_value()) {
        if (insertingNodes_.empty()) {
            ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
        }

        nodeFormat = GetFormatPlaceholderType();
        if (std::get<0>(*nodeFormat) || std::get<1>(*nodeFormat) != IDENTIFIER_FORMAT_NODE) {
            ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
        }
    }

    auto const placeholderNumber = std::get<2>(*nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsExpression() || !insertingNode->AsExpression()->IsIdentifier()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    auto *const insertIdentifier = insertingNode->AsExpression()->AsIdentifier();
    Lexer()->NextToken();
    return insertIdentifier;
}

ir::Statement *ETSParser::ParseStatementFormatPlaceholder() const
{
    if (insertingNodes_.empty()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (std::get<0>(nodeFormat) || std::get<1>(nodeFormat) != STATEMENT_FORMAT_NODE) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsStatement()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    Lexer()->NextToken();
    return insertingNode->AsStatement();
}

ir::AstNode *ETSParser::ParseTypeParametersFormatPlaceholder() const
{
    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (std::get<0>(nodeFormat) || std::get<1>(nodeFormat) != EXPRESSION_FORMAT_NODE) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    if (placeholderNumber >= insertingNodes_.size()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    auto *const insertingNode = insertingNodes_[placeholderNumber];
    if (insertingNode != nullptr && !insertingNode->IsTSTypeParameterDeclaration() &&
        !insertingNode->IsTSTypeParameterInstantiation()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    Lexer()->NextToken();
    return insertingNode;
}

ArenaVector<ir::AstNode *> &ETSParser::ParseAstNodesArrayFormatPlaceholder() const
{
    if (insertingNodes_.empty()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (!std::get<0>(nodeFormat) || std::get<1>(nodeFormat) != GENERAL_FORMAT_NODE) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsTSInterfaceBody()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    Lexer()->NextToken();
    return insertingNode->AsTSInterfaceBody()->Body();
}

ArenaVector<ir::Statement *> &ETSParser::ParseStatementsArrayFormatPlaceholder() const
{
    if (insertingNodes_.empty()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (!std::get<0>(nodeFormat) || std::get<1>(nodeFormat) != STATEMENT_FORMAT_NODE) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsBlockExpression()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    Lexer()->NextToken();
    return insertingNode->AsBlockExpression()->Statements();
}

ArenaVector<ir::Expression *> &ETSParser::ParseExpressionsArrayFormatPlaceholder() const
{
    if (insertingNodes_.empty()) {
        ThrowSyntaxError(INSERT_NODE_ABSENT, Lexer()->GetToken().Start());
    }

    ParserImpl::NodeFormatType nodeFormat = GetFormatPlaceholderType();
    if (!std::get<0>(nodeFormat) || std::get<1>(nodeFormat) != EXPRESSION_FORMAT_NODE) {
        ThrowSyntaxError(INVALID_FORMAT_NODE, Lexer()->GetToken().Start());
    }

    auto const placeholderNumber = std::get<2>(nodeFormat);
    auto *const insertingNode =
        placeholderNumber < insertingNodes_.size() ? insertingNodes_[placeholderNumber] : nullptr;
    if (insertingNode == nullptr || !insertingNode->IsSequenceExpression()) {
        ThrowSyntaxError(INVALID_INSERT_NODE, Lexer()->GetToken().Start());
    }

    Lexer()->NextToken();
    return insertingNode->AsSequenceExpression()->Sequence();
}

ir::Statement *ETSParser::CreateStatement(std::string_view const sourceCode)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    lexer::SourcePosition const startLoc = lexer->GetToken().Start();
    lexer->NextToken();

    auto statements = ParseStatementList(StatementParsingFlags::STMT_GLOBAL_LEXICAL);
    auto const statementNumber = statements.size();
    if (statementNumber == 0U) {
        return nullptr;
    }

    if (statementNumber == 1U) {
        return statements[0U];
    }

    auto *const blockStmt = AllocNode<ir::BlockStatement>(Allocator(), std::move(statements));
    blockStmt->SetRange({startLoc, lexer->GetToken().End()});

    for (auto *statement : blockStmt->Statements()) {
        statement->SetParent(blockStmt);
    }

    return blockStmt;
}

ir::Statement *ETSParser::CreateFormattedStatement(std::string_view const sourceCode,
                                                   std::vector<ir::AstNode *> &insertingNodes)
{
    insertingNodes_.swap(insertingNodes);
    auto const statement = CreateStatement(sourceCode);
    insertingNodes_.swap(insertingNodes);
    return statement;
}

ArenaVector<ir::Statement *> ETSParser::CreateStatements(std::string_view const sourceCode)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    lexer->NextToken();
    return ParseStatementList(StatementParsingFlags::STMT_GLOBAL_LEXICAL);
}

ArenaVector<ir::Statement *> ETSParser::CreateFormattedStatements(std::string_view const sourceCode,
                                                                  std::vector<ir::AstNode *> &insertingNodes)
{
    insertingNodes_.swap(insertingNodes);
    auto statements = CreateStatements(sourceCode);
    insertingNodes_.swap(insertingNodes);
    return statements;
}

//  NOTE: this method returns only a single (the first) class filed definition.
//  It seems that it is possible to have several of them be parsed at a time but nobody knows how...
ir::AstNode *ETSParser::CreateFormattedClassFieldDefinition(std::string_view sourceCode,
                                                            std::vector<ir::AstNode *> &insertingNodes)
{
    static ArenaVector<ir::AstNode *> const DUMMY_ARRAY {Allocator()->Adapter()};
    insertingNodes_.swap(insertingNodes);

    auto *const property = CreateClassElement(sourceCode, DUMMY_ARRAY, ir::ClassDefinitionModifiers::NONE);
    if (!property->IsTSInterfaceBody() || property->AsTSInterfaceBody()->Body().empty()) {
        ThrowSyntaxError(INVALID_CLASS_FIELD);
    }

    insertingNodes_.swap(insertingNodes);
    return property->AsTSInterfaceBody()->Body().front();
}

ir::AstNode *ETSParser::CreateFormattedClassMethodDefinition(std::string_view sourceCode,
                                                             std::vector<ir::AstNode *> &insertingNodes)
{
    static ArenaVector<ir::AstNode *> const DUMMY_ARRAY {Allocator()->Adapter()};
    insertingNodes_.swap(insertingNodes);

    auto *const property = CreateClassElement(sourceCode, DUMMY_ARRAY, ir::ClassDefinitionModifiers::NONE);
    if (!property->IsMethodDefinition()) {
        ThrowSyntaxError(INVALID_CLASS_METHOD);
    }

    insertingNodes_.swap(insertingNodes);
    return property;
}

ir::AstNode *ETSParser::CreateFormattedClassElement(std::string_view sourceCode,
                                                    std::vector<ir::AstNode *> &insertingNodes,
                                                    const ArenaVector<ir::AstNode *> &properties,
                                                    ir::ClassDefinitionModifiers modifiers)
{
    insertingNodes_.swap(insertingNodes);
    auto *const classElement = CreateClassElement(sourceCode, properties, modifiers);
    insertingNodes_.swap(insertingNodes);
    return classElement;
}

//  NOTE: the method has limited functionality - it returns 'ir::TSInterfaceBody' placeholder for the field
//  declaration(s) (properties themselves are in ->Body() member) and does not perform any check of the node returned.
//  Also the node isn't placed in the providing properties container.
ir::AstNode *ETSParser::CreateClassElement(std::string_view sourceCode, const ArenaVector<ir::AstNode *> &properties,
                                           ir::ClassDefinitionModifiers modifiers)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    auto savedCtx = SavedStatusContext<ParserStatus::IN_CLASS_BODY>(&GetContext());
    SavedClassPrivateContext classContext(this);

    lexer->NextToken(lexer::NextTokenFlags::KEYWORD_TO_IDENT);

    return ParseClassElement(properties, modifiers, ir::ModifierFlags::NONE);
}

ir::ClassDeclaration *ETSParser::CreateFormattedClassDeclaration(std::string_view sourceCode,
                                                                 std::vector<ir::AstNode *> &insertingNodes,
                                                                 bool const allowStatic)
{
    insertingNodes_.swap(insertingNodes);
    auto *const classDeclaration = CreateClassDeclaration(sourceCode, allowStatic);
    insertingNodes_.swap(insertingNodes);
    return classDeclaration;
}

ir::ClassDeclaration *ETSParser::CreateClassDeclaration(std::string_view sourceCode, bool allowStatic)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    auto savedCtx = SavedStatusContext<ParserStatus::IN_CLASS_BODY>(&GetContext());

    auto modifiers = ir::ClassDefinitionModifiers::ID_REQUIRED | ir::ClassDefinitionModifiers::CLASS_DECL;
    ir::ModifierFlags flags = ir::ModifierFlags::NONE;

    lexer->NextToken();

    switch (auto tokenType = Lexer()->GetToken().Type(); tokenType) {
        case lexer::TokenType::KEYW_STATIC: {
            if (!allowStatic) {
                ThrowUnexpectedToken(Lexer()->GetToken().Type());
            }
            [[fallthrough]];
        }
        case lexer::TokenType::KEYW_ABSTRACT:
        case lexer::TokenType::KEYW_FINAL: {
            flags = ParseClassModifiers();
            if (allowStatic && (flags & ir::ModifierFlags::STATIC) == 0U) {
                modifiers |= ir::ClassDefinitionModifiers::INNER;
            }

            if (auto const tokType = Lexer()->GetToken().Type(); tokType != lexer::TokenType::KEYW_CLASS) {
                ThrowUnexpectedToken(tokType);
            }
            [[fallthrough]];
        }
        case lexer::TokenType::KEYW_CLASS: {
            return ParseClassDeclaration(modifiers);
        }
        default: {
            ThrowUnexpectedToken(tokenType);
        }
    }
}

ir::MethodDefinition *ETSParser::CreateConstructorDefinition(ir::ModifierFlags modifiers,
                                                             std::string_view const sourceCode)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    auto const startLoc = Lexer()->GetToken().Start();
    Lexer()->NextToken();

    if (IsClassMethodModifier(Lexer()->GetToken().Type())) {
        modifiers |= ParseClassMethodModifiers(false);
    }

    if (Lexer()->GetToken().Type() != lexer::TokenType::KEYW_CONSTRUCTOR) {
        ThrowSyntaxError({"Unexpected token. 'Constructor' keyword is expected."});
    }

    if ((modifiers & ir::ModifierFlags::ASYNC) != 0) {
        ThrowSyntaxError({"Constructor should not be async."});
    }

    auto *memberName = AllocNode<ir::Identifier>(Lexer()->GetToken().Ident(), Allocator());
    modifiers |= ir::ModifierFlags::CONSTRUCTOR;
    Lexer()->NextToken();

    auto *const methodDefinition = ParseClassMethodDefinition(memberName, modifiers);
    methodDefinition->SetStart(startLoc);

    return methodDefinition;
}

ir::Expression *ETSParser::CreateExpression(std::string_view const sourceCode, ExpressionParseFlags const flags)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    lexer::SourcePosition const startLoc = lexer->GetToken().Start();
    lexer->NextToken();

    ir::Expression *returnExpression = ParseExpression(flags);
    returnExpression->SetRange({startLoc, lexer->GetToken().End()});

    return returnExpression;
}

ir::Expression *ETSParser::CreateFormattedExpression(std::string_view const sourceCode,
                                                     std::vector<ir::AstNode *> &insertingNodes)
{
    ir::Expression *returnExpression;
    insertingNodes_.swap(insertingNodes);

    if (auto statements = CreateStatements(sourceCode);
        statements.size() == 1U && statements.back()->IsExpressionStatement()) {
        returnExpression = statements.back()->AsExpressionStatement()->GetExpression();
    } else {
        returnExpression = AllocNode<ir::BlockExpression>(std::move(statements));
    }

    insertingNodes_.swap(insertingNodes);
    return returnExpression;
}

ir::Statement *ETSParser::CreateTopLevelStatement(std::string_view const sourceCode)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    lexer->NextToken();

    return ParseTopLevelStatement();
}

ir::Statement *ETSParser::CreateFormattedTopLevelStatement(std::string_view const sourceCode,
                                                           std::vector<ir::AstNode *> &insertingNodes)
{
    insertingNodes_.swap(insertingNodes);
    auto const statement = CreateTopLevelStatement(sourceCode);
    insertingNodes_.swap(insertingNodes);
    return statement;
}

ir::TypeNode *ETSParser::CreateTypeAnnotation(TypeAnnotationParsingOptions *options, std::string_view const sourceCode)
{
    util::UString source {sourceCode, Allocator()};
    auto const isp = InnerSourceParser(this);
    auto const lexer = InitLexer({GetContext().FormattingFileName(), source.View().Utf8()});

    lexer->NextToken();
    return ParseTypeAnnotation(options);
}
}  // namespace ark::es2panda::parser
