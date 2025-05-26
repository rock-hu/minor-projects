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

#include "core/components_ng/pattern/dialog/alert_dialog_model_ng.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/common/ace_engine.h"
#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/pattern/dialog/dialog_pattern.h"
#include "core/components_ng/pattern/overlay/dialog_manager.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "interfaces/inner_api/ui_session/ui_session_manager.h"

namespace OHOS::Ace::NG {
void AlertDialogModelNG::SetParseButtonObj(
    std::function<void()>&& eventFunc, ButtonInfo& buttonInfo, DialogProperties& arg, const std::string& property)
{
    auto func = eventFunc;
    auto eventFuncNg = [func](GestureEvent& /* info */) { func(); };
    buttonInfo.action = AceType::MakeRefPtr<NG::ClickEvent>(std::move(eventFuncNg));
}

void AlertDialogModelNG::SetOnCancel(std::function<void()>&& eventFunc, DialogProperties& arg)
{
    arg.onCancel = eventFunc;
}

void AlertDialogModelNG::SetOnWillDismiss(std::function<void(const int32_t& info,
    const int32_t& instanceId)>&& onWillDismissFunc, DialogProperties& arg)
{
    arg.onWillDismiss = std::move(onWillDismissFunc);
}

void AlertDialogModelNG::SetShowDialog(const DialogProperties& arg)
{
    auto container = Container::Current();
    CHECK_NULL_VOID(container);

    auto executor = container->GetTaskExecutor();
    CHECK_NULL_VOID(executor);
    executor->PostTask(
        [currentId = Container::CurrentId(), arg]() mutable {
            auto container = AceEngine::Get().GetContainer(currentId);
            CHECK_NULL_VOID(container);
            if (container->IsSubContainer()) {
                currentId = SubwindowManager::GetInstance()->GetParentContainerId(currentId);
                container = AceEngine::Get().GetContainer(currentId);
                CHECK_NULL_VOID(container);
            }
            ContainerScope scope(currentId);
            RefPtr<NG::FrameNode> dialog;
            auto pipelineContext = container->GetPipelineContext();
            CHECK_NULL_VOID(pipelineContext);
            auto context = AceType::DynamicCast<NG::PipelineContext>(pipelineContext);
            CHECK_NULL_VOID(context);
            auto overlayManager = context->GetOverlayManager();
            CHECK_NULL_VOID(overlayManager);
            if (arg.dialogLevelMode == LevelMode::EMBEDDED) {
                auto embeddedOverlay = NG::DialogManager::GetEmbeddedOverlay(arg.dialogLevelUniqueId, context);
                if (embeddedOverlay) {
                    overlayManager = embeddedOverlay;
                }
            }
            if (arg.isShowInSubWindow) {
                dialog = SubwindowManager::GetInstance()->ShowDialogNG(arg, nullptr);
                CHECK_NULL_VOID(dialog);
                if (arg.isModal && !container->IsUIExtensionWindow()) {
                    DialogProperties Maskarg;
                    Maskarg.isMask = true;
                    Maskarg.autoCancel = arg.autoCancel;
                    Maskarg.onWillDismiss = arg.onWillDismiss;
                    Maskarg.shadow = arg.shadow;
                    auto mask = overlayManager->ShowDialog(Maskarg, nullptr, false);
                    CHECK_NULL_VOID(mask);
                    overlayManager->SetMaskNodeId(dialog->GetId(), mask->GetId());
                }
            } else {
                dialog = overlayManager->ShowDialog(arg, nullptr, false);
                CHECK_NULL_VOID(dialog);
            }
            UiSessionManager::GetInstance()->ReportComponentChangeEvent("onVisibleChange", "show");
            auto hub = dialog->GetOrCreateEventHub<NG::DialogEventHub>();
            CHECK_NULL_VOID(hub);
            hub->SetOnCancel(arg.onCancel);
            auto pattern = dialog->GetPattern<DialogPattern>();
            CHECK_NULL_VOID(pattern);
            pattern->SetOnWillDismiss(arg.onWillDismiss);
            if (SystemProperties::ConfigChangePerform()) {
                CreateWithResourceObj(pattern, arg);
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIDialogShowAlertDialog");
}

void AlertDialogModelNG::CreateWithResourceObj(const RefPtr<DialogPattern>& pattern, const DialogProperties& arg)
{
    if (arg.resourceTitleObj) {
        ProcessContentResourceObj(pattern, arg.resourceTitleObj, DialogResourceType::TITLE);
    }
    if (arg.resourceSubTitleObj) {
        ProcessContentResourceObj(pattern, arg.resourceSubTitleObj, DialogResourceType::SUBTITLE);
    }
    if (arg.resourceContentObj) {
        ProcessContentResourceObj(pattern, arg.resourceContentObj, DialogResourceType::MESSAGE);
    }
    ProcessButtonInfo(pattern, arg);
}

void AlertDialogModelNG::ProcessContentResourceObj(
    const RefPtr<DialogPattern>& pattern, const RefPtr<ResourceObject>& object, const DialogResourceType type)
{
    std::string key = "dialog." + DialogTypeUtils::ConvertDialogTypeToString(type);
    auto&& updateFunc = [pattern, key, type](const RefPtr<ResourceObject>& resObj) {
        std::string result = pattern->GetResCacheMapByKey(key);
        if (result.empty()) {
            ResourceParseUtils::ParseResString(resObj, result);
            pattern->AddResCache(key, result);
        }
        pattern->UpdateContentValue(result, type);
    };
    updateFunc(object);
    pattern->AddResObj(key, object, std::move(updateFunc));
}

void AlertDialogModelNG::SetButtonText(
    const RefPtr<DialogPattern>& pattern, const RefPtr<ResourceObject>& resObj, int32_t index)
{
    CHECK_NULL_VOID(pattern && resObj);
    std::string key = "dialog.button.text" + std::to_string(index);
    auto&& updateFunc = [pattern, key, index](const RefPtr<ResourceObject>& resObj) {
        std::string result = pattern->GetResCacheMapByKey(key);
        if (result.empty()) {
            ResourceParseUtils::ParseResString(resObj, result);
            pattern->AddResCache(key, result);
        }
        pattern->UpdateButtonText(result, index);
    };
    updateFunc(resObj);
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void AlertDialogModelNG::SetButtonFontColor(
    const RefPtr<DialogPattern>& pattern, const RefPtr<ResourceObject>& resObj, int32_t index)
{
    CHECK_NULL_VOID(pattern && resObj);
    std::string key = "dialog.button.fontColor" + std::to_string(index);
    auto&& updateFunc = [pattern, key, index](const RefPtr<ResourceObject>& resObj) {
        std::string result = pattern->GetResCacheMapByKey(key);
        Color color;
        if (result.empty()) {
            ResourceParseUtils::ParseResColor(resObj, color);
            pattern->AddResCache(key, color.ToString());
        } else {
            color = Color::ColorFromString(result);
        }
        pattern->UpdateButtonFontColor(color.ToString(), index);
    };
    updateFunc(resObj);
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void AlertDialogModelNG::SetButtonBackgroundColor(
    const RefPtr<DialogPattern>& pattern, const RefPtr<ResourceObject>& resObj, int32_t index)
{
    CHECK_NULL_VOID(pattern && resObj);
    std::string key = "dialog.button.backgroundColor" + std::to_string(index);
    auto&& updateFunc = [pattern, key, index](const RefPtr<ResourceObject>& resObj) {
        std::string result = pattern->GetResCacheMapByKey(key);
        Color color;
        if (result.empty()) {
            ResourceParseUtils::ParseResColor(resObj, color);
            pattern->AddResCache(key, color.ToString());
        } else {
            color = Color::ColorFromString(result);
        }
        pattern->UpdateButtonBackgroundColor(color, index);
    };
    updateFunc(resObj);
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void AlertDialogModelNG::ProcessButtonInfo(const RefPtr<DialogPattern>& pattern, const DialogProperties& arg)
{
    std::int32_t index = 0;
    for (auto& button : arg.buttons) {
        if (button.resourceTextObj) {
            SetButtonText(pattern, button.resourceTextObj, index);
        }
        if (button.resourceFontColorObj) {
            SetButtonFontColor(pattern, button.resourceFontColorObj, index);
        }
        if (button.resourceBgColorObj) {
            SetButtonBackgroundColor(pattern, button.resourceBgColorObj, index);
        }
        ++index;
    }
}
} // namespace OHOS::Ace::NG
