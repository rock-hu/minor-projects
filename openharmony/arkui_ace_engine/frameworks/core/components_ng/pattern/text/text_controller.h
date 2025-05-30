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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_CONTROLLER_H

#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/text/text_model.h"

namespace OHOS::Ace::NG {
class TextPattern;

class ACE_EXPORT TextController : public TextControllerBase {
    DECLARE_ACE_TYPE(TextController, TextControllerBase);

public:
    void SetPattern(const WeakPtr<TextPattern>& pattern);
    void CloseSelectionMenu() override;
    void SetStyledString(const RefPtr<SpanStringBase>& value, bool closeSelectOverlay = true) override;
    WeakPtr<LayoutInfoInterface> GetLayoutInfoInterface() override;

private:
    WeakPtr<TextPattern> pattern_;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_CONTROLLER_H