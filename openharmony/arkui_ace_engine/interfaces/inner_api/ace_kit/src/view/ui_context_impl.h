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

#ifndef FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_SRC_VIEW_UI_CONTEXT_IMPL_H
#define FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_SRC_VIEW_UI_CONTEXT_IMPL_H

#include "ui/view/ui_context.h"


namespace OHOS::Ace::NG {
class PipelineContext;
}

namespace OHOS::Ace::Kit {

class UIContextImpl : public UIContext {
    DECLARE_ACE_TYPE(UIContextImpl, UIContext);
public:
    UIContextImpl(NG::PipelineContext* context) : context_(context) {}
    ~UIContextImpl();

    void Reset();

    void RunScopeUITaskSync(Task&& task, const std::string& name) override;
    void RunScopeUITask(Task&& task, const std::string& name) override;
    void RunScopeUIDelayedTask(Task&& task, const std::string& name, uint32_t delayTime) override;

    void OnBackPressed() override;

    ColorMode GetLocalColorMode() override;
    ColorMode GetColorMode() override;
    float GetFontScale() override;

    RefPtr<OverlayManager> GetOverlayManager() override;

    void AddAfterLayoutTask(Task&& task, bool isFlushInImplicitAnimationTask = false) override;
    void RequestFrame() override;

    int32_t GetApiTargetVersion() override;
    bool GreatOrEqualTargetAPIVersion(int32_t version) override;
    int32_t GetContainerModalTitleHeight() override;
    int32_t GetContainerModalButtonsWidth() override;
    NG::OffsetF GetContainerModalButtonsOffset() override;
    void RegisterArkUIObjectLifecycleCallback(ArkUIObjectLifecycleCallback&& callback) override;
    void UnregisterArkUIObjectLifecycleCallback() override;
    sptr<IRemoteObject> GetToken() override;

    RefPtr<DisplayInfo> GetDisplayInfo() override;
    WindowMode GetWindowMode() override;
    bool GetIsMidScene() override;
    bool IsAccessibilityEnabled() override;

    int32_t RegisterSurfaceChangedCallback(
        std::function<void(int32_t, int32_t, int32_t, int32_t, WindowSizeChangeReason)>&& callback) override;
    void UnregisterSurfaceChangedCallback(int32_t callbackId) override;
    int32_t RegisterFoldStatusChangedCallback(std::function<void(FoldStatus)>&& callback) override;
    void UnRegisterFoldStatusChangedCallback(int32_t callbackId) override;
    int32_t RegisterRotationEndCallback(std::function<void()>&& callback) override;
    void UnregisterRotationEndCallback(int32_t callbackId) override;
    void AddWindowSizeChangeCallback(int32_t nodeId) override;

private:
    NG::PipelineContext* context_ = nullptr;
    RefPtr<OverlayManager> overlayManager_;
};

} // namespace OHOS::Ace::Kit

#endif // FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_SRC_VIEW_UI_CONTEXT_IMPL_H
