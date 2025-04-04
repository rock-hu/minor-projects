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

#include "core/common/modal_ui_extension.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_model_ng.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/ui_extension_pattern.h"

namespace OHOS::Ace {
RefPtr<NG::FrameNode> ModalUIExtension::Create(const AAFwk::Want& want, const ModalUIExtensionCallbacks& callbacks,
    const NG::InnerModalUIExtensionConfig& config)
{
    return NG::UIExtensionModelNG::Create(want, callbacks, config);
}

int32_t ModalUIExtension::GetSessionId(const RefPtr<NG::FrameNode>& uiExtNode)
{
    auto pattern = uiExtNode->GetPattern<NG::UIExtensionPattern>();
    CHECK_NULL_RETURN(pattern, 0);
    return pattern->GetSessionId();
}

void ModalUIExtension::SetBindModalCallback(const RefPtr<NG::FrameNode>& uiExtNode,
    std::function<void()>&& bindModalCallback)
{
    CHECK_NULL_VOID(uiExtNode);
    auto pattern = uiExtNode->GetPattern<NG::UIExtensionPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetBindModalCallback(std::move(bindModalCallback));
}
} // namespace OHOS::Ace
