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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_LINK_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_LINK_PATTERN_H

#include "core/components_ng/pattern/form_link/form_link_info.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class FormLinkPattern : public Pattern {
    DECLARE_ACE_TYPE(FormLinkPattern, Pattern);

public:
    FormLinkPattern() = default;
    ~FormLinkPattern() override = default;

    bool IsMeasureBoundary() const override
    {
        return true;
    }

    bool IsAtomicNode() const override
    {
        return false;
    }

    void SetAction(const std::string& action)
    {
        formLinkInfo_.SetAction(action);
    }

private:
    void OnAttachToFrameNode() override;
    void OnAreaChangedInner() override;

    FormLinkInfo formLinkInfo_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_LINK_PATTERN_H
