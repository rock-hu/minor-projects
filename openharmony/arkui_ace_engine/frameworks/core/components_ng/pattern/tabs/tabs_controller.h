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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TABS_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TABS_CONTROLLER_H

#include "core/components/swiper/swiper_controller.h"

namespace OHOS::Ace::NG {

using StartShowTabBarFunc = std::function<void(int32_t delay)>;
using StopShowTabBarFunc = std::function<void()>;
using SetUpdateTabBarHiddenRatioFunc = std::function<void(float ratio)>;
using SetTabBarTranslateFunc = std::function<void(const TranslateOptions& options)>;
using SetTabBarOpacityFunc = std::function<void(float opacity)>;

class TabsControllerNG : public SwiperController {
    DECLARE_ACE_TYPE(TabsControllerNG, SwiperController);

public:
    TabsControllerNG() = default;
    ~TabsControllerNG() override = default;

    void SetStartShowTabBarImpl(const StartShowTabBarFunc& startShowTabBarImpl)
    {
        startShowTabBarImpl_ = startShowTabBarImpl;
    }

    void StartShowTabBar(int32_t delay = 0)
    {
        if (startShowTabBarImpl_) {
            startShowTabBarImpl_(delay);
        }
    }

    void SetStopShowTabBarImpl(const StopShowTabBarFunc& stopShowTabBarImpl)
    {
        stopShowTabBarImpl_ = stopShowTabBarImpl;
    }

    void StopShowTabBar()
    {
        if (stopShowTabBarImpl_) {
            stopShowTabBarImpl_();
        }
    }

    void SetUpdateTabBarHiddenRatioImpl(const SetUpdateTabBarHiddenRatioFunc& updateTabBarHiddenRatioImpl)
    {
        updateTabBarHiddenRatioImpl_ = updateTabBarHiddenRatioImpl;
    }

    void UpdateTabBarHiddenRatio(float ratio)
    {
        if (updateTabBarHiddenRatioImpl_) {
            updateTabBarHiddenRatioImpl_(ratio);
        }
    }

    void SetTabBarTranslateImpl(const SetTabBarTranslateFunc& setTabBarTranslateImpl)
    {
        setTabBarTranslateImpl_ = setTabBarTranslateImpl;
    }

    void SetTabBarTranslate(const TranslateOptions& options)
    {
        if (setTabBarTranslateImpl_) {
            setTabBarTranslateImpl_(options);
        }
    }

    void SetTabBarOpacityImpl(const SetTabBarOpacityFunc& setTabBarOpacityImpl)
    {
        setTabBarOpacityImpl_ = setTabBarOpacityImpl;
    }

    void SetTabBarOpacity(float opacity)
    {
        if (setTabBarOpacityImpl_) {
            setTabBarOpacityImpl_(opacity);
        }
    }

private:
    StartShowTabBarFunc startShowTabBarImpl_;
    StopShowTabBarFunc stopShowTabBarImpl_;
    SetUpdateTabBarHiddenRatioFunc updateTabBarHiddenRatioImpl_;
    SetTabBarTranslateFunc setTabBarTranslateImpl_;
    SetTabBarOpacityFunc setTabBarOpacityImpl_;
};

} // namespace OHOS::ACE
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TABS_CONTROLLER_H