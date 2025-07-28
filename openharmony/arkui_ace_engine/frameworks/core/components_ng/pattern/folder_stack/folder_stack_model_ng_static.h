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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FOLDER_STACK_MODEL_NG_STATIC_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FOLDER_STACK_MODEL_NG_STATIC_H

#include "base/utils/macros.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/folder_stack/folder_stack_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT FolderStackModelNGStatic : public FolderStackModel {
public:
    static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId);
    static void SetUpdateUpperItems(FrameNode* frameNode, const std::vector<std::string>& itemId);
    static void SetAlignment(FrameNode* frameNode, const std::optional<Alignment>& valueOpt);
};
}

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FOLDER_STACK_MODEL_NG_STATIC_H
