/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_context_menu_ffi.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/common/container.h"

using namespace OHOS::Ace;

extern "C" {
void FfiOHOSAceFrameworkContextMenuClose()
{
#if defined(MULTIPLE_WINDOW_SUPPORTED)
    SubwindowManager::GetInstance()->HideMenuNG();
#endif
    // Close context menu.
    auto container = Container::CurrentSafely();
    if (!container) {
        return;
    }
    auto context = AceType::DynamicCast<PipelineContext>(container->GetPipelineContext());
    auto executor = container->GetTaskExecutor();
    if (!executor) {
        return;
    }
    executor->PostTask(
        [context]() {
            if (context) {
                context->CloseContextMenu();
            }
        },
        TaskExecutor::TaskType::UI, "FfiOHOSAceFrameworkContextMenuClose");
}
}
