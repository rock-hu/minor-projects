/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "createoraddfileintozip_fuzzer.h"
#include "libpandafile/file.h"
#include "libziparchive/zip_archive.h"

namespace OHOS {
    void CreateOrAddFileIntoZipFuzzTest(const uint8_t* data, size_t size)
    {
        std::string str(data, data + size);
        {
            // zipOpen test
            const char* zipname = nullptr;
            const char* filename = str.c_str();
            panda::CreateOrAddFileIntoZip(zipname, filename, data, size, APPEND_STATUS_CREATE, Z_NO_COMPRESSION);
            (void)remove(zipname);
        }
        {
            // zipOpenNewFileInZip test
            const char* zipname = str.c_str();
            const char* filename = nullptr;
            panda::CreateOrAddFileIntoZip(zipname, filename, data, size, APPEND_STATUS_CREATE, Z_NO_COMPRESSION);
            (void)remove(zipname);
        }
        {
            // zipWriteInFileInZip test
            const char* zipname = str.c_str();
            const char* filename = panda::panda_file::ARCHIVE_FILENAME;
            panda::CreateOrAddFileIntoZip(zipname, filename, data, 0, APPEND_STATUS_CREATE, Z_NO_COMPRESSION);
            (void)remove(zipname);
        }
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CreateOrAddFileIntoZipFuzzTest(data, size);
    return 0;
}

