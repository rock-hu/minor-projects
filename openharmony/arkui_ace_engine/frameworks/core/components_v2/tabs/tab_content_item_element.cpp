/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/tabs/tab_content_item_element.h"

#include "core/components_v2/tabs/tabs_helper.h"

namespace OHOS::Ace::V2 {

void TabContentItemElement::Update()
{
    auto component = AceType::DynamicCast<TabContentItemComponent>(component_);
    if (!component) {
        LOGE("TabContentItemComponent is null");
        return;
    }
    barIcon_ = component->GetBarIcon();
    barText_ = component->GetBarText();
    if (component->GetBarElementId() != ElementRegister::UndefinedElementId) {
        tabBarItemElementId_ = component->GetBarElementId();
    }

    FlexElement::Update();
}

void TabContentItemElement::PerformBuild()
{
    auto component = AceType::DynamicCast<TabContentItemComponent>(component_);
    if (!component) {
        LOGE("TabContentItemComponent is null");
        return;
    }

    if (!component->HasBuilder()) {
        // No builder function, continue with old code path
        ColumnElement::PerformBuild();
        return;
    }

    // In new code path we try to execute tab's builder function here
    auto newComponets = component->ExecuteBuilder();
    UpdateChild(GetFirstChild(), newComponets);
}

} // namespace OHOS::Ace::v2
