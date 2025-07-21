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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLL_BAR_2D_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLL_BAR_2D_PAINTER_H

#include "scroll_bar_overlay_modifier.h"

namespace OHOS::Ace::NG {
class ScrollBar2D;
class ScrollBar2DPainter : public OverlayModifier {
    DECLARE_ACE_TYPE(ScrollBar2DPainter, OverlayModifier);
    ACE_DISALLOW_COPY_AND_MOVE(ScrollBar2DPainter);
public:
    ScrollBar2DPainter();
    ~ScrollBar2DPainter() override = default;

    void onDraw(DrawingContext& drawingContext) override;

    /**
     * @brief update properties and trigger related animations
     */
    void UpdateFrom(const ScrollBar2D& bar);

private:
    RefPtr<ScrollBarOverlayModifier> vertical_ = MakeRefPtr<ScrollBarOverlayModifier>();
    RefPtr<ScrollBarOverlayModifier> horizontal_ = MakeRefPtr<ScrollBarOverlayModifier>();
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLL_BAR_2D_PAINTER_H
