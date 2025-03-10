/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "image_decoder.h"

#include "drawing/engine_adapter/skia_adapter/skia_data.h"
#include "drawing/engine_adapter/skia_adapter/skia_image_info.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkGraphics.h"

#include "base/image/image_source.h"
#include "base/image/pixel_map.h"
#include "base/log/ace_trace.h"
#include "core/components_ng/image_provider/adapter/rosen/drawing_image_data.h"
#include "core/components_ng/image_provider/image_utils.h"
#include "core/components_ng/render/adapter/pixelmap_image.h"
#include "core/components_ng/render/adapter/rosen/drawing_image.h"
#include "core/image/image_compressor.h"
#include "core/image/image_loader.h"

namespace OHOS::Ace::NG {
ImageDecoder::ImageDecoder(const RefPtr<ImageObject>& obj, const SizeF& size, bool forceResize)
    : obj_(obj), desiredSize_(size), forceResize_(forceResize)
{
    CHECK_NULL_VOID(obj_);
    CHECK_NULL_VOID(ImageProvider::PrepareImageData(obj_));

    auto imageData = obj_->GetData();
    auto data = AceType::DynamicCast<DrawingImageData>(imageData);
    CHECK_NULL_VOID(data);
    data_ = data->GetRSData();
}

RefPtr<CanvasImage> ImageDecoder::MakeDrawingImage()
{
    CHECK_NULL_RETURN(obj_ && data_, nullptr);
    ACE_SCOPED_TRACE("MakeSkiaImage %s", obj_->GetSourceInfo().ToString().c_str());
    // check compressed image cache
    {
        auto image = QueryCompressedCache();
        if (image) {
            return image;
        }
    }

    auto image = ResizeDrawingImage();
    CHECK_NULL_RETURN(image, nullptr);
    auto canvasImage = CanvasImage::Create(&image);

    if (ImageCompressor::GetInstance()->CanCompress()) {
        TryCompress(DynamicCast<DrawingImage>(canvasImage));
    }
    return canvasImage;
}

RefPtr<CanvasImage> ImageDecoder::MakePixmapImage(
    AIImageQuality imageQuality, bool isHdrDecoderNeed, PixelFormat photoDecodeFormat)
{
    CHECK_NULL_RETURN(obj_ && data_, nullptr);
    auto imageDfxConfig = obj_->GetImageDfxConfig();
    auto src = imageDfxConfig.imageSrc_;
    auto source = ImageSource::Create(static_cast<const uint8_t*>(data_->GetData()), data_->GetSize());
    if (!source) {
        TAG_LOGE(AceLogTag::ACE_IMAGE, "ImageSouce Create Fail, %{private}s-%{public}s.", src.c_str(),
            imageDfxConfig.ToStringWithoutSrc().c_str());
        return nullptr;
    }

    auto width = static_cast<int32_t>(std::lround(desiredSize_.Width()));
    auto height = static_cast<int32_t>(std::lround(desiredSize_.Height()));
    std::pair<int32_t, int32_t> sourceSize = source->GetImageSize();
    // Determine whether to decode the width and height of each other based on the orientation
    SwapDecodeSize(width, height);
    std::string isTrimMemRebuild = "False";
    if (imageDfxConfig.isTrimMemRecycle_) {
        isTrimMemRebuild = "True";
        TAG_LOGI(AceLogTag::ACE_IMAGE, "CreateImagePixelMapRebuild, %{private}s-%{public}s.",
            src.c_str(), imageDfxConfig.ToStringWithoutSrc().c_str());
    }
    ACE_SCOPED_TRACE("CreateImagePixelMap %s, sourceSize: [ %d, %d ], targetSize: [ %d, %d ],"
        "[%d-%d-%d], isTrimMemRebuild: [%s]",
        src.c_str(), sourceSize.first, sourceSize.second, width, height, static_cast<int32_t>(isHdrDecoderNeed),
        static_cast<int32_t>(imageQuality), static_cast<int32_t>(photoDecodeFormat), isTrimMemRebuild.c_str());

    auto pixmap = source->CreatePixelMap({ width, height }, imageQuality, isHdrDecoderNeed, photoDecodeFormat);
    if (!pixmap) {
        TAG_LOGE(AceLogTag::ACE_IMAGE, "PixelMap Create Fail, src = %{private}s-%{public}s.", src.c_str(),
            imageDfxConfig.ToStringWithoutSrc().c_str());
        return nullptr;
    }

    auto image = PixelMapImage::Create(pixmap);
    if (SystemProperties::GetDebugPixelMapSaveEnabled()) {
        TAG_LOGI(AceLogTag::ACE_IMAGE,
            "Image Decode success, Info:%{public}s-%{public}s-%{public}s-%{public}d x %{public}d-%{public}d-%{public}d",
            imageDfxConfig.ToStringWithSrc().c_str(), GetResolutionQuality(imageQuality).c_str(),
            desiredSize_.ToString().c_str(), image->GetWidth(), image->GetHeight(), isHdrDecoderNeed,
            static_cast<int32_t>(photoDecodeFormat));
        pixmap->SavePixelMapToFile(imageDfxConfig.ToStringWithoutSrc() + "_decode_");
    }

    return image;
}

void ImageDecoder::SwapDecodeSize(int32_t& width, int32_t& height)
{
    if (width == 0 || height == 0 || obj_->GetUserOrientation() == ImageRotateOrientation::UP) {
        return;
    }
    auto orientation = obj_->GetOrientation();
    if (orientation == ImageRotateOrientation::LEFT || orientation == ImageRotateOrientation::RIGHT) {
        std::swap(width, height);
    }
}

std::shared_ptr<RSImage> ImageDecoder::ForceResizeImage(const std::shared_ptr<RSImage>& image, const RSImageInfo& info)
{
    ACE_FUNCTION_TRACE();
    RSBitmap bitmap;
    bitmap.Build(info);

    auto res = image->ScalePixels(bitmap, RSSamplingOptions(RSFilterMode::LINEAR, RSMipmapMode::NONE), false);

    CHECK_NULL_RETURN(res, image);

    bitmap.SetImmutable();
    auto drImage = std::make_shared<RSImage>();
    drImage->BuildFromBitmap(bitmap);
    return drImage;
}

std::shared_ptr<RSImage> ImageDecoder::ResizeDrawingImage()
{
    CHECK_NULL_RETURN(data_, nullptr);
    auto rsSkiaData = data_->GetImpl<Rosen::Drawing::SkiaData>();
    CHECK_NULL_RETURN(rsSkiaData, nullptr);
    auto skData = rsSkiaData->GetSkData();
    auto encodedImage = std::make_shared<RSImage>();
    if (!encodedImage->MakeFromEncoded(data_)) {
        return nullptr;
    }
    CHECK_NULL_RETURN(desiredSize_.IsPositive(), encodedImage);

    auto width = std::lround(desiredSize_.Width());
    auto height = std::lround(desiredSize_.Height());

    auto codec = SkCodec::MakeFromData(skData);
    CHECK_NULL_RETURN(codec, {});
    auto info = codec->getInfo();

    ACE_SCOPED_TRACE("ImageResize %s, sourceSize: [ %d, %d ], targetSize: [ %d, %d ]",
        obj_->GetSourceInfo().ToString().c_str(), info.width(), info.height(), static_cast<int32_t>(width),
        static_cast<int32_t>(height));

    // sourceSize is set by developer, then we will force scaling to [TargetSize] using SkImage::scalePixels,
    // this method would succeed even if the codec doesn't support that size.
    if (forceResize_) {
        info = info.makeWH(width, height);
        auto imageInfo = Rosen::Drawing::SkiaImageInfo::ConvertToRSImageInfo(info);
        return ForceResizeImage(encodedImage, imageInfo);
    }

    if ((info.width() > width && info.height() > height)) {
        // If the image is larger than the target size, we will scale it down to the target size.
        // DesiredSize might not be compatible with the codec, so we find the closest size supported by the codec
        auto scale = std::max(static_cast<float>(width) / info.width(), static_cast<float>(height) / info.height());
        auto idealSize = codec->getScaledDimensions(scale);
        if (SystemProperties::GetDebugEnabled()) {
            TAG_LOGD(AceLogTag::ACE_IMAGE, "desiredSize = %{public}s, codec idealSize: %{public}dx%{public}d",
                desiredSize_.ToString().c_str(), idealSize.width(), idealSize.height());
        }

        info = info.makeWH(idealSize.width(), idealSize.height());
        auto imageInfo = Rosen::Drawing::SkiaImageInfo::ConvertToRSImageInfo(info);
        RSBitmap bitmap;
        bitmap.Build(imageInfo);
        auto res = codec->getPixels(info, bitmap.GetPixels(), bitmap.GetRowBytes());
        CHECK_NULL_RETURN(res == SkCodec::kSuccess, encodedImage);
        auto image = std::make_shared<RSImage>();
        image->BuildFromBitmap(bitmap);
        return image;
    }
    return encodedImage;
}

RefPtr<CanvasImage> ImageDecoder::QueryCompressedCache()
{
    auto key = ImageUtils::GenerateImageKey(obj_->GetSourceInfo(), desiredSize_);
    auto cachedData = ImageLoader::LoadImageDataFromFileCache(key, ".astc");
    CHECK_NULL_RETURN(cachedData, {});

    auto rosenImageData = AceType::DynamicCast<DrawingImageData>(cachedData);
    CHECK_NULL_RETURN(rosenImageData, {});
    auto stripped = ImageCompressor::StripFileHeader(rosenImageData->GetRSData());
    TAG_LOGI(AceLogTag::ACE_IMAGE, "use astc cache %{private}s", key.c_str());

    // create encoded SkImage to use its uniqueId
    CHECK_NULL_RETURN(data_, {});
    auto image = std::make_shared<RSImage>();
    if (!image->MakeFromEncoded(data_)) {
        return nullptr;
    }
    auto canvasImage = AceType::DynamicCast<DrawingImage>(CanvasImage::Create(&image));
    // round width and height to nearest int
    int32_t dstWidth = std::lround(desiredSize_.Width());
    int32_t dstHeight = std::lround(desiredSize_.Height());
    canvasImage->SetCompressData(stripped, dstWidth, dstHeight);
    canvasImage->ReplaceRSImage(nullptr);
    return canvasImage;
}

void ImageDecoder::TryCompress(const RefPtr<DrawingImage>& image)
{
#ifdef UPLOAD_GPU_DISABLED
    // If want to dump draw command or gpu disabled, should use CPU image.
    return;
#else
    // decode image to texture if not decoded
    auto rsImage = image->GetImage();
    CHECK_NULL_VOID(rsImage);
    RSBitmapFormat rsBitmapFormat { rsImage->GetColorType(), rsImage->GetAlphaType() };
    RSBitmap rsBitmap;
    if (!rsBitmap.Build(rsImage->GetWidth(), rsImage->GetHeight(), rsBitmapFormat)) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "rsBitmap build fail.");
        return;
    }
    CHECK_NULL_VOID(rsImage->ReadPixels(rsBitmap, 0, 0));
    auto width = rsBitmap.GetWidth();
    auto height = rsBitmap.GetHeight();
    // try compress image
    if (ImageCompressor::GetInstance()->CanCompress()) {
        auto key = ImageUtils::GenerateImageKey(obj_->GetSourceInfo(), desiredSize_);
        auto compressData = ImageCompressor::GetInstance()->GpuCompress(key, rsBitmap, width, height);
        ImageCompressor::GetInstance()->WriteToFile(key, compressData, { width, height });
        if (compressData) {
            // replace rsImage of [CanvasImage] with [rasterizedImage]
            image->SetCompressData(compressData, width, height);
            image->ReplaceRSImage(nullptr);
        } else {
            auto rasterizedImage = std::make_shared<RSImage>();
            rasterizedImage->BuildFromBitmap(rsBitmap);
            image->ReplaceRSImage(rasterizedImage);
        }
        auto taskExecutor = Container::CurrentTaskExecutor();
        auto releaseTask = ImageCompressor::GetInstance()->ScheduleReleaseTask();
        if (taskExecutor) {
            taskExecutor->PostDelayedTask(releaseTask, TaskExecutor::TaskType::UI, ImageCompressor::releaseTimeMs,
                "ArkUIImageCompressorScheduleRelease");
        } else {
            ImageUtils::PostToBg(std::move(releaseTask), "ArkUIImageCompressorScheduleRelease");
        }
    }
    SkGraphics::PurgeResourceCache();
#endif
}

std::string ImageDecoder::GetResolutionQuality(AIImageQuality imageQuality)
{
    switch (imageQuality) {
        case AIImageQuality::NONE:
            return "NONE";
        case AIImageQuality::LOW:
            return "LOW";
        case AIImageQuality::NORMAL:
            return "MEDIUM";
        case AIImageQuality::HIGH:
            return "HIGH";
        default:
            return "LOW";
    }
}
} // namespace OHOS::Ace::NG
