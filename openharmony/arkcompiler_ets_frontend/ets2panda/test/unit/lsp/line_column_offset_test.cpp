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

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdio>
#include "lsp_api_test.h"
#include "public/es2panda_lib.h"
#include "lsp/include/internal_api.h"
#include "lsp/include/line_column_offset.h"

using ark::es2panda::lsp::Initializer;

class LCOTests : public LSPAPITests {};

TEST_F(LCOTests, LCOPositionIsZero)
{
    auto posZero = 0;
    Initializer initializer = Initializer();
    es2panda_Context *context =
        initializer.CreateContext("not-found-node.ets", ES2PANDA_STATE_CHECKED,
                                  "interface X\n{ name: string};\nlet variable: X = { name: 'Test' };\n");
    auto lineAndChar = ark::es2panda::lsp::ToLineColumnOffset(context, posZero);

    auto expectedLine = 0;
    ASSERT_EQ(lineAndChar.GetLine(), expectedLine);
    ASSERT_EQ(lineAndChar.GetCharacter(), posZero);

    initializer.DestroyContext(context);
}
TEST_F(LCOTests, LCOPositionIsCorrect)
{
    const size_t position = 33;
    Initializer initializer = Initializer();
    es2panda_Context *context =
        initializer.CreateContext("not-found-node.ets", ES2PANDA_STATE_CHECKED,
                                  "interface X\n{ name: string};\nlet variable: X = { name: 'Test' };\n");
    auto lineAndChar = ark::es2panda::lsp::ToLineColumnOffset(context, position);

    auto expectedLine = 2;
    ASSERT_EQ(lineAndChar.GetLine(), expectedLine);
    ASSERT_EQ(lineAndChar.GetCharacter(), position);

    initializer.DestroyContext(context);
}
