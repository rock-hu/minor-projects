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

#include "core/components/form/rosen_render_form.h"

#include "core/components/form/render_form.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

std::unique_ptr<DrawDelegate> RosenRenderForm::GetDrawDelegate()
{
    auto drawDelegate = std::make_unique<DrawDelegate>();

    drawDelegate->SetDrawRSFrameCallback(
        [weakForm = WeakClaim(this)](std::shared_ptr<RSNode>& node, const Rect& dirty) {
            auto form = weakForm.Upgrade();
            if (!form) {
                return;
            }
            if (!form->GetRSNode()) {
                form->SyncRSNodeBoundary(true, true);
            }
            auto rsNode = form->GetRSNode();
            rsNode->AddChild(node, -1);

            form->MarkNeedLayout(true, true);
            form->MarkNeedRender(true);
        });

    return drawDelegate;
}

void RosenRenderForm::RemoveChildren()
{
    auto rsNode = GetRSNode();
    if (rsNode) {
        rsNode->ClearChildren();
    }
    MarkNeedLayout(true, true);
}

} // namespace OHOS::Ace