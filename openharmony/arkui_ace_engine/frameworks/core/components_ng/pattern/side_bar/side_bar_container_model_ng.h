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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SIDE_BAR_CONTAINER_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SIDE_BAR_CONTAINER_MODEL_NG_H

#include "core/components_ng/pattern/side_bar/side_bar_container_model.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_pattern.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT SideBarContainerModelNG : public SideBarContainerModel {
public:
    void Create() override;
    void SetSideBarContainerType(SideBarContainerType type) override;
    void SetShowSideBar(bool isShow) override;
    void SetShowControlButton(bool showControlButton) override;
    void ParseAndSetWidth(WidthType widthType, Dimension& width, bool isDoubleBind = false) override;
    void SetSideBarWidth(const Dimension& sideBarWidth, bool isDoubleBind = false) override;
    void SetMinSideBarWidth(const Dimension& minSideBarWidth) override;
    void SetMaxSideBarWidth(const Dimension& maxSideBarWidth) override;
    void SetAutoHide(bool autoHide) override;
    void SetSideBarPosition(SideBarPosition sideBarPosition) override;
    void SetControlButtonWidth(const Dimension& width) override;
    void SetControlButtonHeight(const Dimension& height) override;
    void SetControlButtonLeft(const Dimension& left) override;
    void ResetControlButtonLeft() override;
    void SetControlButtonTop(const Dimension& top) override;
    void SetControlButtonShowIconInfo(
        const std::string& showIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap) override;
    void SetControlButtonHiddenIconInfo(
        const std::string& hiddenIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap) override;
    void SetControlButtonSwitchingIconInfo(
        const std::string& switchingIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap) override;
    void ResetControlButtonIconInfo() override;
    void SetOnChange(std::function<void(const bool)>&& onChange) override;
    void SetDividerStrokeWidth(const Dimension& strokeWidth) override;
    void SetDividerColor(const Color& color) override;
    void SetDividerStartMargin(const Dimension& startMargin) override;
    void SetDividerEndMargin(const Dimension& endMargin) override;
    void SetOnChangeEvent(std::function<void(const bool)>&& onChangeEvent) override;
    void SetOnSideBarWidthChangeEvent(OnSideBarWidthChangeEvent&& event) override;
    void SetMinContentWidth(const Dimension& minContentWidth) override;
    void ResetControlButton() override;
    static void SetSideBarWidth(FrameNode* frameNode, const Dimension& sideBarWidth);
    static void SetMinSideBarWidth(FrameNode* frameNode, const Dimension& minSideBarWidth);
    static void SetControlButtonWidth(FrameNode* frameNode, const Dimension& width);
    static void SetControlButtonHeight(FrameNode* frameNode, const Dimension& height);
    static void SetControlButtonLeft(FrameNode* frameNode, const Dimension& left);
    static void SetControlButtonTop(FrameNode* frameNode, const Dimension& top);
    static void SetControlButtonShowIconInfo(
        FrameNode* frameNode, const std::string& showIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap);
    static void SetControlButtonHiddenIconInfo(
        FrameNode* frameNode, const std::string& hiddenIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap);
    static void SetControlButtonSwitchingIconInfo(
        FrameNode* frameNode, const std::string& switchingIconStr, bool isPixelMap, RefPtr<PixelMap> pixMap);
    static void SetShowControlButton(FrameNode* frameNode, bool showControlButton);
    static void SetAutoHide(FrameNode* frameNode, bool autoHide);
    static void SetMaxSideBarWidth(FrameNode* frameNode, const Dimension& maxSideBarWidth);
    static void SetMinContentWidth(FrameNode* frameNode, const Dimension& minContentWidth);
    static void SetSideBarPosition(FrameNode* frameNode, SideBarPosition sideBarPosition);
    static void SetShowSideBar(FrameNode* frameNode, bool isShow);

    static void SetDividerStrokeWidth(FrameNode* frameNode, const Dimension& strokeWidth);
    static void SetDividerColor(FrameNode* frameNode, const Color& color);
    static void SetDividerStartMargin(FrameNode* frameNode, const Dimension& startMargin);
    static void SetDividerEndMargin(FrameNode* frameNode, const Dimension& endMargin);
    static void ResetControlButtonLeft(FrameNode* frameNode);
    static void ResetControlButtonIconInfo(FrameNode* frameNode);
    static bool IsDoubleBindBlock(const RefPtr<SideBarContainerPattern>& sideBarContainerPattern);
private:
    void MarkNeedInitRealSideBarWidth() override;
    static void MarkNeedInitRealSideBarWidth(FrameNode* frameNode);
    static bool sideBarWidthDoubleBind_;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SIDE_BAR_CONTAINER_MODEL_H
