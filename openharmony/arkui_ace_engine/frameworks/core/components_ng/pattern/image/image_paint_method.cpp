/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/image/image_paint_method.h"

#include "base/utils/utils.h"
#include "core/components_ng/pattern/image/image_dfx.h"
#include "core/components_ng/render/adapter/svg_canvas_image.h"

namespace {
constexpr unsigned int TOP_LEFT = 0;
constexpr unsigned int TOP_RIGHT = 1;
constexpr unsigned int BOTTOM_LEFT = 2;
constexpr unsigned int BOTTOM_RIGHT = 3;
} // namespace

namespace OHOS::Ace::NG {
namespace {
void NormalizeRadius(BorderRadiusArray& radius, const SizeF& size)
{
    auto maxRadius = std::min(size.Width(), size.Height()) / 2;
    // radius shouldn't be larger than half of image size
    for (auto&& corner : radius) {
        if (corner.GetX() > maxRadius) {
            corner.SetX(maxRadius);
        }
        if (corner.GetY() > maxRadius) {
            corner.SetY(maxRadius);
        }
    }
}
} // namespace

void ImagePaintMethod::UpdateBorderRadius(PaintWrapper* paintWrapper, ImageDfxConfig& imageDfxConfig)
{
    auto renderCtx = paintWrapper->GetRenderContext();
    CHECK_NULL_VOID(renderCtx);
    auto borderRadius = renderCtx->GetBorderRadius();

    BorderRadiusArray radiusXY = {
        PointF(borderRadius->radiusTopLeft->ConvertToPx(), borderRadius->radiusTopLeft->ConvertToPx()),
        PointF(borderRadius->radiusTopRight->ConvertToPx(), borderRadius->radiusTopRight->ConvertToPx()),
        PointF(borderRadius->radiusBottomLeft->ConvertToPx(), borderRadius->radiusBottomLeft->ConvertToPx()),
        PointF(borderRadius->radiusBottomRight->ConvertToPx(), borderRadius->radiusBottomRight->ConvertToPx())
    };

    // adjust image radius to match border (concentric round rects)
    auto width = renderCtx->GetBorderWidth();
    if (width) {
        if (width->leftDimen) {
            auto diff = width->leftDimen->ConvertToPx();
            radiusXY[TOP_LEFT].SetX(radiusXY[TOP_LEFT].GetX() - diff);
            radiusXY[BOTTOM_LEFT].SetX(radiusXY[BOTTOM_LEFT].GetX() - diff);
        }
        if (width->rightDimen) {
            auto diff = width->rightDimen->ConvertToPx();
            radiusXY[TOP_RIGHT].SetX(radiusXY[TOP_RIGHT].GetX() - diff);
            radiusXY[BOTTOM_RIGHT].SetX(radiusXY[BOTTOM_RIGHT].GetX() - diff);
        }
        if (width->topDimen) {
            auto diff = width->topDimen->ConvertToPx();
            radiusXY[TOP_LEFT].SetY(radiusXY[TOP_LEFT].GetY() - diff);
            radiusXY[TOP_RIGHT].SetY(radiusXY[TOP_RIGHT].GetY() - diff);
        }
        if (width->bottomDimen) {
            auto diff = width->bottomDimen->ConvertToPx();
            radiusXY[BOTTOM_LEFT].SetY(radiusXY[BOTTOM_LEFT].GetY() - diff);
            radiusXY[BOTTOM_RIGHT].SetY(radiusXY[BOTTOM_RIGHT].GetY() - diff);
        }
    }

    NormalizeRadius(radiusXY, paintWrapper->GetContentSize());
    auto&& config = canvasImage_->GetPaintConfig();
    config.borderRadiusXY_ = std::make_shared<BorderRadiusArray>(radiusXY);
    imageDfxConfig.borderRadiusValue_ = borderRadius->ToString();
}

void ImagePaintMethod::UpdatePaintConfig(PaintWrapper* paintWrapper)
{
    auto&& config = canvasImage_->GetPaintConfig();
    auto renderProps = DynamicCast<ImageRenderProperty>(paintWrapper->GetPaintProperty());
    config.renderMode_ = renderProps->GetImageRenderMode().value_or(ImageRenderMode::ORIGINAL);
    config.imageInterpolation_ = renderProps->GetImageInterpolation().value_or(interpolationDefault_);
    config.imageRepeat_ = renderProps->GetImageRepeat().value_or(ImageRepeat::NO_REPEAT);
    config.smoothEdge_ = renderProps->GetSmoothEdge().value_or(0.0f);
    config.dynamicMode = renderProps->GetDynamicModeValue(DynamicRangeMode::STANDARD);
    config.svgFillColor_ = renderProps->GetSvgFillColor();
    config.resizableSlice_ = renderProps->GetImageResizableSliceValue({});
    config.resizableLattice_ = renderProps->GetImageResizableLatticeValue(nullptr);

    bool isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
    config.flipHorizontally_ = isRightToLeft && renderProps->GetMatchTextDirection().value_or(false);
    config.colorFilter_.Reset();

    auto colorFilterMatrix = renderProps->GetColorFilter();
    if (colorFilterMatrix.has_value()) {
        config.colorFilter_.colorFilterMatrix_ = std::make_shared<std::vector<float>>(colorFilterMatrix.value());
    }
    auto drawingColorFilter = renderProps->GetDrawingColorFilter();
    if (drawingColorFilter.has_value()) {
        config.colorFilter_.colorFilterDrawing_ = drawingColorFilter.value();
    }
    auto renderCtx = paintWrapper->GetRenderContext();
    if (renderCtx) {
        config.obscuredReasons_ = renderCtx->GetObscured().value_or(std::vector<ObscuredReasons>());
    }

    if (renderProps->GetNeedBorderRadiusValue(false)) {
        UpdateBorderRadius(paintWrapper, canvasImage_->GetImageDfxConfig());
    }
}

RefPtr<Modifier> ImagePaintMethod::GetOverlayModifier(PaintWrapper* paintWrapper)
{
    return imageOverlayModifier_;
}

RefPtr<Modifier> ImagePaintMethod::GetContentModifier(PaintWrapper* paintWrapper)
{
    return imageContentModifier_;
}

void ImagePaintMethod::UpdateOverlayModifier(PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(paintWrapper);
    CHECK_NULL_VOID(imageOverlayModifier_);
    auto size = paintWrapper->GetContentSize();
    auto offset = paintWrapper->GetContentOffset();
    imageOverlayModifier_->SetSize(size);
    imageOverlayModifier_->SetOffset(offset);
    imageOverlayModifier_->SetIsSelected(selected_);
}

void ImagePaintMethod::UpdateContentModifier(PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(canvasImage_);
    CHECK_NULL_VOID(paintWrapper);
    CHECK_NULL_VOID(imageContentModifier_);
    UpdatePaintConfig(paintWrapper);
    auto size = paintWrapper->GetContentSize();
    imageContentModifier_->SetSize(size);
    imageContentModifier_->SetSensitive(sensitive_);
    imageContentModifier_->SetCanvasImageWrapper(CanvasImageModifierWrapper(canvasImage_));;
}
} // namespace OHOS::Ace::NG
