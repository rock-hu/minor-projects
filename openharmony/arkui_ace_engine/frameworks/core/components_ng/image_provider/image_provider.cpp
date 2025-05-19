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

#include "core/components_ng/image_provider/image_provider.h"

#include "base/log/log_wrapper.h"
#include "base/network/download_manager.h"
#include "base/subwindow/subwindow_manager.h"
#include "core/components_ng/image_provider/adapter/image_decoder.h"
#include "core/components_ng/image_provider/adapter/drawing_image_data.h"
#include "core/components_ng/image_provider/animated_image_object.h"
#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/image_provider/image_object.h"
#include "core/components_ng/image_provider/image_utils.h"
#include "core/components_ng/image_provider/pixel_map_image_object.h"
#include "core/components_ng/image_provider/static_image_object.h"
#include "core/components_ng/image_provider/svg_image_object.h"
#include "core/components_ng/pattern/image/image_dfx.h"
#include "core/components_ng/render/adapter/drawing_image.h"
#include "core/image/image_loader.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr uint64_t MAX_WAITING_TIME_FOR_TASKS = 1000; // 1000ms
}
void ImageProvider::CacheImageObject(const RefPtr<ImageObject>& obj)
{
    CHECK_NULL_VOID(obj);
    auto pipelineCtx = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineCtx);
    auto cache = pipelineCtx->GetImageCache();
    CHECK_NULL_VOID(cache);
    if (cache && obj->IsSupportCache()) {
        cache->CacheImgObjNG(obj->GetSourceInfo().GetKey(), obj);
    }
}

std::timed_mutex ImageProvider::taskMtx_;
std::unordered_map<std::string, ImageProvider::Task> ImageProvider::tasks_;

bool ImageProvider::PrepareImageData(const RefPtr<ImageObject>& imageObj)
{
    CHECK_NULL_RETURN(imageObj, false);
    auto&& dfxConfig = imageObj->GetImageDfxConfig();
    // Attempt to acquire a timed lock (maximum wait time: 1000ms)
    auto lock = imageObj->GetPrepareImageDataLock();
    if (!lock.owns_lock()) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "Failed to acquire lock within timeout. %{private}s-%{public}s.",
            dfxConfig.GetImageSrc().c_str(), dfxConfig.ToStringWithoutSrc().c_str());
        return false;
    }
    // data already loaded
    if (imageObj->GetData()) {
        return true;
    }

    auto container = Container::Current();
    if (container && container->IsSubContainer()) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "%{private}s-%{public}s. subContainer's pipeline's dataProviderManager is null.",
            dfxConfig.GetImageSrc().c_str(), dfxConfig.ToStringWithoutSrc().c_str());
        auto currentId = SubwindowManager::GetInstance()->GetParentContainerId(Container::CurrentId());
        container = Container::GetContainer(currentId);
    }
    CHECK_NULL_RETURN(container, false);
    auto pipeline = container->GetPipelineContext();
    CHECK_NULL_RETURN(pipeline, false);
    // if image object has no skData, reload data.
    auto imageLoader = ImageLoader::CreateImageLoader(imageObj->GetSourceInfo());
    if (!imageLoader) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "Failed to create loader in prepareImageData. %{public}s-[%{private}s]",
            dfxConfig.ToStringWithoutSrc().c_str(), dfxConfig.GetImageSrc().c_str());
        return false;
    }
    ImageErrorInfo errorInfo;
    auto newLoadedData = imageLoader->GetImageData(imageObj->GetSourceInfo(), errorInfo, WeakClaim(RawPtr(pipeline)));
    CHECK_NULL_RETURN(newLoadedData, false);
    // load data success
    imageObj->SetData(newLoadedData);
    return true;
}

RefPtr<ImageObject> ImageProvider::QueryThumbnailCache(const ImageSourceInfo& src)
{
    // query thumbnail from cache
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto cache = pipeline->GetImageCache();
    CHECK_NULL_RETURN(cache, nullptr);
    auto data = DynamicCast<PixmapData>(cache->GetCacheImageData(src.GetKey()));
    if (data) {
        return PixelMapImageObject::Create(src, data);
    }
    return nullptr;
}

RefPtr<ImageObject> ImageProvider::QueryImageObjectFromCache(const ImageSourceInfo& src)
{
    if (src.GetSrcType() == SrcType::DATA_ABILITY_DECODED) {
        return QueryThumbnailCache(src);
    }
    if (!src.SupportObjCache()) {
        return nullptr;
    }
    auto pipelineCtx = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipelineCtx, nullptr);
    auto imageCache = pipelineCtx->GetImageCache();
    CHECK_NULL_RETURN(imageCache, nullptr);
    RefPtr<ImageObject> imageObj = imageCache->GetCacheImgObjNG(src.GetKey());
    return imageObj;
}

void ImageProvider::FailCallback(const std::string& key, const std::string& errorMsg, const ImageErrorInfo& errorInfo,
    bool sync, int32_t containerId)
{
    auto ctxs = EndTask(key);
    auto notifyLoadFailTask = [ctxs, errorMsg, errorInfo] {
        for (auto&& it : ctxs) {
            auto ctx = it.Upgrade();
            if (!ctx) {
                continue;
            }
            ctx->FailCallback(errorMsg, errorInfo);
        }
    };

    if (sync) {
        notifyLoadFailTask();
    } else {
        ImageUtils::PostToUI(std::move(notifyLoadFailTask), "ArkUIImageProviderFail", containerId);
    }
}

void ImageProvider::SuccessCallback(
    const RefPtr<CanvasImage>& canvasImage, const std::string& key, bool sync, int32_t containerId)
{
    canvasImage->Cache(key);
    auto ctxs = EndTask(key);
    // when upload success, pass back canvasImage to LoadingContext
    auto notifyLoadSuccess = [ctxs, canvasImage] {
        for (auto&& it : ctxs) {
            auto ctx = it.Upgrade();
            if (!ctx) {
                continue;
            }
            ctx->SuccessCallback(canvasImage->Clone());
        }
    };

    if (sync) {
        notifyLoadSuccess();
    } else {
        ImageUtils::PostToUI(std::move(notifyLoadSuccess), "ArkUIImageProviderSuccess", containerId);
    }
}

void ImageProvider::CreateImageObjHelper(const ImageSourceInfo& src, bool sync)
{
    const ImageDfxConfig& imageDfxConfig = src.GetImageDfxConfig();
    ACE_SCOPED_TRACE("CreateImageObj %s", imageDfxConfig.ToStringWithSrc().c_str());
    ImageErrorInfo errorInfo;
    // load image data
    auto imageLoader = ImageLoader::CreateImageLoader(src);
    if (!imageLoader) {
        errorInfo = { ImageErrorCode::CREATE_IMAGE_UNKNOWN_SOURCE_TYPE, "unknown source type." };
        FailCallback(src.GetTaskKey(), "Failed to create image loader.", errorInfo, sync, src.GetContainerId());
        return;
    }
    auto pipeline = PipelineContext::GetCurrentContext();
    RefPtr<ImageData> data = imageLoader->GetImageData(src, errorInfo, WeakClaim(RawPtr(pipeline)));
    if (!data) {
        FailCallback(src.GetTaskKey(), "Failed to load image data", errorInfo, sync, src.GetContainerId());
        return;
    }

    // build ImageObject
    RefPtr<ImageObject> imageObj = ImageProvider::BuildImageObject(src, errorInfo, data);
    if (!imageObj) {
        FailCallback(src.GetTaskKey(), "Failed to build image object", errorInfo, sync, src.GetContainerId());
        return;
    }

    auto cloneImageObj = imageObj->Clone();

    // ImageObject cache is only for saving image size info, clear data to save memory
    cloneImageObj->ClearData();

    CacheImageObject(cloneImageObj);

    auto ctxs = EndTask(src.GetTaskKey());

    // callback to LoadingContext
    auto notifyDataReadyTask = [ctxs, imageObj, src] {
        for (auto&& it : ctxs) {
            auto ctx = it.Upgrade();
            if (!ctx) {
                continue;
            }
            ctx->DataReadyCallback(imageObj);
        }
    };

    if (sync) {
        notifyDataReadyTask();
    } else {
        ImageUtils::PostToUI(std::move(notifyDataReadyTask), "ArkUIImageProviderDataReady", src.GetContainerId());
    }
}

bool ImageProvider::RegisterTask(const std::string& key, const WeakPtr<ImageLoadingContext>& ctx)
{
    if (!taskMtx_.try_lock_for(std::chrono::milliseconds(MAX_WAITING_TIME_FOR_TASKS))) {
        TAG_LOGW(AceLogTag::ACE_IMAGE,
            "Failed to acquire mutex within %{public}" PRIu64 "milliseconds, proceeding without registerTask access.",
            MAX_WAITING_TIME_FOR_TASKS);
        return false;
    }
    // Adopt the already acquired lock
    std::scoped_lock lock(std::adopt_lock, taskMtx_);
    // key exists -> task is running
    auto it = tasks_.find(key);
    if (it != tasks_.end()) {
        it->second.ctxs_.insert(ctx);
        return false;
    }
    tasks_[key].ctxs_.insert(ctx);
    return true;
}

std::set<WeakPtr<ImageLoadingContext>> ImageProvider::EndTask(const std::string& key, bool isErase)
{
    if (!taskMtx_.try_lock_for(std::chrono::milliseconds(MAX_WAITING_TIME_FOR_TASKS))) {
        TAG_LOGW(AceLogTag::ACE_IMAGE,
            "Failed to acquire mutex within %{public}" PRIu64 "milliseconds, proceeding without endTask access.",
            MAX_WAITING_TIME_FOR_TASKS);
        return {};
    }
    // Adopt the already acquired lock
    std::scoped_lock lock(std::adopt_lock, taskMtx_);
    auto it = tasks_.find(key);
    if (it == tasks_.end()) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "Task InvalidKey %{private}s", key.c_str());
        return {};
    }
    auto ctxs = it->second.ctxs_;
    if (ctxs.empty()) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "registered task has empty context %{private}s", key.c_str());
    }
    if (isErase) {
        tasks_.erase(it);
    }
    return ctxs;
}

bool ImageProvider::CancelTask(const std::string& key, const WeakPtr<ImageLoadingContext>& ctx)
{
    if (!taskMtx_.try_lock_for(std::chrono::milliseconds(MAX_WAITING_TIME_FOR_TASKS))) {
        TAG_LOGW(AceLogTag::ACE_IMAGE,
            "Failed to acquire mutex within %{public}" PRIu64 "milliseconds, proceeding without cancelTask access.",
            MAX_WAITING_TIME_FOR_TASKS);
        return false;
    }
    // Adopt the already acquired lock
    std::scoped_lock lock(std::adopt_lock, taskMtx_);
    auto it = tasks_.find(key);
    CHECK_NULL_RETURN(it != tasks_.end(), false);
    CHECK_NULL_RETURN(it->second.ctxs_.find(ctx) != it->second.ctxs_.end(), false);
    // only one LoadingContext waiting for this task, can just cancel
    if (it->second.ctxs_.size() == 1) {
        // task should be deleted regardless of whether the cancellation is successful or not
        it->second.bgTask_.Cancel();
        tasks_.erase(it);
        return true;
    }
    // other LoadingContext still waiting for this task, remove ctx from set
    it->second.ctxs_.erase(ctx);
    return false;
}

void ImageProvider::DownLoadSuccessCallback(
    const RefPtr<ImageObject>& imageObj, const std::string& key, bool sync, int32_t containerId)
{
    ImageProvider::CacheImageObject(imageObj);
    auto ctxs = EndTask(key);
    auto notifyDownLoadSuccess = [ctxs, imageObj] {
        for (auto&& it : ctxs) {
            auto ctx = it.Upgrade();
            if (!ctx) {
                continue;
            }
            ctx->DataReadyCallback(imageObj);
        }
    };

    if (sync) {
        notifyDownLoadSuccess();
    } else {
        ImageUtils::PostToUI(std::move(notifyDownLoadSuccess), "ArkUIImageProviderDownLoadSuccess", containerId);
    }
}

void ImageProvider::DownLoadOnProgressCallback(
    const std::string& key, bool sync, const uint32_t& dlNow, const uint32_t& dlTotal, int32_t containerId)
{
    auto ctxs = EndTask(key, false);
    auto notifyDownLoadOnProgressCallback = [ctxs, dlNow, dlTotal] {
        for (auto&& it : ctxs) {
            auto ctx = it.Upgrade();
            if (!ctx) {
                continue;
            }
            ctx->DownloadOnProgress(dlNow, dlTotal);
        }
    };

    if (sync) {
        notifyDownLoadOnProgressCallback();
    } else {
        ImageUtils::PostToUI(
            std::move(notifyDownLoadOnProgressCallback), "ArkUIImageDownloadOnProcessCallback", containerId);
    }
}

RefPtr<ImageData> ImageProvider::QueryDataFromCache(const ImageSourceInfo& src)
{
    ACE_FUNCTION_TRACE();
    std::string result;
    if (DownloadManager::GetInstance()->fetchCachedResult(src.GetSrc(), result)) {
        auto data = ImageData::MakeFromDataWithCopy(result.data(), result.size());
        return data;
    }
    return nullptr;
}

void ImageProvider::DownLoadImage(const UriDownLoadConfig& downLoadConfig)
{
    ACE_SCOPED_TRACE("PerformDownload %s", downLoadConfig.imageDfxConfig.ToStringWithSrc().c_str());
    auto queryData = QueryDataFromCache(downLoadConfig.src);
    if (queryData) {
        ImageErrorInfo errorInfo;
        RefPtr<ImageObject> imageObj = ImageProvider::BuildImageObject(downLoadConfig.src, errorInfo, queryData);
        if (imageObj) {
            ACE_SCOPED_TRACE("Hit network image cache %s", downLoadConfig.imageDfxConfig.ToStringWithSrc().c_str());
            ImageProvider::DownLoadSuccessCallback(
                imageObj, downLoadConfig.taskKey, downLoadConfig.sync, downLoadConfig.src.GetContainerId());
            return;
        }
    }
    DownloadCallback downloadCallback;
    downloadCallback.successCallback = [downLoadConfig, containerId = downLoadConfig.src.GetContainerId()](
                                           const std::string&& imageData, bool async, int32_t instanceId) {
        ContainerScope scope(instanceId);
        ACE_SCOPED_TRACE("DownloadImageSuccess %s, [%zu]", downLoadConfig.imageDfxConfig.ToStringWithSrc().c_str(),
            imageData.size());
        ImageErrorInfo errorInfo;
        if (!GreatNotEqual(imageData.size(), 0)) {
            ImageProvider::FailCallback(downLoadConfig.taskKey, "The length of imageData from netStack is not positive",
                errorInfo, downLoadConfig.sync, containerId);
            return;
        }
        auto data = ImageData::MakeFromDataWithCopy(imageData.data(), imageData.size());
        RefPtr<ImageObject> imageObj = ImageProvider::BuildImageObject(downLoadConfig.src, errorInfo, data);
        if (!imageObj) {
            ImageProvider::FailCallback(downLoadConfig.taskKey, "After download successful, imageObject Create fail",
                errorInfo, downLoadConfig.sync, containerId);
            return;
        }
        ImageProvider::DownLoadSuccessCallback(imageObj, downLoadConfig.taskKey, downLoadConfig.sync, containerId);
    };
    downloadCallback.failCallback = [taskKey = downLoadConfig.taskKey, sync = downLoadConfig.sync,
                                        containerId = downLoadConfig.src.GetContainerId()](std::string errorMessage,
                                        ImageErrorInfo errorInfo, bool async, int32_t instanceId) {
        ContainerScope scope(instanceId);
        ImageProvider::FailCallback(taskKey, errorMessage, errorInfo, sync, containerId);
    };
    downloadCallback.cancelCallback = downloadCallback.failCallback;
    if (downLoadConfig.hasProgressCallback) {
        downloadCallback.onProgressCallback = [taskKey = downLoadConfig.taskKey, sync = downLoadConfig.sync,
                                                  containerId = downLoadConfig.src.GetContainerId()](
                                                  uint32_t dlTotal, uint32_t dlNow, bool async, int32_t instanceId) {
            ContainerScope scope(instanceId);
            ImageProvider::DownLoadOnProgressCallback(taskKey, sync, dlNow, dlTotal, containerId);
        };
    }
    NetworkImageLoader::DownloadImage(std::move(downloadCallback), downLoadConfig.src.GetSrc(), downLoadConfig.sync);
}

void ImageProvider::CreateImageObject(const ImageSourceInfo& src, const WeakPtr<ImageLoadingContext>& ctxWp, bool sync)
{
    if (src.GetSrcType() == SrcType::NETWORK && SystemProperties::GetDownloadByNetworkEnabled()) {
        auto ctx = ctxWp.Upgrade();
        CHECK_NULL_VOID(ctx);
        const std::string taskKey = src.GetTaskKey() + (ctx->GetOnProgressCallback() ? "1" : "0");
        if (!RegisterTask(taskKey, ctxWp)) {
            // task is already running, only register callbacks
            return;
        }
        UriDownLoadConfig downloadConfig = {
            .src = src,
            .imageDfxConfig = ctx->GetImageDfxConfig(),
            .taskKey = taskKey,
            .sync = sync,
            .hasProgressCallback = static_cast<bool>(ctx->GetOnProgressCallback())
        };
        if (sync) {
            DownLoadImage(downloadConfig);
        } else {
            auto downloadConfigPtr = std::make_shared<UriDownLoadConfig>(std::move(downloadConfig));
            auto downloadImageTask = [downloadConfigPtr]() {
                DownLoadImage(*downloadConfigPtr);
            };
            ImageUtils::PostToBg(downloadImageTask, "ArkUIImageDownload", src.GetContainerId());
        }
        return;
    }
    if (!RegisterTask(src.GetTaskKey(), ctxWp)) {
        // task is already running, only register callbacks
        return;
    }
    if (sync) {
        CreateImageObjHelper(src, true);
    } else {
        if (!taskMtx_.try_lock_for(std::chrono::milliseconds(MAX_WAITING_TIME_FOR_TASKS))) {
            TAG_LOGW(AceLogTag::ACE_IMAGE,
                "Failed to acquire mutex within %{public}" PRIu64
                "milliseconds, proceeding without createImageObject access.",
                MAX_WAITING_TIME_FOR_TASKS);
            return;
        }
        // Adopt the already acquired lock
        std::scoped_lock lock(std::adopt_lock, taskMtx_);
        // wrap with [CancelableCallback] and record in [tasks_] map
        CancelableCallback<void()> task;
        task.Reset([src] { ImageProvider::CreateImageObjHelper(src); });
        tasks_[src.GetTaskKey()].bgTask_ = task;
        auto ctx = ctxWp.Upgrade();
        CHECK_NULL_VOID(ctx);
        ImageUtils::PostToBg(task, "ArkUIImageProviderCreateImageObject", ctx->GetContainerId());
    }
}

RefPtr<ImageObject> ImageProvider::BuildImageObject(
    const ImageSourceInfo& src, ImageErrorInfo& errorInfo, const RefPtr<ImageData>& data)
{
    auto imageDfxConfig = src.GetImageDfxConfig();
    if (!data) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "data is null when try ParseImageObjectType, [%{private}s]-%{public}s.",
            imageDfxConfig.GetImageSrc().c_str(), imageDfxConfig.ToStringWithoutSrc().c_str());
        return nullptr;
    }
    if (src.IsSvg()) {
        // SVG object needs to make SVG dom during creation
        return SvgImageObject::Create(src, errorInfo, data);
    }
    if (src.IsPixmap()) {
        return PixelMapImageObject::Create(src, data);
    }

    auto rosenImageData = DynamicCast<DrawingImageData>(data);
    if (!rosenImageData) {
        TAG_LOGW(AceLogTag::ACE_IMAGE, "rosenImageData null, [%{private}s]-%{public}s.",
            imageDfxConfig.GetImageSrc().c_str(), imageDfxConfig.ToStringWithoutSrc().c_str());
        return nullptr;
    }
    rosenImageData->SetDfxConfig(imageDfxConfig.GetNodeId(), imageDfxConfig.GetAccessibilityId());
    auto codec = rosenImageData->Parse();
    if (!codec.imageSize.IsPositive()) {
        TAG_LOGW(AceLogTag::ACE_IMAGE,
            "Image of src: %{private}s, imageData's size = %{public}d is invalid, and the parsed size is invalid "
            "%{private}s, frameCount is %{public}d, nodeId = %{public}s.",
            src.ToString().c_str(), static_cast<int32_t>(data->GetSize()), codec.imageSize.ToString().c_str(),
            codec.frameCount, imageDfxConfig.ToStringWithoutSrc().c_str());
        if (errorInfo.errorCode == ImageErrorCode::DEFAULT) {
            errorInfo = { ImageErrorCode::BUILD_IMAGE_DATA_SIZE_INVALID, "Image Data is invalid." };
        }
        return nullptr;
    }
    RefPtr<ImageObject> imageObject;
    if (codec.frameCount > 1) {
        auto imageObject = MakeRefPtr<AnimatedImageObject>(src, codec.imageSize, data);
        imageObject->SetFrameCount(codec.frameCount);
        return imageObject;
    }
    imageObject = MakeRefPtr<StaticImageObject>(src, codec.imageSize, data);
    imageObject->SetOrientation(codec.orientation);
    return imageObject;
}

void ImageProvider::MakeCanvasImage(const RefPtr<ImageObject>& obj, const WeakPtr<ImageLoadingContext>& ctxWp,
    const SizeF& size, const ImageDecoderOptions& imageDecoderOptions)
{
    auto key = ImageUtils::GenerateImageKey(obj->GetSourceInfo(), size);
    // check if same task is already executing
    if (!RegisterTask(key, ctxWp)) {
        return;
    }
    if (imageDecoderOptions.sync) {
        MakeCanvasImageHelper(obj, size, key, imageDecoderOptions);
    } else {
        if (!taskMtx_.try_lock_for(std::chrono::milliseconds(MAX_WAITING_TIME_FOR_TASKS))) {
            TAG_LOGW(AceLogTag::ACE_IMAGE,
                "Failed to acquire mutex within %{public}" PRIu64
                "milliseconds, proceeding without makeCanvasImage access.",
                MAX_WAITING_TIME_FOR_TASKS);
            return;
        }
        // Adopt the already acquired lock
        std::scoped_lock lock(std::adopt_lock, taskMtx_);
        // wrap with [CancelableCallback] and record in [tasks_] map
        CancelableCallback<void()> task;
        task.Reset(
            [key, obj, size, imageDecoderOptions] { MakeCanvasImageHelper(obj, size, key, imageDecoderOptions); });
        tasks_[key].bgTask_ = task;
        auto ctx = ctxWp.Upgrade();
        CHECK_NULL_VOID(ctx);
        ImageUtils::PostToBg(task, "ArkUIImageProviderMakeCanvasImage", ctx->GetContainerId());
    }
}

void ImageProvider::MakeCanvasImageHelper(const RefPtr<ImageObject>& obj, const SizeF& size, const std::string& key,
    const ImageDecoderOptions& imageDecoderOptions)
{
    RefPtr<CanvasImage> image = nullptr;
    ImageDecoderConfig imageDecoderConfig = {
        .desiredSize_ = size,
        .forceResize_ = imageDecoderOptions.forceResize,
        .imageQuality_ = imageDecoderOptions.imageQuality,
        .isHdrDecoderNeed_ = imageDecoderOptions.isHdrDecoderNeed,
        .photoDecodeFormat_ = imageDecoderOptions.photoDecodeFormat,
    };
    ImageErrorInfo errorInfo;
    // preview and ohos platform
    if (SystemProperties::GetImageFrameworkEnabled()) {
        image = ImageDecoder::MakePixmapImage(obj, imageDecoderConfig, errorInfo);
    } else {
        image = ImageDecoder::MakeDrawingImage(obj, imageDecoderConfig);
    }

    if (image) {
        SuccessCallback(image, key, imageDecoderOptions.sync, obj->GetSourceInfo().GetContainerId());
    } else {
        FailCallback(
            key, "Failed to decode image", errorInfo, imageDecoderOptions.sync, obj->GetSourceInfo().GetContainerId());
    }
}
} // namespace OHOS::Ace::NG
