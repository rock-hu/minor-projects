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

#include "adapter/preview/osal/image_packer_preview.h"

namespace OHOS::Ace {
RefPtr<ImagePacker> ImagePacker::Create()
{
    return nullptr;
}

uint32_t ImagePackerPreview::AddImage(PixelMap& pixelMap)
{
    return 1;
}

uint32_t ImagePackerPreview::StartPacking(uint8_t* data, uint32_t maxSize, const PackOption& option)
{
    return 1;
}

uint32_t ImagePackerPreview::StartPacking(const std::string& filePath, const PackOption& option)
{
    return 1;
}

uint32_t ImagePackerPreview::FinalizePacking(int64_t& packedSize)
{
    return 1;
}
} // namespace OHOS::Ace
