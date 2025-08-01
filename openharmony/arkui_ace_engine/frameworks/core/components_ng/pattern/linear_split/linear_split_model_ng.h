/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LINEAR_SPLIT_LINEAR_SPLIT_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LINEAR_SPLIT_LINEAR_SPLIT_MODEL_NG_H

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/linear_split/linear_split_model.h"

namespace OHOS::Ace::NG {

class  ACE_EXPORT LinearSplitModelNG : public OHOS::Ace::LinearSplitModel {
public:
    void Create(SplitType splitType) override;
    void SetResizable(NG::SplitType splitType, bool resizable) override;
    void SetDivider(NG::SplitType splitType, const NG::ColumnSplitDivider& divider) override;
    void ResetResObj(const std::string& key) override;
    static void SetResizable(FrameNode* frameNode, NG::SplitType splitType, bool resizable);
    static void SetDivider(FrameNode* frameNode, NG::SplitType splitType, const NG::ColumnSplitDivider& divider);
    static void ResetResObj(FrameNode* frameNode, const std::string& key);
    static void RegisterResObj(
        const RefPtr<ResourceObject>& resObj, NG::ColumnSplitDivider& divider, const std::string key);
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LINEAR_SPLIT_LINEAR_SPLIT_MODEL_NG_H
