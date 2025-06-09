/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rename.h"
#include "get_adjusted_location.h"
#include "macros.h"
#include "lexer/token/letters.h"
#include "public/public.h"
#include <string>
#include <utility>

namespace ark::es2panda::lsp {

constexpr size_t FIRST_CHAR_INDEX = 0;
constexpr size_t QUOTE_END_OFFSET = 2;
constexpr size_t MIN_QUOTED_LENGTH = 2;
constexpr size_t QUOTE_START_OFFSET = 1;

RenameInfoType GetRenameInfo(es2panda_Context *context, size_t pos)
{
    auto ctx = reinterpret_cast<public_lib::Context *>(context);
    auto checker = reinterpret_cast<public_lib::Context *>(ctx)->checker->AsETSChecker();
    auto program = reinterpret_cast<public_lib::Context *>(ctx)->parserProgram;
    auto node = GetAdjustedLocation(GetTouchingPropertyName(context, pos), true, ctx->allocator);
    if (node.has_value() && NodeIsEligibleForRename(node.value())) {
        auto renameInfo = GetRenameInfoForNode(node.value(), checker, program);
        if (renameInfo.has_value()) {
            return renameInfo.value();
        }
    }
    const std::string diagnosticMessage = "You cannot rename this element";
    return GetRenameInfoError(diagnosticMessage);
}

RenameInfoFailure GetRenameInfoError(std::string diagnosticMessage)
{
    return RenameInfoFailure(false, std::move(diagnosticMessage));
}

RenameInfoSuccess GetRenameInfoSuccess(std::string displayName, std::string fullDisplayName, std::string kind,
                                       std::string kindModifiers, ir::AstNode *node)
{
    TextSpan triggerSpan = CreateTriggerSpanForNode(node);
    return RenameInfoSuccess(true, "", std::move(kind), std::move(displayName), std::move(fullDisplayName),
                             std::move(kindModifiers), triggerSpan);
}

TextSpan CreateTriggerSpanForNode(ir::AstNode *node)
{
    TextSpan span(node->Range().start.index, node->Range().end.index - node->Range().start.index);

    if (node->IsStringLiteral()) {
        span.start = span.start + QUOTE_START_OFFSET;
        span.length = span.length - QUOTE_END_OFFSET;
    }

    return span;
}

std::optional<RenameInfoType> GetRenameInfoForNode(ir::AstNode *node, checker::ETSChecker *checker,
                                                   parser::Program *program)
{
    if (node->IsStringLiteral()) {
        auto type = GetContextualTypeFromParentOrAncestorTypeNode(node, checker);
        if (type) {
            const std::string kind = "string";
            return GetRenameInfoSuccess(node->AsStringLiteral()->ToString(), node->AsStringLiteral()->ToString(), kind,
                                        "", node);
        }
    }
    if (node->IsLabelledStatement() ||
        (node->IsIdentifier() && (node->Parent()->IsContinueStatement() || node->Parent()->IsBreakStatement()))) {
        const std::string name = GetTextOfNode(node, program);
        const std::string kind = "label";
        return GetRenameInfoSuccess(name, name, kind, "", node);
    }

    if (node->IsIdentifier()) {
        return std::nullopt;
    }

    if (node->IsStringLiteral() && TryGetImportFromModuleSpecifier(node) != nullptr) {
        return GetRenameInfoForModule(node, program);
    }

    const std::string kind = GetNodeKindForRenameInfo(node);

    std::optional<std::string> specifierName;
    if ((IsImportOrExportSpecifierName(node) || IsStringOrNumericLiteralLike(node)) &&
        node->Parent()->Type() == ir::AstNodeType::PROPERTY) {
        specifierName = StripQuotes(static_cast<std::string>(node->AsIdentifier()->Name()));
    } else {
        specifierName = std::nullopt;
    }
    const std::string displayName = specifierName.has_value() ? specifierName.value() : "";
    const std::string fullDisplayName = specifierName.has_value() ? specifierName.value() : "";
    return GetRenameInfoSuccess(displayName, fullDisplayName, kind, "", node);
}

std::optional<checker::VerifiedType> GetContextualTypeFromParentOrAncestorTypeNode(ir::AstNode *node,
                                                                                   checker::ETSChecker *checker)
{
    auto contextualType = node->Check(checker);
    if (contextualType != nullptr) {
        return contextualType;
    }

    auto ancestorTypeNode = node;

    while (ancestorTypeNode != nullptr) {
        if (IsValidAncestorType(ancestorTypeNode->Type())) {
            break;
        }
        ancestorTypeNode = ancestorTypeNode->Parent();
    }

    if (ancestorTypeNode == nullptr) {
        return std::nullopt;
    }
    return ancestorTypeNode->Check(checker);
}

std::string GetTextOfNode(ir::AstNode *node, parser::Program *program)
{
    auto sourceCode = program->SourceCode();
    return GetSourceTextOfNodeFromSourceFile(sourceCode, node);
}

std::string GetSourceTextOfNodeFromSourceFile(util::StringView sourceCode, ir::AstNode *node)
{
    if (NodeIsMissing(node)) {
        return "";
    }

    size_t pos = node->Range().start.index;
    size_t end = node->Range().end.index;

    auto text = std::string(sourceCode.Substr(pos, end));
    return text;
}

ir::AstNode *TryGetImportFromModuleSpecifier(ir::AstNode *node)
{
    if (node == nullptr) {
        return nullptr;
    }

    ir::AstNode *parent = node->Parent();
    if (parent == nullptr) {
        return nullptr;
    }

    switch (parent->Type()) {
        case ir::AstNodeType::IMPORT_DECLARATION:
        case ir::AstNodeType::ETS_IMPORT_DECLARATION:
        case ir::AstNodeType::EXPORT_DEFAULT_DECLARATION:
            return parent;
        case ir::AstNodeType::TS_EXTERNAL_MODULE_REFERENCE:
            return parent->Parent();
        case ir::AstNodeType::CALL_EXPRESSION:
            return parent;
        case ir::AstNodeType::TS_LITERAL_TYPE:
            ASSERT(node->IsStringLiteral());
            if (parent->Parent()->IsTSImportType()) {
                return parent->Parent();
            }
            return nullptr;
        default:
            return nullptr;
    }
}

bool NodeIsMissing(ir::AstNode *node)
{
    if (node == nullptr) {
        return true;
    }
    size_t pos = node->Range().start.index;
    size_t end = node->Range().end.index;
    return pos == end;
}

bool IsValidAncestorType(ir::AstNodeType type)
{
    const int countAncestorType = 10;
    const std::array<ir::AstNodeType, countAncestorType> validTypes = {
        ir::AstNodeType::TS_ANY_KEYWORD,    ir::AstNodeType::TS_UNKNOWN_KEYWORD, ir::AstNodeType::TS_NUMBER_KEYWORD,
        ir::AstNodeType::TS_BIGINT_KEYWORD, ir::AstNodeType::TS_OBJECT_KEYWORD,  ir::AstNodeType::TS_BOOLEAN_KEYWORD,
        ir::AstNodeType::TS_STRING_KEYWORD, ir::AstNodeType::TS_VOID_KEYWORD,    ir::AstNodeType::TS_UNDEFINED_KEYWORD,
        ir::AstNodeType::TS_NEVER_KEYWORD};
    return std::find(validTypes.begin(), validTypes.end(), type) != validTypes.end();
}

std::string StripQuotes(std::string name)
{
    auto length = name.length();
    if (length >= MIN_QUOTED_LENGTH && name[FIRST_CHAR_INDEX] == name[length - QUOTE_START_OFFSET] &&
        IsQuoteOrBacktick(static_cast<int>(name[FIRST_CHAR_INDEX]))) {
        return name.substr(QUOTE_START_OFFSET, length - QUOTE_END_OFFSET);
    }
    return name;
}

bool IsQuoteOrBacktick(int charCode)
{
    return charCode == lexer::LEX_CHAR_SINGLE_QUOTE || charCode == lexer::LEX_CHAR_DOUBLE_QUOTE ||
           charCode == lexer::LEX_CHAR_BACK_TICK;
}

std::optional<RenameInfoSuccess> GetRenameInfoForModule(ir::AstNode *node, parser::Program *program)
{
    auto moduleSourceFile = program->SourceFile();
    if (moduleSourceFile.GetPath().Empty()) {
        return std::nullopt;
    }
    std::string suffix = "/index";
    std::optional<std::string> withoutIndex;
    if (util::Helpers::EndsWith(node->AsStringLiteral()->ToString(), suffix)) {
        withoutIndex = std::nullopt;
    } else {
        std::string name = node->AsStringLiteral()->ToString();
        size_t dotPos = name.find_last_of('.');
        name = dotPos == std::string::npos ? name : name.substr(FIRST_CHAR_INDEX, dotPos);
        if (util::Helpers::EndsWith(name, suffix)) {
            withoutIndex = name.substr(FIRST_CHAR_INDEX, name.length() - suffix.length());
        } else {
            withoutIndex = std::nullopt;
        }
    }
    std::string name =
        withoutIndex.has_value() ? static_cast<std::string>(withoutIndex.value()) : node->AsStringLiteral()->ToString();
    std::string displayName =
        withoutIndex.has_value() ? static_cast<std::string>(withoutIndex.value()) : node->AsStringLiteral()->ToString();
    std::string fullDisplayName =
        withoutIndex.has_value() ? static_cast<std::string>(withoutIndex.value()) : node->AsStringLiteral()->ToString();
    std::string kind =
        withoutIndex.has_value() ? static_cast<std::string>("directory") : static_cast<std::string>("module");
    auto indexAfterLastSlash = node->AsStringLiteral()->ToString().find_last_of('/') + 1;
    auto triggerSpan = TextSpan(node->Range().start.index + 1 + indexAfterLastSlash,
                                node->AsStringLiteral()->ToString().length() - indexAfterLastSlash);
    return RenameInfoSuccess(true, std::move(name), std::move(kind), std::move(displayName), std::move(fullDisplayName),
                             "", triggerSpan);
}

std::string GetNodeKindForRenameInfo(ir::AstNode *node)
{
    switch (node->Type()) {
        case ir::AstNodeType::TS_ENUM_DECLARATION:
            return "enum";
        case ir::AstNodeType::TS_TYPE_ALIAS_DECLARATION:
            return "type";
        case ir::AstNodeType::TS_INTERFACE_DECLARATION:
            return "interface";
        case ir::AstNodeType::TS_TYPE_PARAMETER:
            return "type parameter";
        case ir::AstNodeType::TS_ENUM_MEMBER:
            return "enum member";
        case ir::AstNodeType::TS_MODULE_DECLARATION:
            return "module";
        default:
            return "";
    }
}

bool IsImportOrExportSpecifierName(ir::AstNode *node)
{
    return (node->IsImportSpecifier() || node->IsExportSpecifier()) || node->IsIdentifier();
}

bool IsStringOrNumericLiteralLike(ir::AstNode *node)
{
    return node->IsStringLiteral() || node->IsNumberLiteral();
}

bool NodeIsEligibleForRename(ir::AstNode *node)
{
    switch (node->Type()) {
        case ir::AstNodeType::IDENTIFIER:
        case ir::AstNodeType::STRING_LITERAL:
        case ir::AstNodeType::TEMPLATE_LITERAL:
        case ir::AstNodeType::THIS_EXPRESSION:
            return true;
        case ir::AstNodeType::NUMBER_LITERAL:
            return IsLiteralNameOfPropertyDeclarationOrIndexAccess(node->AsNumberLiteral());
        default:
            return false;
    }
}

bool IsLiteralNameOfPropertyDeclarationOrIndexAccess(ir::AstNode *node)
{
    switch (node->Parent()->Type()) {
        case ir::AstNodeType::PROPERTY:
        case ir::AstNodeType::TS_PROPERTY_SIGNATURE:
        case ir::AstNodeType::ASSIGNMENT_EXPRESSION:
        case ir::AstNodeType::TS_ENUM_MEMBER:
        case ir::AstNodeType::FUNCTION_DECLARATION:
        case ir::AstNodeType::TS_METHOD_SIGNATURE:
        case ir::AstNodeType::TS_MODULE_DECLARATION:
            return GetNameOfDeclaration(node->Parent()) == node;
        case ir::AstNodeType::TS_LITERAL_TYPE:
            return node->Parent()->Parent()->Type() == ir::AstNodeType::TS_INDEXED_ACCESS_TYPE;
        default:
            return false;
    }
}

ir::AstNode *GetNameOfDeclaration(ir::AstNode *node)
{
    if (node == nullptr) {
        return nullptr;
    }
    if (GetNonAssignedNameOfDeclaration(node) != nullptr) {
        return GetNonAssignedNameOfDeclaration(node);
    }
    if (node->IsFunctionExpression() || node->IsArrowFunctionExpression() || node->IsClassExpression()) {
        return GetAssignedName(node);
    }
    return nullptr;
}

ir::AstNode *GetNonAssignedNameOfDeclaration(ir::AstNode *node)
{
    if (node->Type() == ir::AstNodeType::IDENTIFIER) {
        return node->AsIdentifier();
    }
    if (node->Type() == ir::AstNodeType::CALL_EXPRESSION || node->Type() == ir::AstNodeType::BINARY_EXPRESSION) {
        if (node->AsBinaryExpression()->IsExportedType() || node->AsBinaryExpression()->IsTSThisType() ||
            node->AsBinaryExpression()->IsProperty()) {
            return node->AsBinaryExpression();
        }
        if (node->AsCallExpression()->IsExportedType() || node->AsCallExpression()->IsTSThisType() ||
            node->AsCallExpression()->IsProperty()) {
            return node->AsCallExpression();
        }
        return nullptr;
    }
    if (node->Type() == ir::AstNodeType::EXPORT_DEFAULT_DECLARATION) {
        return node->AsExportAllDeclaration()->IsIdentifier() ? node->AsExportAllDeclaration() : nullptr;
    }
    if (node->IsMemberExpression() &&
        node->AsMemberExpression()->HasMemberKind(ir::MemberExpressionKind::ELEMENT_ACCESS)) {
        return node->AsMemberExpression();
    }
    return node->IsNamedType() ? node->AsNamedType() : nullptr;
}

ir::AstNode *GetAssignedName(ir::AstNode *node)
{
    auto parent = node->Parent();
    if (parent == nullptr) {
        return nullptr;
    }

    if (parent->IsProperty()) {
        return parent->AsProperty();
    }
    if (parent->IsBinaryExpression() && node == parent->AsBinaryExpression()->Right()) {
        if (parent->AsBinaryExpression()->Left()->IsIdentifier() ||
            parent->AsBinaryExpression()->Left()->Type() == ir::AstNodeType::TS_INDEXED_ACCESS_TYPE) {
            return parent->AsBinaryExpression()->Left();
        }
    }
    if (parent->IsVariableDeclaration() && parent->IsIdentifier()) {
        return parent;
    }
    return nullptr;
}

}  // namespace ark::es2panda::lsp