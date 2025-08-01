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

#include "test/mock/core/common/mock_container.h"
#include "test/mock/interfaces/ipc_single/iremote_object.h"

#include "core/common/ace_engine.h"
#include "core/common/container.h"

namespace OHOS::Ace {
namespace {
int32_t g_id = 0;
} // namespace

RefPtr<MockContainer> MockContainer::container_;
ColorMode MockContainer::mockColorMode_ = ColorMode::LIGHT;
bool Frontend::MaybeRelease()
{
    return AceType::MaybeRelease();
}

Frontend::~Frontend() = default;

int32_t Container::CurrentId()
{
    return g_id;
}

int32_t Container::SafelyId()
{
    return g_id;
}

int32_t Container::CurrentIdSafely()
{
    return g_id;
}

int32_t Container::CurrentIdSafelyWithCheck()
{
    return g_id;
}

RefPtr<Container> Container::GetActive()
{
    RefPtr<Container> activeContainer;
    return activeContainer;
}

RefPtr<TaskExecutor> Container::CurrentTaskExecutor()
{
    return MockContainer::Current()->GetTaskExecutor();
}

RefPtr<TaskExecutor> Container::CurrentTaskExecutorSafely()
{
    return MockContainer::Current()->GetTaskExecutor();
}

RefPtr<TaskExecutor> Container::CurrentTaskExecutorSafelyWithCheck()
{
    return MockContainer::Current()->GetTaskExecutor();
}

void Container::UpdateCurrent(int32_t id)
{
    g_id = id;
}

RefPtr<Container> Container::Current()
{
    return MockContainer::Current();
}

RefPtr<Container> Container::CurrentSafely()
{
    return MockContainer::Current();
}

RefPtr<Container> Container::CurrentSafelyWithCheck()
{
    return MockContainer::Current();
}

bool Container::Dump(const std::vector<std::string>& /* params */, std::vector<std::string>& /* info */)
{
    return true;
}

void MockContainer::SetUp()
{
    container_ = AceType::MakeRefPtr<MockContainer>();
}

void MockContainer::SetUp(RefPtr<PipelineBase> pipelineContext)
{
    container_ = AceType::MakeRefPtr<MockContainer>(pipelineContext);
}

void MockContainer::TearDown()
{
    container_ = nullptr;
}

RefPtr<MockContainer> MockContainer::Current()
{
    return container_;
}

RefPtr<Container> Container::GetContainer(int32_t containerId)
{
    return MockContainer::Current();
}

ColorMode Container::CurrentColorMode()
{
    return MockContainer::mockColorMode_;
}

std::string Container::CurrentBundleName()
{
    return "";
}

void MockContainer::SetMockColorMode(ColorMode mode)
{
    mockColorMode_ = mode;
}

ColorMode MockContainer::GetMockColorMode()
{
    return mockColorMode_;
}

int32_t MockContainer::RequestAutoFill(const RefPtr<NG::FrameNode>& node, AceAutoFillType autoFillType,
    bool isNewPassWord, bool& isPopup, uint32_t& autoFillSessionId, bool isNative,
    const std::function<void()>& onFinish, const std::function<void()>& onUIExtNodeBindingCompleted)
{
    if (autoFillType == AceAutoFillType::ACE_USER_NAME) {
        isPopup = true; // if TextInputType::USER_NAME
    } else if (autoFillType == AceAutoFillType::ACE_PASSWORD || autoFillType == AceAutoFillType::ACE_NEW_PASSWORD) {
        isPopup = false; // if TextInputType::VISIBLE_PASSWORD or TextInputType::NEW_PASSWORD
    }
    if (autoFillType == AceAutoFillType::ACE_PASSWORD) {
        return AceAutoFillError::ACE_AUTO_FILL_SUCCESS; // cover DoProcessAutoFill
    }
    return isPopup ? AceAutoFillError::ACE_AUTO_FILL_SUCCESS : AceAutoFillError::ACE_AUTO_FILL_DEFAULT;
}

void MockContainer::SetDisplayInfo(RefPtr<DisplayInfo> displayInfo)
{
    displayInfo_ = displayInfo;
}

RefPtr<DisplayInfo> Container::GetDisplayInfo()
{
    return MockContainer::Current()->GetMockDisplayInfo();
}

void Container::InitIsFoldable() {}

bool Container::IsFoldable()
{
    return MockContainer::Current()->GetMockDisplayInfo()->GetIsFoldable();
}

RefPtr<Container> Container::GetByWindowId(uint32_t windowId)
{
    return MockContainer::Current();
}

FoldStatus Container::GetCurrentFoldStatus()
{
    return MockContainer::Current()->GetMockDisplayInfo()->GetFoldStatus();
}

std::vector<Rect> Container::GetCurrentFoldCreaseRegion()
{
    return {};
}

RefPtr<Container> Container::GetFocused()
{
    RefPtr<Container> focusContainer;
    AceEngine::Get().NotifyContainers([&focusContainer](const RefPtr<Container>& container) {
        auto pipeline = container->GetPipelineContext();
        if (pipeline && pipeline->IsWindowFocused()) {
            focusContainer = container;
        }
    });
    return focusContainer;
}

bool Container::IsNodeInKeyGuardWindow(const RefPtr<NG::FrameNode>& node)
{
    return false;
}

NG::SafeAreaInsets Container::GetKeyboardSafeArea()
{
    return {};
}

bool Container::LessThanAPIVersion(PlatformVersion version)
{
    return static_cast<int32_t>(version) < 15
               ? PipelineBase::GetCurrentContext() &&
                     PipelineBase::GetCurrentContext()->GetMinPlatformVersion() < static_cast<int32_t>(version)
               : LessThanAPITargetVersion(version);
}
bool Container::GreatOrEqualAPIVersion(PlatformVersion version)
{
    return static_cast<int32_t>(version) < 15
               ? PipelineBase::GetCurrentContext() &&
                     PipelineBase::GetCurrentContext()->GetMinPlatformVersion() >= static_cast<int32_t>(version)
               : GreatOrEqualAPITargetVersion(version);
}

bool Container::LessThanAPIVersionWithCheck(PlatformVersion version)
{
    return static_cast<int32_t>(version) < 14
               ? PipelineBase::GetCurrentContextSafelyWithCheck() &&
                     PipelineBase::GetCurrentContextSafelyWithCheck()->GetMinPlatformVersion() <
                         static_cast<int32_t>(version)
               : LessThanAPITargetVersion(version);
}

bool Container::GreatOrEqualAPIVersionWithCheck(PlatformVersion version)
{
    return static_cast<int32_t>(version) < 14
               ? PipelineBase::GetCurrentContextSafelyWithCheck() &&
                     PipelineBase::GetCurrentContextSafelyWithCheck()->GetMinPlatformVersion() >=
                         static_cast<int32_t>(version)
               : GreatOrEqualAPITargetVersion(version);
}

sptr<IRemoteObject> Container::GetToken()
{
    return nullptr;
}
} // namespace OHOS::Ace
