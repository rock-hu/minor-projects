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

#include "core/components_ng/image_provider/static_image_object.h"

#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/image_provider/image_utils.h"
#include "core/components_ng/render/adapter/rosen/drawing_image.h"
#include "frameworks/core/components_ng/render/adapter/pixelmap_image.h"

namespace OHOS::Ace::NG {

void StaticImageObject::MakeCanvasImage(
    const WeakPtr<ImageLoadingContext>& ctxWp, const SizeF& targetSize, bool forceResize, bool syncLoad)
{
    auto ctx = ctxWp.Upgrade();
    CHECK_NULL_VOID(ctx);
    RefPtr<CanvasImage> cachedImage;
    auto key = ImageUtils::GenerateImageKey(src_, targetSize);
    if (SystemProperties::GetImageFrameworkEnabled()) {
        cachedImage = PixelMapImage::QueryFromCache(key);
    } else {
        cachedImage = DrawingImage::QueryFromCache(key);
    }
    if (cachedImage) {
        ctx->SuccessCallback(cachedImage);
        return;
    }
    ImageProvider::MakeCanvasImage(Claim(this), ctx, targetSize,
        { .forceResize = forceResize,
            .sync = syncLoad,
            .imageQuality = ctx->GetImageQuality(),
            .isHdrDecoderNeed = ctx->GetIsHdrDecoderNeed(),
            .photoDecodeFormat = ctx->GetPhotoDecodeFormat() });
}

RefPtr<ImageObject> StaticImageObject::Clone()
{
    auto object = MakeRefPtr<StaticImageObject>(src_, imageSize_, data_);
    object->SetOrientation(orientation_);
    return object;
}
} // namespace OHOS::Ace::NG
