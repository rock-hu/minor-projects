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

#ifndef UI_TEST_EDIT_TEXT_H
#define UI_TEST_EDIT_TEXT_H

#include "common/input_method_manager.h"
#include "components/ui_edit_text.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
constexpr char* UI_TEST_EDIT_TEXT_1 = "editName";
constexpr char* UI_TEST_EDIT_TEXT_2 = "inputName";
constexpr char* UI_TEST_EDIT_TEXT_3 = "editPwd";
constexpr char* UI_TEST_EDIT_TEXT_4 = "inputPwd";
constexpr char* UI_TEST_EDIT_TEXT_5 = "redPwd";
class UITestEditText : public UITest, public UIView::OnClickListener, public UIEditText::OnChangeListener {
public:
    UITestEditText() {}
    ~UITestEditText() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitUIEditTextTestDisplay001();

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnChange(UIView& view, const char* value) override;

private:
    UIScrollView* container_ = nullptr;
    UILabel* changeHint_ = nullptr;
    UIEditText* SetupEditText(const char* value, const char* placeholder, const char* viewId);
};
} // namespace OHOS
#endif // UI_TEST_EDIT_TEXT_H
