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

#include "core/components_v2/inspector/navigation_title_composed_element.h"

namespace OHOS::Ace::V2 {

std::unique_ptr<JsonValue> NavigationTitleComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    return resultJson;
}

void NavigationTitleComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto element = AceType::Claim(this);
    auto child = GetElementChildBySlot(element, slot);
    if (!child) {
        return;
    }
    element->UpdateChild(child, newComponent);
}

} // namespace OHOS::Ace::V2