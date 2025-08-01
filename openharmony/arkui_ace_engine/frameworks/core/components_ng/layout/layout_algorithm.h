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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_LAYOUTS_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_LAYOUTS_LAYOUT_ALGORITHM_H

#include <optional>

#include "ui/view/layout/layout_algorithm.h"

#include "base/memory/ace_type.h"
#include "base/thread/cancelable_callback.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::Kit {
class LayoutAlgorithm;
}

namespace OHOS::Ace::NG {
class LayoutWrapper;

class ACE_EXPORT LayoutAlgorithm : public virtual AceType {
    DECLARE_ACE_TYPE(LayoutAlgorithm, AceType);

public:
    LayoutAlgorithm() = default;
    ~LayoutAlgorithm() override = default;

    void Reset()
    {
        OnReset();
    }

    virtual std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& /*contentConstraint*/, LayoutWrapper* /*layoutWrapper*/)
    {
        return std::nullopt;
    }

    virtual void Measure(LayoutWrapper* layoutWrapper) {}

    virtual void Layout(LayoutWrapper* layoutWrapper) {}

    virtual bool SkipMeasure()
    {
        return false;
    }

    virtual bool SkipLayout()
    {
        return false;
    }

    virtual TaskThread CanRunOnWhichThread()
    {
        return MAIN_TASK;
    }

    virtual bool MeasureInNextFrame() const
    {
        return false;
    }

    void SetHasMeasured(bool measured)
    {
        hasMeasured_ = measured;
    }

    bool GetHasMeasured()
    {
        return hasMeasured_;
    }

    void SetNeedPostponeForIgnore(bool needed = true)
    {
        postponeForIgnore_ = needed;
    }

    bool GetNeedPostponeForIgnore() const
    {
        return postponeForIgnore_;
    }

protected:
    virtual void OnReset() {}
    bool hasMeasured_ = false;
    bool postponeForIgnore_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(LayoutAlgorithm);
};

class ACE_EXPORT LayoutAlgorithmWrapper : public LayoutAlgorithm {
    DECLARE_ACE_TYPE(LayoutAlgorithmWrapper, LayoutAlgorithm);

public:
    explicit LayoutAlgorithmWrapper(
        const RefPtr<LayoutAlgorithm>& layoutAlgorithmT, bool skipMeasure = false, bool skipLayout = false);
    ~LayoutAlgorithmWrapper() override;

    static RefPtr<LayoutAlgorithmWrapper> CreateLayoutAlgorithmWrapper(
        const RefPtr<Kit::LayoutAlgorithm>& layoutAlgorithm);

    void OnReset() override
    {
        layoutAlgorithm_.Reset();
        skipMeasure_ = false;
        skipLayout_ = false;
    }

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper) override;
    void Measure(LayoutWrapper* layoutWrapper) override;
    void Layout(LayoutWrapper* layoutWrapper) override;

    void SetSkipMeasure()
    {
        skipMeasure_ = true;
    }

    void SetSkipLayout()
    {
        skipLayout_ = true;
    }

    void SetNeedMeasure()
    {
        skipMeasure_ = false;
    }

    void SetNeedLayout()
    {
        skipLayout_ = false;
    }

    bool SkipMeasure() override
    {
        return skipMeasure_;
    }

    bool SkipLayout() override
    {
        return skipLayout_;
    }

    bool MeasureInNextFrame() const override
    {
        return layoutAlgorithm_ && layoutAlgorithm_->MeasureInNextFrame();
    }

    const RefPtr<LayoutAlgorithm>& GetLayoutAlgorithm() const
    {
        return layoutAlgorithm_;
    }

    void SetLayoutAlgorithm(RefPtr<LayoutAlgorithm> algorithm)
    {
        layoutAlgorithm_ = std::move(algorithm);
    }

    bool IsExpire() const
    {
        return frameId != UITaskScheduler::GetFrameId();
    }

    void SetPercentHeight(bool value)
    {
        percentHeight_ = value;
    }

    void SetPercentWidth(bool value)
    {
        percentWidth_ = value;
    }

    bool GetPercentHeight() const
    {
        return percentHeight_;
    }

    bool GetPercentWidth() const
    {
        return percentWidth_;
    }

    void SetAbsLayoutAlgorithm(const RefPtr<Kit::LayoutAlgorithm>& absLayoutAlgorithm);

private:
    RefPtr<LayoutAlgorithm> layoutAlgorithm_;

    bool skipMeasure_ = false;
    bool skipLayout_ = false;
    bool percentHeight_ = false;
    bool percentWidth_ = false;
    uint64_t frameId = UITaskScheduler::GetFrameId();
    RefPtr<Kit::LayoutAlgorithm> absLayoutAlgorithm_;

    ACE_DISALLOW_COPY_AND_MOVE(LayoutAlgorithmWrapper);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_LAYOUTS_LAYOUT_ALGORITHM_H
