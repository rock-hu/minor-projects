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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_ATOMIC_SERVICE_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_ATOMIC_SERVICE_PATTERN_H

#include <cstddef>
#include "core/components_ng/pattern/app_bar/app_bar_theme.h"
#include "core/components_ng/pattern/app_bar/atomic_service_layout_algorithm.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"

namespace OHOS::Ace::NG {
class AtomicServicePattern : public LinearLayoutPattern {
    DECLARE_ACE_TYPE(AtomicServicePattern, LinearLayoutPattern);
    ACE_DISALLOW_COPY_AND_MOVE(AtomicServicePattern);

public:
    AtomicServicePattern() : LinearLayoutPattern(true) {};
    ~AtomicServicePattern() override = default;

    void OnAttachToFrameNode() override;
    void BeforeCreateLayoutWrapper() override;
    void OnLanguageConfigurationUpdate() override;
    void OnColorConfigurationUpdate() override;
    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<AtomicServiceLayoutAlgorithm>();
    }
    RefPtr<FrameNode> GetMenuBarRow();
    RefPtr<FrameNode> GetMenuBar();
    RefPtr<FrameNode> GetMenuButton();
    RefPtr<FrameNode> GetDivider();
    RefPtr<FrameNode> GetCloseButton();
    RefPtr<FrameNode> GetMenuIcon();
    RefPtr<FrameNode> GetCloseIcon();
    void UpdateColor(std::optional<bool> isLight);
    void UpdateMenuBarColor(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& menuBar, bool isLight);
    void UpdateButtonColor(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& button, bool isLight);
    void UpdateDividerColor(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& divider, bool isLight);
    void UpdateIconColor(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& icon, bool isLight);
    void UpdateLayout();
    void UpdateMenuBarLayout(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& menuBar, bool isRtl);
    void UpdateButtonLayout(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& button, bool isLeft);
    void UpdateIconLayout(RefPtr<AppBarTheme>& theme, RefPtr<FrameNode>& icon, bool isLeft);

    std::optional<bool> settedColorMode = std::nullopt;

private:
    void UpdateLayoutMargin();
    void UpdateOverlayLayout();
    double safeAreaLeft_ = 0;
    double safeAreaRight_ = 0;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_ATOMIC_SERVICE_PATTERN_H
