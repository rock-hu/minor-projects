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
#include "core/interfaces/native/node/web_modifier.h"

#include "bridge/common/utils/utils.h"
#include "core/components_ng/pattern/web/web_model_ng.h"

namespace OHOS::Ace::NG {
namespace {
constexpr bool DEFAULT_JS_ACCESS_ENABLED = true;
constexpr bool DEFAULT_FILE_ACCESS_ENABLED = false;
constexpr bool DEFAULT_DOM_STORAGE_ACCESS_ENABLED = false;
constexpr MixedModeContent DEFAULT_MIXED_MODE = MixedModeContent::MIXED_CONTENT_NEVER_ALLOW;
constexpr int32_t DEFAULT_ZOOM_ACCESS_ENABLED = true;
constexpr WebCacheMode DEFAULT_CACHE_MODE = WebCacheMode::DEFAULT;
constexpr WebDarkMode DEFAULT_DARK_MODE = WebDarkMode::Off;
constexpr int32_t DEFAULT_MULTIWINDOW_ACCESS_ENABLED = false;
constexpr int32_t DEFAULT_ALLOW_WINDOWOPEN_METHOD = false;
constexpr WebKeyboardAvoidMode DEFAULT_KEYBOAED_AVIOD_MODE = WebKeyboardAvoidMode::RESIZE_CONTENT;
constexpr bool DEFAULT_VERTICALSCROLL_BAR_ACCESS_ENABLED = true;
constexpr bool DEFAULT_HORIZONTALSCROLL_BAR_ACCESS_ENABLED = true;
constexpr int32_t DEFAULT_TEXT_ZOOM_RATIO = 100;
constexpr float DEFAULT_INITIAL_SCALE = 100.0f;
constexpr bool DEFAULT_GEOLOCATION_ACCESS_ENABLED = true;
constexpr bool DEFAULT_DATABASE_ACCESS_ENABLED = false;
constexpr bool DEFAULT_OVERVIEWMODE_ACCESS_ENABLED = true;
constexpr bool DEFAULT_FORCEDARK_ACCESS_ENABLED = false;
constexpr bool DEFAULT_PINCH_SMOOTH_ENABLED = false;
constexpr bool DEFAULT_META_VIEWPORT_ENABLED = true;
constexpr bool DEFAULT_ENABLE_FOLLOW_SYSTEM_FONT_WEIGHT = false;
constexpr bool DEFAULT_NATIVE_EMBED_MODE_ENABLE = false;
constexpr int32_t DEFAULT_MINFONT_SIZE = 8;
constexpr int32_t DEFAULT_DEFAULTFONT_SIZE = 16;
constexpr int32_t DEFAULT_DEFAULTFIXEDFONT_SIZE = 13;
constexpr int32_t DEFAULT_MINLOGICALFONT_SIZE = 8;
constexpr char DEFAULT_WEBSTANDARD_FONT[] = "sans serif";
constexpr char DEFAULT_WEBSERIF_FONT[] = "serif";
constexpr char DEFAULT_WEBSANSSERIF_FONT[] = "sans-serif";
constexpr char DEFAULT_WEBFIXED_FONT[] = "monospace";
constexpr char DEFAULT_WEBFANTASY_FONT[] = "fantasy";
constexpr char DEFAULT_WEBCURSIVE_FONT[] = "cursive";
constexpr WebLayoutMode DEFAULT_LAYOUT_MODE = WebLayoutMode::NONE;
constexpr bool DEFAULT_NATIVE_EMBED_OPTIONS = false;
constexpr bool DEFAULT_IMAGE_ACCESS_ENABLED = true;
constexpr bool DEFAULT_ONLINEIMAGE_ACCESS_ENABLED = true;
constexpr bool MEDIA_PLAY_GESTURE_ACCESS_ENABLED = true;
constexpr bool DEFAULT_MEDIA_OPTIONS_ENABLED = true;
constexpr int32_t DEFAULT_RESUMEINTERVAL = 0;
} // namespace

void SetJavaScriptAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetJsEnabled(frameNode, value);
}

void ResetJavaScriptAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetJsEnabled(frameNode, DEFAULT_JS_ACCESS_ENABLED);
}

void SetFileAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetFileAccessEnabled(frameNode, value);
}

void ResetFileAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetFileAccessEnabled(frameNode, DEFAULT_FILE_ACCESS_ENABLED);
}

void SetDomStorageAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDomStorageAccessEnabled(frameNode, value);
}

void ResetDomStorageAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDomStorageAccessEnabled(frameNode, DEFAULT_DOM_STORAGE_ACCESS_ENABLED);
}

void SetMixedMode(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto mixedContentMode = MixedModeContent::MIXED_CONTENT_NEVER_ALLOW;
    switch (value) {
        case 0:
            mixedContentMode = MixedModeContent::MIXED_CONTENT_ALWAYS_ALLOW;
            break;
        case 1:
            mixedContentMode = MixedModeContent::MIXED_CONTENT_COMPATIBILITY_MODE;
            break;
        default:
            mixedContentMode = MixedModeContent::MIXED_CONTENT_NEVER_ALLOW;
            break;
    }
    WebModelNG::SetMixedMode(frameNode, MixedModeContent(mixedContentMode));
}

void ResetMixedMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMixedMode(frameNode, DEFAULT_MIXED_MODE);
}

void SetZoomAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetZoomAccessEnabled(frameNode, value);
}

void ResetZoomAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetZoomAccessEnabled(frameNode, DEFAULT_ZOOM_ACCESS_ENABLED);
}

void SetCacheMode(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetCacheMode(frameNode, WebCacheMode(value));
}

void ResetCacheMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetCacheMode(frameNode, DEFAULT_CACHE_MODE);
}

void SetDarkMode(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDarkMode(frameNode, WebDarkMode(value));
}

void ResetDarkMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDarkMode(frameNode, DEFAULT_DARK_MODE);
}

void SetMultiWindowAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMultiWindowAccessEnabled(frameNode, value);
}

void ResetMultiWindowAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMultiWindowAccessEnabled(frameNode, DEFAULT_MULTIWINDOW_ACCESS_ENABLED);
}

void SetAllowWindowOpenMethod(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetAllowWindowOpenMethod(frameNode, value);
}

void ResetAllowWindowOpenMethod(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetAllowWindowOpenMethod(frameNode, DEFAULT_ALLOW_WINDOWOPEN_METHOD);
}

void SetKeyboardAvoidMode(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetKeyboardAvoidMode(frameNode, WebKeyboardAvoidMode(value));
}

void ResetKeyboardAvoidMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetKeyboardAvoidMode(frameNode, DEFAULT_KEYBOAED_AVIOD_MODE);
}

void SetOnControllerAttached(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto onControllerAttached = reinterpret_cast<std::function<void()>*>(extraParam);
        WebModelNG::SetOnControllerAttached(frameNode, std::move(*onControllerAttached));
    } else {
        WebModelNG::SetOnControllerAttached(frameNode, nullptr);
    }
}

void ResetOnControllerAttached(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnControllerAttached(frameNode, nullptr);
}

void SetVerticalScrollBarAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetVerticalScrollBarAccessEnabled(frameNode, value);
}

void ResetVerticalScrollBarAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetVerticalScrollBarAccessEnabled(frameNode, DEFAULT_VERTICALSCROLL_BAR_ACCESS_ENABLED);
}

void SetHorizontalScrollBarAccessEnabled(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetHorizontalScrollBarAccessEnabled(frameNode, value);
}

void ResetHorizontalScrollBarAccessEnabled(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetHorizontalScrollBarAccessEnabled(frameNode, DEFAULT_HORIZONTALSCROLL_BAR_ACCESS_ENABLED);
}

void SetTextZoomRatio(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetTextZoomRatio(frameNode, value);
}

void ResetTextZoomRatio(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetTextZoomRatio(frameNode, DEFAULT_TEXT_ZOOM_RATIO);
}

void SetInitialScale(ArkUINodeHandle node, ArkUI_Float32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::InitialScale(frameNode, value);
}

void ResetInitialScale(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::InitialScale(frameNode, DEFAULT_INITIAL_SCALE);
}

void SetOnScrollCallBack(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* onScrollPtr = reinterpret_cast<std::function<void(WebOnScrollEvent&)>*>(extraParam);
        CHECK_NULL_VOID(onScrollPtr);
        auto callback = [onScrollCallback = *onScrollPtr](const BaseEventInfo* event) {
            if (auto scrollEvent = static_cast<const WebOnScrollEvent*>(event)) {
                auto& nonConstEvent = const_cast<WebOnScrollEvent&>(*scrollEvent);
                onScrollCallback(nonConstEvent);
            }
        };
        WebModelNG::SetOnScroll(frameNode, std::move(callback));
    } else {
        WebModelNG::SetOnScroll(frameNode, nullptr);
    }
}

void ResetOnScrollCallBack(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnScroll(frameNode, nullptr);
}

void SetOnOverScrollCallBack(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* onOverScrollkPtr = reinterpret_cast<std::function<void(WebOnOverScrollEvent&)>*>(extraParam);
        CHECK_NULL_VOID(onOverScrollkPtr);
        auto callback = [onOverScrollkCallback = *onOverScrollkPtr](const BaseEventInfo* event) {
            if (auto overScrollEvent = static_cast<const WebOnOverScrollEvent*>(event)) {
                auto& nonConstEvent = const_cast<WebOnOverScrollEvent&>(*overScrollEvent);
                onOverScrollkCallback(nonConstEvent);
            }
        };
        WebModelNG::SetOnOverScroll(frameNode, std::move(callback));
    } else {
        WebModelNG::SetOnOverScroll(frameNode, nullptr);
    }
}

void ResetOnOverScrollCallBack(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnOverScroll(frameNode, nullptr);
}

void SetOnScaleChangeCallBack(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* scaleChangePtr = reinterpret_cast<std::function<void(ScaleChangeEvent&)>*>(extraParam);
        CHECK_NULL_VOID(scaleChangePtr);
        auto callback = [scaleChangeCallback = *scaleChangePtr](const BaseEventInfo* event) {
            if (auto scaleChangeEvent = static_cast<const ScaleChangeEvent*>(event)) {
                auto& nonConstEvent = const_cast<ScaleChangeEvent&>(*scaleChangeEvent);
                scaleChangeCallback(nonConstEvent);
            }
        };
        WebModelNG::SetOnScaleChange(frameNode, std::move(callback));
    } else {
        WebModelNG::SetOnScaleChange(frameNode, nullptr);
    }
}

void ResetOnScaleChangeCallBack(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnScaleChange(frameNode, nullptr);
}

void SetOnRequestSelectedCallBack(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* originalCallbackPtr = reinterpret_cast<std::function<void()>*>(extraParam);
        CHECK_NULL_VOID(originalCallbackPtr);
        auto callback = [originalCallback = *originalCallbackPtr](const BaseEventInfo*) { originalCallback(); };
        WebModelNG::SetOnRequestFocus(frameNode, std::move(callback));
    } else {
        WebModelNG::SetOnRequestFocus(frameNode, nullptr);
    }
}

void ResetOnRequestSelectedCallBack(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnRequestFocus(frameNode, nullptr);
}

void SetOnContextMenuHideCallBack(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* originalCallbackPtr = reinterpret_cast<std::function<void()>*>(extraParam);
        CHECK_NULL_VOID(originalCallbackPtr);
        auto callback = [originalCallback = *originalCallbackPtr](const BaseEventInfo*) { originalCallback(); };
        WebModelNG::SetOnContextMenuHide(frameNode, std::move(callback));
    } else {
        WebModelNG::SetOnContextMenuHide(frameNode, nullptr);
    }
}

void ResetOnContextMenuHideCallBack(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnContextMenuHide(frameNode, nullptr);
}

void SetGeolocationAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetGeolocationAccessEnabled(frameNode, value);
}

void ResetGeolocationAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetGeolocationAccessEnabled(frameNode, DEFAULT_GEOLOCATION_ACCESS_ENABLED);
}

void SetDatabaseAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDatabaseAccessEnabled(frameNode, value);
}

void ResetDatabaseAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDatabaseAccessEnabled(frameNode, DEFAULT_DATABASE_ACCESS_ENABLED);
}

void SetOverviewModeAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOverviewModeAccessEnabled(frameNode, value);
}

void ResetOverviewModeAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOverviewModeAccessEnabled(frameNode, DEFAULT_OVERVIEWMODE_ACCESS_ENABLED);
}

void SetForceDarkAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetForceDarkAccess(frameNode, value);
}

void ResetForceDarkAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetForceDarkAccess(frameNode, DEFAULT_FORCEDARK_ACCESS_ENABLED);
}

void SetPinchSmooth(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetPinchSmoothModeEnabled(frameNode, value);
}

void ResetPinchSmooth(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetPinchSmoothModeEnabled(frameNode, DEFAULT_PINCH_SMOOTH_ENABLED);
}

void SetMetaViewport(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMetaViewport(frameNode, value);
}

void ResetMetaViewport(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMetaViewport(frameNode, DEFAULT_META_VIEWPORT_ENABLED);
}

void SetEnableFollowSystemFontWeight(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetEnableFollowSystemFontWeight(frameNode, value);
}

void ResetEnableFollowSystemFontWeight(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetEnableFollowSystemFontWeight(frameNode, DEFAULT_ENABLE_FOLLOW_SYSTEM_FONT_WEIGHT);
}

void SetEnableNativeEmbedMode(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetNativeEmbedModeEnabled(frameNode, value);
}

void ResetEnableNativeEmbedMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetNativeEmbedModeEnabled(frameNode, DEFAULT_NATIVE_EMBED_MODE_ENABLE);
}

void SetMinFontSize(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMinFontSize(frameNode, value);
}

void ResetMinFontSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMinFontSize(frameNode, DEFAULT_MINFONT_SIZE);
}

void SetDefaultFontSize(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDefaultFontSize(frameNode, value);
}

void ResetDefaultFontSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDefaultFontSize(frameNode, DEFAULT_DEFAULTFONT_SIZE);
}

void SetDefaultFixedFontSize(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDefaultFixedFontSize(frameNode, value);
}

void ResetDefaultFixedFontSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetDefaultFixedFontSize(frameNode, DEFAULT_DEFAULTFIXEDFONT_SIZE);
}

void SetMinLogicalFontSize(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMinLogicalFontSize(frameNode, value);
}

void ResetMinLogicalFontSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMinLogicalFontSize(frameNode, DEFAULT_MINLOGICALFONT_SIZE);
}

void SetWebStandardFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebStandardFont(frameNode, std::string(value));
}

void ResetWebStandardFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebStandardFont(frameNode, DEFAULT_WEBSTANDARD_FONT);
}

void SetWebSerifFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebSerifFont(frameNode, std::string(value));
}

void ResetWebSerifFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebSerifFont(frameNode, DEFAULT_WEBSERIF_FONT);
}

void SetWebSansSerifFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebSansSerifFont(frameNode, std::string(value));
}

void ResetWebSansSerifFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebSansSerifFont(frameNode, DEFAULT_WEBSANSSERIF_FONT);
}

void SetWebFixedFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebFixedFont(frameNode, std::string(value));
}

void ResetWebFixedFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebFixedFont(frameNode, DEFAULT_WEBFIXED_FONT);
}

void SetWebFantasyFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebFantasyFont(frameNode, std::string(value));
}

void ResetWebFantasyFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebFantasyFont(frameNode, DEFAULT_WEBFANTASY_FONT);
}

void SetWebCursiveFont(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebCursiveFont(frameNode, std::string(value));
}

void ResetWebCursiveFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetWebCursiveFont(frameNode, DEFAULT_WEBCURSIVE_FONT);
}

void SetLayoutMode(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetLayoutMode(frameNode, WebLayoutMode(value));
}

void ResetLayoutMode(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetLayoutMode(frameNode, DEFAULT_LAYOUT_MODE);
}

void SetOnNativeEmbedLifecycleChange(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto* originalCallbackPtr = reinterpret_cast<std::function<void(NativeEmbedDataInfo&)>*>(extraParam);
    CHECK_NULL_VOID(originalCallbackPtr);
    if (extraParam) {
        auto adaptedCallback = [originalCallback = *originalCallbackPtr](const BaseEventInfo* event) {
            if (auto changeEvent = static_cast<const NativeEmbedDataInfo*>(event)) {
                auto& onNativeEmbedLifecycleChange = const_cast<NativeEmbedDataInfo&>(*changeEvent);
                originalCallback(onNativeEmbedLifecycleChange);
            }
        };
        WebModelNG::SetNativeEmbedLifecycleChangeId(frameNode, std::move(adaptedCallback));
    } else {
        WebModelNG::SetNativeEmbedLifecycleChangeId(frameNode, nullptr);
    }
}

void ResetOnNativeEmbedLifecycleChange(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetNativeEmbedLifecycleChangeId(frameNode, nullptr);
}

void SetRegisterNativeEmbedRule(ArkUINodeHandle node, ArkUI_CharPtr tag, ArkUI_CharPtr type)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::RegisterNativeEmbedRule(frameNode, tag, type);
}

void ResetRegisterNativeEmbedRule(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::RegisterNativeEmbedRule(frameNode, nullptr, nullptr);
}

void SetNativeEmbedOptions(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetNativeEmbedOptions(frameNode, value);
}

void ResetNativeEmbedOptions(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetNativeEmbedOptions(frameNode, DEFAULT_NATIVE_EMBED_OPTIONS);
}

void SetOnFirstContentfulPaint(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto* originalCallbackPtr = reinterpret_cast<std::function<void(AudioStateChangedEvent&)>*>(extraParam);
    CHECK_NULL_VOID(originalCallbackPtr);
    if (extraParam) {
        auto adaptedCallback = [originalCallback = *originalCallbackPtr](const std::shared_ptr<BaseEventInfo>& event) {
            auto* onFirstContentfulPaint = static_cast<AudioStateChangedEvent*>(event.get());
            if (onFirstContentfulPaint != nullptr) {
                originalCallback(*onFirstContentfulPaint);
            }
        };
        WebModelNG::SetFirstContentfulPaintId(frameNode, std::move(adaptedCallback));
    } else {
        WebModelNG::SetFirstContentfulPaintId(frameNode, nullptr);
    }
}

void ResetOnFirstContentfulPaint(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetFirstContentfulPaintId(frameNode, nullptr);
}

void SetOnAudioStateChanged(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto* originalCallbackPtr = reinterpret_cast<std::function<void(AudioStateChangedEvent&)>*>(extraParam);
    CHECK_NULL_VOID(originalCallbackPtr);
    if (extraParam) {
        auto adaptedCallback = [originalCallback = *originalCallbackPtr](const std::shared_ptr<BaseEventInfo>& event) {
            auto* onAudioStateChanged = static_cast<AudioStateChangedEvent*>(event.get());
            if (onAudioStateChanged != nullptr) {
                originalCallback(*onAudioStateChanged);
            }
        };
        WebModelNG::SetAudioStateChangedId(frameNode, std::move(adaptedCallback));
    } else {
        WebModelNG::SetAudioStateChangedId(frameNode, nullptr);
    }
}

void ResetOnAudioStateChanged(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetAudioStateChangedId(frameNode, nullptr);
}

void SetOnFullScreenExit(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto* originalCallbackPtr = reinterpret_cast<std::function<void()>*>(extraParam);
        CHECK_NULL_VOID(originalCallbackPtr);
        auto adaptedCallback = [originalCallback = *originalCallbackPtr](const BaseEventInfo*) { originalCallback(); };
        WebModelNG::SetOnFullScreenExit(frameNode, std::move(adaptedCallback));
    } else {
        WebModelNG::SetOnFullScreenExit(frameNode, nullptr);
    }
}


void ResetOnFullScreenExit(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnFullScreenExit(frameNode, nullptr);
}

void SetImageAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetImageAccessEnabled(frameNode, value);
}

void ResetImageAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetImageAccessEnabled(frameNode, DEFAULT_IMAGE_ACCESS_ENABLED);
}

void SetOnlineImageAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnLineImageAccessEnabled(frameNode, value);
}

void ResetOnlineImageAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetOnLineImageAccessEnabled(frameNode, DEFAULT_ONLINEIMAGE_ACCESS_ENABLED);
}

void SetMediaPlayGestureAccess(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMediaPlayGestureAccess(frameNode, value);
}

void ResetMediaPlayGestureAccess(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetMediaPlayGestureAccess(frameNode, MEDIA_PLAY_GESTURE_ACCESS_ENABLED);
}

void SetMediaOptions(ArkUINodeHandle node, ArkUI_Int32 resumeInterval, ArkUI_Bool audioExclusive)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetAudioResumeInterval(frameNode, resumeInterval);
    WebModelNG::SetAudioExclusive(frameNode, audioExclusive);
}

void ResetMediaOptions(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    WebModelNG::SetAudioResumeInterval(frameNode, DEFAULT_RESUMEINTERVAL);
    WebModelNG::SetAudioExclusive(frameNode, DEFAULT_MEDIA_OPTIONS_ENABLED);
}
namespace NodeModifier {
const ArkUIWebModifier* GetWebModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const ArkUIWebModifier modifier = {
        .getWebController = nullptr,
        .getWebviewController = nullptr,
        .setWebOptions = nullptr,
        .setWebSrcResources = nullptr,
        .setJavaScriptAccess = SetJavaScriptAccess,
        .resetJavaScriptAccess = ResetJavaScriptAccess,
        .setFileAccessEnabled = SetFileAccessEnabled,
        .resetFileAccessEnabled = ResetFileAccessEnabled,
        .setDomStorageAccessEnabled = SetDomStorageAccessEnabled,
        .resetDomStorageAccessEnabled = ResetDomStorageAccessEnabled,
        .setMixedMode = SetMixedMode,
        .resetMixedMode = ResetMixedMode,
        .setZoomAccessEnabled = SetZoomAccessEnabled,
        .resetZoomAccessEnabled = ResetZoomAccessEnabled,
        .setCacheMode = SetCacheMode,
        .resetCacheMode = ResetCacheMode,
        .setDarkMode = SetDarkMode,
        .resetDarkMode = ResetDarkMode,
        .setMultiWindowAccessEnabled = SetMultiWindowAccessEnabled,
        .resetMultiWindowAccessEnabled = ResetMultiWindowAccessEnabled,
        .setAllowWindowOpenMethod = SetAllowWindowOpenMethod,
        .resetAllowWindowOpenMethod = ResetAllowWindowOpenMethod,
        .setKeyboardAvoidMode = SetKeyboardAvoidMode,
        .resetKeyboardAvoidMode = ResetKeyboardAvoidMode,
        .setOnControllerAttached = SetOnControllerAttached,
        .resetOnControllerAttached = ResetOnControllerAttached,
        .setVerticalScrollBarAccessEnabled = SetVerticalScrollBarAccessEnabled,
        .resetVerticalScrollBarAccessEnabled = ResetVerticalScrollBarAccessEnabled,
        .setHorizontalScrollBarAccessEnabled = SetHorizontalScrollBarAccessEnabled,
        .resetHorizontalScrollBarAccessEnabled = ResetHorizontalScrollBarAccessEnabled,
        .setTextZoomRatio = SetTextZoomRatio,
        .resetTextZoomRatio = ResetTextZoomRatio,
        .setInitialScale = SetInitialScale,
        .resetInitialScale = ResetInitialScale,
        .setOnScrollCallBack = SetOnScrollCallBack,
        .resetOnScrollCallBack = ResetOnScrollCallBack,
        .setOnOverScrollCallBack = SetOnOverScrollCallBack,
        .resetOnOverScrollCallBack = ResetOnOverScrollCallBack,
        .setOnScaleChangeCallBack = SetOnScaleChangeCallBack,
        .resetOnScaleChangeCallBack = ResetOnScaleChangeCallBack,
        .setOnRequestSelectedCallBack = SetOnRequestSelectedCallBack,
        .resetOnRequestSelectedCallBack = ResetOnRequestSelectedCallBack,
        .setOnContextMenuHideCallBack= SetOnContextMenuHideCallBack,
        .resetOnContextMenuHideCallBack= ResetOnContextMenuHideCallBack,
        .setGeolocationAccess = SetGeolocationAccess,
        .resetGeolocationAccess = ResetGeolocationAccess,
        .setDatabaseAccess = SetDatabaseAccess,
        .resetDatabaseAccess = ResetDatabaseAccess,
        .setOverviewModeAccess = SetOverviewModeAccess,
        .resetOverviewModeAccess = ResetOverviewModeAccess,
        .setForceDarkAccess = SetForceDarkAccess,
        .resetForceDarkAccess = ResetForceDarkAccess,
        .setPinchSmooth = SetPinchSmooth,
        .resetPinchSmooth = ResetPinchSmooth,
        .setMetaViewport = SetMetaViewport,
        .resetMetaViewport = ResetMetaViewport,
        .setEnableFollowSystemFontWeight = SetEnableFollowSystemFontWeight,
        .resetEnableFollowSystemFontWeight = ResetEnableFollowSystemFontWeight,
        .setEnableNativeEmbedMode = SetEnableNativeEmbedMode,
        .resetEnableNativeEmbedMode = ResetEnableNativeEmbedMode,
        .setMinFontSize = SetMinFontSize,
        .resetMinFontSize = ResetMinFontSize,
        .setDefaultFontSize = SetDefaultFontSize,
        .resetDefaultFontSize = ResetDefaultFontSize,
        .setDefaultFixedFontSize = SetDefaultFixedFontSize,
        .resetDefaultFixedFontSize = ResetDefaultFixedFontSize,
        .setMinLogicalFontSize = SetMinLogicalFontSize,
        .resetMinLogicalFontSize = ResetMinLogicalFontSize,
        .setWebStandardFont = SetWebStandardFont,
        .resetWebStandardFont = ResetWebStandardFont,
        .setWebSerifFont = SetWebSerifFont,
        .resetWebSerifFont = ResetWebSerifFont,
        .setWebSansSerifFont = SetWebSansSerifFont,
        .resetWebSansSerifFont = ResetWebSansSerifFont,
        .setWebFixedFont = SetWebFixedFont,
        .resetWebFixedFont = ResetWebFixedFont,
        .setWebFantasyFont = SetWebFantasyFont,
        .resetWebFantasyFont = ResetWebFantasyFont,
        .setWebCursiveFont = SetWebCursiveFont,
        .resetWebCursiveFont = ResetWebCursiveFont,
        .setLayoutMode = SetLayoutMode,
        .resetLayoutMode = ResetLayoutMode,
        .setOnNativeEmbedLifecycleChange = SetOnNativeEmbedLifecycleChange,
        .resetOnNativeEmbedLifecycleChange = ResetOnNativeEmbedLifecycleChange,
        .setRegisterNativeEmbedRule = SetRegisterNativeEmbedRule,
        .resetRegisterNativeEmbedRule = ResetRegisterNativeEmbedRule,
        .setNativeEmbedOptions = SetNativeEmbedOptions,
        .resetNativeEmbedOptions = ResetNativeEmbedOptions,
        .setOnFirstContentfulPaint = SetOnFirstContentfulPaint,
        .resetOnFirstContentfulPaint = ResetOnFirstContentfulPaint,
        .setOnAudioStateChanged = SetOnAudioStateChanged,
        .resetOnAudioStateChanged = ResetOnAudioStateChanged,
        .setOnFullScreenExit = SetOnFullScreenExit,
        .resetOnFullScreenExit = ResetOnFullScreenExit,
        .setImageAccess = SetImageAccess,
        .resetImageAccess = ResetImageAccess,
        .setOnlineImageAccess = SetOnlineImageAccess,
        .resetOnlineImageAccess = ResetOnlineImageAccess,
        .setMediaPlayGestureAccess = SetMediaPlayGestureAccess,
        .resetMediaPlayGestureAccess = ResetMediaPlayGestureAccess,
        .setMediaOptions = SetMediaOptions,
        .resetMediaOptions = ResetMediaOptions,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line
    return &modifier;
}

const CJUIWebModifier* GetCJUIWebModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const CJUIWebModifier modifier = {
        .getWebController = nullptr,
        .getWebviewController = nullptr,
        .setWebOptions = nullptr,
        .setWebSrcResources = nullptr,
        .setJavaScriptAccess = SetJavaScriptAccess,
        .resetJavaScriptAccess = ResetJavaScriptAccess,
        .setFileAccessEnabled = SetFileAccessEnabled,
        .resetFileAccessEnabled = ResetFileAccessEnabled,
        .setDomStorageAccessEnabled = SetDomStorageAccessEnabled,
        .resetDomStorageAccessEnabled = ResetDomStorageAccessEnabled,
        .setMixedMode = SetMixedMode,
        .resetMixedMode = ResetMixedMode,
        .setZoomAccessEnabled = SetZoomAccessEnabled,
        .resetZoomAccessEnabled = ResetZoomAccessEnabled,
        .setCacheMode = SetCacheMode,
        .resetCacheMode = ResetCacheMode,
        .setDarkMode = SetDarkMode,
        .resetDarkMode = ResetDarkMode,
        .setMultiWindowAccessEnabled = SetMultiWindowAccessEnabled,
        .resetMultiWindowAccessEnabled = ResetMultiWindowAccessEnabled,
        .setAllowWindowOpenMethod = SetAllowWindowOpenMethod,
        .resetAllowWindowOpenMethod = ResetAllowWindowOpenMethod,
        .setKeyboardAvoidMode = SetKeyboardAvoidMode,
        .resetKeyboardAvoidMode = ResetKeyboardAvoidMode,
        .setOnControllerAttached = SetOnControllerAttached,
        .resetOnControllerAttached = ResetOnControllerAttached,
        .setVerticalScrollBarAccessEnabled = SetVerticalScrollBarAccessEnabled,
        .resetVerticalScrollBarAccessEnabled = ResetVerticalScrollBarAccessEnabled,
        .setHorizontalScrollBarAccessEnabled = SetHorizontalScrollBarAccessEnabled,
        .resetHorizontalScrollBarAccessEnabled = ResetHorizontalScrollBarAccessEnabled,
        .setTextZoomRatio = SetTextZoomRatio,
        .resetTextZoomRatio = ResetTextZoomRatio,
        .setInitialScale = SetInitialScale,
        .resetInitialScale = ResetInitialScale,
        .setOnScrollCallBack = SetOnScrollCallBack,
        .resetOnScrollCallBack = ResetOnScrollCallBack,
        .setOnOverScrollCallBack = SetOnOverScrollCallBack,
        .resetOnOverScrollCallBack = ResetOnOverScrollCallBack,
        .setOnScaleChangeCallBack = SetOnScaleChangeCallBack,
        .resetOnScaleChangeCallBack = ResetOnScaleChangeCallBack,
        .setOnRequestSelectedCallBack = SetOnRequestSelectedCallBack,
        .resetOnRequestSelectedCallBack = ResetOnRequestSelectedCallBack,
        .setOnContextMenuHideCallBack= SetOnContextMenuHideCallBack,
        .resetOnContextMenuHideCallBack= ResetOnContextMenuHideCallBack,
        .setGeolocationAccess = SetGeolocationAccess,
        .resetGeolocationAccess = ResetGeolocationAccess,
        .setDatabaseAccess = SetDatabaseAccess,
        .resetDatabaseAccess = ResetDatabaseAccess,
        .setOverviewModeAccess = SetOverviewModeAccess,
        .resetOverviewModeAccess = ResetOverviewModeAccess,
        .setForceDarkAccess = SetForceDarkAccess,
        .resetForceDarkAccess = ResetForceDarkAccess,
        .setPinchSmooth = SetPinchSmooth,
        .resetPinchSmooth = ResetPinchSmooth,
        .setMetaViewport = SetMetaViewport,
        .resetMetaViewport = ResetMetaViewport,
        .setEnableFollowSystemFontWeight = SetEnableFollowSystemFontWeight,
        .resetEnableFollowSystemFontWeight = ResetEnableFollowSystemFontWeight,
        .setEnableNativeEmbedMode = SetEnableNativeEmbedMode,
        .resetEnableNativeEmbedMode = ResetEnableNativeEmbedMode,
        .setMinFontSize = SetMinFontSize,
        .resetMinFontSize = ResetMinFontSize,
        .setDefaultFontSize = SetDefaultFontSize,
        .resetDefaultFontSize = ResetDefaultFontSize,
        .setDefaultFixedFontSize = SetDefaultFixedFontSize,
        .resetDefaultFixedFontSize = ResetDefaultFixedFontSize,
        .setMinLogicalFontSize = SetMinLogicalFontSize,
        .resetMinLogicalFontSize = ResetMinLogicalFontSize,
        .setWebStandardFont = SetWebStandardFont,
        .resetWebStandardFont = ResetWebStandardFont,
        .setWebSerifFont = SetWebSerifFont,
        .resetWebSerifFont = ResetWebSerifFont,
        .setWebSansSerifFont = SetWebSansSerifFont,
        .resetWebSansSerifFont = ResetWebSansSerifFont,
        .setWebFixedFont = SetWebFixedFont,
        .resetWebFixedFont = ResetWebFixedFont,
        .setWebFantasyFont = SetWebFantasyFont,
        .resetWebFantasyFont = ResetWebFantasyFont,
        .setWebCursiveFont = SetWebCursiveFont,
        .resetWebCursiveFont = ResetWebCursiveFont,
        .setLayoutMode = SetLayoutMode,
        .resetLayoutMode = ResetLayoutMode,
        .setOnNativeEmbedLifecycleChange = SetOnNativeEmbedLifecycleChange,
        .resetOnNativeEmbedLifecycleChange = ResetOnNativeEmbedLifecycleChange,
        .setRegisterNativeEmbedRule = SetRegisterNativeEmbedRule,
        .resetRegisterNativeEmbedRule = ResetRegisterNativeEmbedRule,
        .setNativeEmbedOptions = SetNativeEmbedOptions,
        .resetNativeEmbedOptions = ResetNativeEmbedOptions,
        .setOnFirstContentfulPaint = SetOnFirstContentfulPaint,
        .resetOnFirstContentfulPaint = ResetOnFirstContentfulPaint,
        .setOnAudioStateChanged = SetOnAudioStateChanged,
        .resetOnAudioStateChanged = ResetOnAudioStateChanged,
        .setOnFullScreenExit = SetOnFullScreenExit,
        .resetOnFullScreenExit = ResetOnFullScreenExit,
        .setImageAccess = SetImageAccess,
        .resetImageAccess = ResetImageAccess,
        .setOnlineImageAccess = SetOnlineImageAccess,
        .resetOnlineImageAccess = ResetOnlineImageAccess,
        .setMediaPlayGestureAccess = SetMediaPlayGestureAccess,
        .resetMediaPlayGestureAccess = ResetMediaPlayGestureAccess,
        .setMediaOptions = SetMediaOptions,
        .resetMediaOptions = ResetMediaOptions,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line
    return &modifier;
}
} // namespace NodeModifier
} // namespace OHOS::Ace::NG