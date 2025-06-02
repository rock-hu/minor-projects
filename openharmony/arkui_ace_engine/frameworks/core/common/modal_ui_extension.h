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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MODAL_UI_EXTENSION_HELPER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MODAL_UI_EXTENSION_HELPER_H

#include <cstdint>

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_model.h"

namespace OHOS::AAFwk {
class Want;
} // namespace OHOS::AAFwk

namespace OHOS::Ace {
struct ModalUIExtensionCallbacks;
} // namespace OHOS::Ace

namespace OHOS::Ace {
class ModalUIExtension final {
public:
    static RefPtr<NG::FrameNode> Create(const AAFwk::Want& want, const ModalUIExtensionCallbacks& callbacks,
        const NG::InnerModalUIExtensionConfig& config);

    static int32_t GetSessionId(const RefPtr<NG::FrameNode>& uiExtNode);

    static void SetBindModalCallback(const RefPtr<NG::FrameNode>& uiExtNode,
        std::function<void()>&& bindModalCallback);
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MODAL_UI_EXTENSION_HELPER_H
