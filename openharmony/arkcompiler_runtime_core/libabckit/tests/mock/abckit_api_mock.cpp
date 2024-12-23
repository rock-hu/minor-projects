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

#include "mock/abckit_mock.h"
#include "mock/mock_values.h"

#include "include/c/statuses.h"
#include "include/c/abckit.h"

#include <gtest/gtest.h>

namespace libabckit::mock::abckit_api {

// NOLINTBEGIN(readability-identifier-naming)

AbckitStatus GetLastError()
{
    return ABCKIT_STATUS_NO_ERROR;
}

AbckitFile *OpenAbc(const char *path)
{
    g_calledFuncs.push(__func__);
    EXPECT_TRUE(strncmp(path, DEFAULT_PATH, DEFAULT_PATH_SIZE) == 0);
    return DEFAULT_FILE;
}

void WriteAbc(AbckitFile *file, const char *path)
{
    g_calledFuncs.push(__func__);
    EXPECT_TRUE(strncmp(path, DEFAULT_PATH, DEFAULT_PATH_SIZE) == 0);
    EXPECT_TRUE(file == DEFAULT_FILE);
}

void CloseFile(AbckitFile *file)
{
    g_calledFuncs.push(__func__);
    EXPECT_TRUE(file == DEFAULT_FILE);
}

void DestroyGraph(AbckitGraph *graph)
{
    g_calledFuncs.push(__func__);
    EXPECT_TRUE(graph == DEFAULT_GRAPH);
}

// NOLINTEND(readability-identifier-naming)

static AbckitApi g_impl = {
    // ========================================
    // Common API
    // ========================================

    ABCKIT_VERSION_RELEASE_1_0_0,
    GetLastError,

    // ========================================
    // Inspection API entrypoints
    // ========================================

    OpenAbc,
    WriteAbc,
    CloseFile,

    // ========================================
    // IR API entrypoints
    // ========================================

    DestroyGraph,
};

// NOLINTEND(readability-identifier-naming)

}  // namespace libabckit::mock::abckit_api

AbckitApi const *AbckitGetMockApiImpl([[maybe_unused]] AbckitApiVersion version)
{
    return &libabckit::mock::abckit_api::g_impl;
}
