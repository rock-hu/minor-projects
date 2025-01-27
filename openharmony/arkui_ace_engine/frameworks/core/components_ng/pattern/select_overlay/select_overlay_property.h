/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SELECT_OVERLAY_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SELECT_OVERLAY_PROPERTY_H

#include <cstdint>
#include <functional>
#include <vector>

#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/pattern/text/text_menu_extension.h"
#include "core/event/ace_events.h"
#include "core/event/touch_event.h"
#include "frameworks/core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

constexpr int32_t MENU_SHOW_ANIMATION_DURATION = 250;
constexpr int32_t MENU_HIDE_ANIMATION_DURATION = 200;
constexpr int32_t HANDLE_ANIMATION_DURATION = 150;

struct OnMenuItemCallback {
    OnCreateMenuCallback onCreateMenuCallback;
    OnMenuItemClickCallback onMenuItemClick;
    std::function<void(int32_t&, int32_t&)> textRangeCallback;
};

struct SelectHandlePaintInfo {
    OffsetF startPoint;
    OffsetF endPoint;
    float width = 0.0f;

    SelectHandlePaintInfo operator-(const OffsetF& offset) const
    {
        return {
            .startPoint = startPoint - offset,
            .endPoint = endPoint - offset,
            .width = width
        };
    }

    SelectHandlePaintInfo operator+(const OffsetF& offset) const
    {
        return {
            .startPoint = startPoint + offset,
            .endPoint = endPoint + offset,
            .width = width
        };
    }

    bool operator==(const SelectHandlePaintInfo& info) const
    {
        return (startPoint == info.startPoint) && (endPoint == info.endPoint) && (width == info.width);
    }

    bool operator!=(const SelectHandlePaintInfo& info) const
    {
        return !(*this == info);
    }
};

struct SelectHandleInfo {
    bool isShow = true;
    bool needLayout = false;
    bool isPaintHandleWithPoints = false;
    bool isCircleShow = true;
    bool forceDraw = false;
    // in Global coordinates.
    RectF paintRect;
    RectF localPaintRect;
    SelectHandlePaintInfo paintInfo;
    std::function<RectF(const SelectHandlePaintInfo&)> paintInfoConverter;

    bool operator==(const SelectHandleInfo& info) const
    {
        return (isShow == info.isShow) && (paintRect == info.paintRect) && (paintInfo == info.paintInfo);
    }

    bool operator!=(const SelectHandleInfo& info) const
    {
        return !(*this == info);
    }

    const RectF GetPaintRect() const
    {
        if (isPaintHandleWithPoints) {
            auto offsetX = std::max(paintInfo.startPoint.GetX(), paintInfo.endPoint.GetX());
            auto offsetY = std::min(paintInfo.startPoint.GetY(), paintInfo.endPoint.GetY());
            auto height = paintInfo.endPoint.GetY() - paintInfo.startPoint.GetY();
            return RectF(OffsetF(offsetX, offsetY), SizeF(paintInfo.width, std::abs(height)));
        }
        return paintRect;
    }

    static Dimension GetDefaultLineWidth();

    std::string ToString() const
    {
        auto jsonValue = JsonUtil::Create(true);
        JSON_STRING_PUT_BOOL(jsonValue, isShow);
        JSON_STRING_PUT_BOOL(jsonValue, needLayout);
        JSON_STRING_PUT_STRINGABLE(jsonValue, paintRect);
        JSON_STRING_PUT_STRINGABLE(jsonValue, localPaintRect);
        return jsonValue->ToString();
    }
};

using SelectOverlayDirtyFlag = uint32_t;
inline constexpr SelectOverlayDirtyFlag DIRTY_FIRST_HANDLE = 1;
inline constexpr SelectOverlayDirtyFlag DIRTY_SECOND_HANDLE = 1 << 1;
inline constexpr SelectOverlayDirtyFlag DIRTY_SELECT_AREA = 1 << 2;
inline constexpr SelectOverlayDirtyFlag DIRTY_ALL_MENU_ITEM = 1 << 3;
inline constexpr SelectOverlayDirtyFlag DIRTY_COPY_ALL_ITEM = 1 << 4;
inline constexpr SelectOverlayDirtyFlag DIRTY_SELECT_TEXT = 1 << 5;
inline constexpr SelectOverlayDirtyFlag DIRTY_VIEWPORT = 1 << 6;
inline constexpr SelectOverlayDirtyFlag DIRTY_HANDLE_COLOR_FLAG = 1 << 7;
inline constexpr SelectOverlayDirtyFlag DIRTY_DOUBLE_HANDLE = DIRTY_FIRST_HANDLE | DIRTY_SECOND_HANDLE;
inline constexpr SelectOverlayDirtyFlag DIRTY_ALL =
    DIRTY_DOUBLE_HANDLE | DIRTY_ALL_MENU_ITEM | DIRTY_SELECT_AREA | DIRTY_SELECT_TEXT | DIRTY_VIEWPORT;

inline constexpr int32_t REQUEST_RECREATE = 1;

enum class OptionMenuType { NO_MENU, MOUSE_MENU, TOUCH_MENU };
enum class OptionMenuActionId {
    COPY,
    CUT,
    PASTE,
    SELECT_ALL,
    TRANSLATE,
    SHARE,
    SEARCH,
    CAMERA_INPUT,
    AI_WRITE,
    APPEAR,
    DISAPPEAR
};
enum class CloseReason {
    CLOSE_REASON_NORMAL = 1,
    CLOSE_REASON_HOLD_BY_OTHER,
    CLOSE_REASON_BY_RECREATE,
    CLOSE_REASON_TOOL_BAR,
    CLOSE_REASON_BACK_PRESSED,
    CLOSE_REASON_CLICK_OUTSIDE,
    CLOSE_REASON_DRAG_FLOATING,
    CLOSE_REASON_WINDOW_SIZE_CHANGE
};

struct HoldSelectionInfo {
    std::function<bool(const PointF&)> checkTouchInArea;
    std::function<void()> resetSelectionCallback;
    std::function<bool(SourceType, TouchType)> eventFilter;

    bool IsAcceptEvent(SourceType sourceType, TouchType touchType)
    {
        if (eventFilter) {
            return eventFilter(sourceType, touchType);
        }
        return sourceType == SourceType::MOUSE && touchType == TouchType::DOWN;
    }
};

// end SelectOverlayManagerNG

struct SelectMenuInfo {
    bool menuDisable = false;
    bool menuIsShow = false;
    bool singleHandleMenuIsShow = false;
    bool showCopy = true;
    bool showPaste = true;
    bool showCopyAll = true;
    bool showCut = true;
    bool showTranslate = false;
    bool showSearch = false;
    bool showShare = false;
    bool showCameraInput = false;
    bool showAIWrite = false;
    std::optional<OffsetF> menuOffset;
    OptionMenuType menuType = OptionMenuType::TOUCH_MENU;

    // Customize menu information.
    std::optional<int32_t> responseType;
    std::optional<int32_t> editorType;
    std::function<void()> menuBuilder;

    bool IsIconChanged(const SelectMenuInfo& info) const
    {
        if (menuBuilder != nullptr || info.menuBuilder != nullptr) {
            return true;
        }
        return !((showCopy == info.showCopy) && (showPaste == info.showPaste) && (showCopyAll == info.showCopyAll) &&
                 (showTranslate == info.showTranslate) &&
                 (showCut == info.showCut) && (showSearch == info.showSearch) && (showShare == info.showShare) &&
                 (showCameraInput == info.showCameraInput) &&
                 (showAIWrite == info.showAIWrite));
    }

    std::string ToString() const
    {
        auto jsonValue = JsonUtil::Create(true);
        JSON_STRING_PUT_BOOL(jsonValue, menuDisable);
        JSON_STRING_PUT_BOOL(jsonValue, menuIsShow);
        JSON_STRING_PUT_BOOL(jsonValue, singleHandleMenuIsShow);
        JSON_STRING_PUT_BOOL(jsonValue, showCopy);
        JSON_STRING_PUT_BOOL(jsonValue, showPaste);
        JSON_STRING_PUT_BOOL(jsonValue, showCopyAll);
        JSON_STRING_PUT_BOOL(jsonValue, showCut);
        JSON_STRING_PUT_BOOL(jsonValue, showTranslate);
        JSON_STRING_PUT_BOOL(jsonValue, showSearch);
        JSON_STRING_PUT_BOOL(jsonValue, showShare);
        JSON_STRING_PUT_BOOL(jsonValue, showCameraInput);
        return jsonValue->ToString();
    }
};

struct SelectMenuCallback {
    std::function<void()> onCopy;
    std::function<void()> onPaste;
    std::function<void()> onSelectAll;
    std::function<void()> onCut;
    std::function<void()> onTranslate;
    std::function<void()> onSearch;
    std::function<void()> onShare;
    std::function<void()> onCameraInput;
    std::function<void()> onAIWrite;

    std::function<void()> onAppear;
    std::function<void()> onDisappear;
    std::function<void()> onMenuShow;
    std::function<void()> onMenuHide;
    std::function<bool()> showMenuOnMoveDone;
};

struct SelectedByMouseInfo {
    WeakPtr<FrameNode> selectedNode;
    std::function<void()> onResetSelection;

    bool operator!=(const SelectedByMouseInfo& info) const
    {
        CHECK_NULL_RETURN(selectedNode.Upgrade(), true);
        CHECK_NULL_RETURN(info.selectedNode.Upgrade(), true);
        return selectedNode.Upgrade() != info.selectedNode.Upgrade();
    }

    void clear()
    {
        selectedNode.Reset();
        onResetSelection = nullptr;
    }
};

struct CallerFrameNodeInfo {
    RectF paintFrameRect;
    OffsetF paintOffset;
};

enum class SelectOverlayMode {
    ALL, MENU_ONLY, HANDLE_ONLY
};

enum class HandleLevelMode {
    OVERLAY, EMBED
};

struct SelectOverlayInfo {
    WeakPtr<Pattern> pattern;
    bool isUsingMouse = false;
    bool isSingleHandle = false;
    // when handleReverse is true, The first one is on the right side of the second.
    bool handleReverse = false;
    // Used to determine the range of judgment that is parallel to the first and second handles.
    float singleLineHeight = 10.0f;
    bool isSelectRegionVisible = false;
    bool isNewAvoid = false;
    bool recreateOverlay = false;
    bool isUseOverlayNG = false;
    SelectHandleInfo firstHandle;
    SelectHandleInfo secondHandle;
    std::function<bool(const RectF&, const RectF&)> checkHandleReverse;
    std::optional<Color> handlerColor;
    HitTestMode hitTestMode = HitTestMode::HTMTRANSPARENT_SELF;

    // show area
    bool useFullScreen = true;
    RectF showArea;
    RectF selectArea;

    OffsetF rightClickOffset;

    // handle touch event
    std::function<void(const TouchEventInfo&)> onTouchDown;
    std::function<void(const TouchEventInfo&)> onTouchUp;
    std::function<void(const TouchEventInfo&)> onTouchMove;
    std::function<void(const GestureEvent&, bool isFirst)> onClick;
    std::function<void(const GestureEvent&, bool isFirst)> afterOnClick;
    std::function<void(const MouseInfo&)> onMouseEvent;

    // handle move callback.
    std::function<void(const GestureEvent&, bool isFirst)> onHandleMoveStart;
    std::function<void(const RectF&, bool isFirst)> onHandleMove;
    std::function<void(const RectF&, bool isFirst)> onHandleMoveDone;
    std::function<void(bool)> onHandleReverse;

    std::function<void(const GestureEvent&, bool isFirst)> onHandlePanMove;
    std::function<void(const GestureEvent&, bool isFirst)> onHandlePanEnd;
    std::function<OffsetF()> getDeltaHandleOffset;

    // menu info.
    SelectMenuInfo menuInfo;
    SelectMenuCallback menuCallback;

    std::vector<MenuOptionsParam> menuOptionItems;
    OnMenuItemCallback onCreateCallback;

    // force hide callback, which may be called when other textOverlay shows.
    std::function<void(bool)> onClose;

    std::function<bool(const PointF&)> checkIsTouchInHostArea;
    std::function<void()> onHandleIsHidden;

    OHOS::Ace::WeakPtr<FrameNode> callerFrameNode;
    std::optional<CallerFrameNodeInfo> callerNodeInfo;
    std::optional<RectF> ancestorViewPort;

    bool isHandleLineShow = true;
    std::string selectText;
    bool isSingleLine = false;

    HandleLevelMode handleLevelMode = HandleLevelMode::OVERLAY;
    bool enableHandleLevel = false;
    VectorF scale = VectorF(1.0f, 1.0f);
    bool clipHandleDrawRect = false;
    std::optional<RectF> clipViewPort;

    std::string ToString() const
    {
        auto jsonValue = JsonUtil::Create(true);
        JSON_STRING_PUT_INT(jsonValue, handleLevelMode);
        JSON_STRING_PUT_BOOL(jsonValue, isUsingMouse);
        JSON_STRING_PUT_BOOL(jsonValue, isSingleHandle);
        JSON_STRING_PUT_BOOL(jsonValue, handleReverse);
        JSON_STRING_PUT_BOOL(jsonValue, isSelectRegionVisible);
        JSON_STRING_PUT_BOOL(jsonValue, isNewAvoid);
        JSON_STRING_PUT_BOOL(jsonValue, recreateOverlay);
        JSON_STRING_PUT_BOOL(jsonValue, isUseOverlayNG);
        JSON_STRING_PUT_STRINGABLE(jsonValue, firstHandle);
        JSON_STRING_PUT_STRINGABLE(jsonValue, secondHandle);
        JSON_STRING_PUT_STRINGABLE(jsonValue, showArea);
        JSON_STRING_PUT_STRINGABLE(jsonValue, selectArea);
        JSON_STRING_PUT_STRINGABLE(jsonValue, rightClickOffset);
        JSON_STRING_PUT_STRING(jsonValue, selectText);
        return jsonValue->ToString();
    }

    void GetCallerNodeAncestorViewPort(RectF& viewPort);
    const RectF& GetFirstHandlePaintRect();
    const RectF& GetSecondHandlePaintRect();
    bool enableSubWindowMenu = false;
    OffsetF containerModalOffset;
};

enum class TextMenuShowMode {
    BEGIN = -1,
    UNSPECIFIED = BEGIN,
    // Display the text selection menu in the current window
    DEFAULT = 0,
    /**
     * Prefer to display the text selection menu in a separate window
     * and continue to display it within the current window if a separate window is not supported
     */
    PREFER_WINDOW = 1,
    END = PREFER_WINDOW,
};

TextMenuShowMode CastToTextMenuShowMode(int32_t value);

struct TextMenuOptions {
    std::optional<TextMenuShowMode> showMode;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SELECT_OVERLAY_PROPERTY_H
