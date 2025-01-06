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

#ifndef TEST_UNITTEST_CORE_EVENT_FOCUS_TEST_BASE_H
#define TEST_UNITTEST_CORE_EVENT_FOCUS_TEST_BASE_H
#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components/theme/app_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/event/touch_event.h"
#include "core/components_ng/manager/focus/focus_view.h"
#include "core/components_ng/pattern/pattern.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/key_event.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"

namespace OHOS::Ace::NG {

class FocusTestPattern : public Pattern {
public:

    FocusTestPattern() = default;
    ~FocusTestPattern() override = default;

    FocusPattern GetFocusPattern() const override
    {
        return *AceType::RawPtr(focusPattern_);
    }

    void SetFocusPattern(FocusPattern focusPattern)
    {
        focusPattern_ = new FocusPattern(focusPattern);
    }

    ScopeFocusAlgorithm GetScopeFocusAlgorithm() override
    {
        return scopeFocusAlgorithm_;
    }

    void SetScopeFocusAlgorithm(ScopeFocusAlgorithm scopeFocusAlgorithm)
    {
        scopeFocusAlgorithm_ = scopeFocusAlgorithm;
    }

    RefPtr<FocusPattern> focusPattern_;
    ScopeFocusAlgorithm scopeFocusAlgorithm_;
};

class FocusViewTestPattern : public Pattern, public FocusView {
    DECLARE_ACE_TYPE(FocusViewTestPattern, Pattern, FocusView);
public:

    FocusViewTestPattern() = default;
    ~FocusViewTestPattern() override = default;

    FocusPattern GetFocusPattern() const override
    {
        return *AceType::RawPtr(focusPattern_);
    }

    void SetFocusPattern(FocusPattern focusPattern)
    {
        focusPattern_ = new FocusPattern(focusPattern);
    }

    ScopeFocusAlgorithm GetScopeFocusAlgorithm() override
    {
        return scopeFocusAlgorithm_;
    }

    void SetScopeFocusAlgorithm(ScopeFocusAlgorithm scopeFocusAlgorithm)
    {
        scopeFocusAlgorithm_ = scopeFocusAlgorithm;
    }

    std::list<int32_t> GetRouteOfFirstScope() override
    {
        return routeOfFirstScope;
    }

    void SetRouteOfFirstScope(std::list<int32_t>& list)
    {
        routeOfFirstScope = list;
    }

    RefPtr<FocusPattern> focusPattern_;
    ScopeFocusAlgorithm scopeFocusAlgorithm_;
    std::list<int32_t> routeOfFirstScope = { 0 };
};

class FocusTestBaseNg {
public:
    bool InitFocusTestBaseNG();
    static RefPtr<FrameNode> CreateRootNode();
    static RefPtr<FrameNode> CreateNode(const std::string& tag);
    static RefPtr<FrameNode> CreateNodeWithFocusPattern(const std::string& tag, FocusType focusType, bool focusable);
    static RefPtr<FrameNode> CreateNodeWithFocusPattern(const std::string& tag, FocusType focusType,
        bool focusable, FocusStyleType styleType);
    static RefPtr<FrameNode> CreateNodeWithFocusPattern(const std::string& tag, FocusType focusType, bool focusable,
        FocusStyleType styleType, const FocusPaintParam& paintParams);
    static RefPtr<FrameNode> CreateNodeWithFocusPatternAndAlgorithm(const std::string& tag, FocusType focusType,
        bool focusable, ScopeFocusAlgorithm algorithm);
    static RefPtr<FrameNode> CreateNodeWithFocusPatternAndAlgorithm(const std::string& tag, FocusType focusType,
        bool focusable, FocusStyleType styleType, ScopeFocusAlgorithm algorithm);
    static RefPtr<FrameNode> CreateNodeWithFocusPatternAndAlgorithm(const std::string& tag, FocusType focusType,
        bool focusable, FocusStyleType styleType, const FocusPaintParam& paintParams, ScopeFocusAlgorithm algorithm);

    static RefPtr<FrameNode> CreateFocusViewWithFocusPattern(const std::string& tag, std::list<int32_t>& list,
        FocusType focusType, bool focusable);
    static RefPtr<FrameNode> CreateFocusViewWithFocusPattern(const std::string& tag, std::list<int32_t>& list,
        FocusType focusType, bool focusable, FocusStyleType styleType);
    static RefPtr<FrameNode> CreateFocusViewWithFocusPattern(const std::string& tag, std::list<int32_t>& list,
        FocusType focusType, bool focusable, FocusStyleType styleType, const FocusPaintParam& paintParams);
    static RefPtr<FrameNode> CreateFocusViewWithFocusPatternAndAlgorithm(const std::string& tag,
        std::list<int32_t>& list,
        FocusType focusType, bool focusable, ScopeFocusAlgorithm algorithm);
    static RefPtr<FrameNode> CreateFocusViewWithFocusPatternAndAlgorithm(const std::string& tag,
        std::list<int32_t>& list,
        FocusType focusType, bool focusable, FocusStyleType styleType, ScopeFocusAlgorithm algorithm);
    static RefPtr<FrameNode> CreateFocusViewWithFocusPatternAndAlgorithm(const std::string& tag,
        std::list<int32_t>& list,
        FocusType focusType, bool focusable, FocusStyleType styleType,
        const FocusPaintParam& paintParams, ScopeFocusAlgorithm algorithm);

    RefPtr<FocusManager> focusManager_;
};
} // namespace OHOS::Ace::NG

#endif //TEST_UNITTEST_CORE_EVENT_FOCUS_TEST_BASE_H