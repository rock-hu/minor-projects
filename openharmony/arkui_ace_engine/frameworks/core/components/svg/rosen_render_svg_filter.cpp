/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "frameworks/core/components/svg/rosen_render_svg_filter.h"

#include "frameworks/core/components/svg/rosen_render_svg_fe_colormatrix.h"
#include "frameworks/core/components/svg/rosen_render_svg_fe_composite.h"
#include "frameworks/core/components/svg/rosen_render_svg_fe_gaussianblur.h"
#include "frameworks/core/components/svg/rosen_render_svg_fe_offset.h"

namespace OHOS::Ace {

void RosenRenderSvgFilter::Paint(RenderContext& context, const Offset& offset)
{
    return;
}

#ifndef USE_ROSEN_DRAWING
const SkPaint RosenRenderSvgFilter::OnAsPaint()
{
    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    sk_sp<SkImageFilter> imageFilter = nullptr;
    ColorInterpolationType currentColor = ColorInterpolationType::SRGB;
    for (const auto& item : GetChildren()) {
        GetImageFilter(AceType::DynamicCast<RenderSvgFe>(item), imageFilter, currentColor);
    }
    ConverImageFilterColor(imageFilter, currentColor, ColorInterpolationType::SRGB);
    skPaint.setImageFilter(imageFilter);
    return skPaint;
}
#else
const RSBrush RosenRenderSvgFilter::OnAsPaint()
{
    RSBrush rsBrush;
    rsBrush.SetAntiAlias(true);
    std::shared_ptr<RSImageFilter> imageFilter = nullptr;
    ColorInterpolationType currentColor = ColorInterpolationType::SRGB;
    for (const auto& item : GetChildren()) {
        GetImageFilter(AceType::DynamicCast<RenderSvgFe>(item), imageFilter, currentColor);
    }
    ConverImageFilterColor(imageFilter, currentColor, ColorInterpolationType::SRGB);
    RSFilter filter;
    filter.SetImageFilter(imageFilter);
    rsBrush.SetFilter(filter);
    return rsBrush;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RosenRenderSvgFilter::GetImageFilter(
    const RefPtr<RenderSvgFe>& fe, sk_sp<SkImageFilter>& imageFilter, ColorInterpolationType& currentColor)
#else
void RosenRenderSvgFilter::GetImageFilter(
    const RefPtr<RenderSvgFe>& fe, std::shared_ptr<RSImageFilter>& imageFilter, ColorInterpolationType& currentColor)
#endif
{
    if (!fe) {
        return;
    }
    ColorInterpolationType srcColor = currentColor;
    InitFilterColor(fe, currentColor);

    auto feComposite = AceType::DynamicCast<RosenRenderSvgFeComposite>(fe);
    if (feComposite) {
        auto foreImageFilter = MakeImageFilter(feComposite->GetInType(), imageFilter);
        auto backImageFilter = MakeImageFilter(feComposite->GetIn2Type(), imageFilter);
        ConverImageFilterColor(foreImageFilter, srcColor, currentColor);
        ConverImageFilterColor(backImageFilter, srcColor, currentColor);
        feComposite->OnAsImageFilter(backImageFilter, foreImageFilter, imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    imageFilter = MakeImageFilter(fe->GetInType(), imageFilter);
    auto feOffset = AceType::DynamicCast<RosenRenderSvgFeOffset>(fe);
    if (feOffset) {
        feOffset->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    auto feColorMatrix = AceType::DynamicCast<RosenRenderSvgFeColorMatrix>(fe);
    if (feColorMatrix) {
        feColorMatrix->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    auto feGaussianBlur = AceType::DynamicCast<RosenRenderSvgFeGaussianBlur>(fe);
    if (feGaussianBlur) {
        feGaussianBlur->OnAsImageFilter(imageFilter);
        ConverImageFilterColor(imageFilter, srcColor, currentColor);
        return;
    }

    currentColor = srcColor;
}

void RosenRenderSvgFilter::InitFilterColor(const RefPtr<RenderSvgFe>& fe, ColorInterpolationType& currentColor)
{
    if (!fe) {
        return;
    }

    if (fe->GetInType() == FeInType::SOURCE_GRAPHIC) {
        currentColor = ColorInterpolationType::SRGB;
    } else {
        currentColor = fe->GetColorType();
    }
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RosenRenderSvgFilter::MakeImageFilter(const FeInType& in, sk_sp<SkImageFilter>& imageFilter)
#else
std::shared_ptr<RSImageFilter> RosenRenderSvgFilter::MakeImageFilter(
    const FeInType& in, std::shared_ptr<RSImageFilter>& imageFilter)
#endif
{
    switch (in) {
        case FeInType::SOURCE_GRAPHIC:
            return nullptr;
        case FeInType::SOURCE_ALPHA: {
#ifndef USE_ROSEN_DRAWING
            SkColorMatrix m;
            m.setScale(0, 0, 0, 1.0f);

            return SkImageFilters::ColorFilter(SkColorFilters::Matrix(m), nullptr);
#else
            RSColorMatrix m;
            m.SetScale(0, 0, 0, 1.0f);
            auto colorFilter = RSRecordingColorFilter::CreateMatrixColorFilter(m);
            if (colorFilter != nullptr) {
                return RSRecordingImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
            }
            break;
#endif
        }
        case FeInType::BACKGROUND_IMAGE:
            break;
        case FeInType::BACKGROUND_ALPHA:
            break;
        case FeInType::FILL_PAINT:
            break;
        case FeInType::STROKE_PAINT:
            break;
        case FeInType::PRIMITIVE:
            break;
        default:
            break;
    }
    return imageFilter;
}

#ifndef USE_ROSEN_DRAWING
void RosenRenderSvgFilter::ConverImageFilterColor(
    sk_sp<SkImageFilter>& imageFilter, const ColorInterpolationType& src, const ColorInterpolationType& dst)
{
    if (dst == ColorInterpolationType::LINEAR_RGB && src == ColorInterpolationType::SRGB) {
        imageFilter = SkImageFilters::ColorFilter(SkColorFilters::SRGBToLinearGamma(), imageFilter);
    } else if (dst == ColorInterpolationType::SRGB && src == ColorInterpolationType::LINEAR_RGB) {
        imageFilter = SkImageFilters::ColorFilter(SkColorFilters::LinearToSRGBGamma(), imageFilter);
    }
}
#else
void RosenRenderSvgFilter::ConverImageFilterColor(
    std::shared_ptr<RSImageFilter>& imageFilter, const ColorInterpolationType& src, const ColorInterpolationType& dst)
{
    std::shared_ptr<RSRecordingColorFilter> colorFilter = nullptr;
    if (dst == ColorInterpolationType::LINEAR_RGB && src == ColorInterpolationType::SRGB) {
        colorFilter = RSRecordingColorFilter::CreateSrgbGammaToLinear();
    } else if (dst == ColorInterpolationType::SRGB && src == ColorInterpolationType::LINEAR_RGB) {
        colorFilter = RSRecordingColorFilter::CreateLinearToSrgbGamma();
    }
    if (colorFilter != nullptr) {
        imageFilter = RSRecordingImageFilter::CreateColorFilterImageFilter(*colorFilter, imageFilter);
    }
}
#endif

} // namespace OHOS::Ace
