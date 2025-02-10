/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_BUTTON_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_BUTTON_MODEL_NG_H

#include "base/utils/macros.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/form/resource/form_request_data.h"
#include "core/components_ng/pattern/form_button/form_button_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT FormButtonModelNG : public OHOS::Ace::FormButtonModel {
public:
    void Create(Alignment align, const std::string& action, const RequestFormInfo& info) override;

private:
    void Create(const std::string& action, const RequestFormInfo& info) override;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_FORM_BUTTON_MODEL_NG_H