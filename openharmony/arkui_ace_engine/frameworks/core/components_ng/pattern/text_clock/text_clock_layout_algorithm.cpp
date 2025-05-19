/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/text_clock/text_clock_layout_algorithm.h"

#include "core/components_ng/pattern/text_clock/text_clock_pattern.h"
namespace OHOS::Ace::NG {
void TextClockLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern<TextClockPattern>();
    CHECK_NULL_VOID(pattern);
    const auto& layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);

    auto textWrapper = layoutWrapper->GetOrCreateChildByIndex(0);
    CHECK_NULL_VOID(textWrapper);
    if (pattern->UseContentModifier()) {
        auto childList = layoutWrapper->GetAllChildrenWithBuild();
        for (const auto& child : childList) {
            if (child->GetHostNode()->GetId() != pattern->GetBuilderId()) {
                child->GetGeometryNode()->SetFrameSize(SizeF());
                child->GetGeometryNode()->SetContentSize(SizeF());
            } else {
                auto layoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
                child->Measure(layoutConstraint);
            }
        }
    } else {
        auto childConstraint = layoutProperty->CreateChildConstraint();
        textWrapper->Measure(childConstraint);
    }

    auto textSize = textWrapper->GetGeometryNode()->GetFrameSize();
    OptionalSizeF textClockFrameSize = { textSize.Width(), textSize.Height() };
    auto padding = layoutProperty->CreatePaddingAndBorder();
    AddPaddingToSize(padding, textClockFrameSize);
    auto constraint = layoutProperty->GetLayoutConstraint();
    const auto& minSize = constraint->minSize;
    const auto& maxSize = constraint->maxSize;
    textClockFrameSize.Constrain(minSize, maxSize);
    if (constraint->selfIdealSize.Width()) {
        textClockFrameSize.SetWidth(constraint->selfIdealSize.Width().value());
    }
    if (constraint->selfIdealSize.Height()) {
        textClockFrameSize.SetHeight(constraint->selfIdealSize.Height().value());
    }
    layoutWrapper->GetGeometryNode()->SetFrameSize(textClockFrameSize.ConvertToSizeT());
}
} // namespace OHOS::Ace::NG
