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

#include "core/components_ng/pattern/navigation/tool_bar_node.h"

#include "core/components_ng/pattern/navigation/tool_bar_pattern.h"

namespace OHOS::Ace::NG {
NavToolbarNode::NavToolbarNode(const std::string& tag, int32_t nodeId)
    : FrameNode(tag, nodeId, MakeRefPtr<NavToolbarPattern>())
{}

NavToolbarNode::~NavToolbarNode()
{
    auto pipeline = GetContextRefPtr();
    CHECK_NULL_VOID(pipeline);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    auto toolBarPattern = GetPattern<NavToolbarPattern>();
    CHECK_NULL_VOID(toolBarPattern);
    auto toolBarDialog = toolBarPattern->GetDialogNode();
    if (toolBarDialog) {
        overlayManager->CloseDialog(toolBarDialog);
    }
}

RefPtr<NavToolbarNode> NavToolbarNode::GetOrCreateToolbarNode(
    const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator)
{
    auto frameNode = GetFrameNode(tag, nodeId);
    CHECK_NULL_RETURN(!frameNode, AceType::DynamicCast<NavToolbarNode>(frameNode));
    auto pattern = patternCreator ? patternCreator() : MakeRefPtr<Pattern>();
    auto toolbarNode = AceType::MakeRefPtr<NavToolbarNode>(tag, nodeId, pattern);
    toolbarNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(toolbarNode);
    return toolbarNode;
}
} // namespace OHOS::Ace::NG
