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

#include "core/common/ai/image_analyzer_manager.h"

namespace OHOS::Ace {

ImageAnalyzerManager::ImageAnalyzerManager(const RefPtr<NG::FrameNode>& frameNode, ImageAnalyzerHolder holder)
{
    holder_ = holder;
    overlayData_ = nullptr;
    isAnalyzerOverlayBuild_ = false;
}

void ImageAnalyzerManager::CreateAnalyzerOverlay(const RefPtr<OHOS::Ace::PixelMap>& pixelMap,
    const NG::OffsetF& offset)
{
}

void ImageAnalyzerManager::CreateMovingPhotoAnalyzerOverlay(const RefPtr<OHOS::Ace::PixelMap>& pixelMap,
    MovingPhotoAnalyzerInfo info)
{
}

void ImageAnalyzerManager::UpdateAnalyzerOverlay(const RefPtr<OHOS::Ace::PixelMap>& pixelMap,
    const NG::OffsetF& offset)
{
}

void ImageAnalyzerManager::UpdateMovingPhotoAnalyzerOverlay(const RefPtr<OHOS::Ace::PixelMap>& pixelMap,
    MovingPhotoAnalyzerInfo info)
{
}

void ImageAnalyzerManager::DestroyAnalyzerOverlay()
{
}

bool ImageAnalyzerManager::IsSupportImageAnalyzerFeature()
{
    return false;
}

bool ImageAnalyzerManager::IsOverlayCreated()
{
    return false;
}

void ImageAnalyzerManager::UpdateAnalyzerOverlayLayout()
{
}

void ImageAnalyzerManager::UpdateAnalyzerUIConfig(const RefPtr<NG::GeometryNode>& geometryNode,
    const PixelMapInfo& info)
{
}

void ImageAnalyzerManager::UpdateKeyEvent(KeyEvent keyEvent)
{
}

void ImageAnalyzerManager::SetImageAnalyzerConfig(void* config)
{
}

void ImageAnalyzerManager::SetImageAIOptions(void* options)
{
}

void ImageAnalyzerManager::SetImageAnalyzerCallback(OnAnalyzedCallback& callback)
{
}

void ImageAnalyzerManager::SetOnCanPlayCallback(OnCanPlayCallback&& callback)
{
}

void ImageAnalyzerManager::ReleaseImageAnalyzer()
{
}
} // namespace OHOS::Ace