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

#include "core/components_ng/image_provider/image_object.h"

#include "core/components/common/layout/constants.h"

namespace OHOS::Ace::NG {
const SizeF& ImageObject::GetImageSize() const
{
    return imageSize_;
}

void ImageObject::SetImageSize(const SizeF& imageSize)
{
    imageSize_ = imageSize;
}

const ImageSourceInfo& ImageObject::GetSourceInfo() const
{
    return src_;
}

void ImageObject::SetImageSourceInfoHdr(bool isHdr)
{
    src_.SetImageHdr(isHdr);
}

RefPtr<ImageData> ImageObject::GetData() const
{
    std::shared_lock lock(dataMutex_);
    return data_;
}

void ImageObject::SetData(const RefPtr<ImageData>& data)
{
    std::unique_lock lock(dataMutex_);
    data_ = data;
}

void ImageObject::ClearData()
{
    std::unique_lock lock(dataMutex_);
    data_.Reset();
}

int32_t ImageObject::GetFrameCount() const
{
    return frameCount_;
}

void ImageObject::SetFrameCount(int32_t frameCount)
{
    frameCount_ = frameCount;
}

void ImageObject::SetOrientation(ImageRotateOrientation orientation)
{
    orientation_ = orientation;
}

ImageRotateOrientation ImageObject::GetOrientation() const
{
    return orientation_;
}

void ImageObject::SetUserOrientation(ImageRotateOrientation orientation)
{
    userOrientation_ = orientation;
}

ImageRotateOrientation ImageObject::GetUserOrientation() const
{
    return userOrientation_;
}

void ImageObject::SetImageFileSize(size_t fileSize)
{
    fileSize_ = fileSize;
}

size_t ImageObject::GetImageFileSize() const
{
    return fileSize_;
}

size_t ImageObject::GetImageDataSize() const
{
    return imageDataSize_;
}
} // namespace OHOS::Ace::NG
