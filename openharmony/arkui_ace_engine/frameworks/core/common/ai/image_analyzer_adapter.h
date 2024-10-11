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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_OSAL_IMAGE_ANALYZER_ADAPTER_H
#define FOUNDATION_ACE_ADAPTER_OHOS_OSAL_IMAGE_ANALYZER_ADAPTER_H

#include <functional>

#include "base/image/pixel_map.h"
#include "base/memory/ace_type.h"

namespace OHOS::Ace {
class ImageAnalyzerAdapter : public AceType {
    DECLARE_ACE_TYPE(ImageAnalyzerAdapter, AceType);
public:
    virtual void SetImageAnalyzerConfig(void* config, bool isOptions = false) = 0;
    virtual void* GetImageAnalyzerConfig() = 0;
    virtual void* ConvertPixmapNapi(const RefPtr<PixelMap>& pixelMap) = 0;
    virtual bool HasImageAnalyzerConfig() = 0;
};
ImageAnalyzerAdapter* CreateImageAnalyzerAdapter();
}
#endif
