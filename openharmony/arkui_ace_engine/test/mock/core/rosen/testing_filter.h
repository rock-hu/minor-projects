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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MOCK_ROSEN_TEST_TESTING_FILTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MOCK_ROSEN_TEST_TESTING_FILTER_H

#include <memory>

#include "testing_color.h"
#include "testing_color_filter.h"
#include "testing_mask_filter.h"
#include "testing_image_filter.h"

namespace OHOS::Ace::Testing {
class TestingFilter {
public:
    TestingFilter() = default;
    virtual ~TestingFilter() = default;

    enum class FilterQuality {
        NONE,
        LOW,
        MEDIUM,
        HIGH,
    };

    virtual void SetMaskFilter(std::shared_ptr<TestingMaskFilter> /* maskFilter */) {}

    virtual void SetImageFilter(std::shared_ptr<TestingImageFilter> /* imageFilter */) {}

    virtual void SetFilterQuality(FilterQuality fq) {}

    virtual void SetColorFilter(std::shared_ptr<TestingColorFilter> colorFilter) {}
};
} // namespace OHOS::Ace::Testing
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MOCK_ROSEN_TEST_TESTING_FILTER_H
