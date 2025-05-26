/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/common/display_info_utils.h"

#include "display_manager.h"

namespace OHOS::Ace {
constexpr uint64_t DEFAULT_DISPLAY_ID = 0;
RefPtr<DisplayInfo> DisplayInfoUtils::GetDisplayInfo(int32_t displayId)
{
    auto displayManager = Rosen::DisplayManager::GetInstance().GetDisplayById(displayId);
    if (!displayManager) {
        displayManager = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    }
    CHECK_NULL_RETURN(displayManager, nullptr);
    displayInfo_->SetWidth(displayManager->GetWidth());
    displayInfo_->SetHeight(displayManager->GetHeight());
    displayInfo_->SetDisplayId(displayManager->GetId());
    auto dmRotation = displayManager->GetRotation();
    displayInfo_->SetRotation(static_cast<Rotation>(static_cast<uint32_t>(dmRotation)));
    GetIsFoldable();
    GetCurrentFoldStatus();
    GetCurrentFoldCreaseRegion();
    return displayInfo_;
}

void DisplayInfoUtils::InitIsFoldable()
{
    auto isFoldable = Rosen::DisplayManager::GetInstance().IsFoldable();
    displayInfo_->SetIsFoldable(isFoldable);
}

bool DisplayInfoUtils::GetIsFoldable()
{
    if (hasInitIsFoldable_) {
        return displayInfo_->GetIsFoldable();
    }
    auto isFoldable = Rosen::DisplayManager::GetInstance().IsFoldable();
    displayInfo_->SetIsFoldable(isFoldable);
    hasInitIsFoldable_ = true;
    return isFoldable;
}

FoldStatus DisplayInfoUtils::GetCurrentFoldStatus()
{
    auto dmFoldStatus = Rosen::DisplayManager::GetInstance().GetFoldStatus();
    displayInfo_->SetFoldStatus(static_cast<FoldStatus>(static_cast<uint32_t>(dmFoldStatus)));
    return displayInfo_->GetFoldStatus();
}

std::vector<Rect> DisplayInfoUtils::GetCurrentFoldCreaseRegion()
{
    if (hasInitFoldCreaseRegion_) {
        return displayInfo_->GetCurrentFoldCreaseRegion();
    }
    std::vector<Rect> rects;
    auto foldCreaseRegion = Rosen::DisplayManager::GetInstance().GetCurrentFoldCreaseRegion();
    if (!foldCreaseRegion) {
        TAG_LOGW(AceLogTag::ACE_OVERLAY, "failed to get foldCreaseRegion");
        return rects;
    }

    auto creaseRects = foldCreaseRegion->GetCreaseRects();
    if (creaseRects.empty()) {
        return rects;
    }

    for (const auto& item : creaseRects) {
        Rect rect;
        rect.SetRect(item.posX_, item.posY_, item.width_, item.height_);
        rects.insert(rects.end(), rect);
    }
    displayInfo_->SetCurrentFoldCreaseRegion(rects);
    hasInitFoldCreaseRegion_ = true;
    return rects;
}

Rect DisplayInfoUtils::GetDisplayAvailableRect(int32_t displayId) const
{
    auto display = Rosen::DisplayManager::GetInstance().GetDisplayById(displayId);
    if (!display) {
        TAG_LOGW(AceLogTag::ACE_WINDOW, "failed to get display by id: %{public}u", (uint32_t)displayId);
        return Rect();
    }

    Rosen::DMRect availableArea;
    Rosen::DMError ret = display->GetAvailableArea(availableArea);
    if (ret != Rosen::DMError::DM_OK) {
        TAG_LOGW(AceLogTag::ACE_WINDOW, "failed to get availableArea of displayId: %{public}u", (uint32_t)displayId);
        return Rect();
    }

    return Rect(availableArea.posX_, availableArea.posY_, availableArea.width_, availableArea.height_);
}

Rect DisplayInfoUtils::GetFoldExpandAvailableRect() const
{
    Rosen::DMRect rect;
    Rosen::DMError ret = Rosen::DisplayManager::GetInstance().GetExpandAvailableArea(DEFAULT_DISPLAY_ID, rect);
    if (ret != Rosen::DMError::DM_OK) {
        TAG_LOGW(AceLogTag::ACE_WINDOW, "failed to get expandAvailableArea");
        return Rect();
    }

    return Rect(rect.posX_, rect.posY_, rect.width_, rect.height_);
}
} // namespace OHOS::Ace::DisplayInfoUtils
