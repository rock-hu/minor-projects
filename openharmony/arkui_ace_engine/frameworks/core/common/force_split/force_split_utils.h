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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FORCE_SPLIT_FORCE_SPLIT_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FORCE_SPLIT_FORCE_SPLIT_UTILS_H

#include "core/components_ng/base/frame_node.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

class NavDestinationGroupNode;

class ForceSplitUtils {
public:
    static RefPtr<FrameNode> CreatePlaceHolderContent(const RefPtr<PipelineContext>& context);
    static RefPtr<NavDestinationGroupNode> CreatePlaceHolderNavDestination(const RefPtr<PipelineContext>& context);
    static bool IsNavDestinationHomePage(const RefPtr<NavDestinationGroupNode>& node);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FORCE_SPLIT_FORCE_SPLIT_UTILS_H
