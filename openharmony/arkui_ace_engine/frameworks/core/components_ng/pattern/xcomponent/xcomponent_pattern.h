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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H

#include <optional>
#include <string>
#include <utility>

#include "base/geometry/dimension.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/geometry/size.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/common/thread_checker.h"
#include "core/components/common/layout/constants.h"
#include "core/components/xcomponent/native_interface_xcomponent_impl.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/event/input_event.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/xcomponent/inner_xcomponent_controller.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_accessibility_provider.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_event_hub.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_layout_algorithm.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_layout_property.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_paint_method.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/render_surface.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/components_ng/manager/display_sync/ui_display_sync.h"
#include "core/gestures/velocity.h"

namespace OHOS::Ace {
class ImageAnalyzerManager;
}
namespace OHOS::Ace::NG {
class XComponentExtSurfaceCallbackClient;
class XComponentPattern : public Pattern {
    DECLARE_ACE_TYPE(XComponentPattern, Pattern);

public:
    XComponentPattern() = default;
    XComponentPattern(const std::optional<std::string>& id, XComponentType type,
        const std::optional<std::string>& libraryname,
        const std::shared_ptr<InnerXComponentController>& xcomponentController, float initWidth = 0.0f,
        float initHeight = 0.0f, bool isTypedNode = false);
    ~XComponentPattern() override = default;

    void OnAttachToMainTree() override;
    void OnDetachFromMainTree() override;

    bool IsAtomicNode() const override
    {
        return type_ == XComponentType::SURFACE || type_ == XComponentType::TEXTURE || type_ == XComponentType::NODE;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<XComponentLayoutProperty>();
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<XComponentEventHub>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<XComponentLayoutAlgorithm>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        if (type_ == XComponentType::TEXTURE) {
            auto paint = MakeRefPtr<XComponentPaintMethod>(renderSurface_, AceType::Claim(this));
            return paint;
        }
        return nullptr;
    }

    FocusPattern GetFocusPattern() const override
    {
        if (type_ == XComponentType::NODE) {
            return { FocusType::SCOPE, true };
        }
        FocusPattern focusPattern = { FocusType::NODE, false };
        focusPattern.SetIsFocusActiveWhenFocused(true);
        return focusPattern;
    }

    bool NeedSoftKeyboard() const override
    {
        return nativeXComponentImpl_ ? nativeXComponentImpl_->IsNeedSoftKeyboard() : false;
    }

    std::pair<RefPtr<OHOS::Ace::NativeXComponentImpl>, std::weak_ptr<OH_NativeXComponent>> GetNativeXComponent()
    {
        if (!nativeXComponent_ || !nativeXComponentImpl_) {
            // for XComponentType::NODE
            nativeXComponentImpl_ = AceType::MakeRefPtr<NativeXComponentImpl>();
            nativeXComponent_ = std::make_shared<OH_NativeXComponent>(AceType::RawPtr(nativeXComponentImpl_));
        }
        return std::make_pair(nativeXComponentImpl_, nativeXComponent_);
    }

    void NativeXComponentDispatchTouchEvent(const OH_NativeXComponent_TouchEvent& touchEvent,
        const std::vector<XComponentTouchPoint>& xComponentTouchPoints);
    void NativeXComponentDispatchMouseEvent(const OH_NativeXComponent_MouseEvent& mouseEvent);
    void NativeXComponentDispatchAxisEvent(AxisEvent* axisEvent);

    void InitXComponent();
    void InitNativeXComponent();
    void InitNativeWindow(float textureWidth, float textureHeight);
    void XComponentSizeInit();
    void XComponentSizeChange(const RectF& surfaceRect, bool needFireNativeEvent);
    void NativeXComponentInit()
    {
        if (!isTypedNode_) {
            OnSurfaceCreated();
        }
    }

    std::string GetId() const
    {
        if (id_.has_value()) {
            return id_.value();
        }
        auto host = GetHost();
        return "nodeId:" + (host ? std::to_string(host->GetId()) : "-1");
    }

    void SetId(const std::string& id)
    {
        id_ = id;
    }

    const std::optional<std::string>& GetLibraryName() const
    {
        return libraryname_;
    }

    void SetLibraryName(const std::optional<std::string>& libraryname)
    {
        libraryname_ = libraryname;
    }

    const std::optional<std::string>& GetSoPath() const
    {
        return soPath_;
    }

    void SetSoPath(const std::string& soPath)
    {
        soPath_ = soPath;
    }

    XComponentType GetType()
    {
        return type_;
    }

    void SetType(XComponentType type)
    {
        type_ = type;
    }

    const SizeF& GetDrawSize() const
    {
        return drawSize_;
    }

    const SizeF& GetSurfaceSize() const
    {
        return surfaceSize_;
    }

    const OffsetF& GetSurfaceOffset() const
    {
        return surfaceOffset_;
    }

    OffsetF GetOffsetRelativeToWindow();

    const RefPtr<RenderContext>& GetRenderContextForSurface()
    {
        return renderContextForSurface_;
    }

    void SetSurfaceRotation(bool isLock);

    bool GetSurfaceRotation()
    {
        return isSurfaceLock_;
    }

    void SetIsTypeNode(bool isTypeNode)
    {
        isTypedNode_ = isTypeNode;
    }

    std::shared_ptr<InnerXComponentController> GetXComponentController()
    {
        return xcomponentController_;
    }

    void SetHandlingRenderContextForSurface(const RefPtr<RenderContext>& otherRenderContext);

    void RestoreHandlingRenderContextForSurface();

    XComponentControllerErrorCode SetExtController(const RefPtr<XComponentPattern>& extPattern);
    XComponentControllerErrorCode ResetExtController(const RefPtr<XComponentPattern>& extPattern);

    void SetExpectedRateRangeInit()
    {
        CHECK_NULL_VOID(nativeXComponentImpl_);
        nativeXComponentImpl_->SetExpectedRateRangeEventCallback([weak = AceType::WeakClaim(this)]() {
            auto xComponentPattern = weak.Upgrade();
            CHECK_NULL_VOID(xComponentPattern);
            xComponentPattern->HandleSetExpectedRateRangeEvent();
        });
    }

    void OnFrameEventInit()
    {
        CHECK_NULL_VOID(nativeXComponentImpl_);
        nativeXComponentImpl_->SetOnFrameEventCallback([weak = AceType::WeakClaim(this)]() {
            auto xComponentPattern = weak.Upgrade();
            CHECK_NULL_VOID(xComponentPattern);
            xComponentPattern->HandleOnFrameEvent();
        });
    }

    void UnregisterOnFrameEventInit()
    {
        CHECK_NULL_VOID(nativeXComponentImpl_);
        nativeXComponentImpl_->SetUnregisterOnFrameEventCallback([weak = AceType::WeakClaim(this)]() {
            auto xComponentPattern = weak.Upgrade();
            CHECK_NULL_VOID(xComponentPattern);
            xComponentPattern->HandleUnregisterOnFrameEvent();
        });
    }

    void SetXcomponentInit(bool isInit)
    {
        hasXComponentInit_ = isInit;
    }

    bool ChangeRenderType(NodeRenderType renderType);

    void SetRenderType(NodeRenderType renderType)
    {
        renderType_ = renderType;
    }

    void UpdateTransformHintChangedCallbackId(std::optional<int32_t> id)
    {
        transformHintChangedCallbackId_ = id;
    }

    bool HasTransformHintChangedCallbackId()
    {
        return transformHintChangedCallbackId_.has_value();
    }

    bool NeedTriggerLoadEventImmediately() const
    {
        return isTypedNode_ && isNativeXComponent_ && hasLoadNativeDone_;
    }

    void SetExportTextureSurfaceId(const std::string& surfaceId);
    void FireExternalEvent(RefPtr<NG::PipelineContext> context,
        const std::string& componentId, const uint32_t nodeId, const bool isDestroy);
    void ConfigSurface(uint32_t surfaceWidth, uint32_t surfaceHeight);

    // accessibility
    void InitializeAccessibility();
    void UninitializeAccessibility();
    bool OnAccessibilityChildTreeRegister(uint32_t windowId, int32_t treeId);
    bool OnAccessibilityChildTreeDeregister();
    void OnSetAccessibilityChildTree(int32_t childWindowId, int32_t childTreeId);
    void SetAccessibilityState(bool state) {}
    RefPtr<AccessibilitySessionAdapter> GetAccessibilitySessionAdapter() override;
    void InitializeAccessibilityCallback();
    void HandleRegisterAccessibilityEvent(bool isRegister);

    void SetIdealSurfaceWidth(float surfaceWidth);
    void SetIdealSurfaceHeight(float surfaceHeight);
    void SetIdealSurfaceOffsetX(float offsetX);
    void SetIdealSurfaceOffsetY(float offsetY);
    void ClearIdealSurfaceOffset(bool isXAxis);
    std::tuple<bool, bool, bool> UpdateSurfaceRect();
    void HandleSurfaceChangeEvent(bool needForceRender, bool offsetChanged, bool sizeChanged, bool needFireNativeEvent,
        bool frameOffsetChange = false);
    void EnableAnalyzer(bool enable);
    void SetImageAIOptions(void* options);
    void StartImageAnalyzer(void* config, OnAnalyzedCallback& onAnalyzed);
    void StopImageAnalyzer();
    RectF AdjustPaintRect(float positionX, float positionY, float width, float height, bool isRound);
    float RoundValueToPixelGrid(float value, bool isRound, bool forceCeil, bool forceFloor);
    void OnSurfaceDestroyed();
    void SetRenderFit(RenderFit renderFit);
    void HandleSurfaceCreated();
    void HandleSurfaceDestroyed();
    void ChangeSurfaceCallbackMode(SurfaceCallbackMode mode)
    {
        if (surfaceCallbackModeChangeEvent_) {
            surfaceCallbackModeChangeEvent_(mode);
        }
    }
    void OnSurfaceCallbackModeChange(SurfaceCallbackMode mode);
    void EnableSecure(bool isSecure);
    void HdrBrightness(float hdrBrightness);
    void EnableTransparentLayer(bool isTransparentLayer);

private:
    void OnAttachToFrameNode() override;
    void OnDetachFromFrameNode(FrameNode* frameNode) override;
    void BeforeSyncGeometryProperties(const DirtySwapConfig& config) override;
    void OnRebuildFrame() override;
    void OnAreaChangedInner() override;
    void OnWindowHide() override;
    void OnWindowShow() override;
    void OnModifyDone() override;
    void DumpInfo() override;
    void DumpInfo(std::unique_ptr<JsonValue>& json) override;
    void DumpSimplifyInfo(std::unique_ptr<JsonValue>& json) override {}
    void DumpAdvanceInfo() override;
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json) override;
    void OnAttachContext(PipelineContext *context) override;
    void OnDetachContext(PipelineContext *context) override;
    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    void NativeXComponentOffset(double x, double y);

    void LoadNative();
    void OnNativeLoad(FrameNode* frameNode);
    void OnNativeUnload(FrameNode* frameNode);

    void OnSurfaceCreated();
    void OnSurfaceChanged(const RectF& surfaceRect, bool needResizeNativeWindow);

    void NativeSurfaceShow();
    void NativeSurfaceHide();

    void Initialize();
    void InitController();
    void InitSurface();
    void InitNativeNodeCallbacks();
    void InitEvent();
    void InitTouchEvent(const RefPtr<GestureEventHub>& gestureHub);
    void InitOnTouchIntercept(const RefPtr<GestureEventHub>& gestureHub);
    void InitAxisEvent(const RefPtr<InputEventHub>& inputHub);
    void HandleTouchEvent(const TouchEventInfo& info);
    void InitMouseEvent(const RefPtr<InputEventHub>& inputHub);
    void HandleMouseEvent(const MouseInfo& info);
    void HandleAxisEvent(const AxisInfo& info);
    void InitMouseHoverEvent(const RefPtr<InputEventHub>& inputHub);
    void HandleMouseHoverEvent(bool isHover);
    void InitFocusEvent(const RefPtr<FocusHub>& focusHub);
    void HandleFocusEvent();
    bool HandleKeyEvent(const KeyEvent& event);
    void HandleBlurEvent();
    ExternalEvent CreateExternalEvent();

    void SetTouchPoint(
        const std::list<TouchLocationInfo>& touchInfoList, int64_t timeStamp, const TouchType& touchType);
    void HandleSetExpectedRateRangeEvent();
    void HandleOnFrameEvent();
    void HandleUnregisterOnFrameEvent();
    bool ExportTextureAvailable();
    void AddAfterLayoutTaskForExportTexture();
    bool DoTextureExport();
    bool StopTextureExport();
    void InitializeRenderContext();
    void SetSurfaceNodeToGraphic();
    bool IsSupportImageAnalyzerFeature();
    void CreateAnalyzerOverlay();
    void DestroyAnalyzerOverlay();
    void UpdateAnalyzerOverlay();
    void UpdateAnalyzerUIConfig(const RefPtr<NG::GeometryNode>& geometryNode);
    void ReleaseImageAnalyzer();
    void UpdateTransformHint();
    void SetRotation(uint32_t rotation);
    void RegisterSurfaceCallbackModeEvent();

#ifdef RENDER_EXTRACT_SUPPORTED
    RenderSurface::RenderSurfaceType CovertToRenderSurfaceType(const XComponentType& hostType);
    void RegisterRenderContextCallBack();
    void RequestFocus();
#endif

    std::vector<OH_NativeXComponent_HistoricalPoint> SetHistoryPoint(const std::list<TouchLocationInfo>& touchInfoList);
    std::optional<std::string> id_;
    XComponentType type_;
    std::optional<std::string> libraryname_;
    std::shared_ptr<InnerXComponentController> xcomponentController_;
    std::optional<std::string> soPath_;

    RefPtr<RenderSurface> renderSurface_;
    RefPtr<RenderContext> renderContextForSurface_;
    RefPtr<RenderContext> handlingSurfaceRenderContext_;
    WeakPtr<XComponentPattern> extPattern_;

    std::shared_ptr<OH_NativeXComponent> nativeXComponent_;
    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    bool hasXComponentInit_ = false;

    RefPtr<TouchEventImpl> touchEvent_;
    OH_NativeXComponent_TouchEvent touchEventPoint_ = {};
    RefPtr<InputEvent> mouseEvent_;
    RefPtr<InputEvent> axisEvent_;
    RefPtr<InputEvent> mouseHoverEvent_;
    std::vector<XComponentTouchPoint> nativeXComponentTouchPoints_;
    RefPtr<XComponentExtSurfaceCallbackClient> extSurfaceClient_;
    SizeF initSize_;
    OffsetF localPosition_;
    OffsetF globalPosition_;
    OffsetF surfaceOffset_;
    SizeF drawSize_;
    SizeF surfaceSize_;
    RectF paintRect_;
    RefPtr<UIDisplaySync> displaySync_ = AceType::MakeRefPtr<UIDisplaySync>(UIObjectType::DISPLAYSYNC_XCOMPONENT);

    std::optional<float> selfIdealSurfaceWidth_;
    std::optional<float> selfIdealSurfaceHeight_;
    std::optional<float> selfIdealSurfaceOffsetX_;
    std::optional<float> selfIdealSurfaceOffsetY_;
    std::string surfaceId_;
    void* nativeWindow_ = nullptr;

    bool isSurfaceLock_ = false;
    uint32_t windowId_ = 0;
    int32_t treeId_ = 0;
    std::shared_ptr<AccessibilityChildTreeCallback> accessibilityChildTreeCallback_;
    RefPtr<XComponentAccessibilityProvider> accessibilityProvider_;
    RefPtr<AccessibilitySessionAdapter> accessibilitySessionAdapter_;

    // for export texture
    NodeRenderType renderType_ = NodeRenderType::RENDER_TYPE_DISPLAY;
    uint64_t exportTextureSurfaceId_ = 0U;
    bool hasReleasedSurface_ = false;
    std::shared_ptr<ImageAnalyzerManager> imageAnalyzerManager_;
    bool isEnableAnalyzer_ = false;
    std::optional<int32_t> transformHintChangedCallbackId_;
    uint32_t rotation_ = 0;
    bool isTypedNode_ = false;
    bool isNativeXComponent_ = false;
    bool hasLoadNativeDone_ = false;
    bool isEnableSecure_ = false;
    float hdrBrightness_ = 1.0f;
    bool isTransparentLayer_ = false;
    SurfaceCallbackMode surfaceCallbackMode_ = SurfaceCallbackMode::DEFAULT;
    std::function<void(SurfaceCallbackMode)> surfaceCallbackModeChangeEvent_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H
