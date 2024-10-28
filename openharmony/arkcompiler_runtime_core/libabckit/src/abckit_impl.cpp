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

#include "libabckit/include/c/statuses.h"
#include "libabckit/src/macros.h"
#include "libabckit/src/helpers_common.h"
#include "libabckit/src/statuses_impl.h"
#include "libabckit/src/metadata_inspect_impl.h"
#include "libabckit/src/ir_impl.h"
#include "libabckit/src/adapter_dynamic/abckit_dynamic.h"
#include "libabckit/src/adapter_static/abckit_static.h"
#include "libabckit/src/mem_manager/mem_manager.h"
#include "libabckit/src/logger.h"
#include "libabckit/src/abckit_options.h"

#include <cstddef>
#include <iostream>

libabckit::Logger *libabckit::Logger::logger_ = nullptr;
thread_local libabckit::Logger::MODE g_abckitGlobalMode {libabckit::Logger::MODE::RELEASE_MODE};
thread_local libabckit::NullBuffer libabckit::g_nB {};
thread_local std::ostream libabckit::g_nullStream {&g_nB};

namespace libabckit {
libabckit::Options g_abckitOptions("");

extern "C" AbckitStatus GetLastError()
{
    LIBABCKIT_IMPLEMENTED;
    return statuses::GetLastError();
}

extern "C" AbckitFile *OpenAbc(const char *path)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;

    LIBABCKIT_BAD_ARGUMENT(path, nullptr);

    LIBABCKIT_LOG(DEBUG) << path << '\n';

    MemManager::Initialize(g_abckitOptions.GetMemorySizeLimit());

    AbckitFile *file = nullptr;

    file = OpenAbcStatic(path);
    if (file != nullptr) {
        return file;
    }

    LIBABCKIT_LOG(DEBUG) << (std::string() + "Load file with path '" + path +
                             "' failed for static mode, trying dynamic mode\n");

    file = OpenAbcDynamic(path);
    if (file != nullptr) {
        return file;
    }

    LIBABCKIT_LOG(DEBUG) << (std::string() + "Load file with path '" + path + "' failed for both modes\n");

    statuses::SetLastError(AbckitStatus::ABCKIT_STATUS_TODO);

    return nullptr;
}

extern "C" void WriteAbc(AbckitFile *file, const char *path)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;

    LIBABCKIT_BAD_ARGUMENT_VOID(file)
    LIBABCKIT_BAD_ARGUMENT_VOID(path)

    switch (file->frontend) {
        case Mode::DYNAMIC:
            return WriteAbcDynamic(file, path);
        case Mode::STATIC:
            return WriteAbcStatic(file, path);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" void CloseFile(AbckitFile *file)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;

    LIBABCKIT_BAD_ARGUMENT_VOID(file);

    switch (file->frontend) {
        case Mode::DYNAMIC:
            return CloseFileDynamic(file);
        case Mode::STATIC:
            return CloseFileStatic(file);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" void DestroyGraph(AbckitGraph *graph)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;

    LIBABCKIT_BAD_ARGUMENT_VOID(graph);

    if (IsDynamic(graph->function->m->target)) {
        return DestroyGraphDynamic(graph);
    }
    DestroyGraphStatic(graph);
}

AbckitApi g_impl = {
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

}  // namespace libabckit

extern "C" AbckitApi const *AbckitGetApiImpl(AbckitApiVersion version)
{
    switch (version) {
        case ABCKIT_VERSION_RELEASE_1_0_0:
            return &libabckit::g_impl;
        default:
            libabckit::statuses::SetLastError(ABCKIT_STATUS_UNKNOWN_API_VERSION);
            return nullptr;
    }
}
