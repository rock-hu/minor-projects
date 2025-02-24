/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include "core/interfaces/native/node/counter_modifier.h"

#include "core/components_ng/pattern/counter/counter_model_ng.h"

namespace OHOS::Ace::NG {
void SetEnableInc(ArkUINodeHandle node, ArkUI_Bool enableInc)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::SetEnableInc(frameNode, enableInc);
}
void ResetEnableInc(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::SetEnableInc(frameNode, true);
}
void SetEnableDec(ArkUINodeHandle node, ArkUI_Bool enableDec)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::SetEnableDec(frameNode, enableDec);
}
void ResetEnableDec(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::SetEnableDec(frameNode, true);
}

void SetCounterHeight(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension height = Dimension(value, static_cast<OHOS::Ace::DimensionUnit>(unit));
    CounterModelNG::SetHeight(frameNode, height);
}
void ResetCounterHeight(ArkUINodeHandle node) {}
void SetCounterWidth(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension width = Dimension(value, static_cast<OHOS::Ace::DimensionUnit>(unit));
    CounterModelNG::SetWidth(frameNode, width);
}

void ResetCounterWidth(ArkUINodeHandle node) {}

void SetCounterBackgroundColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::SetBackgroundColor(frameNode, Color(color));
}

void ResetCounterBackgroundColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CounterModelNG::ResetBackgroundColor(frameNode);
}

namespace NodeModifier {
const ArkUICounterModifier* GetCounterModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const ArkUICounterModifier modifier = {
        .setEnableInc = SetEnableInc,
        .resetEnableInc = ResetEnableInc,
        .setEnableDec = SetEnableDec,
        .resetEnableDec = ResetEnableDec,
        .setCounterHeight = SetCounterHeight,
        .resetCounterHeight = ResetCounterHeight,
        .setCounterWidth = SetCounterWidth,
        .resetCounterWidth = ResetCounterWidth,
        .setCounterBackgroundColor = SetCounterBackgroundColor,
        .resetCounterBackgroundColor = ResetCounterBackgroundColor,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line

    return &modifier;
}

const CJUICounterModifier* GetCJUICounterModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const CJUICounterModifier modifier = {
        .setEnableInc = SetEnableInc,
        .resetEnableInc = ResetEnableInc,
        .setEnableDec = SetEnableDec,
        .resetEnableDec = ResetEnableDec,
        .setCounterHeight = SetCounterHeight,
        .resetCounterHeight = ResetCounterHeight,
        .setCounterWidth = SetCounterWidth,
        .resetCounterWidth = ResetCounterWidth,
        .setCounterBackgroundColor = SetCounterBackgroundColor,
        .resetCounterBackgroundColor = ResetCounterBackgroundColor,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line

    return &modifier;
}
}
} // namespace OHOS::Ace::NG
