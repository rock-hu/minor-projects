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

#ifndef REFACTOR_TYPES_H
#define REFACTOR_TYPES_H

#include "public/es2panda_lib.h"
#include "../cancellation_token.h"
#include "../user_preferences.h"
#include "../types.h"
#include "es2panda.h"
#include <string>
#include <string_view>
#include <vector>

namespace ark::es2panda::lsp {

struct RefactorEditInfo {
private:
    std::vector<FileTextChanges> fileTextChanges_;

public:
    explicit RefactorEditInfo(std::vector<FileTextChanges> fileTextChanges = {})
        : fileTextChanges_(std::move(fileTextChanges))
    {
    }

    std::vector<FileTextChanges> &GetFileTextChanges()
    {
        return fileTextChanges_;
    }
};

struct TextRange {
    size_t pos;
    size_t end;
};

struct RefactorContext {
    ark::es2panda::lsp::CancellationToken *cancellationToken = nullptr;
    ark::es2panda::lsp::UserPreferences *preferences = nullptr;
    TextRange span = {0, 0};
    es2panda_Context *context = nullptr;
    std::string kind;
};

using RefactorActionView = struct RefactorActionView {
    std::string_view name;
    std::string_view description;
    std::string_view kind;
};

using RefactorAction = struct RefactorAction {
    std::string name;
    std::string description;
    std::string kind;
};

using ApplicableRefactorInfo = struct ApplicableRefactorInfo {
    std::string name;
    std::string description;
    RefactorAction action;
};

namespace refactor_name {
constexpr std::string_view CONVERT_FUNCTION_REFACTOR_NAME = "Convert arrow function or function expression";
constexpr std::string_view CONVERT_EXPORT_REFACTOR_NAME = "Convert export";
constexpr std::string_view CONVERT_IMPORT_REFACTOR_NAME = "Convert import";
constexpr std::string_view CONVERT_TEMPLATE_REFACTOR_NAME = "Convert to template string";
constexpr std::string_view CONVERT_CHAIN_REFACTOR_NAME = "Convert to optional chain expression";
}  // namespace refactor_name

namespace refactor_description {
constexpr std::string_view CONVERT_FUNCTION_REFACTOR_DESC = "Convert arrow function or function expression";
constexpr std::string_view CONVERT_TEMPLATE_REFACTOR_DESC = "Convert to template string";
constexpr std::string_view CONVERT_CHAIN_REFACTOR_DESC = "Convert to optional chain expression";
}  // namespace refactor_description

class Refactor {
private:
    std::vector<std::string> kinds_;

public:
    bool IsKind(const std::string &kind) const;
    void AddKind(const std::string &kind);
    virtual ApplicableRefactorInfo GetAvailableActions(const RefactorContext &context) const = 0;

    virtual std::unique_ptr<RefactorEditInfo> GetEditsForAction(const RefactorContext &context,
                                                                const std::string &actionName) const = 0;
    virtual ~Refactor() = default;
    Refactor() = default;
    Refactor &operator=(const Refactor &other);
    Refactor &operator=(Refactor &&other);
    Refactor(const Refactor &other);
    Refactor(Refactor &&other);
};

}  // namespace ark::es2panda::lsp

#endif  // REFACTOR_TYPES_H