/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/window_scene/scene/transform_scene.h"

#include "core/components_ng/pattern/window_scene/helper/window_scene_helper.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"

namespace OHOS::Ace::NG {
void TransformScene::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = AceType::DynamicCast<NG::RosenRenderContext>(host->GetRenderContext());
    CHECK_NULL_VOID(context);
    auto rsNode = Rosen::RSCanvasNode::Create();
    CHECK_NULL_VOID(rsNode);
    context->SetRSNode(rsNode);
}

uint32_t TransformScene::GetWindowPatternType() const
{
    return static_cast<uint32_t>(WindowPatternType::TRANSFORM_SCENE);
}
} // namespace OHOS::Ace::NG
