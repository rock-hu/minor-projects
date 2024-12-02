/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_IMAGE_SOURCE_PREVIEW_H
#define FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_IMAGE_SOURCE_PREVIEW_H

#include "base/image/image_source.h"

namespace OHOS::Ace {
class ImageSourcePreview : public ImageSource {
    DECLARE_ACE_TYPE(ImageSourcePreview, ImageSource)

public:
    std::string GetProperty(const std::string& key) override;
    RefPtr<PixelMap> CreatePixelMap(const Size& size, AIImageQuality imageQuality = AIImageQuality::NONE,
        bool isHdrDecoderNeed = false, PixelFormat photoDecodeFormat = PixelFormat::UNKNOWN) override;
    RefPtr<PixelMap> CreatePixelMap(uint32_t index, const Size& size,
        AIImageQuality imageQuality = AIImageQuality::NONE, bool isHdrDecoderNeed = false,
        PixelFormat photoDecodeFormat = PixelFormat::UNKNOWN) override;
    RefPtr<PixelMap> CreatePixelMap() override;
    Size GetImageSize() override;
    uint32_t GetFrameCount() override;
    std::string GetEncodedFormat() override;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_IMAGE_SOURCE_PREVIEW_H
