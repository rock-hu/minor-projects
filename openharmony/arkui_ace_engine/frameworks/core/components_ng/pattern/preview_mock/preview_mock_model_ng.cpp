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
#include "core/components_ng/pattern/preview_mock/preview_mock_model_ng.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/preview_mock/preview_mock_model.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/pattern/preview_mock/preview_mock_pattern.h"

namespace OHOS::Ace::NG {
void PreviewMockModelNG::Create(const std::string& content)
{
    constexpr double DEFAULT_HEIGHT = 150;
    const Color bgColor = Color::FromString("#808080");
    constexpr Dimension DEFAULT_FONT_SIZE = 20.0_vp;
    const std::string presentationText("Preview not available for this component.");
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", content.c_str(), nodeId);
    auto frameNode =
        FrameNode::GetOrCreateFrameNode(content, nodeId, []() { return AceType::MakeRefPtr<PreviewMockPattern>(); });
    CHECK_NULL_VOID(frameNode);
    stack->Push(frameNode);
    ACE_UPDATE_LAYOUT_PROPERTY(TextLayoutProperty, Content, presentationText);
    ACE_UPDATE_LAYOUT_PROPERTY(TextLayoutProperty, FontSize, DEFAULT_FONT_SIZE);
    auto renderContext = frameNode->GetRenderContext();
    if (renderContext) {
        renderContext->UpdateBackgroundColor(bgColor);
    }
    ViewAbstract::SetHeight(CalcLength(DEFAULT_HEIGHT));
}
} // namespace OHOS::Ace::NG
