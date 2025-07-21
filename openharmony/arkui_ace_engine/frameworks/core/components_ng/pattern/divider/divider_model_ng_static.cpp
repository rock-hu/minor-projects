/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/divider/divider_model_ng_static.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"

namespace OHOS::Ace::NG {

void DividerModelNGStatic::StrokeWidth(FrameNode* frameNode, std::optional<Dimension> valueOpt)
{
    if (valueOpt) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(DividerLayoutProperty, StrokeWidth, valueOpt.value(), frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(DividerLayoutProperty, StrokeWidth, frameNode);
    }
}

void DividerModelNGStatic::LineCap(FrameNode* frameNode, const std::optional<enum LineCap> valueOpt)
{
    if (valueOpt) {
        ACE_UPDATE_NODE_PAINT_PROPERTY(DividerRenderProperty, LineCap, valueOpt.value(), frameNode);
    } else {
        ACE_RESET_NODE_PAINT_PROPERTY(DividerRenderProperty, LineCap, frameNode);
    }
}

void DividerModelNGStatic::SetDividerColor(FrameNode* frameNode, std::optional<Color> colorOpt)
{
    if (colorOpt) {
        ACE_UPDATE_NODE_PAINT_PROPERTY(DividerRenderProperty, DividerColor, colorOpt.value(), frameNode);
    } else {
        ACE_RESET_NODE_PAINT_PROPERTY(DividerRenderProperty, DividerColor, frameNode);
    }
}
}