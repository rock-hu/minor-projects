/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_ACE_DFX_ADAPTER_H
#define OHOS_ACELITE_ACE_DFX_ADAPTER_H

#include <cstdint>
#include "jsi_types.h"

namespace OHOS {
namespace ACELite {
/**
 * This handler type is so specific for product, should be designed from OS dfx level.
 */
typedef void (*EventPrintHandler)(uint8_t info1, uint8_t info2, uint8_t info3, uint8_t info4);
typedef void (*ErrCodePrintHandler)(uint8_t info1, uint8_t info2, uint8_t info3, uint16_t info4);

/**
 * The hook for outputing user's console log.
 */
typedef void (*JSLogOutputHandler)(uint8_t level, const char *content);

/**
 * The wrapper data structure for get native memory pool info.
 */
struct NativeMemInfo {
    uint32_t totalSize; /* mem pool total size */
    uint32_t freeSize;  /* total free size */
};

/**
 * Native memory pool info getter.
 */
typedef void (*NativeMemInfoGetter)(NativeMemInfo *memInfo);

/**
 * Product modules hook.
 */
typedef Module *(*ProductModulesGetter)(uint16_t &moduleCount);

/**
 * Private modules hook.
 */
typedef PrivateModule *(*PrivateModulesGetter)(uint16_t &moduleCount);

/**
 * The hook for terminating ability.
 */
typedef void (*TerminateAbilityHandler)(uint32_t token, bool forceStop);

/**
 * The hook for restore fail dump message.
 */
typedef void (*RestoreSystemHandler)(const char *crashMessage);

/**
 * The hook for determining the capability of supporting PNG and JPG image formats.
 */
typedef bool (*IsPNGSupportedHandler)(const char *imagePath, const char *bundleName);

/**
 * The hook for set component views parameters.
 */
typedef void (*SetViewsParaHandler)(void *ComponentHandler);

/**
 * As all the UI event handling is driven by the render tick, and to make sure the the event handling is
 * in JS task, the HAL layer will transfer the TE event into the loop of JS task, when JS application go foreground.
 */
typedef void (*RenderTEHandler)();
typedef void (*VoidFuncHook)();

/**
 * The hook for screen on visible.
 */
typedef bool (*SetScreenOnVisibleHandler)(bool visible);

/**
 * The hook for update default font.
 */
typedef void (*UpdateDefaultFontHandler)(const char* curLanguage, const char* curOrigion, bool hasJson);

/**
 * The wrapper data structure for get native memory pool info.
 */
struct TEHandlingHooks {
    RenderTEHandler renderTEHandler;
    VoidFuncHook renderEndHandler;
};

enum TEDispatchingResult : uint8_t {
    REFUSED = 0, // not on foreground, will refuse TE, vsync should send it to other
    ACCEPTED, // on foreground, and TE event is received successfully
    ACCEPT_FAILED, // on foreground, but TE event can not be received, vsync should be retried to send to ACE again
};

/**
 * The wrapper data structure for get native memory pool info.
 */
struct ExtraPresetModulesHook {
    VoidFuncHook loadingHandler;
    VoidFuncHook unloadingHandler;
};

/**
 * The wrapper class for some product feature implementation, since some interfaces are provided by specific product.
 */
class ProductAdapter final {
public:
    ProductAdapter(const ProductAdapter &) = delete;
    ProductAdapter &operator=(const ProductAdapter &) = delete;
    ProductAdapter(ProductAdapter &&) = delete;
    ProductAdapter &operator=(ProductAdapter &&) = delete;
    ProductAdapter() = delete;
    ~ProductAdapter() = delete;

    // initialization functions
    static void InitTraceHandlers(EventPrintHandler eventHandler, ErrCodePrintHandler errCodeHandler);
    static void InitAceTags(uint8_t *aceTags, uint8_t tagCount);
    static void InitConsoleNativeHandler(JSLogOutputHandler handler);
    static void InitNativeMemPoolHook(NativeMemInfoGetter getter);
    static void InitExtraModulesGetter(ProductModulesGetter productModuleGetter,
                                       PrivateModulesGetter privateModuleGetter);
    static void InitDeviceInfo(const char *deviceType);
    static void RegTerminatingHandler(TerminateAbilityHandler handler);
    static void RegTEHandlers(const TEHandlingHooks &teHandlingHooks);
    static TEDispatchingResult DispatchTEMessage();
    static void SetDefaultFontStyle(const char *defaultFontFamily, uint8_t defaultFontSize);
    static void SetScreenSize(uint16_t width, uint16_t height);
    static void RegSetScreenOnVisibleHandler(SetScreenOnVisibleHandler handler);
    static void RegExtraPresetModulesHook(ExtraPresetModulesHook hook);
    static void ConfigPrivateDataRootPath(const char *appDataRoot);
    static void RegRestoreSystemHandler(RestoreSystemHandler handler);
    static void RegIsPNGSupportedHandler(IsPNGSupportedHandler handler);
    static void RegSetViewsParaHandler(SetViewsParaHandler handler);
    static void RegUpdateDefaultFontHandler(UpdateDefaultFontHandler handler);

    // wrapper functions, for ace internal calling
    static void PrintEventTrace(uint8_t info2, uint8_t info3, uint8_t info4);
    static void PrintErrCode(uint8_t info2, uint16_t rfu);
    static void OutputJSConsoleLog(uint8_t level, const char *content);
    static void GetNativeMemInfo(NativeMemInfo *memInfo);
    static void SendTerminatingRequest(uint32_t token, bool forceStop);
    static bool UpdateDefaultFont();
    static bool IsTEHandlersRegisted();
    static void ProcessOneTE();
    static void NotifyRenderEnd();
    static const char *GetDefaultFontFamilyName();
    static uint8_t GetDefaultFontSize();
    static void UpdateRenderTickAcceptable(bool acceptable);
    static void GetScreenSize(uint16_t &width, uint16_t &height);
    static const char *GetDeviceType();
    static bool SetScreenOnVisible(bool visible);
    static void LoadExtraPresetModules();
    static void UnloadExtraPresetModules();
    static const char *GetPrivateDataRootPath();
    static void RestoreSystemWrapper(const char *crashMessage);
    static bool IsPNGSupportedWrapper(const char *imagePath, const char *bundleName);
    static void SetViewsParaWrapper(void *ComponentHandler);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_ACE_DFX_ADAPTER_H
