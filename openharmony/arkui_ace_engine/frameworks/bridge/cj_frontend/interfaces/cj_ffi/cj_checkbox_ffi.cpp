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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_checkbox_ffi.h"

#include "cj_lambda.h"

#include "core/components/checkable/checkable_theme.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/pattern/checkbox/checkbox_model_ng.h"

using namespace OHOS::Ace;
using namespace OHOS::Ace::Framework;

namespace {
constexpr int32_t INVALID_UNIT = -1;
constexpr float CHECK_BOX_MARK_SIZE_INVALID_VALUE = -1.0f;
} // namespace

extern "C" {
void FfiOHOSAceFrameworkCheckBoxCreate(const char* name, const char* group)
{
    auto checkboxName = std::optional<std::string>(name);
    auto checkboxGroup = std::optional<std::string>(group);
    CheckBoxModel::GetInstance()->Create(checkboxName, checkboxGroup, "Checkbox");
}

void FfiOHOSAceFrameworkCheckBoxSelect(bool value)
{
    CheckBoxModel::GetInstance()->SetSelect(value);
}

void FfiOHOSAceFrameworkCheckBoxSetSelectedColor(uint32_t color)
{
    CheckBoxModel::GetInstance()->SetSelectedColor(Color(color));
}

void FfiOHOSAceFrameworkCheckBoxSetWidth(double width, uint32_t unit)
{
    Dimension value(width, static_cast<DimensionUnit>(unit));

    NG::ViewAbstract::SetWidth(NG::CalcLength(value));
}

void FfiOHOSAceFrameworkCheckBoxSetHeight(double height, uint32_t unit)
{
    Dimension value(height, static_cast<DimensionUnit>(unit));

    NG::ViewAbstract::SetHeight(NG::CalcLength(value));
}

void FfiOHOSAceFrameworkCheckBoxSetSize(double width, uint32_t widthUnit, double height, uint32_t heightUnit)
{
    FfiOHOSAceFrameworkCheckBoxSetWidth(width, widthUnit);
    FfiOHOSAceFrameworkCheckBoxSetHeight(height, heightUnit);
}

void FfiOHOSAceFrameworkCheckBoxSetPadding(double padding, uint32_t unit)
{
    Dimension value(padding, static_cast<DimensionUnit>(unit));

    NG::ViewAbstract::SetPadding(NG::CalcLength(value));
}

void FfiOHOSAceFrameworkCheckBoxSetPaddings(CJEdge params)
{
    Dimension top(params.top, static_cast<DimensionUnit>(params.topUnit));
    Dimension right(params.right, static_cast<DimensionUnit>(params.rightUnit));
    Dimension bottom(params.bottom, static_cast<DimensionUnit>(params.bottomUnit));
    Dimension left(params.left, static_cast<DimensionUnit>(params.leftUnit));

    NG::PaddingProperty padding;
    padding.top = NG::CalcLength(top);
    padding.right = NG::CalcLength(right);
    padding.bottom = NG::CalcLength(bottom);
    padding.left = NG::CalcLength(left);

    NG::ViewAbstract::SetPadding(padding);
}

void FfiOHOSAceFrameworkCheckBoxSetOnChange(void (*callback)(bool isOn))
{
    CheckBoxModel::GetInstance()->SetOnChange(CJLambda::Create(callback));
}

void FfiOHOSAceFrameworkCheckBoxSetCheckboxStyle(int32_t checkBoxStyle)
{
    CheckBoxStyle curCheckBoxStyle = static_cast<CheckBoxStyle>(checkBoxStyle);
    CheckBoxModel::GetInstance()->SetCheckboxStyle(curCheckBoxStyle);
}

void FfiCheckBoxSetResponseRegion(CJResponseRegion value)
{
    std::vector<DimensionRect> result;
    ParseCJResponseRegion(value, result);
    CheckBoxModel::GetInstance()->SetResponseRegion(result);
}

void FfiCheckBoxSetResponseRegionArray(VectorStringPtr vecContent)
{
    std::vector<DimensionRect> result;
    ParseVectorStringPtr(vecContent, result);
    CheckBoxModel::GetInstance()->SetResponseRegion(result);
}

void FfiCheckBoxCreateWithIndicator(const char* name, const char* group, void (*indicatorBuilder)())
{
    auto lambda = CJLambda::Create(indicatorBuilder);
    auto targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    std::optional<std::function<void()>> customBuilderFunc = [func = std::move(lambda), node = targetNode]() {
        PipelineContext::SetCallBackNode(node);
        func();
    };
    CheckBoxModel::GetInstance()->Create(name, group, "Checkbox");
    CheckBoxModel::GetInstance()->SetBuilder(customBuilderFunc);
}

void FfiCheckBoxUnselectedColor(uint32_t color)
{
    CheckBoxModel::GetInstance()->SetUnSelectedColor(Color(color));
}

void FfiCheckBoxMarkStyle(
    uint32_t strokeColor, double size, int32_t sizeUnit, double strokeWidth, int32_t strokeWidthUnit)
{
    auto theme = GetTheme<CheckboxTheme>();
    Color strokeColorValue = Color(strokeColor);
    CheckBoxModel::GetInstance()->SetCheckMarkColor(strokeColorValue);
    if (sizeUnit != INVALID_UNIT && static_cast<DimensionUnit>(sizeUnit) != DimensionUnit::PERCENT && size >= 0.0) {
        CheckBoxModel::GetInstance()->SetCheckMarkSize(CalcDimension(size, static_cast<DimensionUnit>(sizeUnit)));
    } else {
        CheckBoxModel::GetInstance()->SetCheckMarkSize(Dimension(CHECK_BOX_MARK_SIZE_INVALID_VALUE));
    }
    if (strokeWidthUnit != INVALID_UNIT && static_cast<DimensionUnit>(strokeWidthUnit) != DimensionUnit::PERCENT &&
        strokeWidth >= 0.0) {
        CheckBoxModel::GetInstance()->SetCheckMarkWidth(
            CalcDimension(strokeWidth, static_cast<DimensionUnit>(strokeWidthUnit)));
    } else {
        CheckBoxModel::GetInstance()->SetCheckMarkWidth(theme->GetCheckStroke());
    }
}
}
