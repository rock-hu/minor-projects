/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_VIEW_BITMAP_H
#define UI_TEST_VIEW_BITMAP_H

#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
constexpr char* UI_TEST_COMPONENT_SCREENSHOT = "component_screenshot";
constexpr char* UI_TEST_FULL_SCREEN_CAPTURE = "full_screen_capture";

class UITestViewBitmap : public UITest {
public:
    UITestViewBitmap() {}
    ~UITestViewBitmap() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void UIKitBitmapTestGetViewBitmap001();
    void UIKitBitmapTestGetViewBitmap002();
    void UIKitBitmapTestGetViewBitmap003();
    void UIKitBitmapTestGetViewBitmap004();
#if !(defined ENABLE_WINDOW && ENABLE_WINDOW)
    void UIKitBitmapTestGetScreenBitmap001();
#endif
private:
    UIScrollView* container_ = nullptr;
    UIImageView* viewBitmap_ = nullptr;
    UIImageView* viewBitmap2_ = nullptr;
    UIImageView* viewBitmap3_ = nullptr;
    UIImageView* viewBitmap4_ = nullptr;
    UIImageView* screenBitmap_ = nullptr;
    UIView::OnClickListener* viewBitmapListener_ = nullptr;
    UIView::OnClickListener* viewBitmapListener2_ = nullptr;
    UIView::OnClickListener* viewBitmapListener3_ = nullptr;
    UIView::OnClickListener* viewBitmapListener4_ = nullptr;
    UIView::OnClickListener* screenBitmapListener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_VIEW_BITMAP_H
