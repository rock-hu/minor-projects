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

#include "core/components_ng/pattern/qrcode/qrcode_layout_algorithm.h"

#include "core/components/qrcode/qrcode_theme.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t PLATFORM_VERSION_11 = 11;
} // namespace

std::optional<SizeF> QRCodeLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_RETURN(layoutWrapper, std::nullopt);
    auto layoutProperty = AceType::DynamicCast<LayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(layoutProperty, std::nullopt);

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    auto qrcodeTheme = pipeline->GetTheme<QrcodeTheme>();
    CHECK_NULL_RETURN(qrcodeTheme, std::nullopt);
    Dimension defaultSize = qrcodeTheme->GetQrcodeDefaultSize();
    if (pipeline->GetMinPlatformVersion() >= PLATFORM_VERSION_11) {
        auto topPadding = 0.0f;
        auto bottomPadding = 0.0f;
        auto leftPadding = 0.0f;
        auto rightPadding = 0.0f;
        const auto& padding = layoutProperty->GetPaddingProperty();
        if (padding) {
            topPadding = padding->top.value_or(CalcLength(0.0_vp)).GetDimension().ConvertToPx();
            bottomPadding = padding->bottom.value_or(CalcLength(0.0_vp)).GetDimension().ConvertToPx();
            leftPadding = padding->left.value_or(CalcLength(0.0_vp)).GetDimension().ConvertToPx();
            rightPadding = padding->right.value_or(CalcLength(0.0_vp)).GetDimension().ConvertToPx();
        }
        auto width = defaultSize.ConvertToPx() - leftPadding - rightPadding;
        if (Negative(width)) {
            width = 0.0f;
        }
        if (contentConstraint.selfIdealSize.Width().has_value()) {
            width = contentConstraint.selfIdealSize.Width().value();
        }
        auto height = defaultSize.ConvertToPx() - topPadding - bottomPadding;
        if (Negative(height)) {
            height = 0.0f;
        }
        if (contentConstraint.selfIdealSize.Height().has_value()) {
            height = contentConstraint.selfIdealSize.Height().value();
        }
        auto qrCodeSize = std::min(width, height);
        qrCodeSize_ = qrCodeSize;
        return SizeF(qrCodeSize, qrCodeSize);
    } else {
        auto idealSize = CreateIdealSize(contentConstraint, Axis::HORIZONTAL, layoutProperty->GetMeasureType(), true);
        if (LessNotEqual(idealSize.Width(), idealSize.Height())) {
            idealSize.SetHeight(idealSize.Width());
        } else if (LessNotEqual(idealSize.Height(), idealSize.Width())) {
            idealSize.SetWidth(idealSize.Height());
        }
        qrCodeSize_ = idealSize.Width();
        return idealSize;
    }
}
} // namespace OHOS::Ace::NG
