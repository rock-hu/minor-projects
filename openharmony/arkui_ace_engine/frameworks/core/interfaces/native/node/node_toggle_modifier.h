/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_INTERFACE_INNER_API_NATIVE_NODE_NODE_TOGGLE_MODIFIER_H
#define FRAMEWORKS_INTERFACE_INNER_API_NATIVE_NODE_NODE_TOGGLE_MODIFIER_H

#include "core/interfaces/native/node/node_api.h"

namespace OHOS::Ace::NG::NodeModifier {
    const ArkUIToggleModifier* GetToggleModifier();
    const CJUIToggleModifier* GetCJUIToggleModifier();
    void SetOnToggleChange(ArkUINodeHandle node, void* extraParam);

    void ResetOnToggleChange(ArkUINodeHandle node);
} // namespace OHOS::Ace::NG::NodeModifier

#endif // FRAMEWORKS_INTERFACE_INNER_API_NATIVE_NODE_NODE_TOGGLE_MODIFIER_H
