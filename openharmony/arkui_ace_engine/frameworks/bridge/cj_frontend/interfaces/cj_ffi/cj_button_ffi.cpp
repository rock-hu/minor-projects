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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_button_ffi.h"

#include "bridge/common/utils/utils.h"
#include "core/components_ng/pattern/button/button_model.h"

using namespace OHOS::Ace;
using namespace OHOS::Ace::Framework;

namespace {
const std::vector<BorderStyle> BORDER_STYLES = { BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED };
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<ButtonType> BUTTON_TYPES = { ButtonType::NORMAL, ButtonType::CAPSULE, ButtonType::CIRCLE };
} // namespace

extern "C" {
void FfiOHOSAceFrameworkButtonCreateWithChild()
{
    CreateWithPara params;
    params.parseSuccess = false;
    params.optionSetFirst = false;
    ButtonModel::GetInstance()->CreateWithChild(params);
    ViewAbstractModel::GetInstance()->SetHoverEffect(HoverEffectType::SCALE);
}

void FfiOHOSAceFrameworkButtonCreateWithLabel(const char* label)
{
    CreateWithPara params;
    LOGI("done create params");
    params.label = label;
    params.parseSuccess = true;
    params.optionSetFirst = false;
    LOGI("done set label");
    auto model = ButtonModel::GetInstance();
    if (!model) {
        LOGE("ButtonModel::GetInstance returns null");
        return;
    }
    LOGI("before create");
    std::list<RefPtr<Component>> children;
    model->CreateWithLabel(params, children);
    model->Create(params, children);
    LOGI("done create with child");
    ViewAbstractModel::GetInstance()->SetHoverEffect(HoverEffectType::SCALE);
    LOGI("done set hover effect");
}

void FfiOHOSAceFrameworkButtonSetFontSize(double fontSize, int32_t unit)
{
    Dimension value(fontSize, static_cast<DimensionUnit>(unit));
    ButtonModel::GetInstance()->SetFontSize(value);
}

void FfiOHOSAceFrameworkButtonSetFontWeight(const char* fontWeight)
{
    ButtonModel::GetInstance()->SetFontWeight(ConvertStrToFontWeight(fontWeight));
}

void FfiOHOSAceFrameworkButtonSetFontColor(uint32_t color)
{
    ButtonModel::GetInstance()->SetFontColor(Color(color));
}

void FfiOHOSAceFrameworkButtonSetRadius(double radius, int32_t unit)
{
    Dimension value(radius, static_cast<DimensionUnit>(unit));
    ButtonModel::GetInstance()->SetBorderRadius(value);
}

void FfiOHOSAceFrameworkButtonSetAllBorderRadius(CJBorderRadius value)
{
    CalcDimension topLeft(value.topLeft, static_cast<DimensionUnit>(value.topLeftUnit));
    CalcDimension topRight(value.topRight, static_cast<DimensionUnit>(value.topRightUnit));
    CalcDimension bottomLeft(value.bottomLeft, static_cast<DimensionUnit>(value.bottomLeftUnit));
    CalcDimension bottomRight(value.bottomRight, static_cast<DimensionUnit>(value.bottomRightUnit));
    ButtonModel::GetInstance()->SetBorderRadius(topLeft, topRight, bottomLeft, bottomRight);
}

void FfiOHOSAceFrameworkButtonSetBorder(CJBorder params)
{
    Dimension widthDime(params.width, static_cast<DimensionUnit>(params.widthUnit));
    Dimension radiusDime(params.radius, static_cast<DimensionUnit>(params.radiusUnit));
    if (!Utils::CheckParamsValid(params.style, BORDER_STYLES.size())) {
        LOGE("invalid value for border style");
        return;
    }

    ViewAbstractModel::GetInstance()->SetBorderWidth(widthDime);
    ViewAbstractModel::GetInstance()->SetBorderColor(Color(params.color));
    ButtonModel::GetInstance()->SetBorderRadius(radiusDime);
    ViewAbstractModel::GetInstance()->SetBorderStyle(BORDER_STYLES[params.style]);
}

void FfiOHOSAceFrameworkButtonSetBackgroundColor(uint32_t color)
{
    ViewAbstractModel::GetInstance()->SetBackgroundColor(Color(color));
}

void FfiOHOSAceFrameworkButtonSetHeight(double height, int32_t unit)
{
    Dimension value(height, static_cast<DimensionUnit>(unit));
    ViewAbstractModel::GetInstance()->SetHeight(value);
}

void FfiOHOSAceFrameworkButtonSetWidth(double width, int32_t unit)
{
    Dimension value(width, static_cast<DimensionUnit>(unit));
    ViewAbstractModel::GetInstance()->SetWidth(value);
}

void FfiOHOSAceFrameworkButtonSetFontStyle(int32_t fontStyle)
{
    if (!Utils::CheckParamsValid(fontStyle, FONT_STYLES.size())) {
        LOGE("invalid value for font style");
        return;
    }
    ButtonModel::GetInstance()->SetFontStyle(FONT_STYLES[fontStyle]);
}

void FfiOHOSAceFrameworkButtonSetFontFamily(const char* fontFamily)
{
    std::vector<std::string> fontFamilies;
    fontFamilies = ConvertStrToFontFamilies(fontFamily);
    ButtonModel::GetInstance()->SetFontFamily(fontFamilies);
}

void FfiOHOSAceFrameworkButtonSetType(int32_t shape)
{
    if (!Utils::CheckParamsValid(shape, BUTTON_TYPES.size())) {
        LOGE("invalid value for button type");
        return;
    }
    ButtonModel::GetInstance()->SetType(static_cast<int>(BUTTON_TYPES[shape]));
}

void FfiOHOSAceFrameworkButtonSetPadding(double top, int32_t topUnit, double right, int32_t rightUnit, double bottom,
    int32_t bottomUnit, double left, int32_t leftUnit)
{
    Dimension topValue(top, static_cast<DimensionUnit>(topUnit));
    Dimension rightValue(right, static_cast<DimensionUnit>(rightUnit));
    Dimension bottomValue(bottom, static_cast<DimensionUnit>(bottomUnit));
    Dimension leftValue(left, static_cast<DimensionUnit>(leftUnit));
    ViewAbstractModel::GetInstance()->SetPaddings(topValue, rightValue, bottomValue, leftValue);
}

void FfiOHOSAceFrameworkButtonSetSize(double width, int32_t widthUnit, double height, int32_t heightUnit)
{
    FfiOHOSAceFrameworkViewAbstractSetSize(width, widthUnit, height, heightUnit);
}

void FfiOHOSAceFrameworkButtonSetStateEffect(bool stateEffect)
{
    ButtonModel::GetInstance()->SetStateEffect(stateEffect);
}
}
