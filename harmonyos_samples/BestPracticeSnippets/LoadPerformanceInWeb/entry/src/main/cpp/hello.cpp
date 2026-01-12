// [Start hello_cpp]
// Registration objects and methods, sending scripts to callbacks after H5 execution, parsing instances passed from the side of the storage application and other code logics are not displayed here, and developers realize them by themselves according to their own business scenarios.
// Send the JS script to the H5 side for execution
#include "napi/native_api.h"
#include <bits/alltypes.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>

#include "hilog/log.h"
#include "web/arkweb_interface.h"
#include "jsbridge_object.h"

constexpr unsigned int LOG_PRINT_DOMAIN = 0xFF00;
std::shared_ptr<JSBridgeObject> jsbridge_object_ptr = nullptr;
static ArkWeb_ControllerAPI *controller = nullptr;
static ArkWeb_ComponentAPI *component = nullptr;

static void RunJavaScriptCallback(const char *webTag, const char *result, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk RunJavaScriptCallback webTag:%{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk RunJavaScriptCallback userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->RunJavaScriptCallback(result);
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb",
                     "ndk RunJavaScriptCallback jsb_weak_ptr lock failed");
    }
}

static void ProxyMethod1(const char *webTag, const ArkWeb_JavaScriptBridgeData *dataArray, size_t arraySize, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod1 webTag:%{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod1 userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->ProxyMethod1(dataArray, arraySize);
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod1 jsb_weak_ptr lock failed");
    }
}

static void ProxyMethod2(const char *webTag, const ArkWeb_JavaScriptBridgeData *dataArray, size_t arraySize, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod2 webTag:%{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod2 userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);

    std::string jsCode = "runJSRetStr()";
    ArkWeb_JavaScriptObject object = {(uint8_t *)jsCode.c_str(), jsCode.size(),
                                     &JSBridgeObject::StaticRunJavaScriptCallback,
                                     static_cast<void *>(jsbridge_object_ptr->GetWeakPtr())};
    controller->runJavaScript(webTag, &object);

    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->ProxyMethod2(dataArray, arraySize);
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ProxyMethod2 jsb_weak_ptr lock failed");
    }
}

void ValidCallback(const char *webTag, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ValidCallback webTag: %{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ValidCallback userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->SaySomething("ValidCallback");
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk ValidCallback jsb_weak_ptr lock failed");
    }
    
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk RegisterJavaScriptProxy begin");
    ArkWeb_ProxyMethod method1 = {"method1", ProxyMethod1, static_cast<void *>(jsbridge_object_ptr->GetWeakPtr())};
    ArkWeb_ProxyMethod method2 = {"method2", ProxyMethod2, static_cast<void *>(jsbridge_object_ptr->GetWeakPtr())};
    ArkWeb_ProxyMethod methodList[2] = {method1, method2};
    ArkWeb_ProxyObject proxyObject = {"ndkProxy", methodList, 2};
    controller->registerJavaScriptProxy(webTag, &proxyObject);

    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk RegisterJavaScriptProxy end");
}

void LoadStartCallback(const char *webTag, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadStartCallback webTag: %{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadStartCallback userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->SaySomething("LoadStartCallback");
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadStartCallback jsb_weak_ptr lock failed");
    }
}

void LoadEndCallback(const char *webTag, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadEndCallback webTag: %{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadEndCallback userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->SaySomething("LoadEndCallback");
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk LoadEndCallback jsb_weak_ptr lock failed");
    }
}

void DestroyCallback(const char *webTag, void *userData) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk DestroyCallback webTag: %{public}s", webTag);
    if (!userData) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk DestroyCallback userData is nullptr");
        return;
    }
    std::weak_ptr<JSBridgeObject> jsb_weak_ptr = *static_cast<std::weak_ptr<JSBridgeObject> *>(userData);
    if (auto jsb_ptr = jsb_weak_ptr.lock()) {
        jsb_ptr->SaySomething("DestroyCallback");
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk DestroyCallback jsb_weak_ptr lock failed");
    }
}

void SetComponentCallback(ArkWeb_ComponentAPI * component, const char* webTagValue) {
    if (!ARKWEB_MEMBER_MISSING(component, onControllerAttached)) {
        component->onControllerAttached(webTagValue, ValidCallback,
                                        static_cast<void *>(jsbridge_object_ptr->GetWeakPtr()));
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "ArkWeb", "component onControllerAttached func not exist");
    }

    if (!ARKWEB_MEMBER_MISSING(component, onPageBegin)) {
        component->onPageBegin(webTagValue, LoadStartCallback,
                                        static_cast<void *>(jsbridge_object_ptr->GetWeakPtr()));
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "ArkWeb", "component onPageBegin func not exist");
    }

    if (!ARKWEB_MEMBER_MISSING(component, onPageEnd)) {
        component->onPageEnd(webTagValue, LoadEndCallback,
                                        static_cast<void *>(jsbridge_object_ptr->GetWeakPtr()));
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "ArkWeb", "component onPageEnd func not exist");
    }

    if (!ARKWEB_MEMBER_MISSING(component, onDestroy)) {
        component->onDestroy(webTagValue, DestroyCallback,
                                        static_cast<void *>(jsbridge_object_ptr->GetWeakPtr()));
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "ArkWeb", "component onDestroy func not exist");
    }
}

static napi_value NativeWebInit(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk NativeWebInit start");
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    size_t webTagSize = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &webTagSize);
    char *webTagValue = new (std::nothrow) char[webTagSize + 1];
    size_t webTagLength = 0;
    napi_get_value_string_utf8(env, args[0], webTagValue, webTagSize + 1, &webTagLength);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "ArkWeb", "ndk NativeWebInit webTag:%{public}s", webTagValue);
    
    jsbridge_object_ptr = std::make_shared<JSBridgeObject>(webTagValue);
    if (jsbridge_object_ptr)
        jsbridge_object_ptr->Init();

    controller = reinterpret_cast<ArkWeb_ControllerAPI *>(OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_CONTROLLER));
    component = reinterpret_cast<ArkWeb_ComponentAPI *>(OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_COMPONENT));
    SetComponentCallback(component, webTagValue);

    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk NativeWebInit end");
    delete[] webTagValue;
    return nullptr;
}

static napi_value RunJavaScript(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    size_t webTagSize = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &webTagSize);
    char *webTagValue = new (std::nothrow) char[webTagSize + 1];
    size_t webTagLength = 0;
    napi_get_value_string_utf8(env, args[0], webTagValue, webTagSize + 1, &webTagLength);
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb", "ndk OH_NativeArkWeb_RunJavaScript webTag:%{public}s",
                 webTagValue);
    
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, args[1], nullptr, 0, &bufferSize);
    char *jsCode = new (std::nothrow) char[bufferSize + 1];
    size_t byteLength = 0;
    napi_get_value_string_utf8(env, args[1], jsCode, bufferSize + 1, &byteLength);

    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "ArkWeb",
                 "ndk OH_NativeArkWeb_RunJavaScript jsCode len:%{public}zu", strlen(jsCode));
    
    ArkWeb_JavaScriptObject object = {(uint8_t *)jsCode, bufferSize, &JSBridgeObject::StaticRunJavaScriptCallback,
                                     static_cast<void *>(jsbridge_object_ptr->GetWeakPtr())};
    controller->runJavaScript(webTagValue, &object);
    delete[] webTagValue;
    delete[] jsCode;
    return nullptr;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"nativeWebInit", nullptr, NativeWebInit, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"runJavaScript", nullptr, RunJavaScript, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
// [End hello_cpp]