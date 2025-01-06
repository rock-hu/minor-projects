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
#include "core/interfaces/native/node/checkboxgroup_modifier.h"

#include "core/components_ng/base/view_abstract.h"

namespace OHOS::Ace::NG {
static uint32_t ERROR_UINT_CODE = -1;
static const float ERROR_FLOAT_CODE = -1.0f;
static const int32_t ERROR_INT_CODE = -1;
static std::string groupNameValue;
const DimensionUnit DEFAULT_UNIT = DimensionUnit::VP;
void SetCheckboxGroupSelectedColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetSelectedColor(frameNode, Color(color));
}

void ResetCheckboxGroupSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto checkBoxTheme = themeManager->GetTheme<CheckboxTheme>();
    CheckBoxGroupModelNG::SetSelectedColor(frameNode, checkBoxTheme->GetActiveColor());
}

void SetCheckboxGroupUnSelectedColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetUnSelectedColor(frameNode, Color(color));
}

void ResetCheckboxGroupUnSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto checkBoxTheme = themeManager->GetTheme<CheckboxTheme>();
    CheckBoxGroupModelNG::SetUnSelectedColor(frameNode, checkBoxTheme->GetInactiveColor());
}

void SetCheckboxGroupSelectAll(ArkUINodeHandle node, ArkUI_Bool isSelected)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetSelectAll(frameNode, isSelected);
}

void ResetCheckboxGroupSelectAll(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetSelectAll(frameNode, false);
}

void SetCheckboxGroupWidth(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit, ArkUI_CharPtr calcValue)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto unitEnum = static_cast<OHOS::Ace::DimensionUnit>(unit);
    if (unitEnum == DimensionUnit::CALC) {
        ViewAbstract::SetWidth(
            frameNode, CalcLength(CalcLength(std::string(calcValue))));
    } else {
        ViewAbstract::SetWidth(frameNode, CalcLength(value, unitEnum));
    }
}

void ResetCheckboxGroupWidth(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ViewAbstract::ClearWidthOrHeight(frameNode, true);
}

void SetCheckboxGroupHeight(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit, ArkUI_CharPtr calcValue)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto unitEnum = static_cast<OHOS::Ace::DimensionUnit>(unit);
    if (unitEnum == DimensionUnit::CALC) {
        ViewAbstract::SetHeight(
            frameNode, CalcLength(CalcLength(std::string(calcValue))));
    } else {
        ViewAbstract::SetHeight(frameNode, CalcLength(value, unitEnum));
    }
}

void ResetCheckboxGroupHeight(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ViewAbstract::ClearWidthOrHeight(frameNode, false);
}

void SetCheckboxGroupMark(ArkUINodeHandle node, uint32_t color, ArkUI_Float32 sizeValue, ArkUI_Float32 widthValue)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetCheckMarkColor(frameNode, Color(color));

    Dimension size = Dimension(sizeValue, DEFAULT_UNIT);
    CheckBoxGroupModelNG::SetCheckMarkSize(frameNode, size);

    Dimension width = Dimension(widthValue, DEFAULT_UNIT);
    CheckBoxGroupModelNG::SetCheckMarkWidth(frameNode, width);
}

void ResetCheckboxGroupMark(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto checkBoxTheme = themeManager->GetTheme<CheckboxTheme>();

    CheckBoxGroupModelNG::SetCheckMarkColor(frameNode, checkBoxTheme->GetPointColor());

    Dimension size = Dimension(-1.0f);
    CheckBoxGroupModelNG::SetCheckMarkSize(frameNode, size);

    CheckBoxGroupModelNG::SetCheckMarkWidth(frameNode, checkBoxTheme->GetCheckStroke());
}

void SetCheckboxGroupStyle(ArkUINodeHandle node, ArkUI_Int32 style)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetCheckboxGroupStyle(frameNode, static_cast<CheckBoxStyle>(style));
}

void ResetCheckboxGroupStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetCheckboxGroupStyle(frameNode, CheckBoxStyle::CIRCULAR_STYLE);
}

void SetCheckboxGroupName(ArkUINodeHandle node, ArkUI_CharPtr group)
{
    CHECK_NULL_VOID(group);
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetCheckboxGroupName(frameNode, std::string(group));
}

ArkUI_CharPtr GetCheckboxGroupName(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, "");
    groupNameValue = CheckBoxGroupModelNG::GetCheckboxGroupName(frameNode);
    return groupNameValue.c_str();
}

ArkUI_Bool GetCheckboxGroupSelectAll(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_INT_CODE);
    return static_cast<ArkUI_Bool>(CheckBoxGroupModelNG::GetSelect(frameNode));
}

ArkUI_Uint32 GetCheckboxGroupSelectedColor(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return CheckBoxGroupModelNG::GetSelectedColor(frameNode).GetValue();
}

ArkUI_Uint32 GetCheckboxGroupUnSelectedColor(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return CheckBoxGroupModelNG::GetUnSelectedColor(frameNode).GetValue();
}

ArkUI_Uint32 GetCheckboxGroupMarkColor(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return CheckBoxGroupModelNG::GetCheckMarkColor(frameNode).GetValue();
}

ArkUI_Float64 GetCheckboxGroupMarkSize(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_FLOAT_CODE);
    return CheckBoxGroupModelNG::GetCheckMarkSize(frameNode).Value();
}

ArkUI_Float64 GetCheckboxGroupMarkWidth(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_FLOAT_CODE);
    return CheckBoxGroupModelNG::GetCheckMarkWidth(frameNode).Value();
}

ArkUI_Int32 GetCheckboxGroupStyle(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_INT_CODE);
    return static_cast<ArkUI_Int32>(CheckBoxGroupModelNG::GetCheckboxGroupStyle(frameNode));
}

namespace NodeModifier {
const ArkUICheckboxGroupModifier* GetCheckboxGroupModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const ArkUICheckboxGroupModifier modifier = {
        .setCheckboxGroupSelectedColor = SetCheckboxGroupSelectedColor,
        .resetCheckboxGroupSelectedColor = ResetCheckboxGroupSelectedColor,
        .setCheckboxGroupUnSelectedColor = SetCheckboxGroupUnSelectedColor,
        .resetCheckboxGroupUnSelectedColor = ResetCheckboxGroupUnSelectedColor,
        .setCheckboxGroupSelectAll = SetCheckboxGroupSelectAll,
        .resetCheckboxGroupSelectAll = ResetCheckboxGroupSelectAll,
        .setCheckboxGroupWidth = SetCheckboxGroupWidth,
        .resetCheckboxGroupWidth = ResetCheckboxGroupWidth,
        .setCheckboxGroupHeight = SetCheckboxGroupHeight,
        .resetCheckboxGroupHeight = ResetCheckboxGroupHeight,
        .setCheckboxGroupMark = SetCheckboxGroupMark,
        .resetCheckboxGroupMark = ResetCheckboxGroupMark,
        .setCheckboxGroupStyle = SetCheckboxGroupStyle,
        .resetCheckboxGroupStyle = ResetCheckboxGroupStyle,
        .setCheckboxGroupName = SetCheckboxGroupName,
        .getCheckboxGroupName = GetCheckboxGroupName,
        .getCheckboxGroupSelectAll = GetCheckboxGroupSelectAll,
        .getCheckboxGroupSelectedColor = GetCheckboxGroupSelectedColor,
        .getCheckboxGroupUnSelectedColor = GetCheckboxGroupUnSelectedColor,
        .getCheckboxGroupMarkColor = GetCheckboxGroupMarkColor,
        .getCheckboxGroupMarkSize = GetCheckboxGroupMarkSize,
        .getCheckboxGroupMarkWidth = GetCheckboxGroupMarkWidth,
        .getCheckboxGroupStyle = GetCheckboxGroupStyle,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line
    return &modifier;
}

const CJUICheckboxGroupModifier* GetCJUICheckboxGroupModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const CJUICheckboxGroupModifier modifier = {
        .setCheckboxGroupSelectedColor = SetCheckboxGroupSelectedColor,
        .resetCheckboxGroupSelectedColor = ResetCheckboxGroupSelectedColor,
        .setCheckboxGroupUnSelectedColor = SetCheckboxGroupUnSelectedColor,
        .resetCheckboxGroupUnSelectedColor = ResetCheckboxGroupUnSelectedColor,
        .setCheckboxGroupSelectAll = SetCheckboxGroupSelectAll,
        .resetCheckboxGroupSelectAll = ResetCheckboxGroupSelectAll,
        .setCheckboxGroupWidth = SetCheckboxGroupWidth,
        .resetCheckboxGroupWidth = ResetCheckboxGroupWidth,
        .setCheckboxGroupHeight = SetCheckboxGroupHeight,
        .resetCheckboxGroupHeight = ResetCheckboxGroupHeight,
        .setCheckboxGroupMark = SetCheckboxGroupMark,
        .resetCheckboxGroupMark = ResetCheckboxGroupMark,
        .setCheckboxGroupStyle = SetCheckboxGroupStyle,
        .resetCheckboxGroupStyle = ResetCheckboxGroupStyle,
        .setCheckboxGroupName = SetCheckboxGroupName,
        .getCheckboxGroupName = GetCheckboxGroupName,
        .getCheckboxGroupSelectAll = GetCheckboxGroupSelectAll,
        .getCheckboxGroupSelectedColor = GetCheckboxGroupSelectedColor,
        .getCheckboxGroupUnSelectedColor = GetCheckboxGroupUnSelectedColor,
        .getCheckboxGroupMarkColor = GetCheckboxGroupMarkColor,
        .getCheckboxGroupMarkSize = GetCheckboxGroupMarkSize,
        .getCheckboxGroupMarkWidth = GetCheckboxGroupMarkWidth,
        .getCheckboxGroupStyle = GetCheckboxGroupStyle,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line
    return &modifier;
}

void SetCheckboxGroupChange(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [extraParam](const bool value) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_CHECKBOX_GROUP_CHANGE;
        event.componentAsyncEvent.data[0].i32 = static_cast<int>(value);
        SendArkUISyncEvent(&event);
    };
    CheckBoxGroupModelNG::SetOnChange(frameNode, std::move(onEvent));
}

void ResetCheckboxGroupChange(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CheckBoxGroupModelNG::SetOnChange(frameNode, nullptr);
}
}
}
