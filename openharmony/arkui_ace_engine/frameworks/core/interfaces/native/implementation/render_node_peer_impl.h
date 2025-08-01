/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_RENDER_NODE_PEER_IMPL_H
#define FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_RENDER_NODE_PEER_IMPL_H

#include "base/memory/referenced.h"
#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/generated/interface/arkoala_api_generated.h"

struct RenderNodePeer {
public:
    RenderNodePeer() = default;
    RenderNodePeer(const OHOS::Ace::RefPtr<OHOS::Ace::NG::FrameNode>& src) : node(src) {}
    ~RenderNodePeer() = default;

    static RenderNodePeer* Create(Ark_UIContext uiContext)
    {
        return new RenderNodePeer;
    }

    static RenderNodePeer* Create(const OHOS::Ace::RefPtr<OHOS::Ace::NG::FrameNode>& src)
    {
        return new RenderNodePeer(src);
    }

    static RenderNodePeer* Create(OHOS::Ace::NG::FrameNode* src)
    {
        return new RenderNodePeer(OHOS::Ace::Referenced::Claim<OHOS::Ace::NG::FrameNode>(src));
    }

    static void Destroy(RenderNodePeer* peer)
    {
        delete peer;
    }

    const OHOS::Ace::RefPtr<OHOS::Ace::NG::FrameNode>& GetFrameNode() const
    {
        return node;
    }

private:
    OHOS::Ace::RefPtr<OHOS::Ace::NG::FrameNode> node;
};
#endif // FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_RENDER_NODE_PEER_IMPL_H
