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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATOR_NAVIGATOR_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATOR_NAVIGATOR_MODEL_NG_H

#include <string>

#include "base/utils/macros.h"
#include "core/components_ng/pattern/navigator/navigator_model.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT NavigatorModelNG : public OHOS::Ace::NavigatorModel {
public:
    void Create() override;
    void SetType(NavigatorType value) override;
    void SetActive(bool active) override;
    void SetUri(const std::string& uri) override;
    void SetParams(const std::string& params) override;
    // empty implementations
    void SetIsDefWidth(bool isDefWidth) override {}
    void SetIsDefHeight(bool isDefHeight) override {}
    static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId);
    static void SetParams(FrameNode* frameNode, const std::string& params);
    static void SetType(FrameNode* frameNode, NavigatorType value);
    static void SetUri(FrameNode* frameNode, const std::string& uri);
    static void SetActive(FrameNode* frameNode, bool active);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATOR_NAVIGATOR_MODEL_NG_H
