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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_select_ffi.h"


#include "cj_lambda.h"
#include "bridge/common/utils/utils.h"
#include "core/components_ng/pattern/select/select_model_ng.h"

using namespace OHOS::Ace;
using namespace OHOS::Ace::Framework;

extern "C" {
VectorNativeSelectOptionHandle FFICJCreateVectorNativeSelectOption(int64_t size)
{
    LOGI("Create NativeSelectOption Vector");
    return new std::vector<NativeSelectOption>(size);
}

void FFICJVectorNativeSelectOptionSetElement(
    VectorNativeSelectOptionHandle vec, int64_t index, NativeSelectOption selectOption)
{
    LOGI("NativeSelectOption Vector Set Element");
    auto actualVec = reinterpret_cast<std::vector<NativeSelectOption>*>(vec);
    (*actualVec)[index] = selectOption;
    LOGI("NativeSelectOption Vector Set Element Success");
}

void FFICJVectorNativeSelectOptionDelete(VectorNativeSelectOptionHandle vec)
{
    auto actualVec = reinterpret_cast<std::vector<NativeSelectOption>*>(vec);
    delete actualVec;
}

void FfiOHOSAceFrameworkSelectCreate(VectorStringPtr vecContent)
{
    auto nativeSelectOptionVec = *reinterpret_cast<std::vector<NativeSelectOption>*>(vecContent);
    std::vector<SelectParam> params(nativeSelectOptionVec.size());
    for (size_t i = 0; i < nativeSelectOptionVec.size(); i++) {
        params[i] = { nativeSelectOptionVec[i].value, nativeSelectOptionVec[i].icon };
    }
    SelectModel::GetInstance()->Create(params);
}

void FfiOHOSAceFrameworkSelectSetSelected(int32_t value)
{
    SelectModel::GetInstance()->SetSelected(value);
}

void FfiOHOSAceFrameworkSelectSetValue(const char* value)
{
    SelectModel::GetInstance()->SetValue(value);
}

void FfiOHOSAceFrameworkSelectSetFont(
    int32_t style, const char* weight, double size, int32_t sizeUnit, const char* family)
{
    SelectModel::GetInstance()->SetFontSize(Dimension(size, static_cast<DimensionUnit>(sizeUnit)));
    SelectModel::GetInstance()->SetFontWeight(ConvertStrToFontWeight(weight));
    SelectModel::GetInstance()->SetFontFamily(ConvertStrToFontFamilies(family));
    SelectModel::GetInstance()->SetItalicFontStyle(static_cast<FontStyle>(style));
}

void FfiOHOSAceFrameworkSelectSetFontColor(uint32_t color)
{
    SelectModel::GetInstance()->SetFontColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetSelectedOptionBgColor(uint32_t color)
{
    SelectModel::GetInstance()->SetSelectedOptionBgColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetSelectedOptionFont(
    int32_t style, const char* weight, double size, int32_t sizeUnit, const char* family)
{
    SelectModel::GetInstance()->SetSelectedOptionFontSize(Dimension(size, static_cast<DimensionUnit>(sizeUnit)));
    SelectModel::GetInstance()->SetSelectedOptionFontWeight(ConvertStrToFontWeight(weight));
    SelectModel::GetInstance()->SetSelectedOptionFontFamily(ConvertStrToFontFamilies(family));
    SelectModel::GetInstance()->SetSelectedOptionItalicFontStyle(static_cast<FontStyle>(style));
}

void FfiOHOSAceFrameworkSelectSetSelectedOptionFontColor(uint32_t color)
{
    SelectModel::GetInstance()->SetSelectedOptionFontColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetOptionBgColor(uint32_t color)
{
    SelectModel::GetInstance()->SetOptionBgColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetOptionFont(
    int32_t style, const char* weight, double size, int32_t sizeUnit, const char* family)
{
    SelectModel::GetInstance()->SetOptionFontSize(Dimension(size, static_cast<DimensionUnit>(sizeUnit)));
    SelectModel::GetInstance()->SetOptionFontWeight(ConvertStrToFontWeight(weight));
    SelectModel::GetInstance()->SetOptionFontFamily(ConvertStrToFontFamilies(family));
    SelectModel::GetInstance()->SetOptionItalicFontStyle(static_cast<FontStyle>(style));
}

void FfiOHOSAceFrameworkSelectSetOptionFontColor(uint32_t color)
{
    SelectModel::GetInstance()->SetOptionFontColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetSpace(double width, int32_t widthUnit)
{
    Dimension dimStrokeWidth(width, static_cast<DimensionUnit>(widthUnit));
    SelectModel::GetInstance()->SetSpace(dimStrokeWidth);
}

void FfiOHOSAceFrameworkSelectSetArrowPosition(int32_t value)
{
    if (static_cast<ArrowPosition>(value) != ArrowPosition::START &&
        static_cast<ArrowPosition>(value) != ArrowPosition::END) {
        value = 0;
    }
    SelectModel::GetInstance()->SetArrowPosition(static_cast<ArrowPosition>(value));
}

void FfiOHOSAceFrameworkSelectSetMenuAlign(
    int32_t value, double dx, int32_t dxUnit, double dy, int32_t dyUnit)
{
    Dimension dimensionX(dx, static_cast<DimensionUnit>(dxUnit));
    Dimension dimensionY(dy, static_cast<DimensionUnit>(dyUnit));

    MenuAlign menuAlignObj;
    menuAlignObj.alignType = static_cast<MenuAlignType>(value);
    menuAlignObj.offset = DimensionOffset(dimensionX, dimensionY);
    SelectModel::GetInstance()->SetMenuAlign(menuAlignObj);
}

void FfiOHOSAceFrameworkSelectSetOptionWidth(double width, int32_t widthUnit)
{
    SelectModel::GetInstance()->SetHasOptionWidth(true);
    Dimension dimStrokeWidth(width, static_cast<DimensionUnit>(widthUnit));
    SelectModel::GetInstance()->SetOptionWidth(dimStrokeWidth);
}

void FfiOHOSAceFrameworkSelectSetOptionWidthWithMode(const char* value)
{
    SelectModel::GetInstance()->SetHasOptionWidth(true);
    std::string modeFlag(value);
    if (modeFlag.compare("fit_content") == 0) {
        SelectModel::GetInstance()->SetOptionWidthFitTrigger(false);
    } else if (modeFlag.compare("fit_trigger") == 0) {
        SelectModel::GetInstance()->SetOptionWidthFitTrigger(true);
    } else {
        LOGE("OptionWidth is null or undefined");
        SelectModel::GetInstance()->SetHasOptionWidth(false);
        CalcDimension dimension;
        SelectModel::GetInstance()->SetOptionWidth(dimension);
    }
}

void FfiOHOSAceFrameworkSelectSetOptionHeight(double height, int32_t heightUnit)
{
    Dimension dimStrokeWidth(height, static_cast<DimensionUnit>(heightUnit));
    SelectModel::GetInstance()->SetOptionHeight(dimStrokeWidth);
}

void FfiOHOSAceFrameworkSelectSetMenuBackgroundColor(uint32_t color)
{
    SelectModel::GetInstance()->SetMenuBackgroundColor(Color(color));
}

void FfiOHOSAceFrameworkSelectSetMenuBackgroundBlurStyle(int32_t value)
{
    BlurStyleOption styleOption;
    if (value >= static_cast<int>(BlurStyle::NO_MATERIAL) &&
        value <= static_cast<int>(BlurStyle::COMPONENT_ULTRA_THICK)) {
        styleOption.blurStyle = static_cast<BlurStyle>(value);
        SelectModel::GetInstance()->SetMenuBackgroundBlurStyle(styleOption);
    }
}

void FfiOHOSAceFrameworkSelectOnSelect(void (*callback)(int32_t index, const char* value))
{
    auto onSelect = [lambda = CJLambda::Create(callback)](int32_t index,
        const std::string& value) -> void { lambda(index, value.c_str()); };
    SelectModel::GetInstance()->SetOnSelect(std::move(onSelect));
}
}
