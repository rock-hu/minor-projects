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

#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <string>
#include <unordered_map>
#include "native_window/external_window.h"
#include "VideoDecoder.h"
#include "PluginRender.h"
#include "multimedia/video_processing_engine/video_processing.h"
namespace NativeXComponentSample {
class PluginManager {
public:
    ~PluginManager();

    static PluginManager *GetInstance() { return &PluginManager::pluginManager_; }
    
    void SetNativeXComponent(std::string &id, OH_NativeXComponent *nativeXComponent);
    PluginRender *GetRender(std::string &id);
    void Export(napi_env env, napi_value exports);
    OHNativeWindow *pluginWindow_ = nullptr; // decoder -> pluginWindow_
    OHNativeWindow *windowOut = nullptr; 
    OHNativeWindow *renderWindow = nullptr; 
    OH_VideoProcessing* processor = nullptr;
    VideoProcessing_Callback* callback = nullptr;
    uint32_t *processedCount = nullptr;
    SampleInfo *sampleInfo_ = nullptr;
    void InitProcessing(SampleInfo &sampleInfo);
    void PrepareSurface();
    void StartProcessing();
    void StopProcessing();
    void SetSampleInfo(SampleInfo *sampleInfo);
    void DestroyProcessing();
    
private:
    static PluginManager pluginManager_;
    std::unordered_map<std::string, OH_NativeXComponent *> nativeXComponentMap_;
    std::unordered_map<std::string, PluginRender *> pluginRenderMap_;
};
} // namespace NativeXComponentSample
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H