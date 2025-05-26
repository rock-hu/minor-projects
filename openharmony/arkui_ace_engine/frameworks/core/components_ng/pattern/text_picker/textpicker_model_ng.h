/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_MODEL_NG_H

#include "base/geometry/dimension.h"
#include "core/components/common/layout/constants.h"
#include "core/components/picker/picker_base_component.h"
#include "core/components/text/text_theme.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_model.h"
#include "core/components_ng/pattern/text_picker/textpicker_properties.h"
#include "core/common/resource/resource_object.h"
namespace OHOS::Ace::NG {
class ACE_FORCE_EXPORT TextPickerModelNG : public TextPickerModel {
public:
    void Create(RefPtr<PickerTheme> pickerTheme, uint32_t columnKind) override;
    void SetSelected(uint32_t value) override;
    void SetRange(const std::vector<NG::RangeContent>& value) override;
    void SetValue(const std::string& value) override;
    void SetDefaultPickerItemHeight(const Dimension& value) override;
    void SetGradientHeight(const Dimension& value) override;
    void SetCanLoop(const bool value) override;
    void SetDigitalCrownSensitivity(int32_t crownSensitivity) override;
    void SetDefaultAttributes(const RefPtr<PickerTheme>& pickerTheme) override;
    void SetDisappearTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) override;
    void SetNormalTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) override;
    void SetSelectedTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value) override;
    void HasUserDefinedDisappearFontFamily(bool isUserDefined) override;
    void HasUserDefinedNormalFontFamily(bool isUserDefined) override;
    void HasUserDefinedSelectedFontFamily(bool isUserDefined) override;
    void MultiInit(const RefPtr<PickerTheme> pickerTheme) override;
    void SetColumns(const std::vector<NG::TextCascadePickerOptions>& options) override;
    void SetIsCascade(bool isCascade) override;
    void SetOnCascadeChange(TextCascadeChangeEvent&& onChange) override;
    void SetOnScrollStop(TextCascadeChangeEvent&& onScrollStop) override;
    void SetOnEnterSelectedArea(TextCascadeChangeEvent&& onEnterSelectedArea) override;
    void SetValues(const std::vector<std::string>& values) override;
    void SetSelecteds(const std::vector<uint32_t>& values) override;
    void SetBackgroundColor(const Color& color) override;
    bool IsSingle() override;
    bool GetSingleRange(std::vector<NG::RangeContent>& rangeValue) override;
    bool IsCascade() override;

    void SetMaxCount(uint32_t maxCount) override
    {
        maxCount_ = maxCount;
    }

    uint32_t GetMaxCount() override
    {
        return maxCount_;
    }

    void SetSingleRange(bool isSingleRange) override;
    bool GetSingleRange() override
    {
        std::lock_guard<std::shared_mutex> lock(isSingleMutex_);
        return isSingleRange_;
    }

    static void SetTextPickerSingeRange(bool isSingleRange)
    {
        std::lock_guard<std::shared_mutex> lock(isSingleMutex_);
        isSingleRange_ = isSingleRange;
    }

    static bool GetTextPickerSingeRange()
    {
        std::lock_guard<std::shared_mutex> lock(isSingleMutex_);
        return isSingleRange_;
    }

    void SetHasSelectAttr(bool value) override;
    bool GetMultiOptions(std::vector<NG::TextCascadePickerOptions>& options) override;
    void SetOnValueChangeEvent(TextCascadeValueChangeEvent&& onChange) override;
    void SetOnSelectedChangeEvent(TextCascadeSelectedChangeEvent&& onChange) override;
    void SetDivider(const ItemDivider& divider) override;
    void HasUserDefinedOpacity() override;
    void SetColumnWidths(const std::vector<Dimension>& widths) override;

    void SetDisableTextStyleAnimation(const bool value) override;
    void SetDefaultTextStyle(const RefPtr<TextTheme>& textTheme, const NG::PickerTextStyle& value) override;
    void SetEnableHapticFeedback(bool isEnableHapticFeedback) override;
    void SetSelectedBackgroundStyle(const NG::PickerBackgroundStyle& value) override;
    void UpdateUserSetSelectColor() override;
    void ParseGradientHeight(const RefPtr<ResourceObject>& resObj) override;
    void ParseDividerResObj() override;
    void ParseDisappearTextStyleResObj(const PickerTextStyle& textStyleOpt) override;
    void ParseSelectedTextStyleResObj(const PickerTextStyle& textStyleOpt) override;
    void ParseNormalTextStyleResObj(const PickerTextStyle& textStyleOpt) override;
    void ParseDefaultTextStyleResObj(const PickerTextStyle& textStyleOpt) override;

    static void SetCanLoop(FrameNode* frameNode, const bool value);
    static void SetDigitalCrownSensitivity(FrameNode* frameNode, int32_t crownSensitivity);
    static void SetSelected(FrameNode* frameNode, uint32_t value);
    static void SetSelecteds(FrameNode* frameNode, const std::vector<uint32_t>& values);
    static void SetHasSelectAttr(FrameNode* frameNode, bool value);
    static void SetIsCascade(FrameNode* frameNode, bool isCascade);
    static void SetColumnKind(FrameNode* frameNode, uint32_t columnKind);
    static void SetNormalTextStyle(
        FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value);
    static void SetSelectedTextStyle(
        FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value);
    static void SetDisappearTextStyle(
        FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value);
    static void SetDefaultTextStyle(
        FrameNode* frameNode, const RefPtr<TextTheme>& textTheme, const NG::PickerTextStyle& value);
    static void SetDefaultPickerItemHeight(FrameNode* frameNode, const Dimension& value);
    static void SetBackgroundColor(FrameNode* frameNode, const Color& color);
    static bool IsSingle(FrameNode* frameNode);
    static bool GetSingleRange(FrameNode* frameNode, std::vector<NG::RangeContent>& rangeValue);
    static bool IsCascade(FrameNode* frameNode);
    static bool GetMultiOptions(FrameNode* frameNode, std::vector<NG::TextCascadePickerOptions>& options);
    static uint32_t GetMaxCount(FrameNode* frameNode);
    static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId);
    static void SetRange(FrameNode* frameNode, const std::vector<NG::RangeContent>& value);
    static void SetValue(FrameNode* frameNode, const std::string& value);
    static void SetValues(FrameNode* frameNode, const std::vector<std::string>& values);
    static void SetColumns(FrameNode* frameNode, const std::vector<NG::TextCascadePickerOptions>& options);
    static void SetDefaultAttributes(RefPtr<FrameNode>& frameNode, const RefPtr<PickerTheme>& pickerTheme);
    static void SetDivider(FrameNode* frameNode, const ItemDivider& divider);
    static PickerTextStyle getSelectedTextStyle(FrameNode* frameNode);
    static PickerTextStyle getNormalTextStyle(FrameNode* frameNode);
    static PickerTextStyle getDisappearTextStyle(FrameNode* frameNode);
    static int32_t getTextPickerSelectedIndex(FrameNode* frameNode);
    static std::string getTextPickerValue(FrameNode* frameNode);
    static std::string getTextPickerRange(FrameNode* frameNode);
    static void SetGradientHeight(FrameNode* frameNode, const Dimension& value);
    static void SetDisableTextStyleAnimation(FrameNode* frameNode, const bool value);
    static void SetOnCascadeChange(FrameNode* frameNode, TextCascadeChangeEvent&& onChange);
    static void SetOnScrollStop(FrameNode* frameNode, TextCascadeChangeEvent&& onScrollStop);
    static int32_t GetSelectedSize(FrameNode* frameNode);
    static int32_t GetColumnWidthsSize(FrameNode* frameNode);
    static std::string getTextPickerValues(FrameNode* frameNode);
    static std::vector<uint32_t> getTextPickerSelecteds(FrameNode* frameNode);
    static int32_t GetCanLoop(FrameNode* frameNode);
    static Dimension GetDefaultPickerItemHeight(FrameNode* frameNode);
    static void SetTextPickerRangeType(FrameNode* frameNode, int32_t rangeType);
    static int32_t GetTextPickerRangeType(FrameNode* frameNode);
    static const Dimension ConvertFontScaleValue(const Dimension& fontSizeValue);
    static void SetColumnWidths(FrameNode* frameNode, const std::vector<Dimension>& widths);
    static std::vector<Dimension> GetColumnWidths(FrameNode* frameNode);
    static void SetEnableHapticFeedback(FrameNode* frameNode, bool isEnableHapticFeedback);
    static bool GetEnableHapticFeedback(FrameNode* frameNode);
    static void SetSelectedBackgroundStyle(FrameNode* frameNode, const NG::PickerBackgroundStyle& value);
    static PickerBackgroundStyle GetSelectedBackgroundStyle(FrameNode* frameNode);

private:
    void SetUnCascadeColumns(const std::vector<NG::TextCascadePickerOptions>& options);
    void SetCascadeColumns(const std::vector<NG::TextCascadePickerOptions>& options);

    static RefPtr<FrameNode> CreateStackNode();
    static RefPtr<FrameNode> CreateColumnNode();
    static RefPtr<FrameNode> CreateButtonNode();
    static RefPtr<FrameNode> CreateColumnNode(uint32_t columnKind, uint32_t showCount);
    static void SetUnCascadeColumnsNode(FrameNode* frameNode, const std::vector<NG::TextCascadePickerOptions>& options);
    static void SetCascadeColumnsNode(FrameNode* frameNode, const std::vector<NG::TextCascadePickerOptions>& options);
    void ParseResTextStyle(const PickerTextStyle& textStyleOpt, const std::string& textStyleType,
        std::function<void(const PickerTextStyle&)> updateTextStyleFunc);

    uint32_t maxCount_ = 0;
    std::vector<uint32_t> kinds_;
    static inline bool isSingleRange_ = true;
    static inline uint32_t showCount_ = 0;
    static inline std::shared_mutex showCountMutex_;
    static inline std::shared_mutex isSingleMutex_;
    static inline uint32_t columnKind_ = TEXT;
};

class ACE_EXPORT TextPickerDialogModelNG : public TextPickerDialogModel {
public:
    RefPtr<AceType> CreateObject() override;
    void SetTextPickerDialogShow(RefPtr<AceType>& PickerText, NG::TextPickerSettingData& settingData,
        std::function<void()>&& onCancel, std::function<void(const std::string&)>&& onAccept,
        std::function<void(const std::string&)>&& onChange, std::function<void(const std::string&)>&& onScrollStop,
        std::function<void(const std::string&)>&& onEnterSelectedArea, TextPickerDialog& textPickerDialog,
        TextPickerDialogEvent& textPickerDialogEvent, const std::vector<ButtonInfo>& buttonInfos) override;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_MODEL_NG_H
