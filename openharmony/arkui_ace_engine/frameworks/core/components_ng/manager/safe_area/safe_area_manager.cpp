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
#include "safe_area_manager.h"

#include "core/components/container_modal/container_modal_constants.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"

namespace OHOS::Ace::NG {
SafeAreaInsets GenerateCutOutAreaWithRoot(const SafeAreaInsets& safeArea, NG::OptionalSize<uint32_t> rootSize)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, {});
    CHECK_NULL_RETURN(pipeline->GetUseCutout(), {});
    // cutout regions adjacent to edges.
    auto cutoutArea = safeArea;

    if (cutoutArea.top_.IsValid()) {
        cutoutArea.top_.start = 0;
    }
    if (safeArea.bottom_.IsValid()) {
        cutoutArea.bottom_.end = rootSize.Height().has_value() ? rootSize.Height().value()
                                                               : PipelineContext::GetCurrentRootHeight();
    }
    if (cutoutArea.left_.IsValid()) {
        cutoutArea.left_.start = 0;
    }
    if (cutoutArea.right_.IsValid()) {
        cutoutArea.right_.end = rootSize.Width().has_value() ? rootSize.Width().value()
                                                             : PipelineContext::GetCurrentRootWidth();
    }
    return cutoutArea;
}

bool SafeAreaManager::CheckCutoutSafeArea(const SafeAreaInsets& safeArea, NG::OptionalSize<uint32_t> rootSize)
{
    return cutoutSafeArea_ != GenerateCutOutAreaWithRoot(safeArea, rootSize);
}

bool SafeAreaManager::UpdateCutoutSafeArea(const SafeAreaInsets& safeArea, NG::OptionalSize<uint32_t> rootSize)
{
    auto safeAreaWithRoot = GenerateCutOutAreaWithRoot(safeArea, rootSize);
    if (cutoutSafeArea_ == safeAreaWithRoot) {
        return false;
    }
    ACE_SCOPED_TRACE("SafeAreaManager::UpdateCutoutSafeArea %s, safeAreaWithRoot %s", safeArea.ToString().c_str(),
        safeAreaWithRoot.ToString().c_str());
    cutoutSafeArea_ = safeAreaWithRoot;
    return true;
}

bool SafeAreaManager::CheckSystemSafeArea(const SafeAreaInsets& safeArea)
{
    return systemSafeArea_ != safeArea;
}

bool SafeAreaManager::UpdateSystemSafeArea(const SafeAreaInsets& safeArea)
{
    if (systemSafeArea_ == safeArea) {
        return false;
    }
    ACE_SCOPED_TRACE("SafeAreaManager::UpdateSystemSafeArea %s", safeArea.ToString().c_str());
    systemSafeArea_ = safeArea;
    return true;
}

bool SafeAreaManager::CheckNavArea(const SafeAreaInsets& safeArea)
{
    return navSafeArea_ != safeArea;
}

bool SafeAreaManager::UpdateNavArea(const SafeAreaInsets& safeArea)
{
    if (navSafeArea_ == safeArea) {
        return false;
    }
    ACE_SCOPED_TRACE("SafeAreaManager::UpdateNavArea %s", safeArea.ToString().c_str());
    navSafeArea_ = safeArea;
    return true;
}

bool SafeAreaManager::UpdateKeyboardSafeArea(float keyboardHeight, std::optional<uint32_t> rootHeight)
{
    uint32_t bottom;
    if (systemSafeArea_.bottom_.IsValid()) {
        bottom = systemSafeArea_.bottom_.start;
    } else {
        bottom = rootHeight.has_value() ? rootHeight.value() : PipelineContext::GetCurrentRootHeight();
    }
    SafeAreaInsets::Inset inset = { .start = bottom - keyboardHeight, .end = bottom };
    if (inset == keyboardInset_) {
        return false;
    }
    keyboardInset_ = inset;
    return true;
}

SafeAreaInsets SafeAreaManager::GetCombinedSafeArea(const SafeAreaExpandOpts& opts) const
{
    SafeAreaInsets res;
    if (!IsSafeAreaValid()) {
        return {};
    }
    if (opts.type & SAFE_AREA_TYPE_CUTOUT) {
        res = res.Combine(cutoutSafeArea_);
    }
    if (opts.type & SAFE_AREA_TYPE_SYSTEM) {
        res = res.Combine(systemSafeArea_).Combine(navSafeArea_);
    }
    if (keyboardSafeAreaEnabled_ && (opts.type & SAFE_AREA_TYPE_KEYBOARD)) {
        res.bottom_ = res.bottom_.Combine(keyboardInset_);
    }
    return res;
}

bool SafeAreaManager::IsSafeAreaValid() const
{
#ifdef PREVIEW
    return !ignoreSafeArea_;
#else
    return !(ignoreSafeArea_ || (!isFullScreen_ && !isNeedAvoidWindow_));
#endif
}

bool SafeAreaManager::SetIsFullScreen(bool value)
{
    if (isFullScreen_ == value) {
        return false;
    }
    isFullScreen_ = value;
    TAG_LOGI(ACE_LAYOUT, "SetIsFullScreen %{public}d", isFullScreen_);
    return true;
}

bool SafeAreaManager::SetIsNeedAvoidWindow(bool value)
{
    if (isNeedAvoidWindow_ == value) {
        return false;
    }
    isNeedAvoidWindow_ = value;
    TAG_LOGI(ACE_LAYOUT, "SetIsNeedAvoidWindow %{public}d", isNeedAvoidWindow_);
    return true;
}

bool SafeAreaManager::SetIgnoreSafeArea(bool value)
{
    if (ignoreSafeArea_ == value) {
        return false;
    }
    ignoreSafeArea_ = value;
    TAG_LOGI(ACE_LAYOUT, "SetIgnoreSafeArea %{public}d", ignoreSafeArea_);
    return true;
}

bool SafeAreaManager::SetKeyBoardAvoidMode(bool value)
{
    if (keyboardSafeAreaEnabled_ == value) {
        return false;
    }
    keyboardSafeAreaEnabled_ = value;
    TAG_LOGI(ACE_LAYOUT, "SetKeyBoardAvoidMode %{public}d", keyboardSafeAreaEnabled_);
    return true;
}

bool SafeAreaManager::SetIsAtomicService(bool value)
{
    if (isAtomicService_ == value) {
        return false;
    }
    isAtomicService_ = value;
    TAG_LOGI(ACE_LAYOUT, "SetIsAtomicService %{public}d", isAtomicService_);
    return true;
}

bool SafeAreaManager::IsAtomicService() const
{
    return isAtomicService_;
}

SafeAreaInsets SafeAreaManager::GetSystemSafeArea() const
{
    return systemSafeArea_;
}

SafeAreaInsets SafeAreaManager::GetCutoutSafeArea() const
{
    if (!IsSafeAreaValid()) {
        return {};
    }
    return cutoutSafeArea_;
}

SafeAreaInsets SafeAreaManager::GetSafeArea() const
{
    if (!IsSafeAreaValid()) {
        return {};
    }
    return systemSafeArea_.Combine(cutoutSafeArea_).Combine(navSafeArea_);
}

SafeAreaInsets SafeAreaManager::GetSafeAreaWithoutCutout() const
{
    if (!IsSafeAreaValid()) {
        return {};
    }
    return systemSafeArea_.Combine(navSafeArea_);
}

SafeAreaInsets SafeAreaManager::GetSafeAreaWithoutProcess() const
{
    return systemSafeArea_.Combine(cutoutSafeArea_).Combine(navSafeArea_);
}

PaddingPropertyF SafeAreaManager::SafeAreaToPadding(bool withoutProcess)
{
    if (!withoutProcess) {
#ifdef PREVIEW
        if (ignoreSafeArea_) {
            return {};
        }
#else
        if (ignoreSafeArea_ || (!isFullScreen_ && !isNeedAvoidWindow_)) {
            return {};
        }
#endif
    }
    auto combinedSafeArea = systemSafeArea_.Combine(cutoutSafeArea_).Combine(navSafeArea_);
    PaddingPropertyF result;
    if (combinedSafeArea.left_.IsValid()) {
        result.left = combinedSafeArea.left_.Length();
    }
    if (combinedSafeArea.top_.IsValid()) {
        result.top = combinedSafeArea.top_.Length();
    }
    if (combinedSafeArea.right_.IsValid()) {
        result.right = combinedSafeArea.right_.Length();
    }
    if (combinedSafeArea.bottom_.IsValid()) {
        result.bottom = combinedSafeArea.bottom_.Length();
    }
    return result;
}

float SafeAreaManager::GetKeyboardOffset() const
{
    if (keyboardSafeAreaEnabled_) {
        return 0.0f;
    }
    return keyboardOffset_;
}

OffsetF SafeAreaManager::GetWindowWrapperOffset()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipelineContext, OffsetF());
    auto windowManager = pipelineContext->GetWindowManager();
    auto isContainerModal = pipelineContext->GetWindowModal() == WindowModal::CONTAINER_MODAL && windowManager &&
                            windowManager->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING;
    if (isContainerModal) {
        auto wrapperOffset = OffsetF(static_cast<float>((CONTAINER_BORDER_WIDTH + CONTENT_PADDING).ConvertToPx()),
            static_cast<float>((pipelineContext->GetCustomTitleHeight() + CONTAINER_BORDER_WIDTH).ConvertToPx()));
        return wrapperOffset;
    }
    return OffsetF();
}

void SafeAreaManager::ExpandSafeArea()
{
    ACE_SCOPED_TRACE("ExpandSafeArea node count %zu, IsSafeAreaValid: %d, ignoreSafeArea: %d, isFullScreen: %d, "
                     "isNeedAvoidWindow %d",
        needExpandNodes_.size(), IsSafeAreaValid(), ignoreSafeArea_, isFullScreen_, isNeedAvoidWindow_);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto manager = pipeline->GetSafeAreaManager();
    auto iter = needExpandNodes_.begin();
    while (iter != needExpandNodes_.end()) {
        auto frameNode = (*iter).Upgrade();
        if (frameNode) {
            manager->AddGeoRestoreNode(frameNode);
            frameNode->ExpandSafeArea();
        }
        ++iter;
    }
    ClearNeedExpandNode();
}

bool SafeAreaManager::AddNodeToExpandListIfNeeded(const WeakPtr<FrameNode>& node)
{
    if (needExpandNodes_.find(node) == needExpandNodes_.end()) {
        AddNeedExpandNode(node);
        return true;
    }
    return false;
}

std::vector<WeakPtr<FrameNode>> SafeAreaManager::GetExpandNodeSet()
{
    // To isolate set comparator, use vector to collect a copy of nodes
    std::vector<WeakPtr<FrameNode>> result;
    std::copy(needExpandNodes_.begin(), needExpandNodes_.end(), std::back_inserter(result));
    return result;
}

bool SafeAreaManager::CheckPageNeedAvoidKeyboard(const RefPtr<FrameNode>& frameNode)
{
    if (frameNode->GetTag() != V2::PAGE_ETS_TAG) {
        return false;
    }
    // page will not avoid keyboard when lastChild is sheet
    RefPtr<OverlayManager> overlay;
    if (frameNode->RootNodeIsPage()) {
        auto pattern = frameNode->GetPattern<PagePattern>();
        CHECK_NULL_RETURN(pattern, true);
        overlay = pattern->GetOverlayManager();
    } else {
        auto navNode = FrameNode::GetFrameNode(V2::NAVDESTINATION_VIEW_ETS_TAG, frameNode->GetRootNodeId());
        CHECK_NULL_RETURN(navNode, true);
        auto pattern = navNode->GetPattern<NavDestinationPattern>();
        CHECK_NULL_RETURN(pattern, true);
        overlay = pattern->GetOverlayManager();
    }
    CHECK_NULL_RETURN(overlay, true);
    return overlay->CheckPageNeedAvoidKeyboard();
}
} // namespace OHOS::Ace::NG
