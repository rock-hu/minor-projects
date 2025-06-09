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

#include "VideoEncoder.h"

#undef LOG_TAG
#define LOG_TAG "VideoEncoder"

namespace {

int32_t ToGraphicPixelFormat(int32_t avPixelFormat, bool isHDRVivid)
{
    if (isHDRVivid) {
        return NATIVEBUFFER_PIXEL_FMT_YCBCR_P010;
    }
    switch (avPixelFormat) {
        case AV_PIXEL_FORMAT_RGBA:
            return NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
        case AV_PIXEL_FORMAT_YUVI420:
            return NATIVEBUFFER_PIXEL_FMT_YCBCR_420_P;
        case AV_PIXEL_FORMAT_NV21:
            return NATIVEBUFFER_PIXEL_FMT_YCRCB_420_SP;
        default: // NV12 and others
            return NATIVEBUFFER_PIXEL_FMT_YCRCB_420_SP;
    }
}
} // namespace

VideoEncoder::~VideoEncoder()
{
    Release();
}

int32_t VideoEncoder::Create(const std::string &videoCodecMime)
{
    encoder_ = OH_VideoEncoder_CreateByMime(videoCodecMime.c_str());
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Create failed");
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::Config(SampleInfo &sampleInfo, CodecUserData *codecUserData)
{
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Encoder is null");
    CHECK_AND_RETURN_RET_LOG(codecUserData != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Invalid param: codecUserData");

    // Configure video encoder
    int32_t ret = Configure(sampleInfo);
    CHECK_AND_RETURN_RET_LOG(ret == AVCODEC_SAMPLE_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Configure failed");

    // GetSurface from video encoder
    ret = GetSurface(sampleInfo);
    CHECK_AND_RETURN_RET_LOG(ret == AVCODEC_SAMPLE_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Get surface failed");
    
    // SetCallback for video encoder
    ret = SetCallback(codecUserData);
    CHECK_AND_RETURN_RET_LOG(ret == AVCODEC_SAMPLE_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR,
                             "Set callback failed, ret: %{public}d", ret);

    // Prepare video encoder
    ret = OH_VideoEncoder_Prepare(encoder_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Prepare failed, ret: %{public}d", ret);

    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::Start()
{
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Encoder is null");

    int ret = OH_VideoEncoder_Start(encoder_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Start failed, ret: %{public}d", ret);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::FreeOutputBuffer(uint32_t bufferIndex)
{
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Encoder is null");

    int32_t ret = OH_VideoEncoder_FreeOutputBuffer(encoder_, bufferIndex);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR,
        "Free output data failed, ret: %{public}d", ret);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::NotifyEndOfStream()
{
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Encoder is null");

    int32_t ret = OH_VideoEncoder_NotifyEndOfStream(encoder_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR,
        "Notify end of stream failed, ret: %{public}d", ret);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::Stop()
{
    CHECK_AND_RETURN_RET_LOG(encoder_ != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "Encoder is null");

    int ret = OH_VideoEncoder_Flush(encoder_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Flush failed, ret: %{public}d", ret);

    ret = OH_VideoEncoder_Stop(encoder_);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Stop failed, ret: %{public}d", ret);
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::Release()
{
    if (encoder_ != nullptr) {
        OH_VideoEncoder_Destroy(encoder_);
        encoder_ = nullptr;
    }
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::SetCallback(CodecUserData *codecUserData)
{
    int32_t ret = OH_VideoEncoder_RegisterCallback(encoder_,
    {SampleCallback::OnCodecError, SampleCallback::OnCodecFormatChange,
        SampleCallback::OnNeedInputBuffer, SampleCallback::OnNewOutputBuffer},
        codecUserData);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Set callback failed, ret: %{public}d", ret);

    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::Configure(const SampleInfo &sampleInfo)
{
    OH_AVFormat *format = OH_AVFormat_Create();
    CHECK_AND_RETURN_RET_LOG(format != nullptr, AVCODEC_SAMPLE_ERR_ERROR, "AVFormat create failed");

    OH_AVFormat_SetIntValue(format, OH_MD_KEY_WIDTH, sampleInfo.videoWidth);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_HEIGHT, sampleInfo.videoHeight);
    OH_AVFormat_SetDoubleValue(format, OH_MD_KEY_FRAME_RATE, sampleInfo.frameRate);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, sampleInfo.pixelFormat);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE, sampleInfo.bitrateMode);
    OH_AVFormat_SetLongValue(format, OH_MD_KEY_BITRATE, sampleInfo.bitrate);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_PROFILE, sampleInfo.hevcProfile);
    if (sampleInfo.isHDRVivid) {
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_I_FRAME_INTERVAL, sampleInfo.iFrameInterval);
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_RANGE_FLAG, sampleInfo.rangFlag);
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_COLOR_PRIMARIES, sampleInfo.primary);
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_TRANSFER_CHARACTERISTICS, sampleInfo.transfer);
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_MATRIX_COEFFICIENTS, sampleInfo.matrix);
    }
    AVCODEC_SAMPLE_LOGI("====== VideoEncoder config ======");
    AVCODEC_SAMPLE_LOGI("%{public}d*%{public}d, %{public}.1ffps",
        sampleInfo.videoWidth, sampleInfo.videoHeight, sampleInfo.frameRate);
    // 1024: ratio of kbps to bps
    AVCODEC_SAMPLE_LOGI("BitRate Mode: %{public}d, BitRate: %{public}" PRId64 "kbps",
        sampleInfo.bitrateMode, sampleInfo.bitrate / 1024);
    AVCODEC_SAMPLE_LOGI("====== VideoEncoder config ======");

    int ret = OH_VideoEncoder_Configure(encoder_, format);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK, AVCODEC_SAMPLE_ERR_ERROR, "Config failed, ret: %{public}d", ret);
    OH_AVFormat_Destroy(format);
    format = nullptr;
    return AVCODEC_SAMPLE_ERR_OK;
}

int32_t VideoEncoder::GetSurface(SampleInfo &sampleInfo)
{
    int32_t ret = OH_VideoEncoder_GetSurface(encoder_, &sampleInfo.window);
    CHECK_AND_RETURN_RET_LOG(ret == AV_ERR_OK && sampleInfo.window, AVCODEC_SAMPLE_ERR_ERROR,
        "Get surface failed, ret: %{public}d", ret);
    (void)OH_NativeWindow_NativeWindowHandleOpt(sampleInfo.window, SET_BUFFER_GEOMETRY, sampleInfo.videoWidth,
                                                sampleInfo.videoHeight);
    (void)OH_NativeWindow_NativeWindowHandleOpt(sampleInfo.window, SET_USAGE, 16425); // 16425: Window usage
    (void)OH_NativeWindow_NativeWindowHandleOpt(sampleInfo.window, SET_FORMAT,
        ToGraphicPixelFormat(sampleInfo.pixelFormat, sampleInfo.isHDRVivid));
    return AVCODEC_SAMPLE_ERR_OK;
}