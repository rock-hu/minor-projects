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

#include "core/components_ng/pattern/window_scene/screen/screen_model.h"

#include "core/components_ng/base/view_stack_processor.h"

#ifndef ARKUI_CAPI_UNITTEST
#include "screen_session_manager_client.h"
#include "core/components_ng/pattern/window_scene/screen/screen_pattern.h"
#include "core/components_ng/pattern/window_scene/screen/screen_node.h"
#else
#include "test/unittest/capi/stubs/mock_window_scene.h"
#endif

namespace OHOS::Ace::NG {
void ScreenModel::Create(uint64_t screenId)
{
    auto screenSession = Rosen::ScreenSessionManagerClient::GetInstance().GetScreenSession(screenId);
    if (screenSession == nullptr) {
        LOGE("screenSession is nullptr");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_SCOPED_TRACE("Create[%s][self:%d][%" PRIu64 "]", V2::SCREEN_ETS_TAG, nodeId, screenSession->GetScreenId());
    auto frameNode = ScreenNode::GetOrCreateScreenNode(V2::SCREEN_ETS_TAG, nodeId,
        [&screenSession]() { return AceType::MakeRefPtr<ScreenPattern>(screenSession); });
    stack->Push(frameNode);
    frameNode->SetExclusiveEventForChild(true);
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, Alignment, Alignment::TOP_LEFT);
}

RefPtr<FrameNode> ScreenModel::CreateFrameNode(int32_t nodeId)
{
    auto frameNode = ScreenNode::GetOrCreateScreenNode(V2::SCREEN_ETS_TAG, nodeId,
        []() { return AceType::MakeRefPtr<ScreenPattern>(nullptr); });
    return frameNode;
}

void ScreenModel::SetOptions(FrameNode* frameNode, uint64_t screenId)
{
    auto pattern = frameNode->GetPattern<ScreenPattern>();
    CHECK_NULL_VOID(pattern);
    auto screenSession = Rosen::ScreenSessionManagerClient::GetInstance().GetScreenSession(screenId);
    CHECK_NULL_VOID(screenSession);
    pattern->SetScreenSession(screenSession);
    frameNode->SetExclusiveEventForChild(true);
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, Alignment, Alignment::TOP_LEFT);
}
} // namespace OHOS::Ace::NG
