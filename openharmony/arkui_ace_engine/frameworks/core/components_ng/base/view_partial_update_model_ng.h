/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_PARTIAL_UPDATE_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_PARTIAL_UPDATE_MODEL_NG_H

#include <functional>

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components_ng/base/view_partial_update_model.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT ViewPartialUpdateModelNG : public ViewPartialUpdateModel {
public:
    RefPtr<AceType> CreateNode(NodeInfoPU&& info) override;

    bool MarkNeedUpdate(const WeakPtr<AceType>& node) override;

    void FlushUpdateTask(const UpdateTask& task) override {}

    void FinishUpdate(const WeakPtr<AceType>& viewNode, int32_t id,
        std::function<void(const UpdateTask&)>&& emplaceTaskFunc) override;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_PARTIAL_UPDATE_MODEL_NG_H
