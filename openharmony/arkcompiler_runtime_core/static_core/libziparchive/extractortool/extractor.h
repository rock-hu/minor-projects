/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef LIBZIPARCHIVE_EXTRACTORTOOL_EXTRACTOR_H
#define LIBZIPARCHIVE_EXTRACTORTOOL_EXTRACTOR_H

#include <memory>
#include <string>
#include <unordered_map>

#include "file_mapper.h"
#include "zip_file.h"

namespace ark::extractor {
class Extractor {  // NOLINT(cppcoreguidelines-special-member-functions)
public:
    explicit Extractor(const std::string &source);
    virtual ~Extractor() = default;

    /**
     * @brief Open compressed file.
     * @return Returns true if the file is successfully opened; returns false otherwise.
     */
    virtual bool Init();
    // For abc file only, to mmap to safe region.
    std::shared_ptr<FileMapper> GetSafeData(const std::string &fileName);

private:
    ZipFile zipFile_;
    std::string filePath_;
};
}  // namespace ark::extractor
#endif
