/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_IMAGE_PROVIDER_IMAGE_OBJECT_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_IMAGE_PROVIDER_IMAGE_OBJECT_NG_H

#include "base/utils/noncopyable.h"
#include "core/components_ng/image_provider/image_data.h"
#include "core/components_ng/image_provider/image_provider.h"
#include "core/components_ng/image_provider/svg_dom_base.h"
#include "core/components_ng/pattern/image/image_dfx.h"
#include "core/components_ng/render/canvas_image.h"
#include "core/image/image_source_info.h"

namespace OHOS::Ace::NG {

class ImageObject : public virtual AceType {
    DECLARE_ACE_TYPE(ImageObject, AceType);

public:
    ImageObject() = delete;
    ImageObject(const ImageSourceInfo& sourceInfo, const SizeF& imageSize, const RefPtr<ImageData>& data)
        : src_(sourceInfo), imageSize_(imageSize), data_(data)
    {}
    ~ImageObject() override = default;

    const SizeF& GetImageSize() const;
    const ImageSourceInfo& GetSourceInfo() const;
    const RefPtr<ImageData>& GetData() const;
    int32_t GetFrameCount() const;

    void SetData(const RefPtr<ImageData>& data);
    void SetImageSize(const SizeF& imageSize);
    virtual void ClearData();
    void SetFrameCount(int32_t frameCount);

    virtual RefPtr<SvgDomBase> GetSVGDom() const
    {
        return nullptr;
    }

    virtual RefPtr<ImageObject> Clone() = 0;
    virtual std::string GetDumpInfo() { return ""; }
    bool IsSupportCache() const
    {
        return src_.SupportObjCache();
    }

    void SetImageDfxConfig(const ImageDfxConfig& imageDfxConfig)
    {
        imageDfxConfig_ = imageDfxConfig;
    }

    ImageDfxConfig GetImageDfxConfig()
    {
        return imageDfxConfig_;
    }

    virtual void MakeCanvasImage(
        const WeakPtr<ImageLoadingContext>& ctxWp, const SizeF& resizeTarget, bool forceResize, bool syncLoad) = 0;

protected:
    const ImageSourceInfo src_;
    SizeF imageSize_ { -1.0, -1.0 };
    // no longer needed after making canvas image
    RefPtr<ImageData> data_;
    int32_t frameCount_ = 1;
    ImageDfxConfig imageDfxConfig_;

    ACE_DISALLOW_COPY_AND_MOVE(ImageObject);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_IMAGE_PROVIDER_IMAGE_OBJECT_NG_H
