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

#ifndef PANDA_ERRORHANDLER_H
#define PANDA_ERRORHANDLER_H

#include <string_view>
#include "parser/program/program.h"
#include "lexer/token/sourceLocation.h"

namespace ark::es2panda::util {

class ErrorHandler {
public:
    explicit ErrorHandler(const parser::Program *program) : program_(program) {}

    [[noreturn]] void ThrowSyntaxError(std::string_view errorMessage, const lexer::SourcePosition &pos) const;

    [[noreturn]] static void ThrowSyntaxError(const parser::Program *program, std::string_view errorMessage,
                                              const lexer::SourcePosition &pos);

private:
    const parser::Program *program_;
};
}  // namespace ark::es2panda::util

#endif  // PANDA_ERRORHANDLER_H
