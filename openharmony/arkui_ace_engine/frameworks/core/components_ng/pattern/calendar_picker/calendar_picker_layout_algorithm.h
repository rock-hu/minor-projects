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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CALENDAR_PICKER_CALENDAR_PICKER_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CALENDAR_PICKER_CALENDAR_PICKER_LAYOUT_ALGORITHM_H

#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_algorithm.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT CalendarPickerLayoutAlgorithm : public LinearLayoutAlgorithm {
    DECLARE_ACE_TYPE(CalendarPickerLayoutAlgorithm, LinearLayoutAlgorithm);

public:
    CalendarPickerLayoutAlgorithm() = default;
    ~CalendarPickerLayoutAlgorithm() override = default;

    void Measure(LayoutWrapper* layoutWrapper) override;

private:
    void CalendarPickerContentMeasure(LayoutWrapper* layoutWrapper);
    void SelfMeasure(LayoutWrapper* layoutWrapper);

    SizeF contentMeasure_;
    SizeF flexMeasure_;
    ACE_DISALLOW_COPY_AND_MOVE(CalendarPickerLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CALENDAR_PICKER_CALENDAR_PICKER_LAYOUT_ALGORITHM_H
