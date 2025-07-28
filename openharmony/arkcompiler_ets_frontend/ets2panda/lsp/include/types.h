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

#ifndef ES2PANDA_LSP_INCLUDE_TYPES_H
#define ES2PANDA_LSP_INCLUDE_TYPES_H

#include <cstddef>
#include <string>
#include <vector>
#include "formatting/formatting.h"
#include "user_preferences.h"

// NOLINTBEGIN

#ifdef __cplusplus
extern "C" {
#endif

struct TextSpan {
    size_t start;
    size_t length;
    TextSpan(size_t s, size_t l) : start(s), length(l) {}
    bool operator==(const TextSpan &other) const
    {
        return start == other.start && length == other.length;
    }
    bool operator!=(const TextSpan &other) const
    {
        return !(*this == other);
    }
};

struct SymbolDisplayPart {
private:
    std::string text_;
    std::string kind_;

public:
    explicit SymbolDisplayPart(std::string text = "", std::string kind = "")
        : text_ {std::move(text)}, kind_ {std::move(kind)}
    {
    }

    void SetText(const std::string &newText)
    {
        text_ = newText;
    }
    void SetKind(const std::string &newKind)
    {
        kind_ = newKind;
    }
    std::string GetText() const
    {
        return text_;
    }
    std::string GetKind() const
    {
        return kind_;
    }

    bool operator==(const SymbolDisplayPart &other) const
    {
        return text_ == other.text_ && kind_ == other.kind_;
    }
    bool operator!=(const SymbolDisplayPart &other) const
    {
        return !(*this == other);
    }
};

struct TextChange {
    TextSpan span;
    std::string newText;
    TextChange(TextSpan s, const std::string &t) : span(s), newText(t) {}
};

struct FileTextChanges {
    std::string fileName;
    std::vector<TextChange> textChanges;
    FileTextChanges(const std::string &f, const std::vector<TextChange> &t) : fileName(f), textChanges(t) {}
    FileTextChanges() = default;
};

enum class InlayHintKind {
    TYPE,
    PARAMETER,
    ENUM,
};

struct InlayHint {
    std::string text;
    int number;
    InlayHintKind kind;
    bool whitespaceBefore;
    bool whitespaceAfter;
};

struct InlayHintList {
    std::vector<InlayHint> hints;
};

struct SignatureHelpParameter {
private:
    std::string name_;
    std::vector<SymbolDisplayPart> documentation_;
    std::vector<SymbolDisplayPart> displayParts_;

public:
    void SetName(const std::string &newName)
    {
        this->name_ = newName;
    }
    void SetDocumentation(const SymbolDisplayPart &part)
    {
        documentation_.push_back(part);
    }

    void SetDisplayParts(const SymbolDisplayPart &part)
    {
        displayParts_.push_back(part);
    }
    const std::string &GetName() const
    {
        return name_;
    }
    std::string &GetName()
    {
        return name_;
    }
    const std::vector<SymbolDisplayPart> &GetDocumentation() const
    {
        return documentation_;
    }
    const std::vector<SymbolDisplayPart> &GetDisplayParts() const
    {
        return displayParts_;
    }
    void Clear()
    {
        displayParts_.clear();
        documentation_.clear();
    }
};
struct SignatureHelpItem {
private:
    std::vector<SymbolDisplayPart> prefixDisplayParts_;
    std::vector<SymbolDisplayPart> suffixDisplayParts_;
    std::vector<SymbolDisplayPart> separatorDisplayParts_;
    std::vector<SignatureHelpParameter> parameters_;
    std::vector<SymbolDisplayPart> documentation_;

public:
    void SetPrefixDisplayParts(const SymbolDisplayPart &part)
    {
        prefixDisplayParts_.push_back(part);
    }

    void SetSuffixDisplayParts(const SymbolDisplayPart &part)
    {
        suffixDisplayParts_.push_back(part);
    }
    void SetSeparatorDisplayParts(const SymbolDisplayPart &part)
    {
        separatorDisplayParts_.push_back(part);
    }
    void SetPrefixDisplayParts(const std::string &text, const std::string &kind)
    {
        prefixDisplayParts_.emplace_back(SymbolDisplayPart(text, kind));
    }

    void SetParameters(SignatureHelpParameter &parameter)
    {
        parameters_.push_back(parameter);
    }
    void SetDocumentation(const std::string &text, const std::string &kind)
    {
        documentation_.emplace_back(SymbolDisplayPart(text, kind));
    }

    const std::vector<SymbolDisplayPart> &GetPrefixDisplayParts() const
    {
        return prefixDisplayParts_;
    }
    const std::vector<SymbolDisplayPart> &GetSuffixDisplayParts() const
    {
        return suffixDisplayParts_;
    }
    const std::vector<SymbolDisplayPart> &GetSeparatorDisplayParts() const
    {
        return separatorDisplayParts_;
    }
    const std::vector<SignatureHelpParameter> &GetParameters() const
    {
        return parameters_;
    }
    const std::vector<SymbolDisplayPart> &GetDocumentation() const
    {
        return documentation_;
    }
    void Clear()
    {
        prefixDisplayParts_.clear();
        suffixDisplayParts_.clear();
        separatorDisplayParts_.clear();
        for (auto parameter : parameters_) {
            parameter.Clear();
        }
        parameters_.clear();
        documentation_.clear();
    }
};

struct LanguageServiceHost {
    std::string name = "lsp";
};

struct TextChangesContext {
    LanguageServiceHost host = {};
    ark::es2panda::lsp::FormatContext formatContext;
    ark::es2panda::lsp::UserPreferences preferences;
};

struct SignatureHelpItems {
private:
    std::vector<SignatureHelpItem> items_;
    TextSpan applicableSpan_ {0, 0};
    size_t selectedItemIndex_ {0};
    size_t argumentIndex_ {0};
    size_t argumentCount_ {0};

public:
    void SetItems(const SignatureHelpItem &item)
    {
        items_.push_back(item);
    }
    void SetApplicableSpan(const size_t &start, const size_t &line)
    {
        applicableSpan_.start = start;
        applicableSpan_.length = line;
        applicableSpan_.start = start;
        applicableSpan_.length = line;
    }
    void SetSelectedItemIndex(const size_t &index)
    {
        selectedItemIndex_ = index;
    }
    void SetArgumentIndex(const size_t &index)
    {
        argumentIndex_ = index;
    }
    void SetArgumentCount(const size_t &count)
    {
        argumentCount_ = count;
    }

    SignatureHelpItem &GetItem(size_t index)
    {
        return items_[index];
    }
    const std::vector<SignatureHelpItem> &GetItems() const
    {
        return items_;
    }
    const TextSpan &GetApplicableSpan() const
    {
        return applicableSpan_;
    }
    size_t GetSelectedItemIndex() const
    {
        return selectedItemIndex_;
    }
    size_t GetArgumentIndex() const
    {
        return argumentIndex_;
    }
    size_t GetArgumentCount() const
    {
        return argumentCount_;
    }
    void Clear()
    {
        for (auto item : items_) {
            item.Clear();
        }
        items_.clear();
    }
};

#ifdef __cplusplus
}
#endif

SymbolDisplayPart CreatePunctuation(std::string punc);
SymbolDisplayPart CreateKeyword(std::string keyword);
SymbolDisplayPart CreateSpace();
SymbolDisplayPart CreateText(std::string text);
SymbolDisplayPart CreateClassName(std::string className);
SymbolDisplayPart CreateFunctionName(std::string functionName);
SymbolDisplayPart CreateTypeName(std::string typeName);
SymbolDisplayPart CreateEnumName(std::string enumName);
SymbolDisplayPart CreateEnumMember(std::string enumMember);
SymbolDisplayPart CreateInterface(std::string interface);
SymbolDisplayPart CreateTypeParameter(std::string typeParameter);
SymbolDisplayPart CreateFunctionParameter(std::string functionParameter);
SymbolDisplayPart CreateOperator(std::string oper);
SymbolDisplayPart CreateReturnType(std::string returnType);
SymbolDisplayPart CreateProperty(std::string property);
SymbolDisplayPart CreateNamespace(std::string name);
SymbolDisplayPart SignatureCreateStructName(const std::string &name);
SymbolDisplayPart SignatureCreateParameterName(std::string &type);

// NOLINTEND

#endif