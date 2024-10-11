/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNER_API_DRAWABLE_DESCRIPTOR_JS_DRAWABLE_DESCRIPTOR_H_
#define INTERFACES_INNER_API_DRAWABLE_DESCRIPTOR_JS_DRAWABLE_DESCRIPTOR_H_

#include "drawable_descriptor.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Ace::Napi {
class JsDrawableDescriptor {
public:
    static ACE_EXPORT napi_value Export(napi_env env, napi_value exports);

    static ACE_EXPORT napi_value ToNapi(napi_env, DrawableDescriptor*,
        DrawableDescriptor::DrawableType type = DrawableDescriptor::DrawableType::LAYERED);

    static ACE_EXPORT constexpr char MODULE_NAME[] = "arkui.drawableDescriptor";
    static std::shared_ptr<Media::PixelMap> GetPixelMapFromNapi(napi_env env, napi_value napiValue);
    static std::shared_ptr<Media::PixelMap> GetPixelMapFromDrawableNapi(napi_env env, napi_value napiValue);

private:
    static napi_value InitDrawable(napi_env env);
    static napi_value InitLayeredDrawable(napi_env env);
    static napi_value InitAnimatedDrawable(napi_env env);
    static napi_value InitPixelMapDrawable(napi_env env);

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static napi_value AnimatedConstructor(napi_env env, napi_callback_info info);
    static napi_value PixelMapConstructor(napi_env env, napi_callback_info info);
    static napi_value LayeredConstructor(napi_env env, napi_callback_info info);

    // methods
    static napi_value GetPixelMap(napi_env env, napi_callback_info info);
    static napi_value GetLayeredPixelMap(napi_env env, napi_callback_info info);
    static napi_value GetForeground(napi_env env, napi_callback_info info);
    static napi_value GetBackground(napi_env env, napi_callback_info info);
    static napi_value GetMask(napi_env env, napi_callback_info info);

    static napi_value GetMaskClipPath(napi_env env, napi_callback_info info);

    static thread_local napi_ref baseConstructor_;
    static thread_local napi_ref layeredConstructor_;
    static thread_local napi_ref animatedConstructor_;
    static thread_local napi_ref pixelMapConstructor_;
};
} // namespace OHOS::Ace::Napi
#endif // INTERFACES_INNER_API_DRAWABLE_DESCRIPTOR_JS_DRAWABLE_DESCRIPTOR_H_
