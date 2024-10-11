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

#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_DEFAULT_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_DEFAULT_H

#include "interfaces/inner_api/ace/ai/image_analyzer_interface.h"

namespace OHOS::Ace {
class ACE_EXPORT ImageAnalyzerDefault final : public ImageAnalyzerInterface {
public:
    explicit ImageAnalyzerDefault() {}

    bool IsImageAnalyzerSupported() override
    {
        return false;
    }
    void BuildNodeFunc(void* pixelMap, void* config, ImageAnalyzerInnerConfig* uiConfig,
        void** overlayData) override {}
    void UpdateImage(void** overlayData, void* pixelMap, void* config,
        ImageAnalyzerInnerConfig* uiConfig) override {}
    void UpdateConfig(void** overlayData, void* config) override {}
    void UpdateInnerConfig(void** overlayData, ImageAnalyzerInnerConfig* config) override {}
    void Release(void** overlayData) override {}
    void UpdatePressOverlay(void** overlayData, ImageAnalyzerInnerConfig* config) override {}
    void UpdateOverlayStatus(void** overlayData, ImageAnalyzerInnerConfig* config) override {}
    void UpdateOverlayActiveStatus(void** overlayData, bool status) override {}
    void UpdateAIButtonConfig(void** overlayData, AIButtonConfig* config) override {}
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_ANALYZER_DEFAULT_H
