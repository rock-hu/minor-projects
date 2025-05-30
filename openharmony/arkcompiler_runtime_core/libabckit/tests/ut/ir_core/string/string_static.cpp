/**
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

#include "libabckit/include/c/abckit.h"
#include "libabckit/include/c/metadata_core.h"
#include "libabckit/include/c/ir_core.h"

#include "helpers/helpers_runtime.h"
#include "helpers/helpers.h"

#include <gtest/gtest.h>

namespace libabckit::test {

namespace {
auto g_impl = AbckitGetApiImpl(ABCKIT_VERSION_RELEASE_1_0_0);
auto g_implI = AbckitGetInspectApiImpl(ABCKIT_VERSION_RELEASE_1_0_0);
auto g_implM = AbckitGetModifyApiImpl(ABCKIT_VERSION_RELEASE_1_0_0);
auto g_implG = AbckitGetGraphApiImpl(ABCKIT_VERSION_RELEASE_1_0_0);
auto g_statG = AbckitGetIsaApiStaticImpl(ABCKIT_VERSION_RELEASE_1_0_0);

void TransformSetStringIrStatic(AbckitGraph *graph, AbckitFile *file)
{
    auto *loadStr = helpers::FindFirstInst(graph, ABCKIT_ISA_API_STATIC_OPCODE_LOADSTRING);
    ASSERT_NE(loadStr, nullptr);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    auto *newStr = g_implM->createString(file, "STRING", strlen("STRING"));
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    g_implG->iSetString(loadStr, newStr);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);
}

void CheckGetStringIrStatic(AbckitGraph *graph)
{
    auto *loadStr = helpers::FindFirstInst(graph, ABCKIT_ISA_API_STATIC_OPCODE_LOADSTRING);
    ASSERT_NE(loadStr, nullptr);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    auto *str = g_implG->iGetString(loadStr);
    ASSERT_NE(str, nullptr);
    ASSERT_EQ(g_impl->getLastError(), ABCKIT_STATUS_NO_ERROR);

    ASSERT_EQ(helpers::AbckitStringToString(str), "string");
}

}  // namespace

class LibAbcKitStringStaticTest : public ::testing::Test {};

// Test: test-kind=api, api=GraphApiImpl::iSetString, abc-kind=ArkTS2, category=positive, extension=c
TEST_F(LibAbcKitStringStaticTest, LibAbcKitTestSetString)
{
    auto output = helpers::ExecuteStaticAbc(ABCKIT_ABC_DIR "ut/ir_core/string/string_static.abc",
                                            "string_static/ETSGLOBAL", "main");
    EXPECT_TRUE(helpers::Match(output, "string\n"));

    helpers::TransformMethod(ABCKIT_ABC_DIR "ut/ir_core/string/string_static.abc",
                             ABCKIT_ABC_DIR "ut/ir_core/string/string_static_modified.abc", "foo",
                             [](AbckitFile *file, AbckitCoreFunction * /*method*/, AbckitGraph *graph) {
                                 TransformSetStringIrStatic(graph, file);
                             });

    output = helpers::ExecuteStaticAbc(ABCKIT_ABC_DIR "ut/ir_core/string/string_static_modified.abc",
                                       "string_static/ETSGLOBAL", "main");
    EXPECT_TRUE(helpers::Match(output, "STRING\n"));
}

// Test: test-kind=api, api=GraphApiImpl::iGetString, abc-kind=ArkTS2, category=positive, extension=c
TEST_F(LibAbcKitStringStaticTest, LibAbcKitTestGetString)
{
    helpers::TransformMethod(ABCKIT_ABC_DIR "ut/ir_core/string/string_static.abc",
                             ABCKIT_ABC_DIR "ut/ir_core/string/string_static_modified.abc", "foo",
                             [](AbckitFile * /*file*/, AbckitCoreFunction * /*method*/, AbckitGraph *graph) {
                                 CheckGetStringIrStatic(graph);
                             });
}

}  // namespace libabckit::test
