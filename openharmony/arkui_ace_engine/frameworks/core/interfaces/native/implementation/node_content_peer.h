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
#ifndef FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_ARKOALA_IMPL_NODE_CONTENT_PEER_H
#define FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_ARKOALA_IMPL_NODE_CONTENT_PEER_H

#include "core/interfaces/native/generated/interface/arkoala_api_generated.h"
#include "core/components_ng/syntax/node_content.h"

struct NodeContentPeer {
    OHOS::Ace::RefPtr<OHOS::Ace::NG::NodeContent> content;

    static NodeContentPeer* Create(const OHOS::Ace::RefPtr<OHOS::Ace::NG::NodeContent>& src)
    {
        auto nodeContent = new NodeContentPeer();
        nodeContent->content = src;
        return nodeContent;
    }

    virtual ~NodeContentPeer() = default;
};

#endif // FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_ARKOALA_IMPL_NODE_CONTENT_PEER_H
