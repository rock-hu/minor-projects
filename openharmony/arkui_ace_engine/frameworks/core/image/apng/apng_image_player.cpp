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

#include "core/image/apng/apng_image_player.h"

#include <cstdlib>
#include <cstdio>
#include "base/log/log.h"
#include "core/image/image_provider.h"
#ifndef USE_ROSEN_DRAWING
#include "include/codec/SkCodecAnimation.h"
#include "include/core/SkPixelRef.h"
#endif

namespace OHOS::Ace {
APngImagePlayer::APngImagePlayer(
    ImageSourceInfo source,
    UploadSuccessCallback successCallback,
    const WeakPtr<PipelineBase>& weakContext,
    const RefPtr<PNGImageDecoder>& decoder,
    int32_t dstWidth,
    int32_t dstHeight)
    : imageSource_(source), successCallback_(successCallback), context_(weakContext),
      apngDecoder_(decoder), dstWidth_(dstWidth), dstHeight_(dstHeight)
{
    auto context = context_.Upgrade();
    if (context) {
        // decode apng
        if (!apngDecoder_ || !apngDecoder_->isApng()) {
            LOGE("APNG Image Player png decoder error!");
            return;
        }

        // get apng all frames, before use decoder need Use GetAllFrames before
        frameCount_ = apngDecoder_->GetFrameCount();
        if (!GetApngAllFrames()) {
            return;
        }

        auto pictureAnimation = AceType::MakeRefPtr<APngImageAnimation>();
        float totalFrameDuration = 0.0f;

        for (int32_t index = 0; index < frameCount_; index++) {
            // if frame duration is 0, set this frame duration as 100ms
            if (frameInfos_[index].duration <= 0) {
                frameInfos_[index].duration = 0;
            }

            totalFrameDuration += frameInfos_[index].duration;
        }

        for (int32_t index = 0; index < frameCount_; index++) {
            pictureAnimation->AddPicture(frameInfos_[index].duration, index);
        }

        pictureAnimation->AddListener([weak = WeakClaim(this)](const int32_t &index) {
            auto player = weak.Upgrade();
            if (player) {
                player->RenderFrame(index);
            }
        });

        pictureAnimation_ = pictureAnimation;

        auto &&callback = [weak = AceType::WeakClaim(this)](uint64_t duration) {
            auto controller = weak.Upgrade();
            if (!controller) {
                LOGE("controller is nullptr, skip frame callback.");
                return;
            }

            controller->pictureAnimation_->OnNormalizedTimestampChanged(duration, false);
        };

        scheduler_ = SchedulerBuilder::Build(callback, context);
        scheduler_->Start();
    }
}

APngImagePlayer::~APngImagePlayer()
{
    if (blendCanvas_) {
        delete blendCanvas_;
    }

    scheduler_->Stop();
}

/**
 * convert fraction to double value
 * @param num
 * @param den
 * @return
 */
const float DefaultDen = 100.0;
const float SecondToMinSec = 1000;

float APngImagePlayer::DelayToSeconds(uint16_t num, uint16_t den)
{
    if (den == 0) {
        return num / DefaultDen * SecondToMinSec;
    } else {
        return static_cast<float>(num) / static_cast<float>(den) * SecondToMinSec;
    }
}

/**
 * decoder apng all frame head information
 * need use at first
 */

bool APngImagePlayer::GetApngAllFrames()
{
    auto pngInfo = apngDecoder_->GetApngInfo();
    if (!pngInfo) {
        return false;
    }

    bool needBlend = false;
    uint32_t lastBlendIndex = 0;
    uint32_t canvasWidth = pngInfo->header.width;
    uint32_t canvasHeight = pngInfo->header.height;
    frameInfos_.clear();

    for (uint32_t i = 0; i < pngInfo->apngFrameNum; i++) {
        APngAnimatedFrameInfo frame;
        PngFrameInfo *frameInfo = pngInfo->apngFrames + i;
        frame.index = i;
        frame.duration = DelayToSeconds(frameInfo->frameControl.delayNum, frameInfo->frameControl.delayDen);
        frame.width = frameInfo->frameControl.width;
        frame.height = frameInfo->frameControl.height;
        frame.offsetX = frameInfo->frameControl.xOffset;
        frame.offsetY = frameInfo->frameControl.yOffset;

        bool sizeEqualsToCanvas = (frame.width == canvasWidth && frame.height == canvasHeight);
        bool offsetIsZero = (frameInfo->frameControl.xOffset == 0 && frameInfo->frameControl.yOffset == 0);
        frame.isFullSize = (sizeEqualsToCanvas && offsetIsZero);

        switch (frameInfo->frameControl.disposeOp) {
            case PNG_DISPOSE_OP_BACKGROUND: {
                frame.dispose = ImageDisposeBackground;
                break;
            }
            case PNG_DISPOSE_OP_PREVIOUS: {
                frame.dispose = ImageDisposePrevious;
                break;
            }
            default: {
                frame.dispose = ImageDisposeNone;
                break;
            }
        }

        switch (frameInfo->frameControl.blendOp) {
            case PNG_BLEND_OP_OVER: {
                frame.blend = ImageBlendOver;
                break;
            }
            default: {
                frame.blend = ImageBlendNone;
                break;
            }
        }

        if (frame.blend == ImageBlendNone && frame.isFullSize) {
            frame.blendFromIndex = i;
            if (frame.dispose != ImageDisposePrevious) {
                lastBlendIndex = i;
            }
        } else {
            if (frame.dispose == ImageDisposeBackground && frame.isFullSize) {
                frame.blendFromIndex = lastBlendIndex;
                lastBlendIndex = i + 1;
            } else {
                frame.blendFromIndex = lastBlendIndex;
            }
        }

        if (frame.index != frame.blendFromIndex) {
            needBlend = true;
        }

        frameInfos_.push_back(frame);
    }

    repetitionCount_ = pngInfo->apngLoopNum;
    needBlend_ = needBlend;
    width_ = canvasWidth;
    height_ = canvasHeight;

    return true;
}

void APngImagePlayer::Pause()
{
    if (scheduler_ && scheduler_->IsActive()) {
        scheduler_->Stop();
    }
}

void APngImagePlayer::Resume()
{
    if (scheduler_ && !scheduler_->IsActive()) {
        scheduler_->Start();
    }
}

void APngImagePlayer::RenderFrame(const int32_t& index)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("Context may be destroyed!");
        return;
    }

    auto taskExecutor = context->GetTaskExecutor();
    taskExecutor->PostTask(
        [weak = AceType::WeakClaim(this), index, dstWidth = dstWidth_, dstHeight = dstHeight_, taskExecutor] {
            auto player = weak.Upgrade();
            if (!player) {
                return;
            }

            APngAnimatedFrameInfo* frameInfo = player->DecodeFrameImage(index);
            if (!frameInfo || !frameInfo->image) {
                return;
            }

#ifndef USE_ROSEN_DRAWING
            sk_sp<SkImage> skImage = frameInfo->image;
            if (dstWidth > 0 && dstHeight > 0) {
                skImage = ImageProvider::ApplySizeToSkImage(skImage, dstWidth, dstHeight);
            }
            if (!skImage) {
                LOGW("animated player cannot get the %{public}d skImage!", index);
                return;
            }
            auto canvasImage = NG::CanvasImage::Create(&skImage);
#else
            std::shared_ptr<RSImage> dImage = frameInfo->image;
            if (dstWidth > 0 && dstHeight > 0) {
                dImage = ImageProvider::ApplySizeToDrawingImage(dImage, dstWidth, dstHeight);
            }
            if (!dImage) {
                LOGW("animated player cannot get the %{public}d dImage!", index);
                return;
            }
            auto canvasImage = NG::CanvasImage::Create(&dImage);
#endif

            taskExecutor->PostTask([callback = player->successCallback_, canvasImage,
                                       source = player->imageSource_] { callback(source, canvasImage); },
                TaskExecutor::TaskType::UI, "ArkUIImageAPngRenderSuccess");
        },
        TaskExecutor::TaskType::IO, "ArkUIImageAPngRenderFrame");
}

/**
 * Get Frame Image mybe the image need draw in a new canvas
 * **/
#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> APngImagePlayer::GetImage(const int32_t& index, bool extendToCanvas)
{
    uint32_t size = 0;
    if (!apngDecoder_ || !apngDecoder_->isApng()) {
        return nullptr;
    }

    uint8_t *frameData = apngDecoder_->GetFrameData(index, &size);
    if (!frameData || size <= 0) {
        return nullptr;
    }

    APngAnimatedFrameInfo *frameInfo = &frameInfos_[index];
    SkData::ReleaseProc releaseProc = [](const void *ptr, void *context) -> void {
        if (ptr) {
            free((void *) ptr);
        }
    };

    sk_sp<SkData> skData = SkData::MakeWithProc(frameData, size, releaseProc, nullptr);
    if (!skData) {
        return nullptr;
    }

    auto rawImage = SkImage::MakeFromEncoded(skData);
    // if extendToCanvas then draw the image to canvas with offset info
    if (extendToCanvas && rawImage) {
        SkPictureRecorder recorder;
        SkCanvas *canvas = recorder.beginRecording(width_, height_);
        canvas->drawImage(rawImage, frameInfo->offsetX, frameInfo->offsetY, nullptr);
        rawImage = SkImage::MakeFromPicture(recorder.finishRecordingAsPicture(),
                                            SkISize::Make(width_, height_), nullptr, nullptr,
                                            SkImage::BitDepth::kU8,
                                            SkColorSpace::MakeSRGB());
    }

    return rawImage;
}
#else
    // Drawing : SkData::MakeWithProc
#endif
void APngImagePlayer::ClearCanvasRect(const APngAnimatedFrameInfo *frameInfo)
{
    if (!frameInfo || !blendCanvas_) {
        return;
    }

#ifndef USE_ROSEN_DRAWING
    SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX,
                                          frameInfo->offsetY,
                                          frameInfo->width,
                                          frameInfo->height);

    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(SK_ColorTRANSPARENT);
    blendCanvas_->drawRect(unBlendRect, paint);
#else
    RSRect unBlendRect =
        RSRect(frameInfo->offsetX, frameInfo->offsetY,
            frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);

    RSBrush brush;
    brush.SetColor(RSColor::COLOR_TRANSPARENT);
    blendCanvas_->AttachBrush(brush);
    blendCanvas_->DrawRect(unBlendRect);
    blendCanvas_->DetachBrush();
#endif
}

void APngImagePlayer::BlendImage(const APngAnimatedFrameInfo *frameInfo)
{
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> image = nullptr;

    if (!frameInfo) {
        return;
    }

    SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX,
                                          frameInfo->offsetY,
                                          frameInfo->width,
                                          frameInfo->height);

    if (frameInfo->dispose == ImageDisposePrevious) {
    } else if (frameInfo->dispose == ImageDisposeBackground) {
        ClearCanvasRect(frameInfo);
    } else {
        if (frameInfo->blend == ImageBlendOver) {
            sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }
        } else {
            ClearCanvasRect(frameInfo);
            sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }
        }
    }
#else
    if (!frameInfo) {
        return;
    }

    RSRect unBlendRect =
        RSRect(frameInfo->offsetX, frameInfo->offsetY,
            frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);

    RSSamplingOptions sampling =
        RSSamplingOptions(RSFilterMode::NEAREST, RSMipmapMode::NEAREST);
    if (frameInfo->dispose == ImageDisposePrevious) {
    } else if (frameInfo->dispose == ImageDisposeBackground) {
        ClearCanvasRect(frameInfo);
    } else {
        if (frameInfo->blend == ImageBlendOver) {
            std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }
        } else {
            ClearCanvasRect(frameInfo);
            std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }
        }
    }
#endif
}

/**
 * Debug function
 */
void APngImagePlayer::DrawTest()
{
    const uint32_t TestWidth = 10;
    const uint32_t TestX = 10;
#ifndef USE_ROSEN_DRAWING
    SkPaint pt;
    pt.setStyle(SkPaint::kFill_Style);
    pt.setColor(SK_ColorRED);
    blendCanvas_->drawRect(SkRect::MakeXYWH(TestX, TestX, TestWidth, TestWidth), pt);
#else
    RSBrush brush;
    brush.SetColor(RSColor::COLOR_RED);
    blendCanvas_->AttachBrush(brush);
    blendCanvas_->DrawRect(RSRect::MakeXYWH(TestX, TestX, TestWidth, TestWidth));
    blendCanvas_->DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
void APngImagePlayer::DrawTestBorder(SkRect& rect)
{
    SkPaint pt;
    pt.setStyle(SkPaint::kStroke_Style);
    pt.setColor(SK_ColorRED);
    pt.setStrokeWidth(1);
    blendCanvas_->drawRect(rect, pt);
}
#else
void APngImagePlayer::DrawTestBorder(RSRect& rect)
{
    RSPen pen;
    pen.SetColor(RSColor::COLOR_RED);
    pen.SetWidth(1);
    blendCanvas_->AttachPen(pen);
    blendCanvas_->DrawRect(rect);
    blendCanvas_->DetachPen();
}
#endif

/**
 * Get decoded Image with frameinfo
 * @param frameInfo
 * @return
 */
#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> APngImagePlayer::GetImage(const APngAnimatedFrameInfo *frameInfo)
{
    sk_sp<SkImage> image = nullptr;

    if (!frameInfo) {
        return nullptr;
    }

    if (frameInfo->dispose == ImageDisposePrevious) {
        SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX,
                                              frameInfo->offsetY,
                                              frameInfo->width,
                                              frameInfo->height);

        SkBitmap bitmap;
        CopyTo(&bitmap, bitmap_);
        sk_sp<SkImage> previousImage = SkImage::MakeFromBitmap(bitmap);
        sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);

        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);

            blendCanvas_->clear(SK_ColorTRANSPARENT);
            if (previousImage) {
                blendCanvas_->drawImage(previousImage, 0, 0);
            }
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);
            blendCanvas_->clear(SK_ColorTRANSPARENT);
            if (previousImage) {
                blendCanvas_->drawImage(previousImage, 0, 0);
            }
        }
    } else if (frameInfo->dispose == ImageDisposeBackground) {
        sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);
        SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX,
                                              frameInfo->offsetY,
                                              frameInfo->width,
                                              frameInfo->height);
        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);
            blendCanvas_->clear(SK_ColorTRANSPARENT);
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);

                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);
            blendCanvas_->clear(SK_ColorTRANSPARENT);
        }
    } else {
        sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);
        SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX, frameInfo->offsetY, frameInfo->width,
                                              frameInfo->height);
        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);

                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);
        }
    }

    return image;
}
#else
std::shared_ptr<RSImage> APngImagePlayer::GetImage(const APngAnimatedFrameInfo *frameInfo)
{
    std::shared_ptr<RSImage> image = nullptr;
    RSSamplingOptions sampling =
        RSSamplingOptions(RSFilterMode::NEAREST, RSMipmapMode::NEAREST);

    if (!frameInfo) {
        return nullptr;
    }

    if (frameInfo->dispose == ImageDisposePrevious) {
        RSRect unBlendRect =
            RSRect(frameInfo->offsetX, frameInfo->offsetY,
                frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);

        RSBitmap bitmap;
        CopyTo(&bitmap, bitmap_);
        auto previousImage = std::make_shared<RSImage> ();
        previousImage->BuildFromBitmap(bitmap);
        std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);

        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);

            blendCanvas_->Clear(RSColor::COLOR_TRANSPARENT);
            if (previousImage) {
                blendCanvas_->DrawImage(*previousImage, 0, 0, sampling);
            }
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);
            blendCanvas_->Clear(RSColor::COLOR_TRANSPARENT);
            if (previousImage) {
                blendCanvas_->DrawImage(*previousImage, 0, 0, sampling);
            }
        }
    } else if (frameInfo->dispose == ImageDisposeBackground) {
        std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);
        RSRect unBlendRect =
            RSRect(frameInfo->offsetX, frameInfo->offsetY,
                frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);
        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage,
                                            unBlendRect,
                                            sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);
            blendCanvas_->Clear(RSColor::COLOR_TRANSPARENT);
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);

                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);
            blendCanvas_->Clear(RSColor::COLOR_TRANSPARENT);
        }
    } else {
        std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);
        RSRect unBlendRect = RSRect(frameInfo->offsetX, frameInfo->offsetY,
            frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);
        if (frameInfo->blend == ImageBlendOver) {
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);
        } else {
            if (unblendImage) {
                ClearCanvasRect(frameInfo);

                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image->BuildFromBitmap(bitmap);
        }
    }

    return image;
}
#endif

bool APngImagePlayer::PreDecodeAllFrames()
{
    return false;
}

#ifndef USE_ROSEN_DRAWING
SkCanvas *APngImagePlayer::CreateBlendCanvas()
{
    if (!blendCanvas_) {
        blendFrameIndex_ = -1;
        bitmap_.allocN32Pixels(width_, height_);
        blendCanvas_ = new SkCanvas(bitmap_);
    }

    return blendCanvas_;
}
#else
RSCanvas *APngImagePlayer::CreateBlendCanvas()
{
    if (!blendCanvas_) {
        blendFrameIndex_ = -1;
        ImageInfo imageInfo(width_, height_, COLORTYPE_N32, ALPHATYPE_PREMUL);
        bitmap_.Build(imageInfo);
        blendCanvas_.Bind(bitmap_);
    }

    return blendCanvas_;
}
#endif


/**
 * 解码一帧，并把解码后的图片缓存在内存
 * 注: 所有的帧必须顺序进行，apng图片优化后会存在按帧透明叠加形成新的一帧
 * @param index
 * @return frameInformation
 */
APngAnimatedFrameInfo *APngImagePlayer::DecodeFrameImage(const int32_t& index)
{
    // first seek in cache
    auto iterator = cachedFrame_.find(index);
    if (iterator != cachedFrame_.end() && iterator->second != nullptr) {
        APngAnimatedFrameInfo *frameInfo = (APngAnimatedFrameInfo *) *iterator->second;
        return frameInfo;
    }

#ifndef USE_ROSEN_DRAWING
    SkBitmap bitmap;
#else
    RSBitmap bitmap;
#endif
    APngAnimatedFrameInfo *frameInfo = &frameInfos_[index];
    bool extendToCanvas = true;

    if (frameInfo->image) {
        return frameInfo;
    }

    if (!needBlend_) {
        if (!frameInfo->image) {
#ifndef USE_ROSEN_DRAWING
            sk_sp<SkImage> image = GetImage(index, true);
#else
            std::shared_ptr<RSImage> image = GetImage(index, true);
#endif
            if (!image) {
                return nullptr;
            }

            frameInfo->image = image;
        }

        return frameInfo;
    }

    // create a canvas
    if (!CreateBlendCanvas()) {
        return nullptr;
    }

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> image = nullptr;
#else
    std::shared_ptr<RSImage> image = nullptr;
#endif

    if (blendFrameIndex_ + 1 == frameInfo->index) {
        image = GetImage(frameInfo);
        blendFrameIndex_ = index;
    } else {
        blendFrameIndex_ = -1;
#ifndef USE_ROSEN_DRAWING
        blendCanvas_->clear(SK_ColorTRANSPARENT);
#else
        blendCanvas_->clear(RSColor::COLOR_TRANSPARENT);
#endif

        if (frameInfo->blendFromIndex == frameInfo->index) {
#ifndef USE_ROSEN_DRAWING
            SkRect unBlendRect = SkRect::MakeXYWH(frameInfo->offsetX,
                                                  frameInfo->offsetY,
                                                  frameInfo->width,
                                                  frameInfo->height);
            sk_sp<SkImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->drawImageRect(unblendImage,
                                            unBlendRect,
                                            nullptr);
            }

            SkBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = SkImage::MakeFromBitmap(bitmap);

            if (frameInfo->dispose == ImageDisposeBackground) {
                SkPaint paint;
                paint.setStyle(SkPaint::kFill_Style);
                paint.setColor(SK_ColorTRANSPARENT);
                blendCanvas_->drawRect(unBlendRect, paint);
            }
#else
            RSRect unBlendRect = RSRect(frameInfo->offsetX, frameInfo->offsetY,
                frameInfo->width + frameInfo->offsetX, frameInfo->height + frameInfo->offsetY);
            RSSamplingOptions sampling = RSSamplingOptions(
                RSFilterMode::NEAREST, RSMipmapMode::NEAREST);
            std::shared_ptr<RSImage> unblendImage = GetImage(frameInfo->index, false);
            if (unblendImage) {
                blendCanvas_->DrawImageRect(*unblendImage, unBlendRect, sampling);
            }

            RSBitmap bitmap;
            CopyTo(&bitmap, bitmap_);
            image = std::make_shared<RSImage>();
            image->BuildFromBitmap(bitmap);

            if (frameInfo->dispose == ImageDisposeBackground) {
                RSBrush brush;
                brush.SetColor(RSColor::COLOR_TRANSPARENT);
                blendCanvas_->AttachBrush(brush);
                blendCanvas_->DrawRect(unBlendRect);
                blendCanvas_->DetachBrush();
            }
#endif

            blendFrameIndex_ = index;
        } else { // canvas is not ready
            for (uint32_t i = (uint32_t) frameInfo->blendFromIndex; i <= (uint32_t) frameInfo->index; i++) {
                if (i == frameInfo->index) {
                    if (!image) {
                        image = GetImage(frameInfo);
                    }
                } else {
                    BlendImage(&frameInfos_[i]);
                }
            }

            blendFrameIndex_ = index;
        }
    }

    if (!image) {
        return nullptr;
    }

    // cache image data
    // if draw frame image in canvas then need modify offset / width / height
    frameInfo->image = image;

    if (extendToCanvas) {
        frameInfo->width = width_;
        frameInfo->height = height_;
        frameInfo->offsetX = 0;
        frameInfo->offsetY = 0;
        frameInfo->dispose = ImageDisposeNone;
        frameInfo->blend = ImageBlendNone;
    }

    return frameInfo;
}

/**
 * copy src bitmap to dest bitmap
 * @param dst
 * @param src
 * @return
 */
#ifndef USE_ROSEN_DRAWING
bool APngImagePlayer::CopyTo(SkBitmap *dst, const SkBitmap& src)
{
    SkPixmap srcPixmap;
    if (!src.peekPixels(&srcPixmap)) {
        return false;
    }

    SkBitmap tempDstBitmap;
    SkImageInfo dstInfo = srcPixmap.info();
    if (!tempDstBitmap.setInfo(dstInfo)) {
        return false;
    }

    if (!tempDstBitmap.tryAllocPixels()) {
        return false;
    }

    SkPixmap dstPixmap;
    if (!tempDstBitmap.peekPixels(&dstPixmap)) {
        return false;
    }

    if (!srcPixmap.readPixels(dstPixmap)) {
        return false;
    }

    dst->swap(tempDstBitmap);
    return true;
}
#else
bool APngImagePlayer::CopyTo(RSBitmap *dst, const RSBitmap &src)
{
    auto info = src.GetImageInfo();
    dst->Build(info);
    src.CopyPixels(*dst, 0, 0);
    return true;
}
#endif

/**
 * back animate render method use for optimization
 * @param paint
 * @param offset
 * @param canvas
 * @param paintRect
 */
void Paint(const flutter::Paint& paint, const Offset& offset, const ScopedCanvas& canvas, const Rect& paintRect) {}
} // namespace OHOS::Ace
