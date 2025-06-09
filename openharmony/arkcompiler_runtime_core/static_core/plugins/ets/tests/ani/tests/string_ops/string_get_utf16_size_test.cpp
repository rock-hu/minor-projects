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

#include "ani_gtest.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
namespace ark::ets::ani::testing {

class StringGetUtf16SizeTest : public AniTest {};

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_NullString)
{
    ani_size result = 0U;
    ani_status status = env_->String_GetUTF16Size(nullptr, &result);
    ASSERT_EQ(status, ANI_INVALID_ARGS);
}

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_NullResultPointer)
{
    const std::string example {"example"};
    ani_string string = nullptr;
    auto status = env_->String_NewUTF8(example.c_str(), example.size(), &string);
    ASSERT_EQ(status, ANI_OK);

    status = env_->String_GetUTF16Size(string, nullptr);
    ASSERT_EQ(status, ANI_INVALID_ARGS);
}

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_ValidString)
{
    const std::string example {"example"};
    ani_string string = nullptr;
    auto status = env_->String_NewUTF8(example.c_str(), example.size(), &string);
    ASSERT_EQ(status, ANI_OK);

    ani_size result = 0U;
    status = env_->String_GetUTF16Size(string, &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_EQ(result, example.size());
}

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_ValidMultibyteString)
{
    const std::string example = {"测测试emoji🙂🙂"};
    ani_string string = nullptr;
    auto status = env_->String_NewUTF8(example.c_str(), example.size(), &string);
    ASSERT_EQ(status, ANI_OK);

    ani_size result = 0U;
    const ani_size utf16Size = 12U;
    status = env_->String_GetUTF16Size(string, &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_EQ(result, utf16Size);
}

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_EmptyString)
{
    // NOLINTNEXTLINE(readability-redundant-string-init)
    const std::string example {""};
    ani_string string = nullptr;
    auto status = env_->String_NewUTF8(example.c_str(), example.size(), &string);
    ASSERT_EQ(status, ANI_OK);

    ani_size result = 0U;
    status = env_->String_GetUTF16Size(string, &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_EQ(result, example.size());
}

TEST_F(StringGetUtf16SizeTest, StringGetUtf16Size_Repeat)
{
    const std::string example = {"测测试emoji🙂🙂"};
    ani_string string = nullptr;
    auto status = env_->String_NewUTF8(example.c_str(), example.size(), &string);
    ASSERT_EQ(status, ANI_OK);
    ani_size result = 0U;
    const ani_size utf16Size = 12U;
    const int32_t loopCount = 3;
    for (int32_t i = 0; i < loopCount; ++i) {
        status = env_->String_GetUTF16Size(string, &result);
        ASSERT_EQ(status, ANI_OK);
        ASSERT_EQ(result, utf16Size);
    }
}
}  // namespace ark::ets::ani::testing

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)