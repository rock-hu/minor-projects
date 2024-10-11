/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "annotationdataaccessor_fuzzer.h"

#include "libpandafile/file.h"
#include "libpandafile/annotation_data_accessor.h"
#include "libpandafile/class_data_accessor-inl.h"

namespace OHOS {
void AnnotationDataAccessorFuzzTest(const uint8_t* data, size_t size)
{
    try {
        auto pf = panda::panda_file::OpenPandaFileFromMemory(data, size);
        if (pf == nullptr) {
            return;
        }
        auto classes = pf->GetClasses();
        const auto &panda_file = *pf;
        for (size_t i = 0; i < classes.Size(); i++) {
            panda::panda_file::File::EntityId id(classes[i]);
            if (panda_file.IsExternal(id)) {
                continue;
            }

            panda::panda_file::ClassDataAccessor cda(panda_file, id);
            cda.EnumerateAnnotations([&](panda::panda_file::File::EntityId id) {
                panda::panda_file::AnnotationDataAccessor ada(panda_file, id);
            });
        }
    } catch (panda::panda_file::helpers::FileAccessException &e) {
        // Known exception, no need exposing
    }
}
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AnnotationDataAccessorFuzzTest(data, size);
    return 0;
}
