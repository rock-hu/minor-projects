/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_IMAGE_IMAGE_CONTENT_MODIFIER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_IMAGE_IMAGE_CONTENT_MODIFIER_H

#include "base/geometry/ng/size_t.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/render/canvas_image.h"

namespace OHOS::Ace::NG {
class ImagePattern;
class ImageContentModifier : public ContentModifier {
    DECLARE_ACE_TYPE(ImageContentModifier, ContentModifier);

public:
    ImageContentModifier();
    explicit ImageContentModifier(const WeakPtr<ImagePattern>& pattern);
    ~ImageContentModifier() override = default;

    void onDraw(DrawingContext& drawingContext) override;

    void DrawDrawable(DrawingContext& drawingContext);

    void SetSensitive(bool sensitive)
    {
        CHECK_NULL_VOID(sensitive_);
        sensitive_->Set(sensitive);
    }

    void SetSize(const SizeF& size)
    {
        CHECK_NULL_VOID(size_);
        size_->Set(size);
    }

    void SetCanvasImageWrapper(const CanvasImageModifierWrapper& wrapper)
    {
        CHECK_NULL_VOID(canvasImageWrapper_);
        canvasImageWrapper_->Set(wrapper);
    }

private:
    // The same svg resource has only one copy of SVGDOM in the object cache. Therefore
    // the value of fill color must be changed on the modifier berfore the onDraw method
    static void UpdateSvgColorFilter(const RefPtr<CanvasImage>& canvasImage);

    WeakPtr<ImagePattern> pattern_;
    RefPtr<PropertyBool> sensitive_;
    RefPtr<PropertySizeF> size_;
    RefPtr<PropertyCanvasImageModifierWrapper> canvasImageWrapper_;

    ACE_DISALLOW_COPY_AND_MOVE(ImageContentModifier);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_IMAGE_IMAGE_CONTENT_MODIFIER_H
