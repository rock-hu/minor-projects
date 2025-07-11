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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WINDOW_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WINDOW_PATTERN_H

#include "common/rs_vector4.h"
#include "key_event.h"
#include "pointer_event.h"
#include "session/host/include/session.h"

#include "base/geometry/ng/size_t.h"
#include "core/common/container.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/image/image_source_info.h"

#include "core/components_ng/pattern/window_scene/helper/starting_window_layout_helper.h"
#include "core/components_ng/pattern/window_scene/scene/window_layout_algorithm.h"

namespace OHOS::Ace::NG {
class WindowPattern : public StackPattern {
    DECLARE_ACE_TYPE(WindowPattern, StackPattern);

public:
    WindowPattern()
    {
        startingWindowLayoutHelper_ = AceType::MakeRefPtr<StartingWindowLayoutHelper>();
    }
    ~WindowPattern()
    {
        startingWindowLayoutHelper_.Reset();
    }

    bool BorderUnoccupied() const override;
    std::vector<Rosen::Rect> GetHotAreas();
    sptr<Rosen::Session> GetSession();
    void CheckAndMeasureStartingWindow(const SizeF& currentParentSize);

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<WindowLayoutAlgorithm>();
    }

protected:
    void OnAttachToFrameNode() override;

    void DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    void DispatchKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed);
    void DisPatchFocusActiveEvent(bool isFocusActive);
    void TransferFocusState(bool focusState);

    virtual bool HasStartingPage() = 0;
    bool IsMainWindow() const;

    void RegisterLifecycleListener();
    void UnregisterLifecycleListener();

#ifdef ATOMIC_SERVICE_ATTRIBUTION_ENABLE
    RefPtr<FrameNode> BuildTextNode(const std::string& appNameInfo);
    RefPtr<FrameNode> BuildAnimateNode(const std::string& base64Resource);
    RefPtr<FrameNode> BuildStaticImageNode(const std::string& base64Resource);
    void CreateASStartingWindow();
#endif

    void CreateAppWindow();
    void CreateBlankWindow(RefPtr<FrameNode>& window);
    void CreateStartingWindow();
    void CreateSnapshotWindow(std::optional<std::shared_ptr<Media::PixelMap>> snapshot = std::nullopt);
    void ClearImageCache(const ImageSourceInfo& sourceInfo, Rosen::SnapshotStatus key, bool freeMultiWindow);

    void AddChild(const RefPtr<FrameNode>& host, const RefPtr<FrameNode>& child,
        const std::string& nodeType, int32_t index = DEFAULT_NODE_SLOT);
    void RemoveChild(const RefPtr<FrameNode>& host, const RefPtr<FrameNode>& child,
        const std::string& nodeType, bool allowTransition = false);
    
    ImageRotateOrientation TransformOrientationForMatchSnapshot(uint32_t lastRotation, uint32_t windowRotation);
    ImageRotateOrientation TransformOrientationForDisMatchSnapshot(uint32_t lastRotation, uint32_t windowRotation,
        uint32_t snapshotRotation);
    uint32_t TransformOrientation(uint32_t lastRotation, uint32_t windowRotation, uint32_t count);

    virtual void OnActivation() {}
    virtual void OnConnect() {}
    virtual void OnForeground() {}
    virtual void OnBackground() {}
    virtual void OnDisconnect() {}
    virtual void OnLayoutFinished() {}
    virtual void OnDrawingCompleted() {}
    virtual void OnRemoveBlank() {}
    virtual void OnAddSnapshot() {}
    virtual void OnRemoveSnapshot() {}
    virtual void OnAppRemoveStartingWindow() {}
    virtual void OnUpdateSnapshotWindow() {}
    virtual void OnPreLoadStartingWindowFinished() {}

    RefPtr<FrameNode> startingWindow_;
    RefPtr<StartingWindowLayoutHelper> startingWindowLayoutHelper_;
    SizeF lastParentSize_ = { 0.0f, 0.0f };
    RefPtr<FrameNode> appWindow_;
    RefPtr<FrameNode> snapshotWindow_;
    RefPtr<FrameNode> blankWindow_;
    RefPtr<FrameNode> newAppWindow_;
    std::string startingWindowName_ = "StartingWindow";
    std::string appWindowName_ = "AppWindow";
    std::string snapshotWindowName_ = "SnapshotWindow";
    std::string blankWindowName_ = "BlankWindow";
    const std::string newAppWindowName_ = "NewAppWindow";
    bool attachToFrameNodeFlag_ = false;
    bool isBlankForSnapshot_ = false;

    sptr<Rosen::Session> session_;
    int32_t instanceId_ = Container::CurrentId();
    std::function<void()> callback_;
    std::function<void(const Rosen::Vector4f&)> boundsChangedCallback_;

private:
    void UpdateSnapshotWindowProperty();
    bool IsSnapshotSizeChanged();
    void UpdateStartingWindowProperty(const Rosen::SessionInfo& sessionInfo,
        Color &color, ImageSourceInfo &sourceInfo);
    bool CheckAndAddStartingWindowAboveLocked();
    void HideStartingWindow();
    CancelableCallback<void()> interruptStartingTask_;

    std::shared_ptr<Rosen::ILifecycleListener> lifecycleListener_;
    friend class LifecycleListener;
    friend class WindowEventProcess;

    ACE_DISALLOW_COPY_AND_MOVE(WindowPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_WINDOW_PATTERN_H
