/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "base/log/dump_log.h"
#if defined(ACE_STATIC)
#include "base/utils/multi_thread.h"
#endif
#include "core/components/progress/progress_theme.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_pattern.h"

namespace OHOS::Ace::NG {

bool LoadingProgressPattern::OnDirtyLayoutWrapperSwap(
    const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout)
{
    if (skipMeasure || dirty->SkipMeasureContent()) {
        return false;
    }
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);

    auto lodingprogressLayoutAlgorithm =
        DynamicCast<LoadingProgressLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(lodingprogressLayoutAlgorithm, false);
    return true;
}

void LoadingProgressPattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
#if defined(ACE_STATIC)
    THREAD_SAFE_NODE_CHECK(host, OnAttachToFrameNode);
#endif
    host->GetRenderContext()->SetClipToFrame(true);
    host->GetRenderContext()->SetClipToBounds(true);
    RegisterVisibleAreaChange();
}

void LoadingProgressPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
#if defined(ACE_STATIC)
    THREAD_SAFE_NODE_CHECK(frameNode, OnDetachFromFrameNode, frameNode);
#endif
    auto pipeline = PipelineContext::GetCurrentContextSafely();
    CHECK_NULL_VOID(pipeline);
    pipeline->RemoveVisibleAreaChangeNode(frameNode->GetId());
    pipeline->RemoveWindowStateChangedCallback(frameNode->GetId());
    hasVisibleChangeRegistered_ = false;
}

#if defined(ACE_STATIC)
void LoadingProgressPattern::OnAttachToMainTree()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    THREAD_SAFE_NODE_CHECK(host, OnAttachToMainTree);
}

void LoadingProgressPattern::OnDetachFromMainTree()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    THREAD_SAFE_NODE_CHECK(host, OnDetachFromMainTree);
}
#endif

void LoadingProgressPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    FireBuilder();
    auto paintProperty = GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    enableLoading_ = paintProperty->GetEnableLoadingValue(true);
    enableLoading_ ? StartAnimation() : StopAnimation();
    InitFocusEvent();
}

void LoadingProgressPattern::OnVisibleChange(bool isVisible)
{
    isVisible_ = isVisible;
    isVisible_ ? StartAnimation() : StopAnimation();
}

void LoadingProgressPattern::StartAnimation()
{
    CHECK_NULL_VOID(loadingProgressModifier_);
    if (loadingProgressModifier_->GetVisible()) {
        return;
    }
    if (isVisibleArea_ && isVisible_ && isShow_ && enableLoading_) {
        loadingProgressModifier_->SetVisible(true);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
}

void LoadingProgressPattern::StopAnimation()
{
    CHECK_NULL_VOID(loadingProgressModifier_);
    if (!loadingProgressModifier_->GetVisible()) {
        return;
    }
    loadingProgressModifier_->SetVisible(false);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void LoadingProgressPattern::RegisterVisibleAreaChange()
{
    if (hasVisibleChangeRegistered_) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto callback = [weak = WeakClaim(this)](bool visible, double ratio) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (visible) {
            pattern->isVisibleArea_ = true;
            pattern->StartAnimation();
        } else {
            pattern->isVisibleArea_ = false;
            pattern->StopAnimation();
        }
    };
    std::vector<double> ratioList = {0.0};
    pipeline->AddVisibleAreaChangeNode(host, ratioList, callback, false, true);
    pipeline->AddWindowStateChangedCallback(host->GetId());
    hasVisibleChangeRegistered_ = true;
}

void LoadingProgressPattern::OnWindowHide()
{
    isShow_ = false;
    StopAnimation();
}

void LoadingProgressPattern::OnWindowShow()
{
    isShow_ = true;
    StartAnimation();
}

void LoadingProgressPattern::DumpInfo()
{
    DumpLog::GetInstance().AddDesc(std::string("IsInVisibleArea: ").append(isVisibleArea_ ? "true" : "false"));
}

void LoadingProgressPattern::FireBuilder()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (!makeFunc_.has_value()) {
        host->RemoveChildAtIndex(0);
        host->GetRenderContext()->SetClipToFrame(true);
        host->GetRenderContext()->SetClipToBounds(true);
        host->MarkNeedFrameFlushDirty(PROPERTY_UPDATE_MEASURE);
        return;
    }
    auto node = BuildContentModifierNode();
    if (contentModifierNode_ == node) {
        return;
    }
    host->GetRenderContext()->SetClipToFrame(false);
    host->GetRenderContext()->SetClipToBounds(false);
    host->RemoveChildAndReturnIndex(contentModifierNode_);
    contentModifierNode_ = node;
    CHECK_NULL_VOID(contentModifierNode_);
    host->AddChild(contentModifierNode_, 0);
    host->MarkNeedFrameFlushDirty(PROPERTY_UPDATE_MEASURE);
}

RefPtr<FrameNode> LoadingProgressPattern::BuildContentModifierNode()
{
    if (!makeFunc_.has_value()) {
        return nullptr;
    }
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto eventHub = host->GetOrCreateEventHub<EventHub>();
    CHECK_NULL_RETURN(eventHub, nullptr);
    auto enabled = eventHub->IsEnabled();
    auto paintProperty = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_RETURN(paintProperty, nullptr);
    auto enableLoading = paintProperty->GetEnableLoadingValue(true);
    LoadingProgressConfiguration loadingProgressConfiguration(enableLoading, enabled);
    return (makeFunc_.value())(loadingProgressConfiguration);
}

void LoadingProgressPattern::DumpInfo(std::unique_ptr<JsonValue>& json)
{
    CHECK_NULL_VOID(json);
    json->Put("IsInVisibleArea", isVisibleArea_ ? "true" : "false");
}

void LoadingProgressPattern::InitThemeValues()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto progressTheme = pipeline->GetTheme<ProgressTheme>(host->GetThemeScopeId());
    CHECK_NULL_VOID(progressTheme);

    defaultColor_ = progressTheme->GetLoadingColor();
    focusedColor_ = progressTheme->GetLoadingFocusedColor();
}

void LoadingProgressPattern::InitFocusEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto focusHub = host->GetOrCreateFocusHub();
    auto focusTask = [weak = WeakClaim(this)](FocusReason reason) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleFocusEvent();
    };
    focusHub->SetOnFocusInternal(focusTask);
    auto blurTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleBlurEvent();
    };
    focusHub->SetOnBlurInternal(blurTask);
}

void LoadingProgressPattern::HandleFocusEvent()
{
    SetFocusStyle();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (pipeline->GetIsFocusActive()) {
        SetFocusStyle();
    }
    AddIsFocusActiveUpdateEvent();
}

void LoadingProgressPattern::HandleBlurEvent()
{
    ClearFocusStyle();
    RemoveIsFocusActiveUpdateEvent();
}

void LoadingProgressPattern::SetFocusStyle()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_VOID(paintProperty);

    if (paintProperty->GetColorValue(defaultColor_) == defaultColor_) {
        paintProperty->UpdateColor(focusedColor_);
        isFocusColorSet_ = true;
    }

    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void LoadingProgressPattern::ClearFocusStyle()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_VOID(paintProperty);

    if (isFocusColorSet_) {
        paintProperty->UpdateColor(defaultColor_);
        isFocusColorSet_ = false;
    }

    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void LoadingProgressPattern::AddIsFocusActiveUpdateEvent()
{
    if (!isFocusActiveUpdateEvent_) {
        isFocusActiveUpdateEvent_ = [weak = WeakClaim(this)](bool isFocusAcitve) {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            isFocusAcitve ? pattern->SetFocusStyle() : pattern->ClearFocusStyle();
        };
    }

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipline = host->GetContext();
    CHECK_NULL_VOID(pipline);
    pipline->AddIsFocusActiveUpdateEvent(GetHost(), isFocusActiveUpdateEvent_);
}

void LoadingProgressPattern::RemoveIsFocusActiveUpdateEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipline = host->GetContext();
    CHECK_NULL_VOID(pipline);
    pipline->RemoveIsFocusActiveUpdateEvent(GetHost());
}

bool LoadingProgressPattern::OnThemeScopeUpdate(int32_t themeScopeId)
{
    bool result = false;
    auto host = GetHost();
    CHECK_NULL_RETURN(host, result);
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, result);
    auto progressTheme = pipeline->GetTheme<ProgressTheme>(host->GetThemeScopeId());
    CHECK_NULL_RETURN(progressTheme, result);
    auto paintProperty = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_RETURN(paintProperty, result);

    result = !paintProperty->HasColor();

    if (themeScopeId && !colorLock_) {
        paintProperty->UpdateColor(progressTheme->GetLoadingColor());
        result = true;
    }
    return result;
}

void LoadingProgressPattern::UpdateColor(const Color& color, bool isFirstLoad)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto pipelineContext = host->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    if (isFirstLoad || pipelineContext->IsSystmColorChange()) {
        paintProperty->UpdateColor(color);
        renderContext->UpdateForegroundColor(color);
        renderContext->ResetForegroundColorStrategy();
        renderContext->UpdateForegroundColorFlag(true);
    }
    if (host->GetRerenderable()) {
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
}
void LoadingProgressPattern::OnColorConfigurationUpdate()
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<ProgressTheme>();
    CHECK_NULL_VOID(theme);
    auto pops = host->GetPaintProperty<LoadingProgressPaintProperty>();
    CHECK_NULL_VOID(pops);
    if (!pops->HasColorSetByUser() || (pops->HasColorSetByUser() && !pops->GetColorSetByUserValue())) {
        if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TEN)) {
            Color progressColor = theme->GetLoadingColor();
            UpdateColor(progressColor);
        }
    }
}
} // namespace OHOS::Ace::NG
