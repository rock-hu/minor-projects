/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_VIDEO_VIDEO_LAYOUT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_VIDEO_VIDEO_LAYOUT_PROPERTY_H

#include "base/geometry/ng/size_t.h"
#include "base/geometry/size.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/pattern/video/video_styles.h"
#include "core/components_ng/property/property.h"
#include "core/image/image_source_info.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT VideoLayoutProperty : public LayoutProperty {
    DECLARE_ACE_TYPE(VideoLayoutProperty, LayoutProperty);

public:
    VideoLayoutProperty() = default;

    ~VideoLayoutProperty() override = default;

    RefPtr<LayoutProperty> Clone() const override
    {
        auto value = MakeRefPtr<VideoLayoutProperty>();
        value->LayoutProperty::UpdateLayoutProperty(DynamicCast<LayoutProperty>(this));
        value->propVideoStyle_ = CloneVideoStyle();
        return value;
    }

    void Reset() override
    {
        LayoutProperty::Reset();
        ResetVideoStyle();
    }

    void fullScreenReset()
    {
        auto videoSrcInfo_ = GetVideoSourceValue(VideoSourceInfo());
        auto objectFit_ = GetObjectFitValue(ImageFit::COVER);
        auto ImageSourceInfo_ = GetPosterImageInfoValue(ImageSourceInfo());
        auto control_ = GetControlsValue(true);
        auto videoSize_ = GetVideoSizeValue(SizeF());
        Reset();
        UpdateVideoSource(videoSrcInfo_);
        UpdateObjectFit(objectFit_);
        UpdateControls(control_);
        UpdatePosterImageInfo(ImageSourceInfo_);
        UpdateVideoSize(videoSize_);
    }

    ACE_DEFINE_PROPERTY_GROUP(VideoStyle, VideoStyle);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(VideoStyle, ObjectFit, ImageFit, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(VideoStyle, VideoSource, VideoSourceInfo, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(VideoStyle, PosterImageInfo, ImageSourceInfo, PROPERTY_UPDATE_LAYOUT);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(VideoStyle, Controls, bool, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(VideoStyle, VideoSize, SizeF, PROPERTY_UPDATE_MEASURE);

private:
    ACE_DISALLOW_COPY_AND_MOVE(VideoLayoutProperty);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_VIDEO_VIDEO_LAYOUT_PROPERTY_H
