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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_WEB_RICHTEXT_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_WEB_RICHTEXT_MODEL_H

#include <mutex>
#include <string>

#include "core/components/web/web_property.h"

namespace OHOS::Ace {
class ACE_FORCE_EXPORT RichTextModel {
public:
    static RichTextModel* GetInstance();
    virtual ~RichTextModel() = default;

    virtual void Create(const std::string&) = 0;
    virtual void SetOnPageStart(std::function<void(const BaseEventInfo*)>&& onStarted) = 0;
    virtual void SetOnPageFinish(std::function<void(const BaseEventInfo*)>&& onFinish) = 0;

private:
    static std::unique_ptr<RichTextModel> instance_;
    static std::mutex mutex_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_WEB_RICHTEXT_MODEL_H
