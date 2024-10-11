/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_LOADER_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_LOADER_H

#define NAPI_VERSION 8

#include <memory>
#include "interfaces/inner_api/ace/ai/image_analyzer_interface.h"

namespace OHOS::Ace {
using ImageAnalyzerInstance = std::unique_ptr<ImageAnalyzerInterface, std::function<void (ImageAnalyzerInterface*)>>;

class ImageAnalyzerLoader : public std::enable_shared_from_this<ImageAnalyzerLoader> {
public:
    static std::shared_ptr<ImageAnalyzerLoader> Load();
    ~ImageAnalyzerLoader();
    ImageAnalyzerLoader() = default;

    ImageAnalyzerLoader(const ImageAnalyzerLoader&) = delete;
    ImageAnalyzerLoader(ImageAnalyzerLoader&&) = delete;
    ImageAnalyzerLoader& operator=(const ImageAnalyzerLoader&) = delete;
    ImageAnalyzerLoader& operator=(ImageAnalyzerLoader&&) = delete;
    ImageAnalyzerInstance CreateImageAnalyzer();

private:
    bool Init();
    void Close();
    
    void *libraryHandle_ = nullptr;
    ImageAnalyzerInterface* (*createImageAnalyzerInstance_)(napi_env) = nullptr;
    void (*destroyImageAnalyzerInstance_)(ImageAnalyzerInterface*) = nullptr;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_LOADER_H
