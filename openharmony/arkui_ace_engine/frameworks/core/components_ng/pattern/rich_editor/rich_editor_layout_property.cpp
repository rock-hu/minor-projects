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

#include "core/components_ng/pattern/rich_editor/rich_editor_layout_property.h"

namespace OHOS::Ace::NG {
RichEditorLayoutProperty::RichEditorLayoutProperty() = default;
RichEditorLayoutProperty::~RichEditorLayoutProperty() = default;

static std::string DisplayModeEnumToString(DisplayMode mode)
{
    switch (mode) {
        case DisplayMode::OFF: return "BarState.Off";
        case DisplayMode::AUTO: return "BarState.Auto";
        case DisplayMode::ON: return "BarState.On";
        default: return "BarState.Auto";
    }
}

void RichEditorLayoutProperty::ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const
{
    TextLayoutProperty::ToJsonValue(json, filter);
    auto barState = GetDisplayMode().value_or(DisplayMode::AUTO);
    json->Put("barState", DisplayModeEnumToString(barState).c_str());
    json->PutExtAttr("fontColor", GetTextColor().value_or(Color::BLACK).ColorToString().c_str(), filter);
};
} // namespace OHOS::Ace::NG
