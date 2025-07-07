/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef AVCODEC_SAMPLE_CALLBACK_H
#define AVCODEC_SAMPLE_CALLBACK_H
#define LOG_DOMAIN 0x3200
#define LOG_TAG "MY_SCNDKDEMO"

#include "SampleInfo.h"
#include <multimedia/player_framework/native_avmuxer.h>
class VideoEncoder {
public:
    void Create(const std::string &videoCodecMime);
    int32_t Config(SampleInfo &sampleInfo, CodecUserData *codecUserData);
    int32_t NotifyEndOfStream();
    int32_t Start();
    int32_t FreeOutputBuffer(uint32_t bufferIndex);
    int32_t Stop();
    void Release();

    
private:
    static void OnCodecError(OH_AVCodec *codec, int32_t errorCode, void *userData);
    static void OnCodecFormatChange(OH_AVCodec *codec, OH_AVFormat *format, void *userData);
    static void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);
    static void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);

    OH_AVCodec *encoder_ = nullptr;
};
#endif
