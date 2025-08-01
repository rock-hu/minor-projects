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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PAINTS_ADAPTER_ROSEN_RENDER_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PAINTS_ADAPTER_ROSEN_RENDER_CONTEXT_H

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "include/core/SkCanvas.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkRefCnt.h"
#include "render_service_client/core/animation/rs_particle_params.h"
#include "render_service_client/core/modifier_ng/appearance/rs_alpha_modifier.h"
#include "render_service_client/core/modifier_ng/appearance/rs_behind_window_filter_modifier.h"
#include "render_service_client/core/modifier_ng/appearance/rs_mask_modifier.h"
#include "render_service_client/core/modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "render_service_client/core/modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "render_service_client/core/modifier_ng/geometry/rs_transform_modifier.h"
#include "render_service_client/core/ui/rs_canvas_node.h"
#include "render_service_client/core/ui/rs_node.h"
#include "render_service_client/core/ui/rs_texture_export.h"
#include "render_service_client/core/ui/rs_ui_context.h"

#include "base/geometry/dimension_offset.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/utils/noncopyable.h"
#include "core/components/common/layout/position_param.h"
#include "core/components/common/properties/color.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_ng/property/progress_mask_property.h"
#include "core/components_ng/render/adapter/focus_animation_modifier.h"
#include "core/components_ng/render/adapter/graphic_modifier.h"
#include "core/components_ng/render/adapter/moon_progress_modifier.h"
#include "core/components_ng/render/adapter/rosen_transition_effect.h"
#include "core/components_ng/render/render_context.h"

namespace OHOS::Ace::NG {
class BackgroundModifier;
class TransitionModifier;
class BorderImageModifier;
class DebugBoundaryModifier;
class MouseSelectModifier;
class FocusStateModifier;
class PageTransitionEffect;
class OverlayTextModifier;
class GradientStyleModifier;
class PipelineContext;
class RosenRenderContext : public RenderContext {
    DECLARE_ACE_TYPE(RosenRenderContext, NG::RenderContext)
public:
    RosenRenderContext() = default;
    ~RosenRenderContext() override;

    void SetSurfaceChangedCallBack(const std::function<void(float, float, float, float)>& callback) override;

    void RemoveSurfaceChangedCallBack() override;

    void MarkNewFrameAvailable(void* nativeWindow) override;
    void AddAttachCallBack(const std::function<void(int64_t, bool)>& attachCallback) override;
    void AddUpdateCallBack(const std::function<void(std::vector<float>&)>& updateCallback) override;
    void AddInitTypeCallBack(const std::function<void(int32_t&)>& initTypeCallback) override;

    void InitContext(bool isRoot, const std::optional<ContextParam>& param) override;

    void InitContext(bool isRoot, const std::optional<ContextParam>& param, bool isLayoutNode) override;

    void SyncGeometryFrame(const RectF& paintRect);

    void SyncGeometryProperties(GeometryNode* geometryNode, bool isRound = true, uint8_t flag = 0) override;

    void SyncGeometryProperties(const RectF& paintRect) override;

    void SetBorderRadius(const BorderRadiusProperty& value) override;

    void SetBorderStyle(const BorderStyleProperty& value) override;

    void SetBorderColor(const BorderColorProperty& value) override;

    void SetBorderWidth(const BorderWidthProperty& value) override;

    void SetDashGap(const BorderWidthProperty& value) override;

    void SetDashWidth(const BorderWidthProperty& value) override;

    void SetOuterBorderRadius(const BorderRadiusProperty& value) override;

    void SetOuterBorderStyle(const BorderStyleProperty& value) override;

    void SetOuterBorderColor(const BorderColorProperty& value) override;

    void SetOuterBorderWidth(const BorderWidthProperty& value) override;

    void SetExtraOffset(const std::optional<OffsetF>& offset) override;

    void SetSandBox(const std::optional<OffsetF>& parentPosition, bool force = false) override;

    bool HasSandBox() const override
    {
        return sandBoxCount_ > 0;
    }

    size_t GetAnimationsCount() const override
    {
        if (rsNode_) {
            return rsNode_->GetAnimationsCount();
        }
        return 0;
    }

    void SetFrameWithoutAnimation(const RectF& paintRect) override;

    void RebuildFrame(FrameNode* self, const std::list<RefPtr<FrameNode>>& children) override;

    void AddFrameChildren(FrameNode* self, const std::list<RefPtr<FrameNode>>& children) override;

    void RemoveFrameChildren(FrameNode* self, const std::list<RefPtr<FrameNode>>& children) override;

    void MoveFrame(FrameNode* self, const RefPtr<FrameNode>& child, int32_t index) override;

    void OnModifyDone() override;

    void ResetBlendBgColor() override;

    void BlendBgColor(const Color& color) override;

    void ResetBlendBorderColor() override;

    void BlendBorderColor(const Color& color) override;

    // Paint focus state by component's setting. It will paint along the paintRect
    void PaintFocusState(const RoundRect& paintRect, const Color& paintColor, const Dimension& paintWidth,
        bool isAccessibilityFocus = false, bool isFocusBoxGlow = false) override;
    // Paint focus state by component's setting. It will paint along the frameRect(padding: focusPaddingVp)
    void PaintFocusState(const RoundRect& paintRect, const Dimension& focusPaddingVp, const Color& paintColor,
        const Dimension& paintWidth, const PaintFocusExtraInfo& paintFocusExtraInfo) override;
    // Paint focus state by default. It will paint along the component rect(padding: focusPaddingVp)
    void PaintFocusState(const Dimension& focusPaddingVp, const Color& paintColor, const Dimension& paintWidth,
        bool isFocusBoxGlow = false) override;

    void ClearFocusState() override;

    const std::shared_ptr<Rosen::RSNode>& GetRSNode();

    void SetRSNode(const std::shared_ptr<Rosen::RSNode>& rsNode);

    uint64_t GetNodeId() const override
    {
        CHECK_NULL_RETURN(rsNode_, 0);
        return static_cast<uint64_t>(rsNode_->GetId());
    }

    void StartRecording() override;

    void StopRecordingIfNeeded() override;

    void SetDrawContentAtLast(bool useDrawContentLastOrder) override
    {
        CHECK_NULL_VOID(rsNode_);
        rsNode_->SetPaintOrder(useDrawContentLastOrder);
    }

    void SetClipToFrame(bool useClip) override
    {
        CHECK_NULL_VOID(rsNode_);
        rsNode_->SetClipToFrame(useClip);
    }

    void SetClipToBounds(bool useClip) override
    {
        CHECK_NULL_VOID(rsNode_);
        rsNode_->SetClipToBounds(useClip);
    }

    void SetVisible(bool visible) override
    {
        CHECK_NULL_VOID(rsNode_);
        rsNode_->SetVisible(visible);
    }

    template<typename ModifierName, auto Setter, typename T>
    void AddOrUpdateModifier(std::shared_ptr<ModifierName>& modifier, const T& value);

    void FlushContentDrawFunction(CanvasDrawFunction&& contentDraw) override;

    void FlushForegroundDrawFunction(CanvasDrawFunction&& foregroundDraw) override;

    void FlushOverlayDrawFunction(CanvasDrawFunction&& overlayDraw) override;

    void AnimateHoverEffectScale(bool isHovered) override;
    void AnimateHoverEffectBoard(bool isHovered) override;
    void UpdateBackBlurRadius(const Dimension& radius) override;
    void UpdateBackBlurStyle(
        const std::optional<BlurStyleOption>& bgBlurStyle, const SysOptions& sysOptions = SysOptions()) override;
    void UpdateBackgroundEffect(
        const std::optional<EffectOption>& effectOption, const SysOptions& sysOptions = SysOptions()) override;
    void UpdateMotionBlur(const MotionBlurOption& motionBlurOption) override;
    void UpdateBackBlur(
        const Dimension& radius, const BlurOption& blurOption, const SysOptions& sysOptions = SysOptions()) override;
    void UpdateNodeBackBlur(const Dimension& radius, const BlurOption& blurOption) override;
    void UpdateFrontBlur(
        const Dimension& radius, const BlurOption& blurOption, const SysOptions& sysOptions = SysOptions()) override;
    void UpdateFrontBlurRadius(const Dimension& radius) override;
    void UpdateFrontBlurStyle(
        const std::optional<BlurStyleOption>& fgBlurStyle, const SysOptions& sysOptions = SysOptions()) override;
    void OnForegroundEffectUpdate(float radius) override;
    void ResetBackBlurStyle() override;
    void OnSphericalEffectUpdate(double radio) override;
    void OnPixelStretchEffectUpdate(const PixStretchEffectOption& option) override;
    void OnLightUpEffectUpdate(double radio) override;
    void OnParticleOptionArrayUpdate(const std::list<ParticleOption>& optionList) override;
    void OnClickEffectLevelUpdate(const ClickEffectInfo& info) override;

    void UpdateVisualEffect(const OHOS::Rosen::VisualEffect* visualEffect) override;
    void UpdateBackgroundFilter(const OHOS::Rosen::Filter* backgroundFilter) override;
    void UpdateForegroundFilter(const OHOS::Rosen::Filter* foregroundFilter) override;
    void UpdateCompositingFilter(const OHOS::Rosen::Filter* compositingFilter) override;
    void UpdateBlender(const OHOS::Rosen::Blender* blender) override;

    Rosen::SHADOW_COLOR_STRATEGY ToShadowColorStrategy(ShadowColorStrategy shadowColorStrategy);
    void OnBackShadowUpdate(const Shadow& shadow) override;
    void OnBackBlendModeUpdate(BlendMode blendMode) override;
    void OnBackBlendApplyTypeUpdate(BlendApplyType applyType) override;
    void UpdateBorderWidthF(const BorderWidthPropertyF& value) override;

    void OnTransformMatrixUpdate(const Matrix4& matrix) override;
    void OnTransform3DMatrixUpdate(const Matrix4& matrix) override;

    void UpdateTransition(const TransitionOptions& options) override;
    void CleanTransition() override;
    void UpdateChainedTransition(const RefPtr<NG::ChainedTransitionEffect>& effect) override;
    bool HasAppearingTransition() const
    {
        return propTransitionAppearing_ != nullptr;
    }
    bool HasDisappearingTransition() const
    {
        return propTransitionDisappearing_ != nullptr;
    }
    void ClearModifiers();
    void OnNodeAppear(bool recursive) override;
    void OnNodeDisappear(bool recursive) override;
    void SetTransitionOutCallback(std::function<void()>&& callback) override;
    void SetTransitionInCallback(std::function<void()>&& callback) override;
    void SetTransitionUserCallback(TransitionFinishCallback&& callback) override;
    void ClipWithRect(const RectF& rectF) override;
    void ClipWithRRect(const RectF& rectF, const RadiusF& radiusF) override;
    void ClipWithRoundRect(const RoundRect& roundRect) override;
    void ClipWithOval(const RectF& rectF) override;
    void ClipWithCircle(const Circle& circle) override;
    void RemoveClipWithRRect() override;
    void UpdateWindowFocusState(bool isFocused) override;
    void UpdateWindowActiveState(bool isActive) override;
    void SetContentClip(const std::variant<RectF, RefPtr<ShapeRect>>& rect) override;
    void ResetContentClip() override;

    void SetSharedTranslate(float xTranslate, float yTranslate) override;
    void ResetSharedTranslate() override;

    static std::vector<std::shared_ptr<Rosen::RSNode>> GetChildrenRSNodes(
        const std::list<RefPtr<FrameNode>>& frameChildren, std::unordered_map<Rosen::RSNode::SharedPtr, bool>& nodeMap);

    // if translate params use percent dimension, frameSize should be given correctly
    static std::shared_ptr<Rosen::RSTransitionEffect> GetRSTransitionWithoutType(
        const std::unique_ptr<TransitionOptions>& options, const SizeF& frameSize = SizeF());

    static float ConvertDimensionToScaleBySize(const Dimension& dimension, float size);

    static SizeF GetPercentReference(const RefPtr<FrameNode>& frameNode);

    void FlushContentModifier(const RefPtr<Modifier>& modifier) override;
    void FlushKitContentModifier(const RefPtr<Kit::Modifier>& modifier) override;
    void FlushOverlayModifier(const RefPtr<Modifier>& modifier) override;
    void FlushForegroundModifier(const RefPtr<Modifier>& modifier) override;

    void AddChild(const RefPtr<RenderContext>& renderContext, int index) override;
    void RemoveChild(const RefPtr<RenderContext>& renderContext) override;
    void ClearChildren() override;
    void SetBounds(float positionX, float positionY, float width, float height) override;
    void SetSecurityLayer(bool isSecure) override;
    void SetHDRBrightness(float hdrBrightness) override;
    void SetImageHDRBrightness(float hdrBrightness) override;
    void SetImageHDRPresent(bool hdrPresent) override;
    void SetTransparentLayer(bool isTransparentLayer) override;
    void SetScreenId(uint64_t screenId) override;
    void SetAlwaysSnapshot(bool enable) override;
    void OnTransformTranslateUpdate(const TranslateOptions& value) override;
    Vector3F MarshallTranslate(const TranslateOptions& translate);
    bool DoTextureExport(uint64_t surfaceId) override;
    bool StopTextureExport() override;
    void SetSurfaceRotation(bool isLock) override;
    void SetRenderFit(RenderFit renderFit) override;
    PipelineContext* GetPipelineContext() const;

    RectF GetPaintRectWithTransform() override;

    std::pair<RectF, bool> GetPaintRectWithTranslate() override;

    RectF GetPaintRectWithoutTransform() override;

    RectF GetPaintRectWithTransformWithoutDegree() override;

    // get position property
    RectF GetPropertyOfPosition() override;

    // append translate value and return origin value.
    void UpdateTranslateInXY(const OffsetF& offset) override;
    OffsetF GetShowingTranslateProperty() override;

    void CancelTranslateXYAnimation() override;

    OffsetF GetTranslateXYProperty() override;

    Matrix4 GetLocalTransformMatrix() override;

    void GetPointWithRevert(PointF& point) override;

    void GetPointTransform(PointF& point) override;

    void GetPointTransformRotate(PointF& point) override;

    Matrix4 GetMatrixWithTransformRotate() override;

    void GetPointWithTransform(PointF& point) override;

    void ClearDrawCommands() override;

    void RemoveOverlayModifier(const RefPtr<OverlayModifier>& modifier) override;
    void RemoveContentModifier(const RefPtr<ContentModifier>& modifier) override;

    void OpacityAnimation(const AnimationOption& option, double begin, double end) override;
    void ScaleAnimation(const AnimationOption& option, double begin, double end) override;

    void PaintAccessibilityFocus() override;

    void UpdateAccessibilityRoundRect() override;

    void ClearAccessibilityFocus() override;

    void OnAccessibilityFocusUpdate(
        bool isAccessibilityFocus, const int64_t accessibilityIdForVirtualNode = INVALID_PARENT_ID) override;
    void OnAccessibilityFocusRectUpdate(RectT<int32_t> accessibilityFocusRect) override;

    void OnMouseSelectUpdate(bool isSelected, const Color& fillColor, const Color& strokeColor) override;
    void UpdateMouseSelectWithRect(const RectF& rect, const Color& fillColor, const Color& strokeColor) override;

    void OnPositionUpdate(const OffsetT<Dimension>& value) override;
    void OnPositionEdgesUpdate(const EdgesParam& value) override;
    void RecalculatePosition() override;
    void OnZIndexUpdate(int32_t value) override;
    void DumpInfo() override;
    void DumpInfo(std::unique_ptr<JsonValue>& json) override;
    void DumpSimplifyStagingProperties(std::unique_ptr<JsonValue>& json);
    void DumpAdvanceInfo() override;
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json) override;
    void SetAdvanceInfo(std::unique_ptr<JsonValue>& json);
    void BuildStagingInfo(std::unique_ptr<JsonValue>& json);
    void BuildPositionInfo(std::unique_ptr<JsonValue>& json);
    void BuildShadowInfo(std::unique_ptr<JsonValue>& json);
    void SetClipBoundsWithCommands(const std::string& commands) override;
    void SetNeedDebugBoundary(bool flag) override
    {
        needDebugBoundary_ = flag;
    }

    bool NeedDebugBoundary() const override
    {
        return needDebugBoundary_;
    }

    void OnBackgroundAlignUpdate(const Alignment& align) override;
    void OnBackgroundPixelMapUpdate(const RefPtr<PixelMap>& value) override;
    void OnCustomBackgroundColorUpdate(const Color& color) override;
    void CreateBackgroundPixelMap(const RefPtr<FrameNode>& customNode) override;
    void OnIsTransitionBackgroundUpdate(bool isTransitionBackground) override {}
    void OnBuilderBackgroundFlagUpdate(bool isBuilderBackground) override;

    void ColorToRSColor(const Color& color, OHOS::Rosen::RSColor& rsColor);
    void OnBackgroundColorUpdate(const Color& value) override;
    void OnOpacityUpdate(double opacity) override;
    void OnDynamicRangeModeUpdate(DynamicRangeMode dynamicRangeMode) override;
    void SetColorGamut(uint32_t colorGamut) override;
    void SetAlphaOffscreen(bool isOffScreen) override;
    void MarkContentChanged(bool isChanged) override;
    void MarkDrivenRender(bool flag) override;
    void MarkDrivenRenderItemIndex(int32_t index) override;
    void MarkDrivenRenderFramePaintState(bool flag) override;
    RefPtr<PixelMap> GetThumbnailPixelMap(bool needScale = false, bool isOffline = true) override;
    void UpdateThumbnailPixelMapScale(float& scaleX, float& scaleY) override;
    bool CreateThumbnailPixelMapAsyncTask(
        bool needScale, std::function<void(const RefPtr<PixelMap>)> &&callback) override;
    bool GetBitmap(RSBitmap& bitmap, std::shared_ptr<RSDrawCmdList> drawCmdList = nullptr);
    bool GetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
        std::shared_ptr<RSDrawCmdList> drawCmdList = nullptr, Rosen::Drawing::Rect* rect = nullptr);
    void SetActualForegroundColor(const Color& value) override;
    void AttachNodeAnimatableProperty(RefPtr<NodeAnimatablePropertyBase> property) override;
    void DetachNodeAnimatableProperty(const RefPtr<NodeAnimatablePropertyBase>& property) override;

    void RegisterSharedTransition(const RefPtr<RenderContext>& other, const bool isInSameWindow) override;
    void UnregisterSharedTransition(const RefPtr<RenderContext>& other) override;

    void SetUsingContentRectForRenderFrame(bool value, bool adjustRSFrameByContentRect = false) override;
    void SetFrameGravity(OHOS::Rosen::Gravity gravity) override;
    void SetUIFirstSwitch(OHOS::Rosen::RSUIFirstSwitch uiFirstSwitch);

    int32_t CalcExpectedFrameRate(const std::string& scene, float speed) override;

    void SetBackgroundShader(const std::shared_ptr<Rosen::RSShader>& shader);

    // used in arkts_native_render_node_modifier set property directly to rsNode
    void SetRotation(float rotationX, float rotationY, float rotationZ) override;
    void SetShadowColor(uint32_t color) override;
    void SetShadowOffset(float offsetX, float offsetY) override;
    void SetShadowAlpha(float alpha) override;
    void SetShadowElevation(float elevation) override;
    void SetShadowRadius(float radius) override;
    void SetRenderFrameOffset(const OffsetF& offset) override;
    void SetScale(float scaleX, float scaleY) override;
    void SetScrollScale(float scale) override;
    void ResetScrollScale() override;
    void SetBackgroundColor(uint32_t colorValue) override;
    void SetRenderPivot(float pivotX, float pivotY) override;
    void SetFrame(float positionX, float positionY, float width, float height) override;
    void SetOpacity(float opacity) override;
    void SetTranslate(float translateX, float translateY, float translateZ) override;
    void SetHostNode(const WeakPtr<FrameNode>& host) override;

    OffsetF GetBaseTransalteInXY() const override;
    void SetBaseTranslateInXY(const OffsetF& offset) override;
    float GetBaseRotateInZ() const override;
    void SetBaseRotateInZ(float degree) override;

    void SetRectMask(const RectF& rect, const ShapeMaskProperty& property) override;
    void SetCircleMask(const Circle& circle, const ShapeMaskProperty& property) override;
    void SetRoundRectMask(const RoundRect& roundRect, const ShapeMaskProperty& property) override;
    void SetOvalMask(const RectF& rect, const ShapeMaskProperty& property) override;
    void SetCommandPathMask(const std::string& commands, const ShapeMaskProperty& property) override;
    void ResetSurface(int width, int height) override;
    void SetMarkNodeGroup(bool isNodeGroup) override;
    int32_t GetRotateDegree() override;
    void PaintDebugBoundary(bool flag) override;
    void UpdateRenderGroup(bool isRenderGroup, bool isForced, bool includeProperty) override;
    void SavePaintRect(bool isRound = true, uint16_t flag = 0) override;
    void SyncPartialRsProperties() override;
    void UpdatePaintRect(const RectF& paintRect) override;
    Matrix4 GetRevertMatrix() override;
    void SuggestOpIncNode(bool isOpincNode, bool isNeedCalculate) override;
    void SetOpacityMultiplier(float opacity) override;
    void UpdateOcclusionCullingStatus(bool enable) override;
    bool IsDisappearing() const override
    {
        return isDisappearing_;
    }
    void UpdateWindowBlur() override;
    void MarkUiFirstNode(bool isUiFirstNode) override;

    void SetRSUIContext(PipelineContext* context) override;

    void SetDrawNode() override;
    bool AddNodeToRsTree() override;
    static std::shared_ptr<Rosen::RSNode> GetRsNodeByFrame(const RefPtr<FrameNode>& frameNode);
    RefPtr<FrameNode> GetFrameNodeById(int32_t frameNodeId);
    bool CanNodeBeDeleted(const RefPtr<FrameNode>& node) const;
    void GetLiveChildren(const RefPtr<FrameNode>& node, std::list<RefPtr<FrameNode>>& childNodes);
    void AddRsNodeForCapture();
    static bool initDrawNodeChangeCallback_;
    static bool initPropertyNodeChangeCallback_;

    void FreezeCanvasNode(bool freezeFlag = false);
    void RemoveCanvasNode();
    void CheckAnimationParametersValid(int32_t& animationParam);
    bool SetCanvasNodeOpacityAnimation(int32_t duration, int32_t delay, bool isDragEnd = false);
    void LinkCanvasNodeToRootNode(const RefPtr<FrameNode>& rootNode);
    void CreateCanvasNode();
    std::shared_ptr<Rosen::RSCanvasNode> GetCanvasNode() const;

    void AddKeyFrameAnimateEndCallback(const std::function<void()>& callback)
    {
        callbackAnimateEnd_ =  callback;
    }

    void AddKeyFrameCachedAnimateActionCallback(const std::function<void()>& callback)
    {
        callbackCachedAnimateAction_ = callback;
    }

    bool GetIsDraggingFlag() const
    {
        return isDraggingFlag_;
    }

    void SetIsDraggingFlag(bool isDraggingFlag)
    {
        isDraggingFlag_ = isDraggingFlag;
    }

    bool GetReDraggingFlag() const
    {
        return reDraggingFlag_;
    }

    void SetReDraggingFlag(bool reDraggingFlag)
    {
        reDraggingFlag_ = reDraggingFlag;
    }

    void SetAnimationPropertyValue(AnimationPropertyType property, const std::vector<float>& value) override;
    void CancelPropertyAnimation(AnimationPropertyType property) override;
    std::vector<float> GetRenderNodePropertyValue(AnimationPropertyType property) override;
    void SyncRSPropertyToRenderContext(AnimationPropertyType property) override;
    void RemoveFromTree() override;

    void SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion) override;

    void UpdateCustomBackground() override;

protected:
    void OnBackgroundImageUpdate(const ImageSourceInfo& src) override;
    void OnBackgroundImageRepeatUpdate(const ImageRepeat& imageRepeat) override;
    void OnBackgroundImageSyncModeUpdate(bool imageRepeat) override;

    void OnBackgroundImageSizeUpdate(const BackgroundImageSize& bgImgSize) override;
    void OnBackgroundImagePositionUpdate(const BackgroundImagePosition& bgImgPosition) override;
    void OnBackgroundImageResizableSliceUpdate(const ImageResizableSlice& slice) override;

    void OnForegroundColorUpdate(const Color& value) override;
    void OnForegroundColorStrategyUpdate(const ForegroundColorStrategy& value) override;

    void OnBorderImageUpdate(const RefPtr<BorderImage>& borderImage) override;
    void OnBorderImageSourceUpdate(const ImageSourceInfo& borderImageSourceInfo) override;
    void OnHasBorderImageSliceUpdate(bool tag) override {}
    void OnHasBorderImageWidthUpdate(bool tag) override {}
    void OnHasBorderImageOutsetUpdate(bool tag) override {}
    void OnHasBorderImageRepeatUpdate(bool tag) override {}
    void OnBorderImageGradientUpdate(const Gradient& gradient) override;

    void OnBorderRadiusUpdate(const BorderRadiusProperty& value) override;
    void OnBorderColorUpdate(const BorderColorProperty& value) override;
    void OnBorderStyleUpdate(const BorderStyleProperty& value) override;
    void OnDashGapUpdate(const BorderWidthProperty& value) override;
    void OnDashWidthUpdate(const BorderWidthProperty& value) override;

    void OnOuterBorderRadiusUpdate(const BorderRadiusProperty& value) override;
    void OnOuterBorderColorUpdate(const BorderColorProperty& value) override;
    void OnOuterBorderStyleUpdate(const BorderStyleProperty& value) override;
    void OnOuterBorderWidthUpdate(const BorderWidthProperty& value) override;

    void OnTransformScaleUpdate(const VectorF& value) override;
    void OnTransformCenterUpdate(const DimensionOffset& value) override;
    void OnTransformRotateUpdate(const Vector5F& value) override;
    void OnTransformRotateAngleUpdate(const Vector4F& value) override;

    void OnOffsetUpdate(const OffsetT<Dimension>& value) override;
    void OnOffsetEdgesUpdate(const EdgesParam& value) override;
    void OnAnchorUpdate(const OffsetT<Dimension>& value) override;

    void OnClipShapeUpdate(const RefPtr<BasicShape>& basicShape) override;
    void OnClipEdgeUpdate(bool isClip) override;
    void OnClipMaskUpdate(const RefPtr<BasicShape>& basicShape) override;

    void OnProgressMaskUpdate(const RefPtr<ProgressMaskProperty>& progress) override;

    void OnLinearGradientUpdate(const NG::Gradient& value) override;
    void OnSweepGradientUpdate(const NG::Gradient& value) override;
    void OnRadialGradientUpdate(const NG::Gradient& value) override;

    void OnFrontBrightnessUpdate(const Dimension& brightness) override;
    void OnFrontGrayScaleUpdate(const Dimension& grayScale) override;
    void OnFrontContrastUpdate(const Dimension& contrast) override;
    void OnFrontSaturateUpdate(const Dimension& saturate) override;
    void OnFrontSepiaUpdate(const Dimension& sepia) override;
    void OnFrontInvertUpdate(const InvertVariant& invert) override;
    void OnSystemBarEffectUpdate(bool systemBarEffect) override;
    void OnFrontHueRotateUpdate(float hueRotate) override;
    void OnFrontColorBlendUpdate(const Color& colorBlend) override;
    void OnLinearGradientBlurUpdate(const NG::LinearGradientBlurPara& blurPara) override;
    void OnMagnifierUpdate(const MagnifierParams& magnifierParams) override;
    void OnDynamicLightUpRateUpdate(const float rate) override;
    void OnDynamicDimDegreeUpdate(const float degree) override;
    void OnDynamicLightUpDegreeUpdate(const float degree) override;
    void OnBgDynamicBrightnessOptionUpdate(const std::optional<BrightnessOption>& brightnessOption) override;
    void OnFgDynamicBrightnessOptionUpdate(const std::optional<BrightnessOption>& brightnessOption) override;

    void OnOverlayTextUpdate(const OverlayOptions& overlay) override;
    void OnMotionPathUpdate(const MotionPathOption& motionPath) override;

    void OnLightPositionUpdate(const TranslateOptions& position) override;
    void OnLightIntensityUpdate(const float lightIntensity) override;
    void OnLightColorUpdate(const Color& lightColor) override;
    void OnLightIlluminatedUpdate(const uint32_t lightIlluminated) override;
    void OnIlluminatedBorderWidthUpdate(const Dimension& illuminatedBorderWidth) override;
    void OnBloomUpdate(const float bloomIntensity) override;

    void OnUseEffectUpdate(bool useEffect) override;
    void OnUseEffectTypeUpdate(EffectType effectType) override;
    bool GetStatusByEffectTypeAndWindow() override;
    void OnUseShadowBatchingUpdate(bool useShadowBatching) override;
    void OnFreezeUpdate(bool isFreezed) override;
    void OnRenderGroupUpdate(bool isRenderGroup) override;
    void OnSuggestedRenderGroupUpdate(bool isRenderGroup) override;
    void OnRenderFitUpdate(RenderFit renderFit) override;
    void OnNodeNameUpdate(const std::string& id) override;
    void OnAttractionEffectUpdate(const AttractionEffect& effect) override;
    void ReCreateRsNodeTree(const std::list<RefPtr<FrameNode>>& children);

    void SyncAdditionalGeometryProperties(const RectF& paintRect);
    void SetChildBounds(const RectF& paintRect) const;
    void NotifyTransitionInner(const SizeF& frameSize, bool isTransitionIn);
    void NotifyTransition(bool isTransitionIn);
    bool HasTransitionOutAnimation() const override
    {
        return disappearingTransitionCount_ > 0;
    }
    bool HasDisappearTransition() const override
    {
        return transitionEffect_ != nullptr && transitionEffect_->HasDisappearTransition();
    }
    bool HasValidBgImageResizable();
    void OnTransitionInFinish();
    void OnTransitionOutFinish();
    void RemoveDefaultTransition();
    void FireTransitionUserCallback(bool isTransitionIn);
    void PostTransitionUserOutCallback();
    void SetTransitionPivot(const SizeF& frameSize, bool transitionIn);
    void SetPivot(float xPivot, float yPivot, float zPivot = 0.0f);
    void SetPositionToRSNode();
    std::shared_ptr<Rosen::RSUIContext> GetRSUIContext(PipelineContext* pipeline);

    // Convert BorderRadiusProperty to Rosen::Vector4f
    static inline void ConvertRadius(const BorderRadiusProperty& value, Rosen::Vector4f& cornerRadius);

    void PaintBackground();
    void PaintClipShape(const std::unique_ptr<ClipProperty>& clip, const SizeF& frameSize);
    void PaintClipMask(const std::unique_ptr<ClipProperty>& clip, const SizeF& frameSize);
    void PaintClip(const SizeF& frameSize);
    void PaintProgressMask();
    void PaintGradient(const SizeF& frameSize);
    void PaintGraphics();
    void PaintOverlayText();
    void PaintBorderImage();
    float ConvertDimensionToPx(Dimension& src, float size);
    Rosen::ParticleParams ConvertParticleOptionToParams(const ParticleOption& particleOption, const RectF& rect);
    Rosen::EmitterConfig ConvertParticleEmitterOption(const EmitterOption& emitterOption, const RectF& rect);
    Rosen::ParticleVelocity ConvertParticleVelocityOption(const VelocityProperty& velocity);
    Rosen::ParticleVelocity ConvertParticleDefaultVelocityOption();
    Rosen::ParticleAcceleration ConvertParticleAccelerationOption(const AccelerationProperty& acceleration);
    Rosen::ParticleAcceleration ConvertParticleDefaultAccelerationOption();
    Rosen::ParticleColorParaType ConvertParticleColorOption(const ParticleColorPropertyOption& colorOption);
    Rosen::ParticleColorParaType ConvertParticleDefaultColorOption(
        std::optional<OHOS::Rosen::Range<OHOS::Rosen::RSColor>> rsInitRangeOpt);
    Rosen::ParticleParaType<float> ConvertParticleFloatOption(const ParticleFloatPropertyOption& floatOption);
    Rosen::ParticleParaType<float> ConvertParticleDefaultFloatOption(OHOS::Rosen::Range<float>& rsInitRange);
    bool NeedPreloadImage(const std::list<ParticleOption>& optionList, RectF& rect);
    void LoadParticleImage(const std::string& src, Dimension& width, Dimension& height);
    void OnParticleImageLoaded(const std::string& src, const RefPtr<CanvasImage> canvas);
    void SetRsParticleImage(std::shared_ptr<Rosen::RSImage>& rsImagePtr, std::string& imageSource);
    void PaintRSBgImage();
    void PaintPixmapBgImage();
    void PaintBorderImageGradient();
    void PaintMouseSelectRect(const RectF& rect, const Color& fillColor, const Color& strokeColor);
    void UpdateForeBlurStyleForColorMode(const std::optional<BlurStyleOption>& fgBlurStyle,
        const SysOptions& sysOptions);
    void SetBackBlurFilter();
    void SetFrontBlurFilter();
    bool UpdateBlurBackgroundColor(const std::optional<BlurStyleOption>& bgBlurStyle);
    bool UpdateBlurBackgroundColor(const std::optional<EffectOption>& efffectOption);
    void GetPaddingOfFirstFrameNodeParent(Dimension& parentPaddingLeft, Dimension& parentPaddingTop);
    void CombineMarginAndPosition(Dimension& resultX, Dimension& resultY, const Dimension& parentPaddingLeft,
        const Dimension& parentPaddingTop, float widthPercentReference, float heightPercentReference);
    OffsetF GetRectOffsetWithOffsetEdges(
        const EdgesParam& offsetEdges, float widthPercentReference, float heightPercentReference);
    OffsetF GetRectOffsetWithPositionEdges(
        const EdgesParam& positionEdges, float widthPercentReference, float heightPercentReference);

    void InitEventClickEffect();
    RefPtr<Curve> UpdatePlayAnimationValue(const ClickEffectLevel& level, float& scaleValue);
    void ClickEffectPlayAnimation(const TouchType& touchType);

    void SetSkipCheckInMultiInstance();

    // helper function to check if paint rect is valid
    bool RectIsNull();

    /** Set data to the modifier and bind it to rsNode_
     *   If [modifier] not initialized, initialize it and add it to rsNode
     *
     *   @param modifier     shared_ptr to a member modifier
     *   @param data         passed to SetCustomData, set to the modifier
     */
    template<typename T, typename D>
    void SetGraphicModifier(std::shared_ptr<T>& modifier, D data);

    void AddModifier(const std::shared_ptr<Rosen::ModifierNG::RSModifier>& modifier);
    void RemoveModifier(const std::shared_ptr<Rosen::ModifierNG::RSModifier>& modifier);

    // helper function to update one of the graphic effects
    template<typename T, typename D>
    void UpdateGraphic(std::shared_ptr<T>& modifier, D data);

    RectF AdjustPaintRect();

    DataReadyNotifyTask CreateBgImageDataReadyCallback();
    LoadSuccessNotifyTask CreateBgImageLoadSuccessCallback();
    DataReadyNotifyTask CreateBorderImageDataReadyCallback();
    LoadSuccessNotifyTask CreateBorderImageLoadSuccessCallback();
    void BdImagePaintTask(RSCanvas& canvas);

    void FlushImplicitTransaction();

    void RegisterDensityChangedCallback();

    bool IsUsingPosition(const RefPtr<FrameNode>& frameNode);

    void SetContentRectToFrame(RectF rect) override;

    float RoundValueToPixelGrid(float value);
    float RoundValueToPixelGrid(float value, bool isRound, bool forceCeil, bool forceFloor);
    float OnePixelValueRounding(float value);
    float OnePixelValueRounding(float value, bool isRound, bool forceCeil, bool forceFloor);
    void RoundToPixelGrid();
    void RoundToPixelGrid(bool isRound, uint16_t flag);
    void OnePixelRounding();
    void OnePixelRounding(uint16_t flag = 0);
    Matrix4 GetMatrix();
    bool IsUniRenderEnabled() override;
    void AddFrameNodeInfoToRsNode();
    // Use rect to update the drawRegion rect at index.
    void UpdateDrawRegion(uint32_t index, const std::shared_ptr<Rosen::RectF>& rect);
    void NotifyHostTransformUpdated(bool changed = true);
    void NotifyHostTransformUpdatedMultiThread(bool changed = true);
    void SetFrontBlurFilterMultiThread();
    void InitAccessibilityFocusModidifer(const RoundRect&, const Color&, float);
    void InitFocusStateModidifer(const RoundRect&, const Color&, float);
    void InitFocusAnimationModidifer(const RoundRect&, const Color&, float);
    std::shared_ptr<TransitionModifier> GetOrCreateTransitionModifier();
    std::shared_ptr<BackgroundModifier> GetOrCreateBackgroundModifier();

    std::shared_ptr<Rosen::RSNode> CreateHardwareSurface(const std::optional<ContextParam>& param,
        bool isTextureExportNode, std::shared_ptr<Rosen::RSUIContext>& rsUIContext);
#ifdef RENDER_EXTRACT_SUPPORTED
    std::shared_ptr<Rosen::RSNode> CreateHardwareTexture(
        const std::optional<ContextParam>& param, bool isTextureExportNode);
#endif
    void DetachModifiers();
    void MarkNeedDrawNode(bool condition);

    void OnEmitterPropertyUpdate();

    RefPtr<ImageLoadingContext> bgLoadingCtx_;
    RefPtr<CanvasImage> bgImage_;
    RefPtr<ImageLoadingContext> bdImageLoadingCtx_;
    RefPtr<CanvasImage> bdImage_;

    PatternType patternType_ = PatternType::DEFAULT;
    std::shared_ptr<Rosen::RSNode> rsNode_;
    bool isHdr_ = false;
    bool isHoveredScale_ = false;
    bool isHoveredBoard_ = false;
    bool firstTransitionIn_ = false;
    bool isBreakingPoint_ = false;
    bool isBackBlurChanged_ = false;
    bool needDebugBoundary_ = true;
    bool isDisappearing_ = false;
    bool hasDefaultTransition_ = false;
    bool measureTriggered_ = false;
    bool particleAnimationPlaying_ = false;
    bool hasScales_ = false;
    int appearingTransitionCount_ = 0;
    int disappearingTransitionCount_ = 0;
    int sandBoxCount_ = 0;
    uint32_t colorGamut_ = 0;
    static constexpr int32_t INVALID_PARENT_ID = -2100000;
    static constexpr uint32_t DRAW_REGION_RECT_COUNT = 8;
    std::map<std::string, RefPtr<ImageLoadingContext>> particleImageContextMap_;
    std::map<std::string, RefPtr<CanvasImage>> particleImageMap_;
    Color blendColor_ = Color::TRANSPARENT;
    Color hoveredColor_ = Color::TRANSPARENT;

    RefPtr<RosenTransitionEffect> transitionEffect_;
    std::function<void()> transitionOutCallback_;
    std::function<void()> transitionInCallback_;
    TransitionFinishCallback transitionUserCallback_;

    std::optional<OffsetF> frameOffset_;
    std::shared_ptr<Rosen::RectF> drawRegionRects_[DRAW_REGION_RECT_COUNT] = { nullptr };

    std::shared_ptr<DebugBoundaryModifier> debugBoundaryModifier_;
    std::shared_ptr<BackgroundModifier> backgroundModifier_;
    std::shared_ptr<TransitionModifier> transitionModifier_;
    std::shared_ptr<BorderImageModifier> borderImageModifier_;
    std::shared_ptr<MouseSelectModifier> mouseSelectModifier_;
    RefPtr<MoonProgressModifier> moonProgressModifier_;
    RefPtr<FocusAnimationModifier> focusAnimationModifier_;

    std::shared_ptr<FocusStateModifier> focusStateModifier_;
    std::shared_ptr<FocusStateModifier> accessibilityFocusStateModifier_;
    std::shared_ptr<OverlayTextModifier> overlayTextModifier_ = nullptr;
    std::shared_ptr<GradientStyleModifier> gradientStyleModifier_;

    std::shared_ptr<Rosen::ModifierNG::RSBoundsClipModifier> clipBoundModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSFrameClipModifier> customClipToFrameModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSMaskModifier> clipMaskModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> transformModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> pivotModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> sharedTransitionModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSBehindWindowFilterModifier> windowBlurModifier_;
    // translate, rotation, scale, alpha modifier for developer
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> translateXYUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> translateZUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> rotationXUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> rotationYUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> rotationZUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> cameraDistanceUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> scaleXYUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> scrollScaleModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSAlphaModifier> alphaUserModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSAlphaModifier> alphaModifier_;
    // translate modifiers for interruption
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> translateXYModifier_;
    // for page orientation feature.
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> baseTranslateInXYModifier_;
    std::shared_ptr<Rosen::ModifierNG::RSTransformModifier> baseRotateInZModifier_;
    // graphics modifiers
    struct GraphicModifiers {
        std::shared_ptr<GrayScaleModifier> grayScale;
        std::shared_ptr<BrightnessModifier> brightness;
        std::shared_ptr<ContrastModifier> contrast;
        std::shared_ptr<SaturateModifier> saturate;
        std::shared_ptr<SepiaModifier> sepia;
        std::shared_ptr<InvertModifier> invert;
        std::shared_ptr<HueRotateModifier> hueRotate;
        std::shared_ptr<ColorBlendModifier> colorBlend;
    };
    std::unique_ptr<GraphicModifiers> graphics_;

    RefPtr<TouchEventImpl> touchListener_;
    VectorF currentScale_ = VectorF(1.0f, 1.0f);
    // borderLeft borderTop borderRight borderBottom
    Rosen::Vector4f borderWidth_ = Rosen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    bool isTouchUpFinished_ = true;

    bool useContentRectForRSFrame_;
    bool adjustRSFrameByContentRect_ = false;
    bool isFocusBoxGlow_ = false;

    RectF paintRect_;
    // offset generated by layout node
    std::optional<OffsetF> extraOffset_;
    std::unique_ptr<RectF> contentClip_;

    std::shared_ptr<Rosen::RSTextureExport> rsTextureExport_;

    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_;
    std::function<void()> callbackAnimateEnd_ = nullptr;
    std::function<void()> callbackCachedAnimateAction_ = nullptr;
    bool isDraggingFlag_ = false;
    bool reDraggingFlag_ = false;
    PipelineContext* pipeline_;

    template <typename Modifier, RSPropertyType PropertyType, typename ValueType>
    friend class PropertyTransitionEffectTemplate;

    friend class RosenPivotTransitionEffect;

    ACE_DISALLOW_COPY_AND_MOVE(RosenRenderContext);

private:
    void ModifyCustomBackground();
    bool ShouldSkipAffineTransformation(std::shared_ptr<RSNode> rsNode);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PAINTS_ADAPTER_ROSEN_RENDER_CONTEXT_H
