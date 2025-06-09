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

#include "Muxer.h"

#undef LOG_TAG
#define LOG_TAG "Muxer"

namespace {
constexpr int32_t VERTICAL_ANGLE = 90;
constexpr int32_t HORIZONTAL_ANGLE = 0;
}

Muxer::~Muxer()
{
    Release();
}
// [Start format_path]
// Create an encapsulator instance object and set the encapsulation format to mp4
int32_t Muxer::Create(int32_t fd)
{
    muxer_ = OH_AVMuxer_Create(fd, AV_OUTPUT_FORMAT_MPEG_4);
    CHECK_AND_RETURN_RET_LOG(muxer_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Muxer create failed, fd: %{public}d", fd);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t Muxer::Config(SampleInfo &sampleInfo)
{
    CHECK_AND_RETURN_RET_LOG(muxer_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Muxer is null");

    OH_AVFormat *formatVideo = OH_AVFormat_CreateVideoFormat(sampleInfo.videoCodecMime.data(),
        sampleInfo.videoWidth, sampleInfo.videoHeight);
    CHECK_AND_RETURN_RET_LOG(formatVideo != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Create video format failed");

    OH_AVFormat_SetDoubleValue(formatVideo, OH_MD_KEY_FRAME_RATE, sampleInfo.frameRate);
    OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_WIDTH, sampleInfo.videoWidth);
    OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_HEIGHT, sampleInfo.videoHeight);
    OH_AVFormat_SetStringValue(formatVideo, OH_MD_KEY_CODEC_MIME, sampleInfo.videoCodecMime.data());
    if (sampleInfo.isHDRVivid) {
        OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_VIDEO_IS_HDR_VIVID, 1);
        OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_RANGE_FLAG, sampleInfo.rangFlag);
        OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_COLOR_PRIMARIES, sampleInfo.primary);
        OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_TRANSFER_CHARACTERISTICS, sampleInfo.transfer);
        OH_AVFormat_SetIntValue(formatVideo, OH_MD_KEY_MATRIX_COEFFICIENTS, sampleInfo.matrix);
    }
    
    int32_t ret = OH_AVMuxer_AddTrack(muxer_, &videoTrackId_, formatVideo);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "AddTrack failed");
    OH_AVFormat_Destroy(formatVideo);
    OH_AVMuxer_SetRotation(muxer_, sampleInfo.videoHeight > sampleInfo.videoWidth ? VERTICAL_ANGLE : HORIZONTAL_ANGLE);
    return AVCODEC_SAMPLE_ERR_OK;
}
// [End format_path]
    
int32_t Muxer::Start()
{
    CHECK_AND_RETURN_RET_LOG(muxer_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Muxer is null");

    int ret = OH_AVMuxer_Start(muxer_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Start failed, ret: %{public}d", ret);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t Muxer::WriteSample(OH_AVBuffer *buffer, OH_AVCodecBufferAttr &attr)
{
    CHECK_AND_RETURN_RET_LOG(muxer_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Muxer is null");
    CHECK_AND_RETURN_RET_LOG(buffer != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Get a empty buffer");
    
    int32_t ret = OH_AVBuffer_SetBufferAttr(buffer, &attr);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "SetBufferAttr failed");

    ret = OH_AVMuxer_WriteSampleBuffer(muxer_, videoTrackId_, buffer);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Write sample failed");
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t Muxer::Release()
{
    if (muxer_ != nullptr) {
        OH_AVMuxer_Destroy(muxer_);
        muxer_ = nullptr;
    }
    return AVCODEC_SAMPLE_ERR_OK;
}