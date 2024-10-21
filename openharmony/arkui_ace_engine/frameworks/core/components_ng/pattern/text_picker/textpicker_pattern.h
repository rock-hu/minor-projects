/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H

#include <optional>

#include "core/components/picker/picker_theme.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components_ng/pattern/text_picker/textpicker_row_accessibility_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_layout_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_paint_method.h"
#include "core/components_ng/pattern/text_picker/toss_animation_controller.h"

namespace OHOS::Ace::NG {
class InspectorFilter;
using EventCallback = std::function<void(bool)>;
using ColumnChangeCallback = std::function<void(const RefPtr<FrameNode>&, bool, uint32_t, bool)>;

namespace {
const Dimension TEXT_FOCUS_PAINT_WIDTH = 2.0_vp;
}
class TextPickerPattern : public LinearLayoutPattern {
    DECLARE_ACE_TYPE(TextPickerPattern, LinearLayoutPattern);

public:
    TextPickerPattern() : LinearLayoutPattern(false) {};

    ~TextPickerPattern() override = default;

    bool IsAtomicNode() const override
    {
        return true;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<TextPickerEventHub>();
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<TextPickerLayoutProperty>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<TextPickerRowAccessibilityProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto textPickerPaintMethod = MakeRefPtr<TextPickerPaintMethod>(WeakClaim(this));
        textPickerPaintMethod->SetDefaultPickerItemHeight(CalculateHeight());
        textPickerPaintMethod->SetEnabled(enabled_);
        return textPickerPaintMethod;
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<LinearLayoutAlgorithm>();
    }

    void SetEventCallback(EventCallback&& value);

    void FireChangeEvent(bool refresh);

    void OnColumnsBuilding();

    void FlushOptions();

    void SetDefaultPickerItemHeight();

    std::map<uint32_t, RefPtr<FrameNode>> GetColumnNodes();

    RefPtr<FrameNode> GetColumnNode();

    uint32_t GetShowOptionCount() const;

    void SetSelected(uint32_t value)
    {
        selectedIndex_ = value;
    }

    uint32_t GetSelected() const
    {
        return selectedIndex_;
    }

    void SetRange(const std::vector<NG::RangeContent>& value)
    {
        if (value.empty()) {
            return;
        }
        range_.clear();
        for (size_t i = 0; i < value.size(); i++) {
            RangeContent content;
            content.icon_ = value[i].icon_;
            content.text_ = value[i].text_;
            range_.emplace_back(content);
        }
    }

    std::vector<NG::RangeContent> GetRange() const
    {
        return range_;
    }

    std::vector<NG::TextCascadePickerOptions> GetMultiOptions() const
    {
        return cascadeOriginptions_;
    }

    void SetColumnsKind(uint32_t columnKind)
    {
        columnsKind_ = columnKind;
    }

    void ClearOption()
    {
        options_.clear();
    }

    void AppendOption(const NG::RangeContent& value)
    {
        options_.emplace_back(value);
    }

    uint32_t GetOptionCount() const
    {
        return options_.size();
    }

    std::string GetSelectedObject(bool isColumnChange, int32_t status = 0) const;

    std::string GetOption(uint32_t index) const
    {
        if (index >= GetOptionCount()) {
            return "";
        }
        return options_[index].text_;
    }

    void SetBackgroundColor(const Color& color)
    {
        if (backgroundColor_ == color) {
            return;
        }
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        backgroundColor_ = color;
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }

    Color GetBackgroundColor()
    {
        return backgroundColor_;
    }

    FocusPattern GetFocusPattern() const override
    {
        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_RETURN(pipeline, FocusPattern());
        auto pickerTheme = pipeline->GetTheme<PickerTheme>();
        CHECK_NULL_RETURN(pickerTheme, FocusPattern());
        auto focusColor = pickerTheme->GetFocusColor();

        FocusPaintParam focusPaintParams;
        focusPaintParams.SetPaintColor(focusColor);
        focusPaintParams.SetPaintWidth(TEXT_FOCUS_PAINT_WIDTH);

        return { FocusType::NODE, true, FocusStyleType::CUSTOM_REGION, focusPaintParams };
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    void SetCascadeOptions(const std::vector<NG::TextCascadePickerOptions>& options,
        const std::vector<NG::TextCascadePickerOptions>& cascadeOptions)
    {
        cascadeOptions_.clear();
        cascadeOriginptions_.clear();
        for (auto& option : cascadeOptions) {
            cascadeOptions_.emplace_back(std::move(option));
        }
        for (auto& option : options) {
            cascadeOriginptions_.emplace_back(std::move(option));
        }
    }

    uint32_t GetCascadeOptionCount() const
    {
        return cascadeOptions_.size();
    }

    uint32_t GetOptionCount(const RefPtr<FrameNode>& frmeNode)
    {
        uint32_t count = 0;
        auto it = optionsWithNode_.find(frmeNode);
        if (it != optionsWithNode_.end()) {
            count = it->second.size();
        }
        return count;
    }

    void SetIsCascade(bool isCascade)
    {
        isCascade_ = isCascade;
    }

    bool GetIsCascade() const
    {
        return isCascade_;
    }

    void SetConfirmNode(WeakPtr<FrameNode> buttonConfirmNode)
    {
        weakButtonConfirm_ = buttonConfirmNode;
    }

    void SetCancelNode(WeakPtr<FrameNode> buttonCancelNode)
    {
        weakButtonCancel_ = buttonCancelNode;
    }

    void SetForwardNode(WeakPtr<FrameNode> buttonForwardNode)
    {
        weakButtonForward_ = buttonForwardNode;
    }

    void SetBackwardNode(WeakPtr<FrameNode> buttonBackwardNode)
    {
        weakButtonBackward_ = buttonBackwardNode;
    }

    void OnLanguageConfigurationUpdate() override;
    void OnFontConfigurationUpdate() override;
    void OnFontScaleConfigurationUpdate() override;

    void SetValues(const std::vector<std::string>& values)
    {
        values_.clear();
        for (auto& value : values) {
            values_.emplace_back(value);
        }
    }

    const std::vector<std::string>& GetValues()
    {
        return values_;
    }

    const std::vector<uint32_t>& GetSelecteds()
    {
        return selecteds_;
    }

    void SetHasSelectAttr(bool value)
    {
        isHasSelectAttr_ = value;
    }

    void SetResizePickerItemHeight(double resizePickerItemHeight)
    {
        resizePickerItemHeight_ = resizePickerItemHeight;
    }

    double GetResizePickerItemHeight() const
    {
        return resizePickerItemHeight_;
    }

    void SetResizeFlag(bool resizeFlag)
    {
        resizeFlag_ = resizeFlag;
    }

    bool GetResizeFlag() const
    {
        return resizeFlag_;
    }

    void SetIsShowInDialog(bool isShowInDialog)
    {
        isShowInDialog_ = isShowInDialog;
    }

    bool GetIsShowInDialog() const
    {
        return isShowInDialog_;
    }

    double GetDefaultPickerItemHeight() const
    {
        return defaultPickerItemHeight_;
    }

    void SetSelecteds(const std::vector<uint32_t>& values);

    void HandleColumnChange(const RefPtr<FrameNode>& tag, bool isAdd, uint32_t index, bool needNotify);

    void SetChangeCallback(ColumnChangeCallback&& value);

    void ProcessCascadeOptions(const std::vector<NG::TextCascadePickerOptions>& options,
        std::vector<NG::TextCascadePickerOptions>& reOptions, uint32_t index);

    size_t ProcessCascadeOptionDepth(const NG::TextCascadePickerOptions& option);

    void OnColorConfigurationUpdate() override;

    void OnDirectionConfigurationUpdate() override;

    void SetContentRowNode(RefPtr<FrameNode>& contentRowNode)
    {
        contentRowNode_ = contentRowNode;
    }

    void SetPickerTag(bool isPicker)
    {
        isPicker_ = isPicker;
    }

    void CheckAndUpdateColumnSize(SizeF& size, bool isNeedAdaptForAging = false);
    bool NeedAdaptForAging();

    void SetDivider(const ItemDivider& divider)
    {
        divider_ = divider;
    }

    ItemDivider GetDivider()
    {
        return divider_;
    }

    void SetCustomDividerFlag(bool customDividerFlag)
    {
        customDividerFlag_ = customDividerFlag;
    }

    bool GetCustomDividerFlag()
    {
        return customDividerFlag_;
    }
    void SetGradientHeight(const Dimension& value)
    {
        value_ = value;
    }

    Dimension GetGradientHeight()
    {
        return value_;
    }

    void SetCanLoop(bool isLoop);

    bool GetCanLoop()
    {
        return canloop_;
    }

    void HasUserDefinedDisappearFontFamily(bool isUserDefined)
    {
        hasUserDefinedDisappearFontFamily_ = isUserDefined;
    }

    bool GetHasUserDefinedDisappearFontFamily()
    {
        return hasUserDefinedDisappearFontFamily_;
    }

    void HasUserDefinedNormalFontFamily(bool isUserDefined)
    {
        hasUserDefinedNormalFontFamily_ = isUserDefined;
    }

    bool GetHasUserDefinedNormalFontFamily()
    {
        return hasUserDefinedNormalFontFamily_;
    }

    void HasUserDefinedSelectedFontFamily(bool isUserDefined)
    {
        hasUserDefinedSelectedFontFamily_ = isUserDefined;
    }

    bool GetHasUserDefinedSelectedFontFamily()
    {
        return hasUserDefinedSelectedFontFamily_;
    }

    void SetRangeType(int32_t rangeType)
    {
        rangeType_ = rangeType;
    }

    int32_t GetRangeType()
    {
        return rangeType_;
    }

    void updateFontConfigurationEvent(const std::function<void()>& closeDialogEvent)
    {
        closeDialogEvent_ = closeDialogEvent;
    }

    const PickerTextProperties& GetTextProperties() const
    {
        return textProperties_;
    }

    void SetTextProperties(const PickerTextProperties& properties)
    {
        textProperties_ = properties;
        if (properties.disappearTextStyle_.fontSize.has_value() && properties.disappearTextStyle_.fontSize->IsValid()) {
            isUserSetGradientFont_ = true;
            gradientHeight_ = properties.disappearTextStyle_.fontSize.value();
        }

        if (properties.normalTextStyle_.fontSize.has_value() && properties.normalTextStyle_.fontSize->IsValid()) {
            isUserSetGradientFont_ = true;
            gradientHeight_ = std::max(properties.normalTextStyle_.fontSize.value(), gradientHeight_);
        }

        if (properties.selectedTextStyle_.fontSize.has_value() && properties.selectedTextStyle_.fontSize->IsValid()) {
            isUserSetDividerSpacingFont_ = true;
            dividerSpacing_ = properties.selectedTextStyle_.fontSize.value();
        }
    }

    bool GetIsUserSetDividerSpacingFont()
    {
        return isUserSetDividerSpacingFont_;
    }

    bool GetIsUserSetGradientFont()
    {
        return isUserSetGradientFont_;
    }

    Dimension GetDividerSpacing()
    {
        return dividerSpacing_;
    }

    Dimension GetTextGradientHeight()
    {
        return gradientHeight_;
    }

    void SetPaintDividerSpacing(float& value)
    {
        paintDividerSpacing_ = value;
    }

    float GetPaintDividerSpacing()
    {
        return paintDividerSpacing_;
    }

    void SetUserDefinedOpacity(double opacity)
    {
        curOpacity_ = opacity;
    }

private:
    void OnModifyDone() override;
    void SetLayoutDirection(TextDirection textDirection);
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    bool OnKeyEvent(const KeyEvent& event);
    bool HandleDirectionKey(KeyCode code);
    double CalculateHeight();

    void InitDisabled();
    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void PaintFocusState();
    void SetButtonIdeaSize();
    std::string GetRangeStr() const;
    std::string GetOptionsMultiStr() const;
    std::string GetOptionsMultiStrInternal() const;
    std::string GetOptionsCascadeStr(
        const std::vector<NG::TextCascadePickerOptions>& options) const;
    bool ChangeCurrentOptionValue(NG::TextCascadePickerOptions& option,
        uint32_t value, uint32_t curColumn, uint32_t replaceColumn);
    void OnColumnsBuildingUnCascade();
    void OnColumnsBuildingCascade();
    std::string GetSelectedObjectMulti(const std::vector<std::string>& values,
        const std::vector<uint32_t>& indexs, int32_t status) const;
    void SupplementOption(const std::vector<NG::TextCascadePickerOptions>& reOptions,
        std::vector<NG::RangeContent>& rangeContents, uint32_t patterIndex);
    void ProcessCascadeOptionsValues(const std::vector<std::string>& rangeResultValue, uint32_t index);
    void SetFocusCornerRadius(RoundRect& paintRect);
    void UpdateConfirmButtonMargin(
        const RefPtr<FrameNode>& buttonConfirmNode, const RefPtr<DialogTheme>& dialogTheme);
    void UpdateCancelButtonMargin(
        const RefPtr<FrameNode>& buttonCancelNode, const RefPtr<DialogTheme>& dialogTheme);
    void CheckFocusID(int32_t childSize);
    bool ParseDirectionKey(RefPtr<TextPickerColumnPattern>& textPickerColumnPattern, KeyCode& code, int32_t childSize);
    RectF CalculatePaintRect(int32_t currentFocusIndex,
        float centerX, float centerY, float piantRectWidth, float piantRectHeight, float columnWidth);

    bool enabled_ = true;
    int32_t focusKeyID_ = 0;
    double defaultPickerItemHeight_ = 0.0;
    double resizePickerItemHeight_ = 0.0;
    uint32_t selectedIndex_ = 0;
    std::vector<NG::RangeContent> range_;
    std::vector<NG::RangeContent> options_;
    uint32_t columnsKind_ = 0;
    std::vector<NG::TextCascadePickerOptions> cascadeOptions_;
    std::map<WeakPtr<FrameNode>, std::vector<NG::RangeContent>> optionsWithNode_;
    std::vector<NG::TextCascadePickerOptions> cascadeOriginptions_;
    bool isCascade_ = false;
    bool isHasSelectAttr_ = false;
    WeakPtr<FrameNode> weakButtonConfirm_;
    WeakPtr<FrameNode> weakButtonCancel_;
    WeakPtr<FrameNode> weakButtonForward_;
    WeakPtr<FrameNode> weakButtonBackward_;
    std::vector<std::string> values_;
    std::vector<uint32_t> selecteds_;
    Color backgroundColor_ = Color::WHITE;
    bool resizeFlag_ = false;
    bool isShowInDialog_ = false;
    bool canloop_ = true;

    // inner focus switch
    bool operationOn_ = false;

    bool hasUserDefinedDisappearFontFamily_ = false;
    bool hasUserDefinedNormalFontFamily_ = false;
    bool hasUserDefinedSelectedFontFamily_ = false;

    double curOpacity_ = 1.0;

    ACE_DISALLOW_COPY_AND_MOVE(TextPickerPattern);

    WeakPtr<NG::FrameNode> contentRowNode_;
    bool isPicker_ = true;
    bool isFiredSelectsChange_ = false;
    std::optional<std::string> firedSelectsStr_;
    
    ItemDivider divider_;
    bool customDividerFlag_ = false;
    Dimension value_;
    int32_t rangeType_ = 0;
    std::function<void()> closeDialogEvent_;
    bool isUserSetDividerSpacingFont_ = false;
    bool isUserSetGradientFont_ = false;
    Dimension gradientHeight_;
    Dimension dividerSpacing_;
    float paintDividerSpacing_ = 1.0f;
    bool isNeedUpdateSelectedIndex_ = true;
    PickerTextProperties textProperties_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_PICKER_TEXT_PICKER_PATTERN_H
