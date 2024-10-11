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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_PATTERN_H

#include <optional>

#include "core/components/common/layout/constants.h"
#include "core/components/picker/picker_data.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/picker/datepicker_accessibility_property.h"
#include "core/components_ng/pattern/picker/datepicker_column_pattern.h"
#include "core/components_ng/pattern/picker/datepicker_event_hub.h"
#include "core/components_ng/pattern/picker/datepicker_layout_property.h"
#include "core/components_ng/pattern/picker/datepicker_row_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/picker/datepicker_dialog_view.h"

namespace OHOS::Ace::NG {
class InspectorFilter;
namespace {
const Dimension FOCUS_PAINT_WIDTH = 2.0_vp;
}

class DatePickerPattern : public LinearLayoutPattern {
    DECLARE_ACE_TYPE(DatePickerPattern, LinearLayoutPattern);

public:
    DatePickerPattern() : LinearLayoutPattern(false) {};

    ~DatePickerPattern() override = default;

    bool IsAtomicNode() const override
    {
        return true;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<DatePickerEventHub>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<LinearLayoutAlgorithm>();
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<DataPickerRowLayoutProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto paintMethod = MakeRefPtr<DatePickerPaintMethod>(WeakClaim(this));
        paintMethod->SetEnabled(enabled_);
        paintMethod->SetBackgroundColor(backgroundColor_);
        return paintMethod;
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<DatePickerAccessibilityProperty>();
    }

    void SetConfirmNode(WeakPtr<FrameNode> buttonConfirmNode)
    {
        weakButtonConfirm_ = buttonConfirmNode;
    }

    void SetCancelNode(WeakPtr<FrameNode> buttonCancelNode)
    {
        weakButtonCancel_ = buttonCancelNode;
    }

    void SetLunarSwitchTextNode(WeakPtr<FrameNode> lunarSwitchTextNode)
    {
        weakLunarSwitchText_ = lunarSwitchTextNode;
    }

    void OnFontConfigurationUpdate() override
    {
        if (closeDialogEvent_) {
            closeDialogEvent_();
        }
    }

    void OnLanguageConfigurationUpdate() override;

    void OnColorConfigurationUpdate() override;

    void SetChangeCallback(ColumnChangeCallback&& value);

    void HandleColumnChange(const RefPtr<FrameNode>& tag, bool isAdd, uint32_t index, bool needNotify);

    void SolarColumnsBuilding(const PickerDate& current);

    void LunarColumnsBuilding(const LunarDate& current);

    void SolarMonthDaysColumnsBuilding(const PickerDate& current);

    void LunarMonthDaysColumnBuilding(const LunarDate& current);

    void HandleYearChange(bool isAdd, uint32_t index, std::vector<RefPtr<FrameNode>>& resultTags);

    void HandleMonthChange(bool isAdd, uint32_t index, std::vector<RefPtr<FrameNode>>& resultTags);

    void HandleLunarMonthChange(bool isAdd, uint32_t index);

    void HandleLunarYearChange(bool isAdd, uint32_t index);

    void HandleSolarYearChange(bool isAdd, uint32_t index);

    LunarDate GetCurrentLunarDate(uint32_t lunarYear) const;

    void OrderCurrentLunarDate(
        RefPtr<FrameNode>& stackYear, RefPtr<FrameNode>& stackMonth, RefPtr<FrameNode>& stackDay) const;

    void HandleSolarMonthChange(bool isAdd, uint32_t index);

    void HandleDayChange(bool isAdd, uint32_t index, std::vector<RefPtr<FrameNode>>& resultTags);

    void HandleReduceLunarDayChange(uint32_t index);

    void HandleLunarDayChange(bool isAdd, uint32_t index);

    void HandleAddLunarDayChange(uint32_t index);

    void HandleSolarDayChange(bool isAdd, uint32_t index);

    void HandleSolarMonthDaysChange(bool isAdd, uint32_t index);

    void HandleLunarMonthDaysChange(bool isAdd, uint32_t index);

    void HandleAddLunarMonthDaysChange(uint32_t index);

    void HandleReduceLunarMonthDaysChange(uint32_t index);

    LunarDate GetCurrentLunarDateByMonthDaysColumn(uint32_t lunarYear) const;

    PickerDate GetCurrentDate() const;

    void SetEventCallback(EventCallback&& value);

    void FireChangeEvent(bool refresh);

    void FlushColumn();

    void FlushMonthDaysColumn();

    void AdjustLunarDate(LunarDate& date);

    int LunarDateCompare(const LunarDate& left, const LunarDate& right) const;

    std::unordered_map<std::string, RefPtr<FrameNode>> GetAllChildNode();

    void OrderAllChildNode(RefPtr<FrameNode>& stackYear, RefPtr<FrameNode>& stackMonth, RefPtr<FrameNode>& stackDay);

    RefPtr<FrameNode> GetColumn(const int32_t& tag) const
    {
        auto iter = std::find_if(datePickerColumns_.begin(), datePickerColumns_.end(), [&tag](const auto& c) {
                auto column = c.Upgrade();
                return column && column->GetId() == tag;
            });
        return (iter == datePickerColumns_.end()) ? nullptr : (*iter).Upgrade();
    }

    void SetColumn(const RefPtr<FrameNode>& value)
    {
        datePickerColumns_.emplace_back(value);
    }

    void ClearColumn()
    {
        datePickerColumns_.clear();
    }

    void SetShowLunar(bool value)
    {
        isForceUpdate_ = value != lunar_;
        lunar_ = value;
    }

    bool IsShowLunar() const
    {
        return lunar_;
    }

    void SetShowMonthDaysFlag(bool value)
    {
        showMonthDays_ = value;
    }

    bool ShowMonthDays() const
    {
        return showMonthDays_;
    }

    void SetShowTimeFlag(bool value)
    {
        showTime_ = value;
    }

    void SetShowLunarSwitch(bool value)
    {
        showLunarSwitch_ = value;
    }

    bool GetShowLunarSwitch()
    {
        return showLunarSwitch_;
    }

    const EventMarker& GetDialogAcceptEvent() const
    {
        return OnDialogAccept_;
    }
    void SetDialogAcceptEvent(const EventMarker& value)
    {
        OnDialogAccept_ = value;
    }

    const EventMarker& GetDialogCancelEvent() const
    {
        return OnDialogCancel_;
    }
    void SetDialogCancelEvent(const EventMarker& value)
    {
        OnDialogCancel_ = value;
    }

    const EventMarker& GetDialogChangeEvent() const
    {
        return OnDialogChange_;
    }
    void SetDialogChangeEvent(const EventMarker& value)
    {
        OnDialogChange_ = value;
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

    uint32_t GetOptionCount(RefPtr<FrameNode>& frameNode)
    {
        return options_[frameNode].size();
    }

    PickerDateF GetOptionValue(RefPtr<FrameNode>& frameNode, uint32_t index)
    {
        if (index >= GetOptionCount(frameNode)) {
            LOGE("index out of range.");
            return {};
        }
        return options_[frameNode][index];
    }

    const std::vector<PickerDateF>& GetAllOptions(RefPtr<FrameNode>& frameNode)
    {
        return options_[frameNode];
    }

    const std::map<WeakPtr<FrameNode>, std::vector<PickerDateF>>& GetOptions() const
    {
        return options_;
    }

    uint32_t GetShowCount() const
    {
        return showCount_;
    }

    void SetShowCount(uint32_t showCount)
    {
        showCount_ = showCount;
    }

    void SetDateOrder(std::string dateOrder)
    {
        dateOrder_ = dateOrder;
    }

    static std::string GetYearFormatString(uint32_t year)
    {
        return PickerStringFormatter::GetYear(year);
    }

    static std::string GetMonthFormatString(uint32_t month, bool isLunar, bool isLeap)
    {
        if (isLunar) {
            return PickerStringFormatter::GetLunarMonth(month, isLeap);
        }
        return PickerStringFormatter::GetSolarMonth(month);
    }

    static std::string GetDayFormatString(uint32_t day, bool isLunar)
    {
        if (isLunar) {
            return PickerStringFormatter::GetLunarDay(day);
        }
        return PickerStringFormatter::GetSolarDay(day);
    }

    uint32_t GetLunarMaxDay(uint32_t year, uint32_t month, bool isLeap) const;

    bool GetLunarLeapMonth(uint32_t year, uint32_t& outLeapMonth) const;

    LunarDate SolarToLunar(const PickerDate& date) const;

    PickerDate LunarToSolar(const LunarDate& date) const;

    void UpdateCurrentOffset(float offset);

    void OnDataLinking(
        const RefPtr<FrameNode>& tag, bool isAdd, uint32_t index, std::vector<RefPtr<FrameNode>>& resultTags);

    void HandleMonthDaysChange(
        const RefPtr<FrameNode>& tag, bool isAdd, uint32_t index, std::vector<RefPtr<FrameNode>>& resultTags);

    std::string GetSelectedObject(bool isColumnChange, int status = -1) const;

    const LunarDate& GetSelectDate()
    {
        return selectedLunar_;
    }

    void SetSelectDate(const PickerDate& value)
    {
        selectedDate_ = value;
        isFiredDateChange_ = firedDateStr_.has_value() && firedDateStr_.value() == GetSelectedObject(false);
        firedDateStr_.reset();
        if (selectedDate_.GetYear() <= 0) {
            LOGW("selectedDate error");
            selectedDate_ = PickerDate::Current();
        }
        AdjustSolarDate(selectedDate_, startDateSolar_, endDateSolar_);
        selectedLunar_ = SolarToLunar(selectedDate_);
    }

    const PickerDate& GetSelectedDate()
    {
        return selectedDate_;
    }

    void SetStartDate(const PickerDate& value)
    {
        startDateSolar_ = value;
        AdjustSolarDate(startDateSolar_, limitStartDate_, limitEndDate_);
        startDateLunar_ = SolarToLunar(startDateSolar_);
    }

    const LunarDate& GetStartDateLunar()
    {
        return startDateLunar_;
    }

    const PickerDate& GetStartDateSolar()
    {
        return startDateSolar_;
    }

    void SetEndDate(const PickerDate& value)
    {
        endDateSolar_ = value;
        AdjustSolarDate(endDateSolar_, limitStartDate_, limitEndDate_);
        endDateLunar_ = SolarToLunar(endDateSolar_);
    }

    const LunarDate& GetEndDateLunar()
    {
        return endDateLunar_;
    }

    const PickerDate& GetEndDateSolar()
    {
        return endDateSolar_;
    }

    void AdjustSolarDate(PickerDate& date, const PickerDate& start, const PickerDate& end) const
    {
        if (SolarDateCompare(date, start) < 0) {
            date = start;
            return;
        }
        if (SolarDateCompare(date, end) > 0) {
            date = end;
        }
    }

    void AdjustSolarDate(PickerDate& date) const
    {
        AdjustSolarDate(date, startDateSolar_, endDateSolar_);
    }

    static int SolarDateCompare(const PickerDate& left, const PickerDate& right)
    {
        static const int leftEqualRight = 0; // means left = right
        static const int leftGreatRight = 1; // means left > right
        static const int leftLessRight = -1; // means left < right
        if (left.GetYear() > right.GetYear()) {
            return leftGreatRight;
        }
        if (left.GetYear() < right.GetYear()) {
            return leftLessRight;
        }
        if (left.GetMonth() > right.GetMonth()) {
            return leftGreatRight;
        }
        if (left.GetMonth() < right.GetMonth()) {
            return leftLessRight;
        }
        if (left.GetDay() > right.GetDay()) {
            return leftGreatRight;
        }
        if (left.GetDay() < right.GetDay()) {
            return leftLessRight;
        }
        return leftEqualRight;
    }

    bool HasYearNode() const
    {
        return yearId_.has_value();
    }

    int32_t GetYearId()
    {
        if (!yearId_.has_value()) {
            yearId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return yearId_.value();
    }

    bool HasMonthNode() const
    {
        return monthId_.has_value();
    }

    int32_t GetMonthId()
    {
        if (!monthId_.has_value()) {
            monthId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return monthId_.value();
    }

    bool HasDayNode() const
    {
        return dayId_.has_value();
    }

    int32_t GetDayId()
    {
        if (!dayId_.has_value()) {
            dayId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return dayId_.value();
    }

    bool HasMonthDaysNode() const
    {
        return monthDaysId_.has_value();
    }

    int32_t GetMonthDaysId()
    {
        if (!monthDaysId_.has_value()) {
            monthDaysId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return monthDaysId_.value();
    }

    bool HasTitleNode() const
    {
        return titleId_.has_value();
    }

    bool SetTitleId(const int32_t id)
    {
        if (HasTitleNode()) {
            return false;
        }
        titleId_ = id;
        return true;
    }

    int32_t GetTitleId()
    {
        if (!titleId_.has_value()) {
            titleId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return titleId_.value();
    }

    bool HasButtonTitleNode() const
    {
        return ButtonTitleId_.has_value();
    }

    int32_t GetButtonTitleId()
    {
        if (!ButtonTitleId_.has_value()) {
            ButtonTitleId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return ButtonTitleId_.value();
    }

    bool HasDividerNode() const
    {
        return DividerId_.has_value();
    }

    int32_t GetDividerId()
    {
        if (!DividerId_.has_value()) {
            DividerId_ = ElementRegister::GetInstance()->MakeUniqueId();
        }
        return DividerId_.value();
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    static const std::string& GetYear(uint32_t year);

    static const std::string& GetSolarMonth(uint32_t month);

    static const std::string& GetSolarDay(uint32_t day);

    static const std::string& GetLunarMonth(uint32_t month, bool isLeap);

    static const std::string& GetLunarDay(uint32_t day);

    FocusPattern GetFocusPattern() const override
    {
        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_RETURN(pipeline, FocusPattern());
        auto pickerTheme = pipeline->GetTheme<PickerTheme>();
        CHECK_NULL_RETURN(pickerTheme, FocusPattern());
        auto focusColor = pickerTheme->GetFocusColor();
        FocusPaintParam focusPaintParams;
        focusPaintParams.SetPaintColor(focusColor);
        focusPaintParams.SetPaintWidth(FOCUS_PAINT_WIDTH);
        return { FocusType::NODE, true, FocusStyleType::CUSTOM_REGION, focusPaintParams };
    }

    void ShowTitle(int32_t titleId);
    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;
    void SetContentRowNode(RefPtr<FrameNode>& contentRowNode)
    {
        contentRowNode_ = contentRowNode;
    }
    void SetbuttonTitleNode(RefPtr<FrameNode>& buttonTitleNode)
    {
        buttonTitleNode_ = buttonTitleNode;
    }

    void SetPickerTag(bool isPicker)
    {
        isPicker_ = isPicker;
    }

    void SetFocusDisable();
    void SetFocusEnable();
    static const std::string GetFormatString(PickerDateF data);

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

    Dimension GetGradientHeight()
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
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;
    static void Init();
    void InitDisabled();
    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void PaintFocusState();
    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    bool OnKeyEvent(const KeyEvent& event);
    bool HandleDirectionKey(KeyCode code);
    PickerDate GetCurrentDateByMonthDaysColumn() const;
    PickerDate GetCurrentDateByYearMonthDayColumn() const;
    void OrderCurrentDateByYearMonthDayColumn(
        RefPtr<FrameNode>& stackYear, RefPtr<FrameNode>& stackMonth, RefPtr<FrameNode>& stackDay) const;
    void FillSolarYearOptions(const PickerDate& current, RefPtr<FrameNode>& yearColumn);
    void UpdateTitleTextColor(const RefPtr<FrameNode>& buttonTitleNode, const RefPtr<PickerTheme>& pickerTheme);
    void FillLunarMonthDaysOptions(const LunarDate& current, RefPtr<FrameNode>& monthDaysColumn);
    void AdjustSolarStartEndDate();
    void AdjustLunarStartEndDate();
    void UpdateConfirmButtonMargin(
        const RefPtr<FrameNode>& buttonConfirmNode, const RefPtr<DialogTheme>& dialogTheme);
    void UpdateCancelButtonMargin(
        const RefPtr<FrameNode>& buttonCancelNode, const RefPtr<DialogTheme>& dialogTheme);
    RefPtr<ClickEvent> clickEventListener_;
    bool enabled_ = true;
    int32_t focusKeyID_ = 0;
    std::map<WeakPtr<FrameNode>, std::vector<PickerDateF>> options_;
    uint32_t showCount_ = 0;
    std::string dateOrder_ = "";
    std::vector<WeakPtr<FrameNode>> datePickerColumns_;
    bool lunar_ = false;
    bool showMonthDays_ = false;
    bool showTime_ = false;
    bool showLunarSwitch_ = false;
    Color backgroundColor_ = Color::WHITE;
    std::optional<int32_t> yearId_;
    std::optional<int32_t> monthId_;
    std::optional<int32_t> dayId_;
    std::optional<int32_t> monthDaysId_;
    std::optional<int32_t> dateNodeId_;
    std::optional<int32_t> titleId_;
    std::optional<int32_t> ButtonTitleId_;
    std::optional<int32_t> DividerId_;
    double resizePickerItemHeight_ = 0.0;
    bool resizeFlag_ = false;
    bool isShowInDialog_ = false;
    EventMarker OnDialogAccept_;
    EventMarker OnDialogCancel_;
    EventMarker OnDialogChange_;
    WeakPtr<FrameNode> weakButtonConfirm_;
    WeakPtr<FrameNode> weakButtonCancel_;
    WeakPtr<FrameNode> weakLunarSwitchText_;
    PickerDate startDateSolar_ = PickerDate(1970, 1, 1); // default start date is 1970-1-1 from FA document.
    LunarDate startDateLunar_;
    PickerDate endDateSolar_ = PickerDate(2100, 12, 31); // default end date is 2100-12-31 from FA document.
    LunarDate endDateLunar_;
    PickerDate selectedDate_ = PickerDate::Current();
    LunarDate selectedLunar_;
    PickerDate startDefaultDateSolar_ = PickerDate(1970, 1, 1); // default start date is 1970-1-1 from FA document.
    PickerDate endDefaultDateSolar_ = PickerDate(2100, 12, 31); // default end date is 2100-12-31 from FA document.
    const PickerDate limitStartDate_ = PickerDate(1900, 1, 31);
    const PickerDate limitEndDate_ = PickerDate(2100, 12, 31);
    static bool inited_;
    static const std::string empty_;
    static const PickerDateF emptyPickerDate_;
    static std::unordered_map<uint32_t, std::string> years_;       // year from 1900 to 2100,count is 201
    static std::unordered_map<uint32_t, std::string> solarMonths_; // solar month from 1 to 12,count is 12
    static std::unordered_map<uint32_t, std::string> solarDays_;   // solar day from 1 to 31, count is 31
    static std::unordered_map<uint32_t, std::string> lunarMonths_; // lunar month from 1 to 24, count is 24
    static std::unordered_map<uint32_t, std::string> lunarDays_;   // lunar day from 1 to 30, count is 30
    static std::vector<std::string> tagOrder_;    // year month day tag order
    static std::vector<std::string> localizedMonths_;
    WeakPtr<FrameNode> contentRowNode_;
    WeakPtr<FrameNode> buttonTitleNode_;
    bool isPicker_ = false;
    bool isFiredDateChange_ = false;
    bool isForceUpdate_ = false;
    std::optional<std::string> firedDateStr_;
    void CalcLeftTotalColumnWidth(const RefPtr<FrameNode>& host, float &leftTotalColumnWidth, float childSize);
    bool CheckFocusID(int32_t childSize);
    bool ParseDirectionKey(RefPtr<DatePickerColumnPattern>& pattern, KeyCode& code, uint32_t totalOptionCount,
                          int32_t childSize);

    bool hasUserDefinedDisappearFontFamily_ = false;
    bool hasUserDefinedNormalFontFamily_ = false;
    bool hasUserDefinedSelectedFontFamily_ = false;
    std::function<void()> closeDialogEvent_;
    bool isUserSetDividerSpacingFont_ = false;
    bool isUserSetGradientFont_ = false;
    Dimension gradientHeight_;
    Dimension dividerSpacing_;
    float paintDividerSpacing_ = 1.0f;
    PickerTextProperties textProperties_;
    double curOpacity_ = 1.0;

    ACE_DISALLOW_COPY_AND_MOVE(DatePickerPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_PATTERN_H
