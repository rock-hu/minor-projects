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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_TYPE_DEFINE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_TYPE_DEFINE_H

#include <string>

#include "core/components/common/properties/text_style.h"
#include "core/components/picker/picker_data.h"
#include "frameworks/base/i18n/time_format.h"

namespace OHOS::Ace::NG {
// update flag of text properties
const uint32_t FLAG_DISAPPEAR_COLOR = 0x0001;
const uint32_t FLAG_COLOR = 0x0002;
const uint32_t FLAG_SELECTED_COLOR = 0x0004;
const uint32_t FLAG_DISAPPEAR_FONTSIZE = 0x0008;
const uint32_t FLAG_FONTSIZE = 0x0010;
const uint32_t FLAG_SELECTED_FONTSIZE = 0x0020;
const uint32_t FLAG_DISAPPEAR_WEIGHT = 0x0040;
const uint32_t FLAG_WEIGHT = 0x0080;
const uint32_t FLAG_SELECTED_WEIGHT = 0x0100;
struct PickerTextStyle {
    std::optional<Color> textColor;
    std::optional<Dimension> fontSize;
    std::optional<FontWeight> fontWeight;
    std::optional<std::vector<std::string>> fontFamily;
    std::optional<Ace::FontStyle> fontStyle;
    std::optional<Dimension> minFontSize;
    std::optional<Dimension> maxFontSize;
    std::optional<Ace::TextOverflow> textOverflow;
};
struct PickerTextProperties {
    PickerTextStyle disappearTextStyle_;
    PickerTextStyle normalTextStyle_;
    PickerTextStyle selectedTextStyle_;
    PickerTextStyle defaultTextStyle_;
};

// textpicker column kind
const uint32_t ICON = 0x01;
const uint32_t TEXT = 0x02;
const uint32_t MIXTURE = 0x03;

struct RangeContent {
    std::string icon_;
    std::string text_;
};

struct TextCascadePickerOptions {
    std::vector<std::string> rangeResult;
    std::vector<TextCascadePickerOptions> children;
};

struct TextCascadePickerOptionsAttr {
    bool isCascade = false;
    bool isHasSelectAttr = false;
};

struct CheckboxSettingData {
    std::optional<Color> selectedColor;
    std::optional<Color> unselectedColor;
    std::optional<Color> strokeColor;
};

struct DatePickerSettingData {
    bool isLunar;
    bool lunarswitch;
    CheckboxSettingData checkboxSettingData;
    bool showTime;
    bool useMilitary;
    DatePickerMode mode;
    std::map<std::string, PickerDate> datePickerProperty;
    std::map<std::string, PickerTime> timePickerProperty;
    PickerTextProperties properties;
    DateTimeType dateTimeOptions;
};

struct TextPickerSettingData {
    std::vector<RangeContent> rangeVector;
    uint32_t selected;
    uint32_t columnKind;
    Dimension height;
    bool canLoop = true;
    PickerTextProperties properties;
    std::vector<uint32_t> selectedValues;
    std::vector<std::string> values;
    std::vector<NG::TextCascadePickerOptions> options;
    NG::TextCascadePickerOptionsAttr attr;
    bool isDisableTextStyleAnimation = false;
};

struct TimePickerSettingData {
    bool isUseMilitaryTime;
    PickerDate dialogTitleDate;
    PickerTextProperties properties;
    DateTimeType dateTimeOptions;
    bool showSecond;
    bool isEnableCascade;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_TYPE_DEFINE_H
