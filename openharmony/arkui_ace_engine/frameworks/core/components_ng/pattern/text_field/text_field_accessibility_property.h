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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_FIELD_TEXT_FIELD_ACCESSIBILITY_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_FIELD_TEXT_FIELD_ACCESSIBILITY_PROPERTY_H

#include <string>

#include "base/utils/utils.h"
#include "core/components_ng/property/accessibility_property.h"

namespace OHOS::Ace::NG {
class TextFieldAccessibilityProperty : public AccessibilityProperty {
    DECLARE_ACE_TYPE(TextFieldAccessibilityProperty, AccessibilityProperty);

public:
    TextFieldAccessibilityProperty() = default;

    ~TextFieldAccessibilityProperty() override = default;

    int32_t GetTextLengthLimit() const override;

    bool IsPassword() const override;

    AceTextCategory GetTextInputType() const override;

    bool IsEditable() const override
    {
        return true;
    }

    bool IsMultiLine() const override;

    bool IsSelected() const override;

    int32_t GetTextSelectionStart() const override;

    int32_t GetTextSelectionEnd() const override;

    std::string GetText() const override;

    bool IsHint() const override;

    std::string GetHintText() const override;

    void SetErrorText(const std::string& errorText)
    {
        errorText_ = errorText;
    }

    std::string GetErrorText() const override
    {
        return errorText_;
    }

    bool GetContentInvalid() const override;

    bool IsScrollable() const override;

protected:
    void SetSpecificSupportAction() override;

private:
    bool IsShowCount() const;
    bool IsShowError() const;
    std::string errorText_;

    ACE_DISALLOW_COPY_AND_MOVE(TextFieldAccessibilityProperty);
};
} // namespace OHOS::Ace::NG

#endif
