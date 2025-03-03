/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WEB_SELECT_OVERLAY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WEB_SELECT_OVERLAY_H

#include <optional>

#include "base/geometry/ng/rect_t.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/pattern/text/base_text_select_overlay.h"
#include "core/components_ng/pattern/text/text_base.h"

namespace OHOS::Ace {
class WebDelegateObserver;
}

namespace OHOS::NWeb {
    class NWebQuickMenuParams;
    class NWebQuickMenuCallback;
    class NWebTouchHandleState;
    class NWebCursorInfo;
    class NWebSelectPopupMenuParam;
    class NWebDateTimeChooser;
    class NWebDateTimeSuggestion;
    class NWebSelectPopupMenuCallback;
    class NWebDateTimeChooserCallback;
    class NWebAccessibilityNodeInfo;
    class NWebMessage;
    class NWebKeyEvent;
    class NWebSelectMenuBound;
    enum class CursorType;
}

namespace OHOS::Ace::NG {

class ViewDataCommon;
class TransitionalNodeInfo;

namespace {
struct TouchHandleState {
    int32_t id = -1;
    int32_t x = -1;
    int32_t y = -1;
    int32_t edge_height = 0;
};

enum WebOverlayType { INSERT_OVERLAY, SELECTION_OVERLAY, INVALID_OVERLAY };
}

class WebSelectOverlay : public BaseTextSelectOverlay {
    DECLARE_ACE_TYPE(WebSelectOverlay, BaseTextSelectOverlay);

public:
    explicit WebSelectOverlay(const WeakPtr<TextBase> &textBase) : BaseTextSelectOverlay(textBase)
    {}
    ~WebSelectOverlay() = default;

    // Web Handle Menu Entry
    bool RunQuickMenu(std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback);
    // Chromium kernel notifies handle position change
    void OnTouchSelectionChanged(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle);
    void RegisterSelectOverlayEvent(SelectOverlayInfo& selectInfo);
    void UpdateSelectHandleInfo();
    bool IsSelectHandleReverse();
    // Check whether the handle status is valid.
    WebOverlayType GetTouchHandleOverlayType(
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle);
    bool IsTouchHandleValid(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> handle);
    void RegisterSelectOverLayOnClose(SelectOverlayInfo& selectInfo);
    void CheckHandles(SelectHandleInfo& handleInfo, const std::shared_ptr<OHOS::NWeb::NWebTouchHandleState>& handle);
    RectF ComputeTouchHandleRect(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> touchHandle);
    void SetMenuOptions(SelectOverlayInfo& selectInfo,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback);
    void StartListenSelectOverlayParentScroll(const RefPtr<FrameNode>& host);
    void RegisterSelectOverlayParentScrollCallback(int32_t parentId, int32_t callbackId);
    void HideHandleAndQuickMenuIfNecessary(bool hide, bool isScroll = false);
    void OnParentScrollStartOrEndCallback(bool isEnd);
    void StopListenSelectOverlayParentScroll(const RefPtr<FrameNode>& host);
    RectF ComputeMouseClippedSelectionBounds(int32_t x, int32_t y, int32_t w, int32_t h);
    void UpdateRunQuickMenuSelectInfo(SelectOverlayInfo& selectInfo,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertTouchHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> beginTouchHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endTouchHandle);
    RectF ComputeClippedSelectionBounds(
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endHandle,
        bool& isNewAvoid);
    void QuickMenuIsNeedNewAvoid(
        SelectOverlayInfo& selectInfo,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endHandle);
    void HideMagnifier();
    void UpdateTouchHandleForOverlay(bool fromOverlay = false);
    void OnMagnifierHandleMove(const RectF& handleRect, bool isFirst);
    RectF ChangeHandleHeight(const GestureEvent& event, bool isFirst);
    void OnSelectHandleStart(const GestureEvent& event, bool isFirst);
    void SelectCancel();
    void UpdateClippedSelectionBounds(int32_t x, int32_t y, int32_t w, int32_t h);
    void ChangeVisibilityOfQuickMenu();
    bool IsShowHandle() { return isShowHandle_; }

    // override BaseTextSelectOverlay
    bool PreProcessOverlay(const OverlayRequest& request) override;
    bool CheckHandleVisible(const RectF& paintRect) override;
    void OnResetTextSelection() override {}
    void AfterCloseOverlay() override;
    bool AllowTranslate() override { return true; }
    // override BaseTextSelectOverlay end

    // override SelectOverlayHolder
    void OnUpdateSelectOverlayInfo(SelectOverlayInfo& selectInfo, int32_t requestCode) override;
    std::optional<SelectHandleInfo> GetFirstHandleInfo() override;
    std::optional<SelectHandleInfo> GetSecondHandleInfo() override;
    void OnUpdateMenuInfo(SelectMenuInfo& menuInfo, SelectOverlayDirtyFlag dirtyFlag) override;
    RectF GetSelectArea() override;
    std::string GetSelectedText() override;
    // override SelectOverlayHolder end

    // override SelectOverlayCallback
    void OnMenuItemAction(OptionMenuActionId id, OptionMenuType type) override;
    void OnHandleMove(const RectF& rect, bool isFirst) override;
    void OnHandleMoveStart(const GestureEvent& event, bool isFirst) override;
    void OnHandleMoveDone(const RectF& rect, bool isFirst) override;
    void OnCloseOverlay(OptionMenuType menuType, CloseReason reason, RefPtr<OverlayInfo> info = nullptr) override;
    void OnHandleGlobalTouchEvent(SourceType sourceType, TouchType touchType, bool touchInside = true) override;
    void OnOverlayClick(const GestureEvent& event, bool isClickCaret) override;
    bool CheckTouchInHostNode(const PointF& touchPoint) override;
    void OnHandleReverse(bool isReverse) override;
    void OnHandleMarkInfoChange(const std::shared_ptr<SelectOverlayInfo> info, SelectOverlayDirtyFlag flag) override;
    void OnAfterSelectOverlayShow(bool isCreated) override;
    // override SelectOverlayCallback end
private:
    bool isShowHandle_ = false;
    bool needResetHandleReverse_ = false;
    SelectOverlayInfo webSelectInfo_;
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle_ = nullptr;
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle_ = nullptr;
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle_ = nullptr;
    std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> quickMenuCallback_ = nullptr;
    bool selectOverlayDragging_ = false;
    bool isCurrentStartHandleDragging_ = false;
    bool selectTemporarilyHidden_ = false;
    bool selectTemporarilyHiddenByScroll_ = false;
    bool isQuickMenuMouseTrigger_ = false;
    ScrollableParentInfo scrollableParentInfo_;
    std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> dropParams_ = nullptr;
    SelectMenuInfo selectMenuInfo_;
    ACE_DISALLOW_COPY_AND_MOVE(WebSelectOverlay);
    OnCreateMenuCallback onCreateMenuCallback_;
    OnMenuItemClickCallback onMenuItemClick_;
};
} // namespace OHOS::Ace::NG

#endif  //FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WEB_SELECT_OVERLAY_H