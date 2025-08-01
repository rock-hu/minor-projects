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

#include "core/components_ng/pattern/window_scene/root/root_scene_model.h"
#include "core/components_ng/base/view_stack_processor.h"

namespace OHOS::Ace::NG {
void RootSceneModel::Create()
{
    auto stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::ROOT_SCENE_ETS_TAG, nodeId);
    auto rootSceneNode = RootSceneNode::GetOrCreateRootSceneNode(nodeId);
    stack->Push(rootSceneNode);
}

RefPtr<RootSceneNode> RootSceneModel::CreateRootSceneNode(int32_t nodeId)
{
    return RootSceneNode::GetOrCreateRootSceneNode(nodeId);
}
} // namespace OHOS::Ace::NG
