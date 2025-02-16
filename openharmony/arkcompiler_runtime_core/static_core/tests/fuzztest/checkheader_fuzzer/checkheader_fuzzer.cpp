/**
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "checkheader_fuzzer.h"
#include "libpandafile/file.h"

namespace OHOS {
void CheckHeaderFuzzTest(const uint8_t *data, size_t size)
{
    // Write data into a temp file
    const char *filename = "__ChechHeaderFuzzTest_data.tmp";
    FILE *fp = fopen(filename, "w+");
    if (fp == nullptr) {
        return;
    }
    (void)fwrite(data, sizeof(uint8_t), size, fp);
    (void)fseek(fp, 0, SEEK_SET);

    auto file = ark::os::file::File(fileno(fp));
    ark::os::mem::ConstBytePtr ptr =
        ark::os::mem::MapFile(file, ark::os::mem::MMAP_PROT_READ, ark::os::mem::MMAP_FLAG_PRIVATE, size, 0).ToConst();
    ark::panda_file::CheckHeader(ptr, filename);
    (void)fclose(fp);
    (void)remove(filename);
}
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::CheckHeaderFuzzTest(data, size);
    return 0;
}
