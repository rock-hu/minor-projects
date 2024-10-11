/*
 * Copyright (c) 2021 - 2023 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_PARSER_CORE_TS_LEXER_H
#define ES2PANDA_PARSER_CORE_TS_LEXER_H

#include "lexer/lexer.h"

namespace ark::es2panda::lexer {
class TSLexer final : public Lexer {
public:
    explicit TSLexer(const parser::ParserContext *parserContext) : Lexer(parserContext, false)
    {
        SkipWhiteSpaces();
    }

    NO_COPY_SEMANTIC(TSLexer);
    NO_MOVE_SEMANTIC(TSLexer);
    ~TSLexer() override = default;
    // NOLINTNEXTLINE(google-default-arguments)
    void NextToken(NextTokenFlags flags = NextTokenFlags::NONE) override;
};
}  // namespace ark::es2panda::lexer

#endif
