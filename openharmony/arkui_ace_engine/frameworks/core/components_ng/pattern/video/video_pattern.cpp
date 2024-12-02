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

#include "core/components_ng/pattern/video/video_pattern.h"

#include "video_node.h"

#include "base/background_task_helper/background_task_helper.h"
#include "base/geometry/dimension.h"
#include "base/geometry/ng/size_t.h"
#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/thread/task_executor.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/ace_engine.h"
#include "core/common/ai/image_analyzer_manager.h"
#include "core/common/udmf/udmf_client.h"
#include "core/components/video/video_theme.h"
#include "core/components_ng/pattern/slider/slider_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/video/video_full_screen_node.h"
#include "core/components_ng/pattern/video/video_full_screen_pattern.h"
#include "core/components_ng/property/gradient_property.h"

#ifdef RENDER_EXTRACT_SUPPORTED
#include "core/common/ace_view.h"
#endif

namespace OHOS::Ace::NG {
namespace {
using HiddenChangeEvent = std::function<void(bool)>;
constexpr uint32_t SECONDS_PER_HOUR = 3600;
constexpr uint32_t SECONDS_PER_MINUTE = 60;
const std::string FORMAT_HH_MM_SS = "%02d:%02d:%02d";
const std::string FORMAT_MM_SS = "%02d:%02d";
constexpr int32_t MILLISECONDS_TO_SECONDS = 1000;
constexpr uint32_t CURRENT_POS = 1;
constexpr uint32_t SLIDER_POS = 2;
constexpr uint32_t DURATION_POS = 3;
constexpr uint32_t FULL_SCREEN_POS = 4;
constexpr int32_t AVERAGE_VALUE = 2;
constexpr int32_t ANALYZER_DELAY_TIME = 100;
constexpr int32_t ANALYZER_CAPTURE_DELAY_TIME = 1000;
const Dimension LIFT_HEIGHT = 28.0_vp;
const std::string PNG_FILE_EXTENSION = "png";
constexpr int32_t MEDIA_TYPE_AUD = 0;

// Default error, empty string.
const std::string ERROR = "";

enum SliderChangeMode {
    BEGIN = 0,
    MOVING,
    END,
};

std::string IntTimeToText(uint32_t time)
{
    auto minutes = (time % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    auto seconds = time % SECONDS_PER_MINUTE;
    if (time >= SECONDS_PER_HOUR) {
        auto hours = time / SECONDS_PER_HOUR;
        return StringUtils::FormatString(FORMAT_HH_MM_SS.c_str(), hours, minutes, seconds);
    }
    return StringUtils::FormatString(FORMAT_MM_SS.c_str(), minutes, seconds);
}

SizeF CalculateFitContain(const SizeF& videoSize, const SizeF& layoutSize)
{
    double layoutRatio = NearZero(layoutSize.Height()) ? 0.0 : layoutSize.Width() / layoutSize.Height();
    double sourceRatio = NearZero(videoSize.Height()) ? layoutRatio : videoSize.Width() / videoSize.Height();

    if (NearZero(layoutRatio) || NearZero(sourceRatio)) {
        return layoutSize;
    }
    if (sourceRatio < layoutRatio) {
        return { static_cast<float>(sourceRatio) * layoutSize.Height(), layoutSize.Height() };
    }
    return { layoutSize.Width(), static_cast<float>(layoutSize.Width() / sourceRatio) };
}

SizeF CalculateFitFill(const SizeF& layoutSize)
{
    return layoutSize;
}

SizeF CalculateFitCover(const SizeF& videoSize, const SizeF& layoutSize)
{
    double layoutRatio = NearZero(layoutSize.Height()) ? 0.0 : layoutSize.Width() / layoutSize.Height();
    double sourceRatio = NearZero(videoSize.Height()) ? layoutRatio : videoSize.Width() / videoSize.Height();

    if (NearZero(layoutRatio) || NearZero(sourceRatio)) {
        return layoutSize;
    }
    if (sourceRatio < layoutRatio) {
        return { layoutSize.Width(), static_cast<float>(layoutSize.Width() / sourceRatio) };
    }
    return { static_cast<float>(layoutSize.Height() * sourceRatio), layoutSize.Height() };
}

SizeF CalculateFitNone(const SizeF& videoSize)
{
    return videoSize;
}

SizeF CalculateFitScaleDown(const SizeF& videoSize, const SizeF& layoutSize)
{
    if (layoutSize.Width() > videoSize.Width()) {
        return CalculateFitNone(videoSize);
    }
    return CalculateFitContain(videoSize, layoutSize);
}

SizeF MeasureVideoContentLayout(const SizeF& layoutSize, const RefPtr<VideoLayoutProperty>& layoutProperty)
{
    if (!layoutProperty || !layoutProperty->HasVideoSize()) {
        return layoutSize;
    }

    auto videoSize = layoutProperty->GetVideoSizeValue(SizeF(0, 0));
    auto imageFit = layoutProperty->GetObjectFitValue(ImageFit::COVER);
    SizeF contentSize = { 0.0, 0.0 };
    switch (imageFit) {
        case ImageFit::CONTAIN:
            contentSize = CalculateFitContain(videoSize, layoutSize);
            break;
        case ImageFit::FILL:
            contentSize = CalculateFitFill(layoutSize);
            break;
        case ImageFit::COVER:
            contentSize = CalculateFitCover(videoSize, layoutSize);
            break;
        case ImageFit::NONE:
            contentSize = CalculateFitNone(videoSize);
            break;
        case ImageFit::SCALE_DOWN:
            contentSize = CalculateFitScaleDown(videoSize, layoutSize);
            break;
        default:
            contentSize = CalculateFitContain(videoSize, layoutSize);
    }

    // Just return contentSize as the video frame area.
    return contentSize;
}

float RoundValueToPixelGrid(float value, bool isRound, bool forceCeil, bool forceFloor)
{
    float fractials = fmod(value, 1.0f);
    if (fractials < 0.0f) {
        ++fractials;
    }
    if (forceCeil) {
        return (value - fractials + 1.0f);
    } else if (forceFloor) {
        return (value - fractials);
    } else if (isRound) {
        if (NearEqual(fractials, 1.0f) || GreatOrEqual(fractials, 0.50f)) {
            return (value - fractials + 1.0f);
        } else {
            return (value - fractials);
        }
    }
    return value;
}

RectF AdjustPaintRect(float positionX, float positionY, float width, float height, bool isRound)
{
    RectF rect;
    float relativeLeft = positionX;
    float relativeTop = positionY;
    float nodeWidth = width;
    float nodeHeight = height;
    float absoluteRight = relativeLeft + nodeWidth;
    float absoluteBottom = relativeTop + nodeHeight;
    float roundToPixelErrorX = 0.0f;
    float roundToPixelErrorY = 0.0f;

    float nodeLeftI = RoundValueToPixelGrid(relativeLeft, isRound, false, false);
    float nodeTopI = RoundValueToPixelGrid(relativeTop, isRound, false, false);
    roundToPixelErrorX += nodeLeftI - relativeLeft;
    roundToPixelErrorY += nodeTopI - relativeTop;
    rect.SetLeft(nodeLeftI);
    rect.SetTop(nodeTopI);

    float nodeWidthI = RoundValueToPixelGrid(absoluteRight, isRound, false, false) - nodeLeftI;
    float nodeWidthTemp = RoundValueToPixelGrid(nodeWidth, isRound, false, false);
    roundToPixelErrorX += nodeWidthI - nodeWidth;
    if (roundToPixelErrorX > 0.5f) {
        nodeWidthI -= 1.0f;
        roundToPixelErrorX -= 1.0f;
    }
    if (roundToPixelErrorX < -0.5f) {
        nodeWidthI += 1.0f;
        roundToPixelErrorX += 1.0f;
    }
    if (nodeWidthI < nodeWidthTemp) {
        roundToPixelErrorX += nodeWidthTemp - nodeWidthI;
        nodeWidthI = nodeWidthTemp;
    }

    float nodeHeightI = RoundValueToPixelGrid(absoluteBottom, isRound, false, false) - nodeTopI;
    float nodeHeightTemp = RoundValueToPixelGrid(nodeHeight, isRound, false, false);
    roundToPixelErrorY += nodeHeightI - nodeHeight;
    if (roundToPixelErrorY > 0.5f) {
        nodeHeightI -= 1.0f;
        roundToPixelErrorY -= 1.0f;
    }
    if (roundToPixelErrorY < -0.5f) {
        nodeHeightI += 1.0f;
        roundToPixelErrorY += 1.0f;
    }
    if (nodeHeightI < nodeHeightTemp) {
        roundToPixelErrorY += nodeHeightTemp - nodeHeightI;
        nodeHeightI = nodeHeightTemp;
    }

    rect.SetWidth(nodeWidthI);
    rect.SetHeight(nodeHeightI);
    return rect;
}
Gradient ConvertToGradient(Color color)
{
    Gradient gradient;
    GradientColor gradientColorBegin;
    gradientColorBegin.SetLinearColor(LinearColor(color));
    gradientColorBegin.SetDimension(Dimension(0.0f));
    gradient.AddColor(gradientColorBegin);
    OHOS::Ace::NG::GradientColor gradientColorEnd;
    gradientColorEnd.SetLinearColor(LinearColor(color));
    gradientColorEnd.SetDimension(Dimension(1.0f));
    gradient.AddColor(gradientColorEnd);

    return gradient;
}
} // namespace

VideoPattern::VideoPattern(const RefPtr<VideoControllerV2>& videoController)
    : instanceId_(Container::CurrentId()), videoControllerV2_(videoController)
{}

void VideoPattern::ResetStatus()
{
    isInitialState_ = true;
    isPlaying_ = false;
#ifndef OHOS_PLATFORM
    isStop_ = false;
#endif
}

void VideoPattern::ResetMediaPlayer()
{
    CHECK_NULL_VOID(mediaPlayer_);
    mediaPlayer_->ResetMediaPlayer();
    SetIsPrepared(false);
    if (!SetSourceForMediaPlayer()) {
        TAG_LOGW(AceLogTag::ACE_VIDEO, "Video set source for mediaPlayer failed.");

        // It need post on ui thread.
        FireError();
        return;
    }

    RegisterMediaPlayerEvent();
    PrepareSurface();
    if (mediaPlayer_ && mediaPlayer_->PrepareAsync() != 0) {
        TAG_LOGE(AceLogTag::ACE_VIDEO, "Player prepare failed");
    }
}

void VideoPattern::UpdateMediaPlayerOnBg()
{
    PrepareMediaPlayer();
    UpdateSpeed();
    UpdateLooping();
    UpdateMuted();
    if (isInitialState_ && autoPlay_) {
        // When video is autoPlay, start playing the video when it is initial state.
        Start();
    }
}

void VideoPattern::PrepareMediaPlayer()
{
    auto videoLayoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(videoLayoutProperty);
    // src has not set/changed
    if (!videoLayoutProperty->HasVideoSource() || videoLayoutProperty->GetVideoSource() == videoSrcInfo_) {
        TAG_LOGI(AceLogTag::ACE_VIDEO, "Video source is null or the source has not changed.");
        return;
    }
    auto videoSrcInfo = videoLayoutProperty->GetVideoSource();
    videoSrcInfo_.src = videoSrcInfo->GetSrc();
    videoSrcInfo_.bundleName = videoSrcInfo->GetBundleName();
    videoSrcInfo_.moduleName = videoSrcInfo->GetModuleName();
    if (mediaPlayer_ && !mediaPlayer_->IsMediaPlayerValid()) {
        mediaPlayer_->CreateMediaPlayer();
    }

    if (mediaPlayer_ && !mediaPlayer_->IsMediaPlayerValid()) {
        // It need post on ui thread.
        FireError();
        return;
    }

    ResetStatus();
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
    platformTask.PostTask([weak = WeakClaim(this)] {
        auto video = weak.Upgrade();
        CHECK_NULL_VOID(video);
        auto targetPattern = video->GetTargetVideoPattern();
        CHECK_NULL_VOID(targetPattern);
        targetPattern->ResetMediaPlayer();
        }, "ArkUIVideoMediaPlayerReset");
}

bool VideoPattern::SetSourceForMediaPlayer()
{
    CHECK_NULL_RETURN(mediaPlayer_, false);
    return mediaPlayer_->SetSource(videoSrcInfo_.GetSrc(), videoSrcInfo_.GetBundleName(),
        videoSrcInfo_.GetModuleName());
}

void VideoPattern::RegisterMediaPlayerEvent()
{
    if (videoSrcInfo_.GetSrc().empty() || !mediaPlayer_) {
        TAG_LOGW(AceLogTag::ACE_VIDEO, "Video src is empty or mediaPlayer is null, register mediaPlayerEvent fail");
        return;
    }
    ContainerScope scope(instanceId_);
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);

    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    auto videoPattern = WeakClaim(this);

    auto&& positionUpdatedEvent = [videoPattern, uiTaskExecutor](uint32_t currentPos) {
        uiTaskExecutor.PostSyncTask([&videoPattern, currentPos] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            ContainerScope scope(video->instanceId_);
            video->OnCurrentTimeChange(currentPos);
            video->StartUpdateImageAnalyzer();
            }, "ArkUIVideoCurrentTimeChange");
    };

    auto&& stateChangedEvent = [videoPattern, uiTaskExecutor](PlaybackStatus status) {
        uiTaskExecutor.PostTask([videoPattern, status] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            ContainerScope scope(video->instanceId_);
            video->OnPlayerStatus(status);
            }, "ArkUIVideoPlayerStatusChange");
    };

    auto&& errorEvent = [videoPattern, uiTaskExecutor]() {
        uiTaskExecutor.PostTask([videoPattern] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            ContainerScope scope(video->instanceId_);
            video->OnError("");
            }, "ArkUIVideoError");
    };

    auto&& resolutionChangeEvent = [videoPattern, uiTaskExecutor]() {
        uiTaskExecutor.PostSyncTask([&videoPattern] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            ContainerScope scope(video->instanceId_);
            video->OnResolutionChange();
            }, "ArkUIVideoResolutionChange");
    };

    auto&& startRenderFrameEvent = [videoPattern, uiTaskExecutor]() {
        uiTaskExecutor.PostSyncTask([&videoPattern] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            ContainerScope scope(video->instanceId_);
            video->OnStartRenderFrameCb();
            }, "ArkUIVideoStartRenderFrame");
    };

    mediaPlayer_->RegisterMediaPlayerEvent(
        positionUpdatedEvent, stateChangedEvent, errorEvent, resolutionChangeEvent, startRenderFrameEvent);

    auto&& seekDoneEvent = [videoPattern, uiTaskExecutor](uint32_t currentPos) {
        uiTaskExecutor.PostSyncTask(
            [&videoPattern, currentPos] {
                auto video = videoPattern.Upgrade();
                CHECK_NULL_VOID(video);
                ContainerScope scope(video->instanceId_);
                video->SetIsSeeking(false);
                video->OnCurrentTimeChange(currentPos);
                }, "ArkUIVideoSeekDone");
    };
    mediaPlayer_->RegisterMediaPlayerSeekDoneEvent(std::move(seekDoneEvent));

#ifdef RENDER_EXTRACT_SUPPORTED
    auto&& textureRefreshEvent = [videoPattern, uiTaskExecutor](int32_t instanceId, int64_t textureId) {
        uiTaskExecutor.PostSyncTask([&videoPattern, instanceId, textureId] {
            auto video = videoPattern.Upgrade();
            CHECK_NULL_VOID(video);
            void* nativeWindow = video->GetNativeWindow(instanceId, textureId);
            if (!nativeWindow) {
                LOGE("the native window is nullptr.");
                return;
            }
            video->OnTextureRefresh(nativeWindow);
            }, "ArkUIVideoTextureRefresh");
    };
    mediaPlayer_->RegisterTextureEvent(textureRefreshEvent);
#endif
}

#ifdef RENDER_EXTRACT_SUPPORTED
void* VideoPattern::GetNativeWindow(int32_t instanceId, int64_t textureId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN(container, nullptr);
    auto nativeView = container->GetAceView();
    CHECK_NULL_RETURN(nativeView, nullptr);
    return const_cast<void*>(nativeView->GetNativeWindowById(textureId));
}

void VideoPattern::OnTextureRefresh(void* surface)
{
    CHECK_NULL_VOID(surface);
    auto renderContextForMediaPlayer = renderContextForMediaPlayerWeakPtr_.Upgrade();
    CHECK_NULL_VOID(renderContextForMediaPlayer);
    renderContextForMediaPlayer->MarkNewFrameAvailable(surface);
}
#endif

void VideoPattern::PrintPlayerStatus(PlaybackStatus status)
{
    switch (status) {
        case PlaybackStatus::ERROR:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is ERROR.");
            break;
        case PlaybackStatus::IDLE:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is IDLE.");
            break;
        case PlaybackStatus::PREPARED:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is PREPARED.");
            break;
        case PlaybackStatus::STARTED:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is STARTED.");
            break;
        case PlaybackStatus::PAUSED:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is PAUSED.");
            break;
        case PlaybackStatus::STOPPED:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is STOPPED.");
            break;
        case PlaybackStatus::PLAYBACK_COMPLETE:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is PLAYBACK_COMPLETE.");
            break;
        case PlaybackStatus::NONE:
            TAG_LOGI(AceLogTag::ACE_VIDEO, "Player current status is NONE.");
            break;
        default:
            TAG_LOGW(AceLogTag::ACE_VIDEO, "Invalid player status.");
            break;
    }
}

void VideoPattern::OnCurrentTimeChange(uint32_t currentPos)
{
    isInitialState_ = isInitialState_ ? currentPos == 0 : false;
    if (currentPos == currentPos_ || isStop_) {
        return;
    }

    if (duration_ == 0) {
        int32_t duration = 0;
        if (mediaPlayer_ && mediaPlayer_->GetDuration(duration) == 0) {
            duration_ = static_cast<uint32_t>(duration / MILLISECONDS_TO_SECONDS);
            OnUpdateTime(duration_, DURATION_POS);
        }
    }

    OnUpdateTime(currentPos, CURRENT_POS);
    currentPos_ = isSeeking_ ? currentPos_ : currentPos;
    auto eventHub = GetEventHub<VideoEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto json = JsonUtil::Create(true);
    json->Put("time", static_cast<double>(currentPos));
    auto param = json->ToString();
    eventHub->FireUpdateEvent(param);
}

void VideoPattern::ChangePlayerStatus(bool isPlaying, const PlaybackStatus& status)
{
    if (isPlaying) {
        auto json = JsonUtil::Create(true);
        json->Put("start", "");
        auto param = json->ToString();
        auto eventHub = GetEventHub<VideoEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FireStartEvent(param);
    }

    if (status == PlaybackStatus::PAUSED) {
        auto json = JsonUtil::Create(true);
        json->Put("pause", "");
        auto param = json->ToString();
        auto eventHub = GetEventHub<VideoEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FirePauseEvent(param);
    }

    if (status == PlaybackStatus::STOPPED) {
        auto json = JsonUtil::Create(true);
        json->Put("stop", "");
        auto param = json->ToString();
        auto eventHub = GetEventHub<VideoEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FireStopEvent(param);
    }

    if (status == PlaybackStatus::PREPARED) {
        ContainerScope scope(instanceId_);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto context = host->GetContext();
        CHECK_NULL_VOID(context);
        if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
            return;
        }
        auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        int32_t milliSecondDuration = 0;
        mediaPlayer_->GetDuration(milliSecondDuration);
        OnPrepared(milliSecondDuration / MILLISECONDS_TO_SECONDS, 0, true);
        return;
    }

    if (status == PlaybackStatus::PLAYBACK_COMPLETE) {
        OnCompletion();
    }
}

void VideoPattern::OnPlayerStatus(PlaybackStatus status)
{
    PrintPlayerStatus(status);
    bool isPlaying = (status == PlaybackStatus::STARTED);
    if (isPlaying_ != isPlaying) {
        isPlaying_ = isPlaying;
        ChangePlayButtonTag();
    }

    if (isInitialState_) {
        isInitialState_ = !isPlaying;
    }

    ChangePlayerStatus(isPlaying, status);
}

void VideoPattern::OnError(const std::string& errorId)
{
    std::string errorcode = Localization::GetInstance()->GetErrorDescription(errorId);
    auto json = JsonUtil::Create(true);
    json->Put("error", "");
    auto param = json->ToString();
    auto eventHub = GetEventHub<VideoEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireErrorEvent(param);
}

void VideoPattern::OnResolutionChange() const
{
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto videoLayoutProperty = host->GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(videoLayoutProperty);
    auto preVideoSize = videoLayoutProperty->GetVideoSize();
    if (!preVideoSize.has_value()) {
        SizeF videoSize = SizeF(
            static_cast<float>(mediaPlayer_->GetVideoWidth()),
            static_cast<float>(mediaPlayer_->GetVideoHeight()));
        videoLayoutProperty->UpdateVideoSize(videoSize);
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
}

void VideoPattern::OnStartRenderFrameCb()
{
    isInitialState_ = false;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto video = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_VOID(video);
    auto image = AceType::DynamicCast<FrameNode>(video->GetPreviewImage());
    CHECK_NULL_VOID(image);
    auto posterLayoutProperty = image->GetLayoutProperty<ImageLayoutProperty>();
    posterLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
    image->MarkModifyDone();
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
        return;
    }
    auto videoLayoutProperty = host->GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(videoLayoutProperty);
    SizeF videoSize =
        SizeF(static_cast<float>(mediaPlayer_->GetVideoWidth()), static_cast<float>(mediaPlayer_->GetVideoHeight()));
    TAG_LOGI(AceLogTag::ACE_VIDEO, "start render frame size:%{public}s", videoSize.ToString().c_str());
    videoLayoutProperty->UpdateVideoSize(videoSize);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

void VideoPattern::OnPrepared(uint32_t duration, uint32_t currentPos, bool needFireEvent)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    CHECK_NULL_VOID(mediaPlayer_);

    duration_ = duration;
    currentPos_ = currentPos;
    isInitialState_ = currentPos != 0 ? false : isInitialState_;
    isPlaying_ = mediaPlayer_->IsPlaying();
    SetIsSeeking(false);
    SetIsPrepared(true);
    OnUpdateTime(duration_, DURATION_POS);
    OnUpdateTime(currentPos_, CURRENT_POS);

    RefPtr<UINode> controlBar = nullptr;
    auto children = host->GetChildren();
    for (const auto& child : children) {
        if (child->GetTag() == V2::ROW_ETS_TAG) {
            controlBar = child;
            break;
        }
    }
    CHECK_NULL_VOID(controlBar);
    auto sliderNode = DynamicCast<FrameNode>(controlBar->GetChildAtIndex(SLIDER_POS));
    auto sliderPaintProperty = sliderNode->GetPaintProperty<SliderPaintProperty>();
    CHECK_NULL_VOID(sliderPaintProperty);
    sliderPaintProperty->UpdateMin(0.0f);
    sliderPaintProperty->UpdateMax(static_cast<float>(duration_));
    sliderNode->MarkModifyDone();
    auto playBtn = DynamicCast<FrameNode>(controlBar->GetChildAtIndex(0));
    ChangePlayButtonTag(playBtn);

    if (needFireEvent) {
        auto json = JsonUtil::Create(true);
        json->Put("duration", static_cast<double>(duration_));
        auto param = json->ToString();
        auto eventHub = GetEventHub<VideoEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FirePreparedEvent(param);
    }
    UpdateLooping();
    UpdateSpeed();
    UpdateMuted();

    checkNeedAutoPlay();
}

void VideoPattern::checkNeedAutoPlay()
{
    if (isStop_) {
        isStop_ = false;
    }
    if (dragEndAutoPlay_) {
        dragEndAutoPlay_ = false;
        Start();
    }
    if (autoPlay_) {
        Start();
    }
}

void VideoPattern::OnCompletion()
{
    isPlaying_ = false;
    currentPos_ = duration_;
    OnUpdateTime(currentPos_, CURRENT_POS);
    auto json = JsonUtil::Create(true);
    json->Put("finish", "");
    auto param = json->ToString();
    auto eventHub = GetEventHub<VideoEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireFinishEvent(param);
}

bool VideoPattern::HasPlayer() const
{
    return mediaPlayer_ != nullptr;
}

void VideoPattern::HiddenChange(bool hidden)
{
    if (isPlaying_ && hidden && HasPlayer()) {
        pastPlayingStatus_ = isPlaying_;
        Pause();
        return;
    }

    if (!hidden && pastPlayingStatus_) {
        pastPlayingStatus_ = false;
        Start();
    }
}

void VideoPattern::OnVisibleChange(bool isVisible)
{
    if (hiddenChangeEvent_) {
        hiddenChangeEvent_(!isVisible);
    }
}

void VideoPattern::UpdateLooping()
{
    if (mediaPlayer_ && mediaPlayer_->IsMediaPlayerValid()) {
        ContainerScope scope(instanceId_);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto context = host->GetContext();
        CHECK_NULL_VOID(context);
        auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
        platformTask.PostTask([weak = WeakClaim(RawPtr(mediaPlayer_)), loop = loop_] {
            auto mediaPlayer = weak.Upgrade();
            CHECK_NULL_VOID(mediaPlayer);
            mediaPlayer->SetLooping(loop);
            }, "ArkUIVideoUpdateLooping");
    }
}

void VideoPattern::UpdateSpeed()
{
    if (mediaPlayer_ && mediaPlayer_->IsMediaPlayerValid()) {
        ContainerScope scope(instanceId_);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto context = host->GetContext();
        CHECK_NULL_VOID(context);
        auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
        platformTask.PostTask([weak = WeakClaim(RawPtr(mediaPlayer_)), progress = progressRate_] {
            auto mediaPlayer = weak.Upgrade();
            CHECK_NULL_VOID(mediaPlayer);
            mediaPlayer->SetPlaybackSpeed(static_cast<float>(progress));
            }, "ArkUIVideoUpdateSpeed");
    }
}

void VideoPattern::UpdateMuted()
{
    if (mediaPlayer_ && mediaPlayer_->IsMediaPlayerValid()) {
        ContainerScope scope(instanceId_);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto context = host->GetContext();
        CHECK_NULL_VOID(context);
        auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
        platformTask.PostTask([weak = WeakClaim(RawPtr(mediaPlayer_)), isMuted = muted_] {
            auto mediaPlayer = weak.Upgrade();
            CHECK_NULL_VOID(mediaPlayer);
            if (isMuted) {
                mediaPlayer->SetMediaMuted(MEDIA_TYPE_AUD, true);
                mediaPlayer->SetVolume(0.0f, 0.0f);
            } else {
                mediaPlayer->SetMediaMuted(MEDIA_TYPE_AUD, false);
                mediaPlayer->SetVolume(1.0f, 1.0f);
            }
            }, "ArkUIVideoUpdateMuted");
    }
}

void VideoPattern::OnUpdateTime(uint32_t time, int pos) const
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    bool needControlBar = layoutProperty->GetControlsValue(true);
    if (!needControlBar) {
        return;
    }

    RefPtr<UINode> controlBar = nullptr;
    auto children = host->GetChildren();
    for (const auto& child : children) {
        if (child->GetTag() == V2::ROW_ETS_TAG) {
            controlBar = child;
            break;
        }
    }

    CHECK_NULL_VOID(controlBar);
    auto durationNode = DynamicCast<FrameNode>(controlBar->GetChildAtIndex(pos));
    CHECK_NULL_VOID(durationNode);
    auto textLayoutProperty = durationNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    std::string timeText = IntTimeToText(time);
    textLayoutProperty->UpdateContent(timeText);
    durationNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    durationNode->MarkModifyDone();
    // if current status is seeking, no need to update slider's value
    if (pos == CURRENT_POS && !isSeeking_) {
        auto sliderNode = DynamicCast<FrameNode>(controlBar->GetChildAtIndex(SLIDER_POS));
        CHECK_NULL_VOID(sliderNode);
        auto sliderPattern = sliderNode->GetPattern<SliderPattern>();
        CHECK_NULL_VOID(sliderPattern);
        sliderPattern->UpdateValue(static_cast<float>(time));
        sliderNode->MarkModifyDone();
    }
}

void VideoPattern::PrepareSurface()
{
    if (!mediaPlayer_ || renderSurface_->IsSurfaceValid()) {
        return;
    }
    if (!SystemProperties::GetExtSurfaceEnabled()) {
        renderSurface_->SetRenderContext(renderContextForMediaPlayer_);
    }
    renderSurface_->InitSurface();
    mediaPlayer_->SetRenderSurface(renderSurface_);
    if (mediaPlayer_->SetSurface() != 0) {
        TAG_LOGW(AceLogTag::ACE_VIDEO, "mediaPlayer renderSurface set failed");
    }
}

void VideoPattern::OnAttachToFrameNode()
{
    // full screen node is not supposed to register js controller event
    if (!InstanceOf<VideoFullScreenPattern>(this)) {
        SetMethodCall();
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->AddWindowStateChangedCallback(host->GetId());
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);

#ifdef RENDER_EXTRACT_SUPPORTED
    CHECK_NULL_VOID(renderSurface_);
    auto contextType = renderSurface_->IsTexture() ?
        RenderContext::ContextType::HARDWARE_TEXTURE : RenderContext::ContextType::HARDWARE_SURFACE;
    static RenderContext::ContextParam param = { contextType, "MediaPlayerSurface",
                                                 RenderContext::PatternType::VIDEO };
#else
    static RenderContext::ContextParam param = { RenderContext::ContextType::HARDWARE_SURFACE, "MediaPlayerSurface",
                                                 RenderContext::PatternType::VIDEO };
#endif
    renderContextForMediaPlayer_->InitContext(false, param);

    if (SystemProperties::GetExtSurfaceEnabled()) {
        RegisterRenderContextCallBack();
    }

    renderContext->UpdateBackgroundColor(Color::BLACK);
    renderContextForMediaPlayer_->UpdateBackgroundColor(Color::BLACK);
    renderContext->SetClipToBounds(true);
}

void VideoPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto id = frameNode->GetId();
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->RemoveWindowStateChangedCallback(id);
}

void VideoPattern::OnDetachFromMainTree()
{
    auto host = GetHost();
    if (host && host->GetNodeStatus() == NodeStatus::BUILDER_NODE_OFF_MAINTREE) {
        Pause();
    }
}

void VideoPattern::RegisterRenderContextCallBack()
{
#ifdef RENDER_EXTRACT_SUPPORTED
    renderSurfaceWeakPtr_ = renderSurface_;
    renderContextForMediaPlayerWeakPtr_ = renderContextForMediaPlayer_;
    auto OnAttachCallBack = [weak = WeakClaim(this)](int64_t textureId, bool isAttach) mutable {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        if (auto renderSurface = videoPattern->renderSurfaceWeakPtr_.Upgrade(); renderSurface) {
            renderSurface->AttachToGLContext(textureId, isAttach);
        }
    };
    renderContextForMediaPlayer_->AddAttachCallBack(OnAttachCallBack);
    auto OnUpdateCallBack = [weak = WeakClaim(this)](std::vector<float>& matrix) mutable {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        if (auto renderSurface = videoPattern->renderSurfaceWeakPtr_.Upgrade(); renderSurface) {
            renderSurface->UpdateTextureImage(matrix);
        }
    };
    renderContextForMediaPlayer_->AddUpdateCallBack(OnUpdateCallBack);
#endif
}

void VideoPattern::OnModifyDone()
{
    Pattern::OnModifyDone();

    if (!hiddenChangeEvent_) {
        SetHiddenChangeEvent(CreateHiddenChangeEvent());
    }

    // src has changed
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
#ifdef RENDER_EXTRACT_SUPPORTED
    if ((layoutProperty && layoutProperty->HasVideoSource() && layoutProperty->GetVideoSource() != videoSrcInfo_)) {
#else
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE) &&
        (layoutProperty && layoutProperty->HasVideoSource() && layoutProperty->GetVideoSource() != videoSrcInfo_)) {
#endif
        ResetStatus();
    }

    // update full screen pattern state
    UpdateFsState();

    // Update the control bar and preview image.
    UpdatePreviewImage();
    UpdateControllerBar();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    // Update the media player when video node is not in full screen or current node is full screen node
    if (!fullScreenNodeId_.has_value() || InstanceOf<VideoFullScreenNode>(this)) {
        ContainerScope scope(instanceId_);
        auto pipelineContext = host->GetContext();
        CHECK_NULL_VOID(pipelineContext);
        auto uiTaskExecutor = SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        uiTaskExecutor.PostTask([weak = WeakClaim(this)]() {
            auto videoPattern = weak.Upgrade();
            CHECK_NULL_VOID(videoPattern);
            ContainerScope scope(videoPattern->instanceId_);
            videoPattern->UpdateMediaPlayerOnBg();
            }, "ArkUIVideoUpdateMediaPlayer");
    }

    if (SystemProperties::GetExtSurfaceEnabled()) {
        auto pipelineContext = host->GetContext();
        CHECK_NULL_VOID(pipelineContext);
        pipelineContext->AddOnAreaChangeNode(host->GetId());
    }
    EnableDrag();
    auto eventHub = GetEventHub<VideoEventHub>();
    if (!AceType::InstanceOf<VideoFullScreenPattern>(this)) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        eventHub->SetInspectorId(host->GetInspectorIdValue(""));
    }
    if (!IsSupportImageAnalyzer()) {
        DestroyAnalyzerOverlay();
    } else if (isPaused_ && !isPlaying_ && !GetAnalyzerState()) {
        StartImageAnalyzer();
    }
}

HiddenChangeEvent VideoPattern::CreateHiddenChangeEvent()
{
    return [weak = WeakClaim(this)](bool hidden) {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        auto fullScreenNode = videoPattern->GetFullScreenNode();
        if (fullScreenNode) {
            auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(fullScreenNode->GetPattern());
            CHECK_NULL_VOID(fullScreenPattern);
            fullScreenPattern->HiddenChange(hidden);
            return;
        }
        videoPattern->HiddenChange(hidden);
    };
}

void VideoPattern::UpdatePreviewImage()
{
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    if (!layoutProperty->HasPosterImageInfo()) {
        return;
    }
    auto posterSourceInfo = layoutProperty->GetPosterImageInfo().value();
    auto imageFit = layoutProperty->GetObjectFitValue(ImageFit::COVER);
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto video = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_VOID(video);
    auto image = AceType::DynamicCast<FrameNode>(video->GetPreviewImage());
    CHECK_NULL_VOID(image);
    if (!isInitialState_) {
        auto posterLayoutProperty = image->GetLayoutProperty<ImageLayoutProperty>();
        posterLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
        image->MarkModifyDone();
        return;
    }

    if (!posterSourceInfo.IsValid()) {
        auto posterLayoutProperty = image->GetLayoutProperty<ImageLayoutProperty>();
        posterLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
        image->MarkModifyDone();
        TAG_LOGI(AceLogTag::ACE_VIDEO, "Src image is not valid.");
        return;
    }

    if (image) {
        image->SetDraggable(false);
        auto posterLayoutProperty = image->GetLayoutProperty<ImageLayoutProperty>();
        posterLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
        posterLayoutProperty->UpdateImageSourceInfo(posterSourceInfo);
        posterLayoutProperty->UpdateImageFit(imageFit);
        image->MarkModifyDone();
    }
}

void VideoPattern::UpdateControllerBar()
{
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto children = host->GetChildren();
    if (layoutProperty->GetControlsValue(true)) {
        auto video = AceType::DynamicCast<VideoNode>(host);
        CHECK_NULL_VOID(video);
        auto controller = AceType::DynamicCast<FrameNode>(video->GetControllerRow());
        if (controller) {
            auto sliderNode = DynamicCast<FrameNode>(controller->GetChildAtIndex(SLIDER_POS));
            CHECK_NULL_VOID(sliderNode);
            auto sliderPattern = sliderNode->GetPattern<SliderPattern>();
            CHECK_NULL_VOID(sliderPattern);
            sliderPattern->UpdateValue(static_cast<float>(currentPos_));
            sliderNode->MarkModifyDone();

            auto textNode = DynamicCast<FrameNode>(controller->GetChildAtIndex(CURRENT_POS));
            CHECK_NULL_VOID(textNode);
            auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            std::string label = IntTimeToText(currentPos_);
            textLayoutProperty->UpdateContent(label);

            auto durationNode = DynamicCast<FrameNode>(controller->GetChildAtIndex(DURATION_POS));
            CHECK_NULL_VOID(durationNode);
            auto durationTextLayoutProperty = durationNode->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(durationTextLayoutProperty);
            std::string durationText = IntTimeToText(duration_);
            durationTextLayoutProperty->UpdateContent(durationText);

            textNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            textNode->MarkModifyDone();
            durationNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            durationNode->MarkModifyDone();
            auto controllerLayoutProperty = controller->GetLayoutProperty<LinearLayoutProperty>();
            controllerLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
            controller->MarkModifyDone();
        }
    } else {
        auto video = AceType::DynamicCast<VideoNode>(host);
        CHECK_NULL_VOID(video);
        auto controller = AceType::DynamicCast<FrameNode>(video->GetControllerRow());
        CHECK_NULL_VOID(controller);
        if (controller) {
            auto controllerLayoutProperty = controller->GetLayoutProperty<LinearLayoutProperty>();
            controllerLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
            controller->MarkModifyDone();
        }
    }
}

void VideoPattern::UpdateVideoProperty()
{
    if (isInitialState_ && autoPlay_) {
        Start();
    }

    UpdateSpeed();
    UpdateLooping();
    UpdateMuted();
}

void VideoPattern::OnRebuildFrame()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto video = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_VOID(video);
    auto column = AceType::DynamicCast<FrameNode>(video->GetMediaColumn());
    CHECK_NULL_VOID(column);
    auto renderContext = column->GetRenderContext();
    renderContext->AddChild(renderContextForMediaPlayer_, 0);
}

void VideoPattern::RemoveMediaPlayerSurfaceNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto video = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_VOID(video);
    auto column = AceType::DynamicCast<FrameNode>(video->GetMediaColumn());
    CHECK_NULL_VOID(column);
    auto renderContext = column->GetRenderContext();
    renderContext->RemoveChild(renderContextForMediaPlayer_);
}

bool VideoPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipMeasure || dirty->SkipMeasureContent()) {
        return false;
    }
    auto geometryNode = dirty->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, false);
    auto videoNodeSize = geometryNode->GetContentSize();
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, false);
    auto videoFrameSize = MeasureVideoContentLayout(videoNodeSize, layoutProperty);
    // Change the surface layout for drawing video frames
    if (renderContextForMediaPlayer_) {
        if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
            auto rect = AdjustPaintRect((videoNodeSize.Width() - videoFrameSize.Width()) / AVERAGE_VALUE,
                (videoNodeSize.Height() - videoFrameSize.Height()) / AVERAGE_VALUE, videoFrameSize.Width(),
                videoFrameSize.Height(), true);
            renderContextForMediaPlayer_->SetBounds(rect.GetX(), rect.GetY(), rect.Width(), rect.Height());
        } else {
            renderContextForMediaPlayer_->SetBounds((videoNodeSize.Width() - videoFrameSize.Width()) / AVERAGE_VALUE,
                (videoNodeSize.Height() - videoFrameSize.Height()) / AVERAGE_VALUE, videoFrameSize.Width(),
                videoFrameSize.Height());
        }
    }

    if (IsSupportImageAnalyzer()) {
        Rect tmpRect;
        auto padding  = layoutProperty->CreatePaddingAndBorder();
        auto imageFit = layoutProperty->GetObjectFitValue(ImageFit::COVER);
        if (imageFit == ImageFit::COVER || imageFit == ImageFit::NONE) {
            tmpRect = Rect(padding.left.value_or(0), padding.top.value_or(0),
                           videoNodeSize.Width(), videoNodeSize.Height());
        } else {
            tmpRect = Rect((videoNodeSize.Width() - videoFrameSize.Width()) / AVERAGE_VALUE + padding.left.value_or(0),
                (videoNodeSize.Height() - videoFrameSize.Height()) / AVERAGE_VALUE + padding.top.value_or(0),
                videoFrameSize.Width(), videoFrameSize.Height());
        }
        if (contentRect_ != tmpRect && ShouldUpdateImageAnalyzer()) {
            StartUpdateImageAnalyzer();
        }
        contentRect_ = tmpRect;
        UpdateAnalyzerUIConfig(geometryNode);
    }

    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    host->MarkNeedSyncRenderTree();
    auto video = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_RETURN(video, false);
    auto column = AceType::DynamicCast<FrameNode>(video->GetMediaColumn());
    CHECK_NULL_RETURN(column, false);
    column->GetRenderContext()->SetClipToBounds(true);
    return false;
}

void VideoPattern::OnAreaChangedInner()
{
    if (SystemProperties::GetExtSurfaceEnabled()) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto geometryNode = host->GetGeometryNode();
        CHECK_NULL_VOID(geometryNode);
        auto videoNodeSize = geometryNode->GetContentSize();
        auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
        CHECK_NULL_VOID(layoutProperty);
        auto videoFrameSize = MeasureVideoContentLayout(videoNodeSize, layoutProperty);
        auto transformRelativeOffset = host->GetTransformRelativeOffset();

        Rect rect =
            Rect(transformRelativeOffset.GetX() + (videoNodeSize.Width() - videoFrameSize.Width()) / AVERAGE_VALUE,
                transformRelativeOffset.GetY() + (videoNodeSize.Height() - videoFrameSize.Height()) / AVERAGE_VALUE,
                videoFrameSize.Width(), videoFrameSize.Height());
        if (renderSurface_ && (rect != lastBoundsRect_)) {
            renderSurface_->SetExtSurfaceBounds(rect.Left(), rect.Top(), rect.Width(), rect.Height());
            lastBoundsRect_ = rect;
        }
    }
}

void VideoPattern::OnColorConfigurationUpdate()
{
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipelineContext = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto videoTheme = pipelineContext->GetTheme<VideoTheme>();
    CHECK_NULL_VOID(videoTheme);
    auto renderContext = controlBar_->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateBackgroundColor(videoTheme->GetBkgColor());
    for (const auto& child : controlBar_->GetChildren()) {
        if (child->GetTag() == V2::TEXT_ETS_TAG) {
            auto frameNode = AceType::DynamicCast<FrameNode>(child);
            if (frameNode) {
                auto textLayoutProperty = frameNode->GetLayoutProperty<TextLayoutProperty>();
                if (textLayoutProperty) {
                    auto textStyle = videoTheme->GetTimeTextStyle();
                    textLayoutProperty->UpdateTextColor(textStyle.GetTextColor());
                }
            }
        }
    }
    host->SetNeedCallChildrenUpdate(false);
    host->MarkModifyDone();
    host->MarkDirtyNode();
}

bool VideoPattern::NeedLift() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, false);
    return IsFullScreen() && renderContext->IsUniRenderEnabled();
}

RefPtr<FrameNode> VideoPattern::CreateControlBar(int32_t nodeId)
{
    ContainerScope scope(instanceId_);
    auto pipelineContext = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    auto videoTheme = pipelineContext->GetTheme<VideoTheme>();
    CHECK_NULL_RETURN(videoTheme, nullptr);
    auto controlBar = FrameNode::GetOrCreateFrameNode(
        V2::ROW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    CHECK_NULL_RETURN(controlBar, nullptr);
    controlBar_ = controlBar;

    auto playButton = CreateSVG();
    CHECK_NULL_RETURN(playButton, nullptr);
    ChangePlayButtonTag(playButton);
    controlBar->AddChild(playButton);

    auto currentPosText = CreateText(currentPos_);
    CHECK_NULL_RETURN(currentPosText, nullptr);
    controlBar->AddChild(currentPosText);

    auto slider = CreateSlider();
    CHECK_NULL_RETURN(currentPosText, nullptr);
    controlBar->AddChild(slider);

    auto durationText = CreateText(duration_);
    CHECK_NULL_RETURN(durationText, nullptr);
    controlBar->AddChild(durationText);

    auto fullScreenButton = CreateSVG();
    CHECK_NULL_RETURN(fullScreenButton, nullptr);
    SetFullScreenButtonCallBack(fullScreenButton);
    ChangeFullScreenButtonTag(InstanceOf<VideoFullScreenNode>(this), fullScreenButton);
    controlBar->AddChild(fullScreenButton);

    auto renderContext = controlBar->GetRenderContext();
    renderContext->UpdateBackgroundColor(videoTheme->GetBkgColor());
    auto controlBarLayoutProperty = controlBar->GetLayoutProperty<LinearLayoutProperty>();
    controlBarLayoutProperty->UpdateMainAxisAlign(FlexAlign::SPACE_BETWEEN);
    if (NeedLift()) {
        PaddingProperty padding;
        padding.bottom = CalcLength(LIFT_HEIGHT);
        controlBarLayoutProperty->UpdatePadding(padding);
    }
    return controlBar;
}

RefPtr<FrameNode> VideoPattern::CreateSlider()
{
    auto pipelineContext = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    auto videoTheme = pipelineContext->GetTheme<VideoTheme>();
    CHECK_NULL_RETURN(videoTheme, nullptr);

    auto sliderNode = FrameNode::CreateFrameNode(V2::SLIDER_ETS_TAG, -1, AceType::MakeRefPtr<SliderPattern>());
    CHECK_NULL_RETURN(sliderNode, nullptr);
    auto sliderLayoutProperty = sliderNode->GetLayoutProperty<SliderLayoutProperty>();
    CHECK_NULL_RETURN(sliderLayoutProperty, nullptr);

    auto sliderEdge = videoTheme->GetSliderEdge();
    PaddingProperty padding;
    padding.left = CalcLength(sliderEdge.Left());
    padding.right = CalcLength(sliderEdge.Right());
    padding.top = CalcLength(sliderEdge.Top());
    padding.bottom = CalcLength(sliderEdge.Bottom());
    sliderLayoutProperty->UpdatePadding(padding);
    sliderLayoutProperty->UpdateLayoutWeight(1.0);

    SliderOnChangeEvent sliderOnChangeEvent = [weak = WeakClaim(this)](float value, int32_t mode) {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        videoPattern->OnSliderChange(value, mode);
    };
    auto sliderEventHub = sliderNode->GetEventHub<SliderEventHub>();
    sliderEventHub->SetOnChange(std::move(sliderOnChangeEvent));
    if (InstanceOf<VideoFullScreenPattern>(this)) {
        auto focusHub = sliderNode->GetOrCreateFocusHub();
        focusHub->SetIsDefaultFocus(true);
    }

    auto sliderPaintProperty = sliderNode->GetPaintProperty<SliderPaintProperty>();
    CHECK_NULL_RETURN(sliderPaintProperty, nullptr);
    sliderPaintProperty->UpdateMax(static_cast<float>(duration_));
    sliderPaintProperty->UpdateSelectColor(videoTheme->GetSelectColor());
    sliderPaintProperty->UpdateTrackBackgroundColor(ConvertToGradient(videoTheme->GetTrackBgColor()));
    sliderPaintProperty->UpdateTrackBackgroundIsResourceColor(true);
    sliderPaintProperty->UpdateValue(static_cast<float>(currentPos_));
    sliderNode->MarkModifyDone();
    return sliderNode;
}

RefPtr<FrameNode> VideoPattern::CreateText(uint32_t time)
{
    auto pipelineContext = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    auto videoTheme = pipelineContext->GetTheme<VideoTheme>();
    CHECK_NULL_RETURN(videoTheme, nullptr);

    auto textNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, -1, AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(textNode, nullptr);
    auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, nullptr);
    auto videoLayoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_RETURN(videoLayoutProperty, nullptr);
    std::string label = "";
    if (videoLayoutProperty->GetControlsValue(true)) {
        label = IntTimeToText(time);
    }
    textLayoutProperty->UpdateContent(label);
    auto textEdge = videoTheme->GetTextEdge();
    PaddingProperty padding;
    padding.left = CalcLength(textEdge.Left());
    padding.right = CalcLength(textEdge.Right());
    padding.top = CalcLength(textEdge.Top());
    padding.bottom = CalcLength(textEdge.Bottom());
    textLayoutProperty->UpdatePadding(padding);
    auto textStyle = videoTheme->GetTimeTextStyle();
    textLayoutProperty->UpdateFontSize(textStyle.GetFontSize());
    textLayoutProperty->UpdateTextColor(textStyle.GetTextColor());
    return textNode;
}

RefPtr<FrameNode> VideoPattern::CreateSVG()
{
    auto pipelineContext = GetHost()->GetContext();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    auto videoTheme = pipelineContext->GetTheme<VideoTheme>();
    CHECK_NULL_RETURN(videoTheme, nullptr);

    auto svgNode = FrameNode::CreateFrameNode(V2::IMAGE_ETS_TAG, -1, AceType::MakeRefPtr<ImagePattern>());
    CHECK_NULL_RETURN(svgNode, nullptr);

    auto imageRenderProperty = svgNode->GetPaintPropertyPtr<ImageRenderProperty>();
    imageRenderProperty->UpdateSvgFillColor(videoTheme->GetIconColor());
    auto renderContext = svgNode->GetRenderContext();
    renderContext->UpdateForegroundColor(videoTheme->GetIconColor());

    auto svgLayoutProperty = svgNode->GetLayoutProperty<ImageLayoutProperty>();

    auto btnEdge = videoTheme->GetBtnEdge();
    PaddingProperty padding;
    padding.left = CalcLength(btnEdge.Left());
    padding.right = CalcLength(btnEdge.Right());
    padding.top = CalcLength(btnEdge.Top());
    padding.bottom = CalcLength(btnEdge.Bottom());
    svgLayoutProperty->UpdatePadding(padding);

    auto btnSize = videoTheme->GetBtnSize();
    SizeF size { static_cast<float>(btnSize.Width()), static_cast<float>(btnSize.Height()) };
    svgLayoutProperty->UpdateMarginSelfIdealSize(size);
    auto width = Dimension(btnSize.Width(), DimensionUnit::VP).ConvertToPx();
    auto height = Dimension(btnSize.Height(), DimensionUnit::VP).ConvertToPx();
    CalcSize idealSize = { CalcLength(width), CalcLength(height) };
    MeasureProperty layoutConstraint;
    layoutConstraint.selfIdealSize = idealSize;
    layoutConstraint.maxSize = idealSize;
    svgNode->UpdateLayoutConstraint(layoutConstraint);
    return svgNode;
}

void VideoPattern::SetStartImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetStartImpl([weak = WeakClaim(this), uiTaskExecutor]() {
        uiTaskExecutor.PostTask([weak]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            ContainerScope scope(pattern->instanceId_);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            targetPattern->Start();
            }, "ArkUIVideoStart");
    });
}

void VideoPattern::SetPausetImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetPausetImpl([weak = WeakClaim(this), uiTaskExecutor]() {
        uiTaskExecutor.PostTask([weak]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            ContainerScope scope(pattern->instanceId_);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            targetPattern->Pause();
            }, "ArkUIVideoPause");
    });
}

void VideoPattern::SetStopImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetStopImpl([weak = WeakClaim(this), uiTaskExecutor]() {
        uiTaskExecutor.PostTask([weak]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            ContainerScope scope(pattern->instanceId_);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            targetPattern->Stop();
            }, "ArkUIVideoStop");
    });
}

void VideoPattern::SetSeekToImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetSeekToImpl([weak = WeakClaim(this), uiTaskExecutor](float pos, SeekMode seekMode) {
        uiTaskExecutor.PostTask([weak, pos, seekMode]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            ContainerScope scope(pattern->instanceId_);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            targetPattern->SetCurrentTime(pos, seekMode);
            }, "ArkUIVideoSetCurrentTime");
    });
}

void VideoPattern::SetRequestFullscreenImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetRequestFullscreenImpl([weak = WeakClaim(this), uiTaskExecutor](bool isFullScreen) {
        uiTaskExecutor.PostTask([weak, isFullScreen]() {
            auto videoPattern = weak.Upgrade();
            CHECK_NULL_VOID(videoPattern);
            ContainerScope scope(videoPattern->instanceId_);
            if (isFullScreen) {
                videoPattern->FullScreen();
            } else {
                videoPattern->ResetLastBoundsRect();
                auto targetPattern = videoPattern->GetTargetVideoPattern();
                CHECK_NULL_VOID(targetPattern);
                auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(targetPattern);
                CHECK_NULL_VOID(fullScreenPattern);
                fullScreenPattern->ExitFullScreen();
            }
            }, "ArkUIVideoFullScreen");
    });
}

void VideoPattern::SetExitFullscreenImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetExitFullscreenImpl([weak = WeakClaim(this), uiTaskExecutor](bool isSync) {
        if (isSync) {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            pattern->ResetLastBoundsRect();
            auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(targetPattern);
            fullScreenPattern->ExitFullScreen();
            return;
        }
        uiTaskExecutor.PostTask([weak]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            ContainerScope scope(pattern->instanceId_);
            pattern->ResetLastBoundsRect();
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(targetPattern);
            CHECK_NULL_VOID(fullScreenPattern);
            fullScreenPattern->ExitFullScreen();
            }, "ArkUIVideoExitFullScreen");
    });
}

void VideoPattern::SetResetImpl(
    const RefPtr<VideoController>& videoController, const SingleTaskExecutor& uiTaskExecutor)
{
    videoController->SetResetImpl([weak = WeakClaim(this), uiTaskExecutor]() {
        uiTaskExecutor.PostTask([weak]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            auto targetPattern = pattern->GetTargetVideoPattern();
            CHECK_NULL_VOID(targetPattern);
            targetPattern->ResetMediaPlayer();
            }, "ArkUIVideoReset");
    });
}

void VideoPattern::SetMethodCall()
{
    ContainerScope scope(instanceId_);
    auto videoController = AceType::MakeRefPtr<VideoController>();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);

    SetStartImpl(videoController, uiTaskExecutor);
    SetPausetImpl(videoController, uiTaskExecutor);
    SetStopImpl(videoController, uiTaskExecutor);
    SetSeekToImpl(videoController, uiTaskExecutor);
    SetRequestFullscreenImpl(videoController, uiTaskExecutor);
    SetExitFullscreenImpl(videoController, uiTaskExecutor);
    SetResetImpl(videoController, uiTaskExecutor);

    CHECK_NULL_VOID(videoControllerV2_);
    videoControllerV2_->AddVideoController(videoController);
}

void VideoPattern::Start()
{
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
        return;
    }

    if (isStop_ && mediaPlayer_->PrepareAsync() != 0) {
        TAG_LOGW(AceLogTag::ACE_VIDEO, "Player has not prepared");
        return;
    }
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);

    DestroyAnalyzerOverlay();
    isPaused_ = false;

    auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
    platformTask.PostTask([weak = WeakClaim(RawPtr(mediaPlayer_))] {
        auto mediaPlayer = weak.Upgrade();
        CHECK_NULL_VOID(mediaPlayer);
        TAG_LOGI(AceLogTag::ACE_VIDEO, "trigger mediaPlayer play");
        mediaPlayer->Play();
        }, "ArkUIVideoPlay");
}

void VideoPattern::Pause()
{
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
        return;
    }
    auto ret = mediaPlayer_->Pause();
    if (ret != -1 && !isPaused_) {
        isPaused_ = true;
        StartImageAnalyzer();
    }
}

void VideoPattern::Stop()
{
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid()) {
        return;
    }

    OnCurrentTimeChange(0);
    mediaPlayer_->Stop();
    isStop_ = true;
    SetIsSeeking(false);
}

void VideoPattern::FireError()
{
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);

    // OnError function must be excuted on ui, so get the uiTaskExecutor.
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    uiTaskExecutor.PostTask([weak = WeakClaim(this)] {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        ContainerScope scope(videoPattern->instanceId_);
        videoPattern->OnError("");
        }, "ArkUIVideoError");
}

void VideoPattern::ChangePlayButtonTag()
{
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    const auto& children = host->GetChildren();
    for (const auto& child : children) {
        if (child->GetTag() == V2::ROW_ETS_TAG) {
            auto playBtn = DynamicCast<FrameNode>(child->GetChildAtIndex(0));
            ChangePlayButtonTag(playBtn);
            break;
        }
    }
}

void VideoPattern::ChangePlayButtonTag(RefPtr<FrameNode>& playBtn)
{
    CHECK_NULL_VOID(playBtn);
    auto playClickCallback = [weak = WeakClaim(this), playing = isPlaying_](GestureEvent& /* info */) {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        if (playing) {
            videoPattern->Pause();
        } else {
            videoPattern->Start();
        }
    };
    auto playBtnEvent = playBtn->GetOrCreateGestureEventHub();
    playBtnEvent->SetUserOnClick(std::move(playClickCallback));
    auto svgLayoutProperty = playBtn->GetLayoutProperty<ImageLayoutProperty>();
    auto resourceId = isPlaying_ ? InternalResource::ResourceId::PAUSE_SVG : InternalResource::ResourceId::PLAY_SVG;
    auto svgSourceInfo = ImageSourceInfo("");
    svgSourceInfo.SetResourceId(resourceId);
    svgLayoutProperty->UpdateImageSourceInfo(svgSourceInfo);
    playBtn->MarkModifyDone();
    playBtn->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void VideoPattern::SetFullScreenButtonCallBack(RefPtr<FrameNode>& fullScreenBtn)
{
    CHECK_NULL_VOID(fullScreenBtn);
    auto fsClickCallback = [weak = WeakClaim(this)](GestureEvent& /* info */) {
        auto videoPattern = weak.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        if (InstanceOf<VideoFullScreenPattern>(videoPattern)) {
            auto pattern = AceType::DynamicCast<VideoFullScreenPattern>(videoPattern);
            CHECK_NULL_VOID(pattern);
            videoPattern->ResetLastBoundsRect();
            pattern->ExitFullScreen();
        } else {
            videoPattern->FullScreen();
        }
    };
    auto fullScreenBtnEvent = fullScreenBtn->GetOrCreateGestureEventHub();
    fullScreenBtnEvent->SetUserOnClick(std::move(fsClickCallback));
}

void VideoPattern::ChangeFullScreenButtonTag(bool isFullScreen, RefPtr<FrameNode>& fullScreenBtn)
{
    CHECK_NULL_VOID(fullScreenBtn);
    auto svgLayoutProperty = fullScreenBtn->GetLayoutProperty<ImageLayoutProperty>();
    auto resourceId =
        isFullScreen ? InternalResource::ResourceId::QUIT_FULLSCREEN_SVG : InternalResource::ResourceId::FULLSCREEN_SVG;
    auto svgSourceInfo = ImageSourceInfo("");
    svgSourceInfo.SetResourceId(resourceId);
    svgLayoutProperty->UpdateImageSourceInfo(svgSourceInfo);
    fullScreenBtn->MarkModifyDone();
    fullScreenBtn->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void VideoPattern::SetCurrentTime(float currentPos, OHOS::Ace::SeekMode seekMode)
{
    if (!mediaPlayer_ || !mediaPlayer_->IsMediaPlayerValid() || !isPrepared_) {
        return;
    }
    if (GreatOrEqual(currentPos, 0.0)) {
        SetIsSeeking(true);
        mediaPlayer_->Seek(static_cast<int32_t>(currentPos * MILLISECONDS_TO_SECONDS), seekMode);
    }
}

void VideoPattern::OnSliderChange(float posTime, int32_t mode)
{
    SetCurrentTime(posTime, OHOS::Ace::SeekMode::SEEK_CLOSEST);
    auto eventHub = GetEventHub<VideoEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto json = JsonUtil::Create(true);
    json->Put("time", static_cast<double>(posTime));
    auto param = json->ToString();
    CHECK_NULL_VOID(eventHub);
    if (mode == SliderChangeMode::BEGIN || mode == SliderChangeMode::MOVING) {
        eventHub->FireSeekingEvent(param);
    } else if (mode == SliderChangeMode::END) {
        eventHub->FireSeekedEvent(param);
    }
}

void VideoPattern::OnFullScreenChange(bool isFullScreen)
{
    auto json = JsonUtil::Create(true);
    json->Put("fullscreen", isFullScreen);
    auto param = json->ToString();
    auto eventHub = GetEventHub<VideoEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireFullScreenChangeEvent(param);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    const auto& children = host->GetChildren();
    for (const auto& child : children) {
        if (child->GetTag() == V2::ROW_ETS_TAG) {
            auto fsBtn = DynamicCast<FrameNode>(child->GetChildAtIndex(FULL_SCREEN_POS));
            ChangeFullScreenButtonTag(isFullScreen, fsBtn);
            break;
        }
    }

    if (!isFullScreen && mediaPlayer_ && mediaPlayer_->IsMediaPlayerValid()) {
        auto videoLayoutProperty = host->GetLayoutProperty<VideoLayoutProperty>();
        CHECK_NULL_VOID(videoLayoutProperty);
        SizeF videoSize = SizeF(
            static_cast<float>(mediaPlayer_->GetVideoWidth()),
            static_cast<float>(mediaPlayer_->GetVideoHeight()));
        videoLayoutProperty->UpdateVideoSize(videoSize);
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }

    if (isEnableAnalyzer_) {
        if (!imageAnalyzerManager_) {
            EnableAnalyzer(isEnableAnalyzer_);
        }
        if (imageAnalyzerManager_ && isAnalyzerCreated_) {
            StartImageAnalyzer();
        }
    }

    if (!SystemProperties::GetExtSurfaceEnabled()) {
        return;
    }
    if (!fullScreenNodeId_.has_value()) {
        SetMediaFullScreen(isFullScreen);
        return;
    }
    auto fullScreenNode = FrameNode::GetFrameNode(V2::VIDEO_ETS_TAG, fullScreenNodeId_.value());
    CHECK_NULL_VOID(fullScreenNode);
    auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(fullScreenNode->GetPattern());
    CHECK_NULL_VOID(fullScreenPattern);
    fullScreenPattern->SetMediaFullScreen(isFullScreen);
}

void VideoPattern::FullScreen()
{
    if (fullScreenNodeId_.has_value()) {
        return;
    }
    ResetLastBoundsRect();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto videoNode = AceType::DynamicCast<VideoNode>(host);
    CHECK_NULL_VOID(videoNode);
    auto fullScreenPattern = AceType::MakeRefPtr<VideoFullScreenPattern>(videoControllerV2_);
    fullScreenPattern->InitFullScreenParam(
        AceType::Claim(this), renderSurface_, mediaPlayer_, renderContextForMediaPlayer_);
    fullScreenNodeId_ = ElementRegister::GetInstance()->MakeUniqueId();
    auto fullScreenNode =
        VideoFullScreenNode::CreateFullScreenNode(V2::VIDEO_ETS_TAG, fullScreenNodeId_.value(), fullScreenPattern);
    CHECK_NULL_VOID(fullScreenNode);
    fullScreenPattern->RequestFullScreen(videoNode);
}

void VideoPattern::EnableDrag()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    auto dragEnd = [wp = WeakClaim(this)](
                       const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& extraParams) {
        auto videoPattern = wp.Upgrade();
        CHECK_NULL_VOID(videoPattern);
        auto videoLayoutProperty = videoPattern->GetLayoutProperty<VideoLayoutProperty>();
        CHECK_NULL_VOID(videoLayoutProperty);
        CHECK_NULL_VOID(event);
        auto unifiedData = event->GetData();
        std::string videoSrc;
        if (unifiedData != nullptr) {
            int ret = UdmfClient::GetInstance()->GetVideoRecordUri(unifiedData, videoSrc);
            if (ret != 0) {
                TAG_LOGW(AceLogTag::ACE_VIDEO, "unifiedRecords is empty");
                return;
            }
        } else {
            auto json = JsonUtil::ParseJsonString(extraParams);
            std::string key = "extraInfo";
            videoSrc = json->GetString(key);
        }

        if (videoSrc == videoPattern->GetSrc()) {
            return;
        }

        std::regex extensionRegex("\\.(" + PNG_FILE_EXTENSION + ")$");
        bool isPng = std::regex_search(videoSrc, extensionRegex);
        if (isPng) {
            event->SetResult(DragRet::DRAG_FAIL);
            return;
        }

        videoPattern->SetIsDragEndAutoPlay(true);
        VideoSourceInfo videoSrcInfo = {videoSrc, "", ""};
        videoLayoutProperty->UpdateVideoSource(videoSrcInfo);
        auto frameNode = videoPattern->GetHost();
        CHECK_NULL_VOID(frameNode);
        frameNode->MarkModifyDone();
    };
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnDrop(std::move(dragEnd));
}

VideoPattern::~VideoPattern()
{
#ifdef RENDER_EXTRACT_SUPPORTED
    if (renderContextForMediaPlayer_) {
        renderContextForMediaPlayer_->RemoveSurfaceChangedCallBack();
    }
#endif
    if (IsSupportImageAnalyzer()) {
        DestroyAnalyzerOverlay();
    }
    if (!fullScreenNodeId_.has_value()) {
        return;
    }
    auto fullScreenNode = FrameNode::GetFrameNode(V2::VIDEO_ETS_TAG, fullScreenNodeId_.value());
    CHECK_NULL_VOID(fullScreenNode);
    auto parent = fullScreenNode->GetParent();
    CHECK_NULL_VOID(parent);
    parent->RemoveChild(fullScreenNode);
    parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void VideoPattern::RecoverState(const RefPtr<VideoPattern>& videoPattern)
{
    CHECK_NULL_VOID(videoPattern);
    currentPos_ = videoPattern->GetCurrentPos();
    if (mediaPlayer_ && mediaPlayer_->IsMediaPlayerValid() && mediaPlayer_->IsPlaying() != isPlaying_) {
        isPlaying_ = mediaPlayer_->IsPlaying();
        ChangePlayButtonTag();
    }
    isInitialState_ = videoPattern->GetInitialState();
    auto layoutProperty = videoPattern->GetLayoutProperty<VideoLayoutProperty>();
    auto videoSrcInfo = layoutProperty->GetVideoSource();
    videoSrcInfo_.src = videoSrcInfo->GetSrc();
    videoSrcInfo_.bundleName = videoSrcInfo->GetBundleName();
    videoSrcInfo_.moduleName = videoSrcInfo->GetModuleName();
    isPrepared_ = videoPattern->GetIsPrepared();
    isSeeking_ = videoPattern->GetIsSeeking();
    isStop_ = videoPattern->GetIsStop();
    muted_ = videoPattern->GetMuted();
    autoPlay_ = videoPattern->GetAutoPlay();
    loop_ = videoPattern->GetLoop();
    duration_ = videoPattern->GetDuration();
    progressRate_ = videoPattern->GetProgressRate();
    isAnalyzerCreated_ = videoPattern->GetAnalyzerState();
    isEnableAnalyzer_ = videoPattern->isEnableAnalyzer_;
    fullScreenNodeId_.reset();
    RegisterMediaPlayerEvent();
    auto videoNode = GetHost();
    CHECK_NULL_VOID(videoNode);
    // change event hub to the origin video node
    videoPattern->GetEventHub<VideoEventHub>()->AttachHost(videoNode);
    videoNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
}

void VideoPattern::UpdateFsState()
{
    if (!fullScreenNodeId_.has_value()) {
        return;
    }
    auto videoNode = FrameNode::GetFrameNode(V2::VIDEO_ETS_TAG, fullScreenNodeId_.value());
    CHECK_NULL_VOID(videoNode);
    auto videoPattern = AceType::DynamicCast<VideoFullScreenPattern>(videoNode->GetPattern());
    CHECK_NULL_VOID(videoPattern);
    // update full screen state
    videoPattern->UpdateState();
}

bool VideoPattern::IsFullScreen() const
{
    return fullScreenNodeId_.has_value();
}

RefPtr<VideoPattern> VideoPattern::GetTargetVideoPattern()
{
    auto isFullScreen = IsFullScreen();
    auto patternIsFullScreen = AceType::InstanceOf<VideoFullScreenPattern>(this);
    if ((isFullScreen && patternIsFullScreen) || (!isFullScreen && !patternIsFullScreen)) {
        return AceType::Claim(this);
    }
    if (patternIsFullScreen) {
        // current is full screen,need to be released
        auto fullScreenPattern = AceType::DynamicCast<VideoFullScreenPattern>(this);
        CHECK_NULL_RETURN(fullScreenPattern, nullptr);
        return fullScreenPattern->GetVideoPattern();
    }
    // current node is origin video node, need to operate full screen node
    auto fullScreenNode = GetFullScreenNode();
    CHECK_NULL_RETURN(fullScreenNode, nullptr);
    return fullScreenNode->GetPattern<VideoPattern>();
}

void VideoPattern::EnableAnalyzer(bool enable)
{
    isEnableAnalyzer_ = enable;
    if (!isEnableAnalyzer_) {
        DestroyAnalyzerOverlay();
        return;
    }

    CHECK_NULL_VOID(!imageAnalyzerManager_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    imageAnalyzerManager_ = std::make_shared<ImageAnalyzerManager>(host, ImageAnalyzerHolder::VIDEO_CUSTOM);
}

void VideoPattern::SetImageAnalyzerConfig(void* config)
{
    if (isEnableAnalyzer_) {
        CHECK_NULL_VOID(imageAnalyzerManager_);
        imageAnalyzerManager_->SetImageAnalyzerConfig(config);
    }
}

void VideoPattern::SetImageAIOptions(void* options)
{
    if (!imageAnalyzerManager_) {
        imageAnalyzerManager_ = std::make_shared<ImageAnalyzerManager>(GetHost(), ImageAnalyzerHolder::VIDEO_CUSTOM);
    }
    CHECK_NULL_VOID(imageAnalyzerManager_);
    imageAnalyzerManager_->SetImageAIOptions(options);
}

bool VideoPattern::IsSupportImageAnalyzer()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto layoutProperty = host->GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, false);
    bool needControlBar = layoutProperty->GetControlsValue(true);
    CHECK_NULL_RETURN(imageAnalyzerManager_, false);
    return isEnableAnalyzer_ && !needControlBar && imageAnalyzerManager_->IsSupportImageAnalyzerFeature();
}

bool VideoPattern::ShouldUpdateImageAnalyzer() {
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, false);
    const auto& constraint = layoutProperty->GetCalcLayoutConstraint();
    if (!constraint || !constraint->selfIdealSize.has_value() || !constraint->selfIdealSize->IsValid()) {
        return false;
    }
    auto selfIdealSize = constraint->selfIdealSize;
    if (!selfIdealSize->PercentWidth() && !selfIdealSize->PercentHeight()) {
        return false;
    }
    auto imageFit = layoutProperty->GetObjectFit().value_or(ImageFit::COVER);
    if (imageFit != ImageFit::COVER && imageFit != ImageFit::NONE) {
        return false;
    }
    return true;
}

void VideoPattern::StartImageAnalyzer()
{
    if (!IsSupportImageAnalyzer() || !imageAnalyzerManager_) {
        return;
    }

    if (imageAnalyzerManager_->IsOverlayCreated()) {
        DestroyAnalyzerOverlay();
    }

    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    uiTaskExecutor.PostDelayedTask([weak = WeakClaim(this)] {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->CreateAnalyzerOverlay();
        }, ANALYZER_DELAY_TIME, "ArkUIVideoCreateAnalyzerOverlay");
}

void VideoPattern::CreateAnalyzerOverlay()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->SetOverlayNode(nullptr);
    auto context = host->GetRenderContext();
    CHECK_NULL_VOID(context);
    auto nailPixelMap = context->GetThumbnailPixelMap();
    CHECK_NULL_VOID(nailPixelMap);
    auto pixelMap = nailPixelMap->GetCropPixelMap(contentRect_);
    CHECK_NULL_VOID(pixelMap);
    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto padding  = layoutProperty->CreatePaddingAndBorder();
    OffsetF contentOffset = { contentRect_.Left() - padding.left.value_or(0),
                              contentRect_.Top() - padding.top.value_or(0) };
    CHECK_NULL_VOID(imageAnalyzerManager_);
    imageAnalyzerManager_->CreateAnalyzerOverlay(pixelMap, contentOffset);
}

void VideoPattern::StartUpdateImageAnalyzer()
{
    CHECK_NULL_VOID(imageAnalyzerManager_);
    if (!imageAnalyzerManager_->IsOverlayCreated()) {
        return;
    }

    UpdateOverlayVisibility(VisibleType::GONE);
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    uiTaskExecutor.PostDelayedTask([weak = WeakClaim(this)] {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (!pattern->isContentSizeChanged_) {
            return;
        }
        pattern->UpdateAnalyzerOverlay();
        pattern->isContentSizeChanged_ = false;
        }, ANALYZER_CAPTURE_DELAY_TIME, "ArkUIVideoUpdateAnalyzerOverlay");
    isContentSizeChanged_ = true;
}

void VideoPattern::UpdateAnalyzerOverlay()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetRenderContext();
    CHECK_NULL_VOID(context);
    auto nailPixelMap = context->GetThumbnailPixelMap();
    CHECK_NULL_VOID(nailPixelMap);
    auto pixelMap = nailPixelMap->GetCropPixelMap(contentRect_);
    CHECK_NULL_VOID(pixelMap);
    UpdateOverlayVisibility(VisibleType::VISIBLE);

    auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto padding  = layoutProperty->CreatePaddingAndBorder();
    OffsetF contentOffset = { contentRect_.Left() - padding.left.value_or(0),
                              contentRect_.Top() - padding.top.value_or(0) };
    CHECK_NULL_VOID(imageAnalyzerManager_);
    imageAnalyzerManager_->UpdateAnalyzerOverlay(pixelMap, contentOffset);
}

void VideoPattern::UpdateAnalyzerUIConfig(const RefPtr<NG::GeometryNode>& geometryNode)
{
    if (IsSupportImageAnalyzer()) {
        auto layoutProperty = GetLayoutProperty<VideoLayoutProperty>();
        CHECK_NULL_VOID(layoutProperty);
        auto padding  = layoutProperty->CreatePaddingAndBorder();
        OffsetF contentOffset = { contentRect_.Left() - padding.left.value_or(0),
                                  contentRect_.Top() - padding.top.value_or(0) };
        PixelMapInfo info = { contentRect_.GetSize().Width(), contentRect_.GetSize().Height(), contentOffset };
        CHECK_NULL_VOID(imageAnalyzerManager_);
        imageAnalyzerManager_->UpdateAnalyzerUIConfig(geometryNode, info);
    }
}

void VideoPattern::DestroyAnalyzerOverlay()
{
    CHECK_NULL_VOID(imageAnalyzerManager_);
    imageAnalyzerManager_->DestroyAnalyzerOverlay();
}

bool VideoPattern::GetAnalyzerState()
{
    CHECK_NULL_RETURN(imageAnalyzerManager_, false);
    return imageAnalyzerManager_->IsOverlayCreated();
}

void VideoPattern::UpdateOverlayVisibility(VisibleType type)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto overlayNode = host->GetOverlayNode();
    CHECK_NULL_VOID(overlayNode);
    auto prop = overlayNode->GetLayoutProperty();
    CHECK_NULL_VOID(prop);
    prop->UpdateVisibility(type);
}

void VideoPattern::OnWindowHide()
{
#if defined(OHOS_PLATFORM)
    if (!BackgroundTaskHelper::GetInstance().HasBackgroundTask()) {
        autoPlay_ = false;
        Pause();
    }
#else
    Pause();
#endif
}

} // namespace OHOS::Ace::NG
