/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_DRAWABLE_DESCRIPTOR_BASE_IMAGE_SOURCE_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_DRAWABLE_DESCRIPTOR_BASE_IMAGE_SOURCE_H
#include "../image_loader/image_define.h"

namespace OHOS::Ace::Napi {
class ImageSource {
public:
    ImageSource() = default;
    virtual ~ImageSource() = default;
    static std::shared_ptr<ImageSource> Create(const uint8_t* data, uint32_t size);
    virtual std::shared_ptr<Media::PixelMap> CreatePixelMap(uint32_t index,
        const Size& size, AIImageQuality imageQuality, bool isHdrDecodeerNeed) = 0;
    virtual Size GetImageSize() = 0;
};
} // namespace OHOS::Ace::Napi

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_DRAWABLE_DESCRIPTOR_BASE_IMAGE_SOURCE_H