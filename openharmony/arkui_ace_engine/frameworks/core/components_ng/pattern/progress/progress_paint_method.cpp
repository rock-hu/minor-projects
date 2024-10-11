/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/progress/progress_paint_method.h"

namespace OHOS::Ace::NG {
void ProgressPaintMethod::GetThemeData()
{
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto progressTheme = pipeline->GetTheme<ProgressTheme>();
    CHECK_NULL_VOID(progressTheme);
    color_ = progressTheme->GetTrackSelectedColor();
    if (progressType_ == ProgressType::CAPSULE) {
        color_ = progressTheme->GetCapsuleSelectColor();
        bgColor_ = progressTheme->GetCapsuleBgColor();
    } else if (progressType_ == ProgressType::RING) {
        bgColor_ = progressTheme->GetRingProgressBgColor();
    } else {
        bgColor_ = progressTheme->GetTrackBgColor();
    }
    scaleWidth_ = progressTheme->GetScaleWidth().ConvertToPx();
    scaleCount_ = progressTheme->GetScaleNumber();
    borderColor_ = progressTheme->GetBorderColor();
    capsuleBorderWidth_ = progressTheme->GetBorderWidth();
    ringProgressEndSideColor_ = progressTheme->GetRingProgressEndSideColor();
    ringProgressBeginSideColor_ = progressTheme->GetRingProgressBeginSideColor();
}

void ProgressPaintMethod::CalculateStrokeWidth(const SizeF& contentSize)
{
    auto length = std::min(contentSize.Width(), contentSize.Height());
    constexpr float HALF = 0.5;
    switch (progressType_) {
        case ProgressType::LINEAR:
            strokeWidth_ = std::min(strokeWidth_, length);
            break;
        case ProgressType::RING:
        case ProgressType::SCALE:
            if (strokeWidth_ >= length * HALF) {
                strokeWidth_ = length * HALF * HALF;
            }
            break;
        case ProgressType::MOON:
        case ProgressType::CAPSULE:
        default:
            break;
    }
}

Gradient ProgressPaintMethod::GenerateRingProgressColor(PaintWrapper* paintWrapper)
{
    auto paintProperty = DynamicCast<ProgressPaintProperty>(paintWrapper->GetPaintProperty());
    if (paintProperty->HasGradientColor()) {
        return paintProperty->GetGradientColorValue();
    }

    Gradient gradient;
    GradientColor gradientColorEnd;
    GradientColor gradientColorStart;
    if (paintProperty->HasColor()) {
        Color color = paintProperty->GetColorValue();
        gradientColorEnd.SetLinearColor(LinearColor(color));
        gradientColorStart.SetLinearColor(LinearColor(color));
    } else {
        gradientColorEnd.SetLinearColor(LinearColor(ringProgressEndSideColor_));
        gradientColorStart.SetLinearColor(LinearColor(ringProgressBeginSideColor_));
    }
    gradientColorEnd.SetDimension(Dimension(0.0));
    gradient.AddColor(gradientColorEnd);
    gradientColorStart.SetDimension(Dimension(1.0));
    gradient.AddColor(gradientColorStart);
    return gradient;
}

} // namespace OHOS::Ace::NG
