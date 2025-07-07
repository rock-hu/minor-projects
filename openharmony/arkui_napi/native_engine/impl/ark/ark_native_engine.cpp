/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ark_native_engine.h"
#include "ecmascript/napi/include/jsnapi_expo.h"

#ifdef ENABLE_HITRACE
#include <sys/prctl.h>
#endif

#include "ark_native_deferred.h"
#include "ark_native_reference.h"
#include "ark_native_timer.h"
#include "native_engine/native_utils.h"
#include "native_sendable.h"
#include "cj_support.h"
#include "securec.h"
#include "utils/file.h"
#include <cinttypes>
#if !defined(PREVIEW) && !defined(ANDROID_PLATFORM) && !defined(IOS_PLATFORM)
#include "parameters.h"
#include <uv.h>
#endif
#ifdef OHOS_STANDARD_PLATFORM
#include "unwinder.h"
#endif
#ifdef ENABLE_CONTAINER_SCOPE
#include "core/common/container_scope.h"
#endif
#if defined(ENABLE_EVENT_HANDLER)
#include "event_handler.h"
#endif

#ifdef ENABLE_HITRACE
#include "hitrace_meter.h"
#include "parameter.h"
#include "musl_preinit_common.h"
#include "memory_trace.h"
#include "ecmascript/base/config.h"

// LCOV_EXCL_START
struct alignas(8) HookJsConfig { // 8 is 8 bit
    int32_t jsStackReport = 0;
    uint8_t maxJsStackDepth = 0;
    bool jsFpUnwind = false;
    char filterNapiName[64] = { "" };
};

class BlockHookScope {
public:
    BlockHookScope()
    {
        previousState_ = __set_hook_flag(false);
    }
    ~BlockHookScope()
    {
        __set_hook_flag(previousState_);
    }
private:
    bool previousState_ {true};
};

static HookJsConfig* g_hookJsConfig = nullptr;
static std::once_flag g_hookOnceFlag;
static std::string JS_CALL_STACK_DEPTH_SEP = ","; // ',' is js call stack depth separator
static std::string JS_SYMBOL_FILEPATH_SEP = "|";  // '|' is js symbol and filepath separator
static constexpr uint64_t BUF_SIZE = 128;
#endif // ENABLE_HITRACE

using panda::JsiRuntimeCallInfo;
using panda::BooleanRef;
using panda::ObjectRef;
using panda::StringRef;
using panda::Global;
using panda::FunctionRef;
using panda::PrimitiveRef;
using panda::ArrayBufferRef;
using panda::TypedArrayRef;
using panda::PromiseCapabilityRef;
using panda::PropertyAttribute;
using panda::NativePointerRef;
using panda::SymbolRef;
using panda::IntegerRef;
using panda::DateRef;
using panda::BigIntRef;
using ArkCrashHolder = panda::ArkCrashHolder;
using ArkIdleMonitor = panda::ecmascript::ArkIdleMonitor;
using AsyncNativeCallbacksPack = panda::AsyncNativeCallbacksPack;
using EcmaVM = panda::ecmascript::EcmaVM;
using TriggerGCData = panda::TriggerGCData;

static constexpr auto PANDA_MAIN_FUNCTION = "_GLOBAL::func_main_0";
static constexpr auto PANDA_MODULE_NAME = "_GLOBAL_MODULE_NAME";
static constexpr auto PANDA_MODULE_NAME_LEN = 32;
static std::unordered_set<std::string> NATIVE_MODULE = {"system.app", "ohos.app", "system.router",
    "system.curves", "ohos.curves", "system.matrix4", "ohos.matrix4"};
static constexpr auto NATIVE_MODULE_PREFIX = "@native:";
static constexpr auto OHOS_MODULE_PREFIX = "@ohos:";
static constexpr int ARGC_THREE = 3;

// See `ArkNativeEngine::RequireNapi` for more information.
#ifndef PREVIEW
static constexpr const char* REQUIRE_NAPI_FUNCTION_NAME = "requireNapi";
#else
static constexpr const char* REQUIRE_NAPI_FUNCTION_NAME = "requireNapiPreview";
#endif
// See `ArkNativeEngine::RequireInternal` for more information.
static constexpr const char* REQUIRE_NAPI_INTERNAL_NAME = "requireInternal";

#ifdef ENABLE_HITRACE
constexpr auto NAPI_PROFILER_PARAM_SIZE = 10;
std::atomic<uint64_t> g_chainId = 0;
constexpr int NAPI_CALL_STACK = 2; // just for napi call stack
#endif

std::string ArkNativeEngine::tempModuleName_ {""};
bool ArkNativeEngine::napiProfilerEnabled {false};
bool ArkNativeEngine::napiProfilerParamReaded {false};
PermissionCheckCallback ArkNativeEngine::permissionCheckCallback_ {nullptr};
std::atomic<NapiModuleValidateCallback> ArkNativeEngine::moduleValidateCallback_ {nullptr};

// This interface is using by ace_engine
napi_value LocalValueToLocalNapiValue(panda::Local<panda::JSValueRef> local)
{
    return JsValueFromLocalValue(local);
}

// This interface is using by ace_engine
panda::Local<panda::JSValueRef> NapiValueToLocalValue(napi_value v)
{
    return LocalValueFromJsValue(v);
}

#ifdef ENABLE_CONTAINER_SCOPE
void FunctionSetContainerId(napi_env env, panda::Local<panda::JSValueRef> &value)
{
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsContainerScopeEnabled()) {
        return;
    }
    auto vm = engine->GetEcmaVm();
    if (!value->IsFunction(vm)) {
        return;
    }
    panda::Local<panda::FunctionRef> funcValue(value);
    if (funcValue->IsNative(vm)) {
        return;
    }

    auto extraInfo = funcValue->GetData(vm);
    if (extraInfo != nullptr) {
        return;
    }

    NapiFunctionInfo *funcInfo = NapiFunctionInfo::CreateNewInstance();
    if (funcInfo == nullptr) {
        HILOG_ERROR("funcInfo is nullptr");
        return;
    }
    funcInfo->scopeId = OHOS::Ace::ContainerScope::CurrentId();
    funcValue->SetData(vm, reinterpret_cast<void*>(funcInfo),
        [](void* env, void *externalPointer, void *data) {
            auto info = reinterpret_cast<NapiFunctionInfo*>(data);
            if (info != nullptr) {
                delete info;
                info = nullptr;
            }
        }, true);
}
#endif

static Local<panda::JSValueRef> NapiNativeCreateFunction(napi_env env, const char* name,
                                                         NapiNativeCallback cb, void* value)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto vm = const_cast<EcmaVM*>(engine->GetEcmaVm());
    NapiFunctionInfo* funcInfo = NapiFunctionInfo::CreateNewInstance();
    if (funcInfo == nullptr) {
        HILOG_ERROR("funcInfo is nullptr");
        return JSValueRef::Undefined(vm);
    }
    funcInfo->callback = cb;
    funcInfo->data = value;
    funcInfo->env = env;
#ifdef ENABLE_CONTAINER_SCOPE
    if (engine->IsContainerScopeEnabled()) {
        funcInfo->scopeId = OHOS::Ace::ContainerScope::CurrentId();
    }
#endif

    Local<JSValueRef> context = engine->GetContext();
    Local<panda::FunctionRef> fn = panda::FunctionRef::NewConcurrentWithName(vm, context, ArkNativeFunctionCallBack,
                                                                             CommonDeleter, name,
                                                                             reinterpret_cast<void*>(funcInfo), true);
    return fn;
}

static Local<panda::JSValueRef> NapiInitAttrValFromProp(napi_env env, const NapiPropertyDescriptor &property,
                                                        bool &writable, Local<panda::JSValueRef> *curKey)
{
    const EcmaVM *vm = reinterpret_cast<NativeEngine *>(env)->GetEcmaVm();
    Local<panda::JSValueRef> val = panda::JSValueRef::Undefined(vm);
    std::string fullName("");
    if (property.getter != nullptr || property.setter != nullptr) {
        Local<panda::JSValueRef> localGetter = panda::JSValueRef::Undefined(vm);
        Local<panda::JSValueRef> localSetter = panda::JSValueRef::Undefined(vm);
        if (property.getter != nullptr) {
            fullName += "getter";
            localGetter = NapiNativeCreateFunction(env, fullName.c_str(), property.getter, property.data);
        }
        if (property.setter != nullptr) {
            fullName += "setter";
            localSetter = NapiNativeCreateFunction(env, fullName.c_str(), property.setter, property.data);
        }
        val = panda::ObjectRef::CreateAccessorData(vm, localGetter, localSetter);
        writable = false; // the default writable of getter and setter is 'false'
    } else if (property.method != nullptr) {
        if (property.utf8name != nullptr) {
            fullName += property.utf8name;
        } else {
            fullName += (*curKey)->IsString(vm) ?
                        Local<panda::StringRef>(*curKey)->ToString(vm) :
                        Local<panda::SymbolRef>(*curKey)->GetDescription(vm)->ToString(vm);
        }
        val = NapiNativeCreateFunction(env, fullName.c_str(), property.method, property.data);
    } else {
        val = LocalValueFromJsValue(property.value);
    }
    return val;
}

static size_t NapiGetKeysAndAttrsFromProps(napi_env env, size_t propertyCount, const NapiPropertyDescriptor *properties,
                                           Local<panda::JSValueRef> *keys, PropertyAttribute *attrs)
{
    auto vm = reinterpret_cast<NativeEngine *>(env)->GetEcmaVm();
    size_t curNonStaticPropIdx = propertyCount - 1; // 1: last index of array is 'lenght - 1'.
    size_t curStaticPropIdx = 0;
    for (size_t i = 0; i < propertyCount; ++i) {
        const NapiPropertyDescriptor &property = properties[i];
        Local<panda::JSValueRef> *curKey;
        PropertyAttribute *curAttr;
        // Properties in keys and attrs are stored in the following way:
        // +------------------+ ... + -------------------------------------+ ... +---------------------+
        // | first staticProp | ... | last staticProp | last nonStaticProp | ... | first nonStaticProp |
        // +------------------+ ... + -------------------------------------+ ... +---------------------+
        if (properties[i].attributes & NATIVE_STATIC) {
            curKey = &keys[curStaticPropIdx];
            curAttr = &attrs[curStaticPropIdx];
            ++curStaticPropIdx;
        } else {
            curKey = &keys[curNonStaticPropIdx];
            curAttr = &attrs[curNonStaticPropIdx];
            --curNonStaticPropIdx;
        }
        if (property.utf8name != nullptr) {
            *curKey = panda::StringRef::NewFromUtf8(vm, property.utf8name);
        } else {
            *curKey = LocalValueFromJsValue(property.name);
        }
        bool writable = (property.attributes & NATIVE_WRITABLE) != 0;
        bool enumable = (property.attributes & NATIVE_ENUMERABLE) != 0;
        bool configable = (property.attributes & NATIVE_CONFIGURABLE) != 0;
        Local<panda::JSValueRef> val = NapiInitAttrValFromProp(env, property, writable, curKey);
        // ~PropertyAttribute() was called in NewConcurrentClassFunctionWithName
        new (curAttr) PropertyAttribute(val, writable, enumable, configable);
    }
    return curStaticPropIdx;
}

static Local<panda::FunctionRef> NapiCreateClassFunction(napi_env env, std::string &className,
                                                         NapiFunctionInfo* funcInfo, size_t propCount,
                                                         const NapiPropertyDescriptor* properties)
{
    const EcmaVM *vm = reinterpret_cast<NativeEngine*>(env)->GetEcmaVm();
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    Local<JSValueRef> context = engine->GetContext();
    Local<panda::FunctionRef> fn;
    if (propCount == 0) {
        fn = panda::FunctionRef::NewConcurrentClassFunctionWithName(vm, context, ArkNativeFunctionCallBack,
                                                                    CommonDeleter, className.c_str(),
                                                                    reinterpret_cast<void*>(funcInfo), true);
    } else if (propCount <= panda::ObjectRef::MAX_PROPERTIES_ON_STACK) {
        Local<panda::JSValueRef> keys[panda::ObjectRef::MAX_PROPERTIES_ON_STACK];
        PropertyAttribute attrs[panda::ObjectRef::MAX_PROPERTIES_ON_STACK];
        size_t staticPropCount = NapiGetKeysAndAttrsFromProps(env, propCount, properties, &keys[0], &attrs[0]);
        fn = panda::FunctionRef::NewConcurrentClassFunctionWithName(vm, context, ArkNativeFunctionCallBack,
                                                                    CommonDeleter, className.c_str(),
                                                                    reinterpret_cast<void*>(funcInfo), true, propCount,
                                                                    staticPropCount, &keys[0], &attrs[0]);
    } else {
        Local<panda::JSValueRef> *keys =
            reinterpret_cast<Local<panda::JSValueRef> *>(malloc(sizeof(Local<panda::JSValueRef>) * propCount));
        PropertyAttribute *attrs = reinterpret_cast<PropertyAttribute *>(malloc(sizeof(PropertyAttribute) * propCount));
        if (attrs != nullptr && keys != nullptr) {
            size_t staticPropCount = NapiGetKeysAndAttrsFromProps(env, propCount, properties, keys, attrs);
            fn = panda::FunctionRef::NewConcurrentClassFunctionWithName(vm, context, ArkNativeFunctionCallBack,
                                                                        CommonDeleter, className.c_str(),
                                                                        reinterpret_cast<void*>(funcInfo), true,
                                                                        propCount, staticPropCount, keys, attrs);
        } else {
            fn = panda::JSValueRef::Undefined(vm);
            napi_throw_error(env, nullptr, "malloc failed in napi_define_class");
        }

        if (keys != nullptr) {
            free(keys);
        }
        if (attrs != nullptr) {
            free(attrs);
        }
    }
    return fn;
}

panda::Local<panda::JSValueRef> NapiDefineClass(napi_env env, const char* name, NapiNativeCallback callback,
    void* data, const NapiPropertyDescriptor* properties, size_t length)
{
    auto vm = const_cast<EcmaVM*>(reinterpret_cast<NativeEngine*>(env)->GetEcmaVm());
    std::string className(name);
    if (ArkNativeEngine::napiProfilerEnabled) {
        className = ArkNativeEngine::tempModuleName_ + "." + name;
    }

    NapiFunctionInfo* funcInfo = NapiFunctionInfo::CreateNewInstance();
    if (funcInfo == nullptr) {
        HILOG_ERROR("funcInfo is nullptr");
        return panda::JSValueRef::Undefined(vm);
    }
    funcInfo->callback = callback;
    funcInfo->data = data;
    funcInfo->env = env;
#ifdef ENABLE_CONTAINER_SCOPE
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsContainerScopeEnabled()) {
        funcInfo->scopeId = OHOS::Ace::ContainerScope::CurrentId();
    }
#endif
    Local<panda::FunctionRef> fn = NapiCreateClassFunction(env, className, funcInfo, length, properties);

    Local<panda::ObjectRef> excep = JSNApi::GetUncaughtException(vm);
    if (!excep.IsNull()) {
        HILOG_DEBUG("ArkNativeObject::NapiDefineClass occur Exception");
        JSNApi::GetAndClearUncaughtException(vm);
    }
#ifdef ENABLE_HITRACE
    Local<panda::ObjectRef> classPrototype = fn->GetFunctionPrototype(vm);
    reinterpret_cast<ArkNativeEngine*>(env)->SetModuleName(classPrototype, className);
#endif
    return fn;
}

panda::Local<panda::JSValueRef> NapiDefineSendableClass(napi_env env,
                                                        const char* name,
                                                        NapiNativeCallback callback,
                                                        void* data,
                                                        const NapiPropertyDescriptor* properties,
                                                        size_t propertiesLength,
                                                        napi_value parent)
{
    const EcmaVM* vm = reinterpret_cast<NativeEngine*>(env)->GetEcmaVm();
    NapiFunctionInfo* funcInfo = NapiFunctionInfo::CreateNewInstance();
    if (funcInfo == nullptr) {
        HILOG_FATAL("funcInfo is nullptr");
        return JSValueRef::Undefined(vm);
    }
    funcInfo->callback = callback;
    funcInfo->data = data;
    funcInfo->isSendable = true;
    std::string className(name);
    if (ArkNativeEngine::napiProfilerEnabled) {
        className = ArkNativeEngine::tempModuleName_ + "." + name;
    }

    Local<panda::StringRef> fnName = panda::StringRef::NewFromUtf8(vm, className.c_str());
    Local<JSValueRef> localParent = JSValueRef::Hole(vm);
    if (parent != nullptr) {
        localParent = LocalValueFromJsValue(parent);
    }

    auto infos = NativeSendable::CreateSendablePropertiesInfos(env, properties, propertiesLength);
    Local<panda::FunctionRef> fn = panda::FunctionRef::NewSendableClassFunction(vm, ArkNativeFunctionCallBack,
        CommonDeleter, reinterpret_cast<void*>(funcInfo), fnName, infos, localParent, true);

    return fn;
}

struct MoudleNameLocker {
    explicit MoudleNameLocker(std::string moduleName)
    {
        ArkNativeEngine::tempModuleName_ = moduleName;
    }
    ~MoudleNameLocker()
    {
        ArkNativeEngine::tempModuleName_ = "";
    }
};

void* ArkNativeEngine::GetNativePtrCallBack(void* data)
{
    if (data == nullptr) {
        HILOG_ERROR("data is nullptr");
        return nullptr;
    }
    auto info = reinterpret_cast<NapiFunctionInfo*>(data);
    auto cb = reinterpret_cast<void*>(info->callback);
    return cb;
}

bool ArkNativeEngine::SetModuleValidateCallback(NapiModuleValidateCallback validateCallback)
{
    if (validateCallback == nullptr) {
        HILOG_WARN("Module Validate Callback nullptr. Please check input.");
        return false;
    }
    if (moduleValidateCallback_.load()) {
        HILOG_ERROR("Module Validate Callback is already set by another module or thread.");
        return false;
    }
    moduleValidateCallback_ = validateCallback;
    return true;
}

bool ArkNativeEngine::CheckArkApiAllowList(
    NativeModule* module, panda::ecmascript::ApiCheckContext context, panda::Local<panda::ObjectRef>& exportCopy)
{
    std::unique_ptr<ApiAllowListChecker>& apiAllowListChecker = module->apiAllowListChecker;
    if (apiAllowListChecker != nullptr) {
        const std::string apiPath = context.moduleName->ToString(context.ecmaVm);
        if ((*apiAllowListChecker)(apiPath)) {
            CopyPropertyApiFilter(apiAllowListChecker, context.ecmaVm, context.exportObj, exportCopy, apiPath);
        }
        return true;
    }
    return false;
}

void ArkNativeEngine::CopyPropertyApiFilter(const std::unique_ptr<ApiAllowListChecker>& apiAllowListChecker,
    const EcmaVM* ecmaVm, const panda::Local<panda::ObjectRef> exportObj, panda::Local<panda::ObjectRef>& exportCopy,
    const std::string& apiPath)
{
    panda::Local<panda::ArrayRef> namesArrayRef = exportObj->GetAllPropertyNames(ecmaVm, NATIVE_DEFAULT);
    for (uint32_t i = 0; i < namesArrayRef->Length(ecmaVm); ++i) {
        const panda::Local<panda::JSValueRef> nameValue = panda::ArrayRef::GetValueAt(ecmaVm, namesArrayRef, i);
        const panda::Local<panda::JSValueRef> value = exportObj->Get(ecmaVm, nameValue);
        const std::string curPath = apiPath + "." + nameValue->ToString(ecmaVm)->ToString(ecmaVm);
        if ((*apiAllowListChecker)(curPath)) {
            const std::string valueType = value->Typeof(ecmaVm)->ToString(ecmaVm);
            if (valueType == "object") {
                panda::Local<panda::ObjectRef> subObject = ObjectRef::New(ecmaVm);
                CopyPropertyApiFilter(apiAllowListChecker, ecmaVm, value, subObject, curPath);
                exportCopy->Set(ecmaVm, nameValue, subObject);
                HILOG_DEBUG("Set the package '%{public}s' to the allow list", curPath.c_str());
            } else if (valueType == "function") {
                exportCopy->Set(ecmaVm, nameValue, value);
                HILOG_DEBUG("Set the function '%{public}s' to the allow list", curPath.c_str());
            } else {
                exportCopy->Set(ecmaVm, nameValue, value);
                HILOG_DEBUG("Set the element type is '%{public}s::%{public}s' to the allow list", valueType.c_str(),
                    curPath.c_str());
            }
        }
    }
}

ArkNativeEngine::ArkNativeEngine(EcmaVM* vm, void* jsEngine, bool isLimitedWorker) : NativeEngine(jsEngine),
                                                                                     vm_(vm),
                                                                                     topScope_(vm),
                                                                                     isLimitedWorker_(isLimitedWorker),
                                                                                     isMainEnvContext_(true)
{
    HILOG_INFO("ArkNativeEngine is created, id %{public}" PRIu64, GetId());

    JSNApi::SetEnv(vm, this);
    // enable napi profiler
    EnableNapiProfiler();
    LocalScope scope(vm_);

    options_ = new NapiOptions();
    // Update cache of ark runtime cross thread check option.
    UpdateCrossThreadCheckStatus();
#ifdef ENABLE_CONTAINER_SCOPE
    containerScopeEnable_ = OHOS::system::GetBoolParameter("persist.ace.napiContainerScope.enabled", true);
#endif
#if defined(OHOS_PLATFORM) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    int napiProperties = OHOS::system::GetIntParameter<int>("persist.ark.napi.properties", -1);
    if (options_ != nullptr) {
        options_->SetProperties(napiProperties);
    }
#endif

    // init common requireNapi and requireInternal
    void* requireData = static_cast<void*>(this);
    Local<ObjectRef> global = JSNApi::GetGlobalObject(vm);
    global->Set(vm, StringRef::NewFromUtf8(vm, REQUIRE_NAPI_FUNCTION_NAME),
                FunctionRef::New(vm, RequireNapi, nullptr, requireData));

    global->Set(vm, StringRef::NewFromUtf8(vm, REQUIRE_NAPI_INTERNAL_NAME),
                FunctionRef::New(vm, RequireInternal, nullptr, requireData));

    JSNApi::SetNativePtrGetter(vm, reinterpret_cast<void*>(ArkNativeEngine::GetNativePtrCallBack));
    // need to call init of base class.
    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    std::function<bool(const std::string&)> func = [moduleManager](const std::string& moduleKey) -> bool {
        return moduleManager->UnloadNativeModule(moduleKey);
    };
    JSNApi::SetUnloadNativeModuleCallback(vm, func);
    Init();
    JSNApi::SetLoop(vm, loop_);
    DFXJSNApi::RegisterAsyncDetectCallBack(vm);
    JSNApi::SetWeakFinalizeTaskCallback(vm, [this] () -> void {
        this->PostFinalizeTasks();
    });
    JSNApi::SetAsyncCleanTaskCallback(vm, [this] (AsyncNativeCallbacksPack *callbacksPack) {
        this->PostAsyncTask(callbacksPack);
    });
    JSNApi::SetHostPromiseRejectionTracker(vm_, reinterpret_cast<void*>(PromiseRejectCallback),
                                           reinterpret_cast<void*>(this));
    JSNApi::SetTimerTaskCallback(vm, NativeTimerCallbackInfo::TimerTaskCallback);
    JSNApi::SetCancelTimerCallback(vm, NativeTimerCallbackInfo::CancelTimerCallback);
    JSNApi::NotifyEnvInitialized(vm);
    if (IsMainThread()) {
        RegisterAllPromiseCallback([this] (napi_value* args) -> void {
            if (!NativeEngine::IsAlive(this)) {
                HILOG_WARN("napi_env has been destoryed!");
                return;
            }
            NapiErrorManager::GetInstance()->NotifyUnhandledRejection(reinterpret_cast<napi_env>(this), args, "", 0);
        });
    }

    // enable idle gc
    ArkIdleMonitor::GetInstance()->EnableIdleGC(this);

    // save the first context
    context_ = Global<JSValueRef>(vm, JSNApi::GetCurrentContext(vm));
}

/**
 * Constructor for Ark Context Engine
 * 1. Must be created from the root engine.
 * 2. Does not directly provide UV capabilities (i.e., libuv-related functionality).
 * 3. Async capabilities are provided by the parent engine.
 */
ArkNativeEngine::ArkNativeEngine(NativeEngine* parent, EcmaVM* vm, const Local<JSValueRef>& context)
    : NativeEngine(parent),
      vm_(vm),
      topScope_(vm),
      context_(vm, context),
      parentEngine_(reinterpret_cast<ArkNativeEngine*>(parent)),
      containerScopeEnable_(parent->IsContainerScopeEnabled()),
      isMainEnvContext_(false),
      isMultiContextEnabled_(true)
{
    HILOG_INFO("ArkContextEngine is created, id %{public}" PRIu64, GetId());

    // The sub-context environment must be deleted before the uv_loop is destroyed.
    // In this case, we cannot delay the destruction of the context environment
    // until all napi_ref callbacks are completed.
    parent->AddCleanupHook(EnvironmentCleanup, this);

    parent->SetMultiContextEnabled(true);

    LocalScope scope(vm);

    // enable napi profiler
    EnableNapiProfiler();

    // Update cache of ark runtime cross thread check option.
    UpdateCrossThreadCheckStatus();

    options_ = new NapiOptions();
#if defined(OHOS_PLATFORM) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    int napiProperties = OHOS::system::GetIntParameter<int>("persist.ark.napi.properties", -1);
    if (options_ != nullptr) {
        options_->SetProperties(napiProperties);
    }
#endif

    // init common requireNapi and requireInternal
    void* requireData = static_cast<void*>(this);
    Local<ObjectRef> global = JSNApi::GetGlobalObject(vm, context);
    global->Set(vm, StringRef::NewFromUtf8(vm, REQUIRE_NAPI_FUNCTION_NAME),
                FunctionRef::New(vm, context, RequireNapiForCtxEnv, nullptr, requireData));
    global->Set(vm, StringRef::NewFromUtf8(vm, REQUIRE_NAPI_INTERNAL_NAME),
                FunctionRef::New(vm, context, RequireInternal, nullptr, requireData));

    InitWithoutUV();
    // The VM scope callback is initialized during the construction of the root engine and is ignored in this context.
}

ArkNativeEngine::~ArkNativeEngine()
{
    HILOG_INFO("%{public}s %{public}" PRIu64 " is deconstructed",
               isMainEnvContext_ ? "ArkNativeEngine" : "ArkContextEngine", GetId());

    engineState_ = ArkNativeEngineState::RELEASING;

    if (isMainEnvContext_) {
        // unregister worker env for idle GC
        ArkIdleMonitor::GetInstance()->UnregisterEnv(this);
        JSNApi::SetTimerTaskCallback(vm_, nullptr);
        JSNApi::SetCancelTimerCallback(vm_, nullptr);
        NativeTimerCallbackInfo::ReleaseTimerList(this);
        // destroy looper resource on the ark native engine
        Deinit();
        if (JSNApi::IsJSMainThreadOfEcmaVM(vm_)) {
            ArkIdleMonitor::GetInstance()->SetMainThreadEcmaVM(nullptr);
        }
    } else {
        DeconstructCtxEnv();
    }
    // Free cached module objects
    for (auto&& [module, exportObj] : loadedModules_) {
        exportObj.FreeGlobalHandleAddr();
    }
    // Free callbackRef
    if (promiseRejectCallbackRef_ != nullptr) {
        delete promiseRejectCallbackRef_;
    }
    if (checkCallbackRef_ != nullptr) {
        delete checkCallbackRef_;
    }
    if (options_ != nullptr) {
        delete options_;
        options_ = nullptr;
    }
}

ArkNativeEngine *ArkNativeEngine::New(NativeEngine* engine, EcmaVM* vm, const Local<JSValueRef>& context)
{
    if (engine == nullptr) {
        HILOG_FATAL("Invalid arguments, parameter engine cannot be nullptr");
    }
    if (vm == nullptr) {
        HILOG_FATAL("Invalid arguments, parameter vm cannot be nullptr");
    }
    if (!engine->IsMainEnvContext()) {
        HILOG_FATAL("Invalid arguments, parameter engine cannot be Ark Context Engine");
    }
    if (!context->IsJsGlobalEnv(vm)) {
        HILOG_FATAL("Invalid arguments, parameter context must be JsGlobalEnv");
    }
    return new ArkNativeEngine(engine, vm, context);
}

bool ArkNativeEngine::IsReadyToDelete()
{
    // Fast-path optimization: main contexts are not deletable and common case.
    // Also ensure the environment is only deleted once (not during destruction).
    return !isMainEnvContext_ && // Check whether current engine is context engine.
        engineState_ == ArkNativeEngineState::STOPPED && // Check whether `DestroyContext` is invoked,
                                                         // and engine is not deconstructing.
        !HasRuntimeOwnedRef(); // Check whether runtime owned ref is released.
}

void ArkNativeEngine::Delete()
{
    context_.FreeGlobalHandleAddr();
    delete this;
}


/**
 * Replacement of NativeEngine::Deinit
 * The Ark Context Engine does not maintain its own uv_loop.
 * Any related resource lists will be checked and cleaned up when the context is destroyed.
 *
 * Cleanup Steps:
 * 1. Cleanup hooks will be invoked to handle any necessary cleanup logic.
 * 2. Owned napi_async_work and napi_threadsafe_function must either
 *    complete execution or be released before the context is destroyed.
 *    - ERROR: uv_run is not supported when loop is running.
 *    - Check `tsfn` and incomplete `async_work` counts, abort if not equal to 0.
 * 3. Trigger all managed reference finalize callbacks.
 * 4. Mark the engine as dead (this is particularly important for preventing conflicts
 *    with any future engine instances that may be created at the same memory address).
 * 5. Recheck async work and tsfn counts to ensure no task leaks after finalize callbacks.
 */
void ArkNativeEngine::DeconstructCtxEnv()
{
    // To avoid blocking the thread, do not wait for any incomplete asynchronous tasks here.
    RunCleanupHooks(false);
    DeinitWithoutUV();

    parentEngine_->RemoveCleanupHook(EnvironmentCleanup, this);

    if (HasWaitingRequest()) {
        HILOG_FATAL("Do not release Ark Context Engine until all async work done");
    }

    // An active TSFN blocks uv_run, indirectly keeping the engine alive in Node.js (unless unrefed).
    // Context doesn't have its own uv_loop, so abort if still exists.
    if (HasActiveTsfn()) {
        HILOG_FATAL("Failed to release Ark Context Engine: active napi_threadsafe_function still exists.");
    }
    SetDead();
    // twice check after wrap/finalizer callbacks
    if (HasWaitingRequest()) {
        HILOG_FATAL("Do not release Ark Context Engine env until all async work done");
    }
    if (HasActiveTsfn()) {
        HILOG_FATAL("Failed to release Ark Context Engine: active napi_threadsafe_function still exists.");
    }

    if (HasNonCallbackRef()) {
        HILOG_ERROR("Non-callback napi_ref is leak after context env teardown, counts: %{public}" PRIu64,
            GetNonCallbackRefCount());
    }
    if (HasCallbackbleRef()) {
        HILOG_ERROR("Callbackble napi_ref is leak after context env teardown, counts: %{public}" PRIu64,
            GetCallbackbleRefCount());
    }
    parentEngine_ = nullptr;
}

void ArkNativeEngine::EnvironmentCleanup(void* arg)
{
    reinterpret_cast<ArkNativeEngine*>(arg)->Delete();
}

Local<JSValueRef> ArkNativeEngine::GetContext() const
{
    return context_.ToLocal();
}

const ArkNativeEngine* ArkNativeEngine::GetParent() const
{
    return parentEngine_;
}

int ArkNativeEngine::CheckAndGetModule(
    JsiRuntimeCallInfo *info,
    NativeModuleManager* moduleManager,
    bool isAppModule,
    Local<panda::StringRef> &moduleName,
    NativeModule *&module,
    Local<JSValueRef> &exports,
    std::string &errInfo)
{
#ifdef IOS_PLATFORM
    if (isLimitedWorker_) {
        if (!moduleManager->CheckModuleRestricted(moduleName->ToString(vm_).c_str())) {
            HILOG_ERROR("module %{public}s does not found in whitelist",
                moduleName->ToString(vm_).c_str());
            return -1;
        }
    }
    module = moduleManager->LoadNativeModule(
        moduleName->ToString(vm_).c_str(), nullptr, false, errInfo, false, "");
    return 0;
#else
    isAppModule_ = isAppModule;
    if (isLimitedWorker_ && !isAppModule) {
        if (!moduleManager->CheckModuleRestricted(moduleName->ToString(vm_).c_str())) {
            HILOG_ERROR("module %{public}s does not found in whitelist",
                moduleName->ToString(vm_).c_str());
            return -1;
        }
    }

    if (info->GetArgsNumber() == 3) { // 3:Determine if the number of parameters is equal to 3
        Local<StringRef> path(info->GetCallArgRef(2)); // 2:Take the second parameter
        if (UNLIKELY(IsCJModule(moduleName->ToString(vm_).c_str()))) {
            exports = NapiValueToLocalValue(
                LoadCJModule((napi_env)this, moduleName->ToString(vm_).c_str()));
            return 1;
        }
        module = moduleManager->LoadNativeModule(moduleName->ToString(vm_).c_str(),
            path->ToString(vm_).c_str(), isAppModule, errInfo, false, "");
    } else if (info->GetArgsNumber() == 4) { // 4:Determine if the number of parameters is equal to 4
        Local<StringRef> path(info->GetCallArgRef(2)); // 2:Take the second parameter
        Local<StringRef> relativePath(info->GetCallArgRef(3)); // 3:Take the second parameter
        module = moduleManager->LoadNativeModule(moduleName->ToString(vm_).c_str(), nullptr, isAppModule,
            errInfo, false, relativePath->ToString(vm_).c_str());
    } else {
        module = moduleManager->LoadNativeModule(moduleName->ToString(vm_).c_str(),
            nullptr, isAppModule, errInfo, false, "");
    }
    return 0;
#endif
}

Local<JSValueRef> ArkNativeEngine::LoadNativeModule(
    NativeModuleManager* moduleManager,
    Local<StringRef> &moduleName,
    NativeModule* module,
    Local<JSValueRef> exports,
    std::string &errInfo)
{
    if (module == nullptr) {
        return exports;
    }
    panda::EscapeLocalScope scope(vm_);
    auto it = loadedModules_.find(module);
    if (it != loadedModules_.end()) {
        return scope.Escape(it->second.ToLocal(vm_));
    }
    std::string strModuleName = moduleName->ToString(vm_);
    moduleManager->SetNativeEngine(strModuleName, this);
    MoudleNameLocker nameLocker(strModuleName);

    if (module->jsCode == nullptr && module->getABCCode != nullptr) {
        module->getABCCode(&module->jsCode, &module->jsCodeLen);
    }
    if (module->jsABCCode != nullptr || module->jsCode != nullptr) {
        char fileName[NAPI_PATH_MAX] = { 0 };
        const char* name = module->name;
        if (sprintf_s(fileName, sizeof(fileName), "lib%s.z.so/%s.js", name, name) == -1) {
            HILOG_ERROR("sprintf_s file name failed");
            return scope.Escape(exports);
        }
        HILOG_DEBUG("load js code from %{public}s", fileName);
        const void *buffer = nullptr;
        if (module->jsABCCode) {
            buffer = static_cast<const void *>(module->jsABCCode);
        } else {
            buffer = static_cast<const void *>(module->jsCode);
        }
        auto exportObject = LoadArkModule(buffer,
            module->jsCodeLen, fileName);
        if (exportObject->IsUndefined()) {
            HILOG_ERROR("load module failed");
            return scope.Escape(exports);
        } else {
            exports = exportObject;
            loadedModules_[module] = Global<JSValueRef>(vm_, exports);
        }
    } else if (module->registerCallback != nullptr) {
        Local<ObjectRef> exportObj = ObjectRef::New(vm_);
#ifdef ENABLE_HITRACE
        StartTrace(HITRACE_TAG_ACE, "NAPI module init, name = " + std::string(module->name));
#endif
        SetModuleName(exportObj, module->name);
        module->registerCallback(reinterpret_cast<napi_env>(this),
                                 JsValueFromLocalValue(exportObj));
#ifdef ENABLE_HITRACE
        FinishTrace(HITRACE_TAG_ACE);
#endif
        panda::Local<panda::ObjectRef> exportCopy = panda::ObjectRef::New(vm_);
        panda::ecmascript::ApiCheckContext context{moduleManager, vm_, moduleName, exportObj, scope};
        if (CheckArkApiAllowList(module, context, exportCopy)) {
            return scope.Escape(exportCopy);
        }
        exports = exportObj;
        loadedModules_[module] = Global<JSValueRef>(vm_, exports);
    } else {
        HILOG_ERROR("init module failed");
        return scope.Escape(exports);
    }

    return scope.Escape(exports);
}

/** require napi module for Ark Native Engine (standard napi_env).
 *
 * TypeScript declaration
 * - for iOS
 *   function requireNapi(name: string): any;
 * - other Platform
 *   function requireNapi(
 *       name: string,
 *       appModule: boolean = false,
 *       path?: string,
 *       relativePath?: string): any;
 */
Local<JSValueRef> ArkNativeEngine::RequireNapi(JsiRuntimeCallInfo *info)
{
    EcmaVM *ecmaVm = info->GetVM();
    panda::EscapeLocalScope scope(ecmaVm);
    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    ArkNativeEngine* arkNativeEngine = static_cast<ArkNativeEngine*>(info->GetData());
    std::string errInfo = "";
    Local<JSValueRef> exports(JSValueRef::Undefined(ecmaVm));
    if (info->GetArgsNumber() == 0) {
        return scope.Escape(exports);
    }
    Local<StringRef> moduleName(info->GetCallArgRef(0));

    bool isAppModule = false;
    if (info->GetArgsNumber() > 1) { // 1: index of arguments, isAppModule
        Local<BooleanRef> ret(info->GetCallArgRef(1));
        isAppModule = ret->Value();
    }

    NativeModule* module = nullptr;
    // Returns the module exports or undefined
    // if the module is fully loaded (code 1)
    // or has failed to load (code -1).
    if (arkNativeEngine->CheckAndGetModule(info, moduleManager, isAppModule,
        moduleName, module, exports, errInfo) != 0) {
        return scope.Escape(exports);
    }
    // process loaded module
    if (module) {
        return scope.Escape(arkNativeEngine->LoadNativeModule(moduleManager, moduleName, module, exports, errInfo));
    } else {
        HILOG_INFO("%{public}s", errInfo.c_str());
        return scope.Escape(panda::ObjectRef::CreateNativeModuleFailureInfo(ecmaVm, errInfo));
    }
}

/** require napi module for Ark Context Engine.
 *  1. The app module is not supported.
 *  2. Whitelist verification will be performed during the callback provided by ArkUI.
 *
 * TypeScript declaration
 * - for iOS
 *   function requireNapi(name: string): any;
 * - other Platform
 *   function requireNapi(
 *       name: string,
 *       appModule: boolean = false,
 *       path?: string,
 *       relativePath?: string): any;
 */
Local<JSValueRef> ArkNativeEngine::RequireNapiForCtxEnv(JsiRuntimeCallInfo *info)
{
    EcmaVM *ecmaVm = info->GetVM();
    panda::EscapeLocalScope scope(ecmaVm);
    Local<JSValueRef> exports(JSValueRef::Undefined(ecmaVm));
    if (info->GetArgsNumber() == 0) {
        return scope.Escape(exports);
    }
    Local<StringRef> moduleName(info->GetCallArgRef(0));

    bool isAppModule = false;
    if (info->GetArgsNumber() > 1) { // 1: index of arguments, isAppModule
        Local<BooleanRef> ret(info->GetCallArgRef(1));
        isAppModule = ret->Value();
    }
    if (isAppModule) {
        HILOG_FATAL("AppModule is not supported in Context Engine.");
    }

    std::string modNameCstr = moduleName->ToString(ecmaVm);
    NapiModuleValidateCallback modCheckCb = moduleValidateCallback_.load();
    if (modCheckCb == nullptr) {
        HILOG_ERROR("Cannot load native modules: module validation callback is null.");
        return scope.Escape(exports);
    }
    if (!modCheckCb(modNameCstr.c_str())) {
        HILOG_ERROR("Module \"%{public}s\" is rejected by the module validation callback.", modNameCstr.c_str());
        return scope.Escape(exports);
    }

    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    ArkNativeEngine* arkNativeEngine = static_cast<ArkNativeEngine*>(info->GetData());
    std::string errInfo = "";
    NativeModule* module = nullptr;
    // Returns the module exports or undefined
    // if the module is fully loaded (code 1)
    // or has failed to load (code -1).
    if (arkNativeEngine->CheckAndGetModule(info, moduleManager, isAppModule,
        moduleName, module, exports, errInfo) != 0) {
        return scope.Escape(exports);
    }
    // process loaded module
    if (module) {
        return scope.Escape(arkNativeEngine->LoadNativeModule(moduleManager, moduleName, module, exports, errInfo));
    } else {
        HILOG_INFO("%{public}s", errInfo.c_str());
        return scope.Escape(panda::ObjectRef::CreateNativeModuleFailureInfo(ecmaVm, errInfo));
    }
}

/** Require native part for mixed module
 *
 * Only support in system mixed module, app module would not have AtkTS part.
 *
 * TypeScript declaration
 * function requireInternal(moduleName: string, relativePath?: string): any;
 */
Local<JSValueRef> ArkNativeEngine::RequireInternal(JsiRuntimeCallInfo *info)
{
    EcmaVM *ecmaVm = info->GetVM();
    panda::EscapeLocalScope scope(ecmaVm);
    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    ArkNativeEngine* arkNativeEngine = static_cast<ArkNativeEngine*>(info->GetData());
    Local<StringRef> moduleName(info->GetCallArgRef(0));
    std::string errInfo = "";
    Local<JSValueRef> exports(JSValueRef::Undefined(ecmaVm));
    if (arkNativeEngine->isLimitedWorker_) {
        if (!moduleManager->CheckModuleRestricted(moduleName->ToString(ecmaVm).c_str())) {
            HILOG_ERROR("module %{public}s does not found in whitelist",
                moduleName->ToString(ecmaVm).c_str());
            return scope.Escape(exports);
        }
    }
    NativeModule* module = moduleManager->LoadNativeModule(moduleName->ToString(ecmaVm).c_str(),
        nullptr, false, errInfo, false, "");
    MoudleNameLocker nameLocker(moduleName->ToString(ecmaVm).c_str());
    if (module != nullptr && arkNativeEngine) {
        if (module->registerCallback == nullptr) {
            if (module->name != nullptr) {
                HILOG_ERROR("requireInternal Init function is nullptr. module name: %{public}s",
                    module->name);
            } else {
                HILOG_ERROR("requireInternal Init function is nullptr.");
            }
            return scope.Escape(exports);
        }

        auto it = arkNativeEngine->loadedModules_.find(module);
        if (it != arkNativeEngine->loadedModules_.end()) {
            return scope.Escape(it->second.ToLocal(ecmaVm));
        }
        std::string strModuleName = moduleName->ToString(ecmaVm);
        moduleManager->SetNativeEngine(strModuleName, arkNativeEngine);
        Local<ObjectRef> exportObj = ObjectRef::New(ecmaVm);
        if (exportObj->IsObject(ecmaVm)) {
            arkNativeEngine->SetModuleName(exportObj, module->name);
            module->registerCallback(reinterpret_cast<napi_env>(arkNativeEngine),
                                     JsValueFromLocalValue(exportObj));
            panda::Local<panda::ObjectRef> exportCopy = panda::ObjectRef::New(ecmaVm);
            panda::ecmascript::ApiCheckContext context{moduleManager, ecmaVm, moduleName, exportObj, scope};
            if (CheckArkApiAllowList(module, context, exportCopy)) {
                return scope.Escape(exportCopy);
            }
            exports = exportObj;
            arkNativeEngine->loadedModules_[module] = Global<JSValueRef>(ecmaVm, exports);
        } else {
            HILOG_ERROR("exportObject is nullptr");
            return scope.Escape(exports);
        }
    }
    return scope.Escape(exports);
}

#ifdef ENABLE_HITRACE
static inline bool CheckHookConfig(const std::string &nameRef)
{
    if (g_hookJsConfig == nullptr || g_hookJsConfig->jsStackReport <= 0 ||
        g_hookJsConfig->maxJsStackDepth == 0 || !g_hookJsConfig->jsFpUnwind) {
        return false;
    } else if (g_hookJsConfig->filterNapiName[0] != '\0' &&
        nameRef.find(g_hookJsConfig->filterNapiName) != std::string::npos) {
        return false;
    }
    return true;
}
#endif

static inline uint64_t StartNapiProfilerTrace(panda::JsiRuntimeCallInfo* runtimeInfo, NativeEngine* engine, void* cb)
{
#ifdef ENABLE_HITRACE
    if (ArkNativeEngine::napiProfilerEnabled) {
        EcmaVM *vm = runtimeInfo->GetVM();
        LocalScope scope(vm);
        Local<panda::FunctionRef> fn = runtimeInfo->GetFunctionRef();
        Local<panda::StringRef> nameRef = fn->GetName(vm);
        char threadName[BUF_SIZE];
        prctl(PR_GET_NAME, threadName);
        StartTraceArgs(HITRACE_TAG_ACE, "Napi called:%s, tname:%s", nameRef->ToString(vm).c_str(), threadName);
    }
    bool hookFlag = __get_hook_flag() && __get_global_hook_flag();
    if (!hookFlag) {
        return 0;
    }
    EcmaVM* vm = runtimeInfo->GetVM();
    LocalScope scope(vm);
    Local<panda::FunctionRef> fn = runtimeInfo->GetFunctionRef();
    Local<panda::StringRef> nameRef = fn->GetName(vm);
    if (g_hookJsConfig == nullptr) {
        std::call_once(g_hookOnceFlag, []() { g_hookJsConfig = (HookJsConfig*)__get_hook_config(); });
    }
    // add memtrace function
    if (g_hookJsConfig != nullptr && g_hookJsConfig->jsStackReport == NAPI_CALL_STACK && !g_hookJsConfig->jsFpUnwind) {
        OHOS::HiviewDFX::HiTraceChain::ClearId();
        std::unique_ptr<OHOS::HiviewDFX::HiTraceId> arkCallBackTraceId = std::make_unique<OHOS::HiviewDFX::HiTraceId>(
            OHOS::HiviewDFX::HiTraceChain::Begin("New ArkCallBackTrace", 0));
        char buffer[256] = {0}; // 256 : buffer size of tag name
        if (sprintf_s(buffer, sizeof(buffer), "napi:0x%x:%s", arkCallBackTraceId->GetChainId(),
                      nameRef->ToString(vm).c_str()) == -1) {
            return 0;
        }
        uint64_t addr = reinterpret_cast<uint64_t>(cb);
        ++g_chainId;
        (void)memtrace(reinterpret_cast<void*>(addr + g_chainId), 8, buffer, true); // 8: the size of addr
        return 0;
    }
    if (!CheckHookConfig(nameRef->ToString(vm))) {
        return 0;
    }
    BlockHookScope blockHook; // block hook
    std::string rawStack;
    std::vector<JsFrameInfo> jsFrames;
    uint64_t nestChainId = 0;
    jsFrames.reserve(g_hookJsConfig->maxJsStackDepth);
    engine->BuildJsStackInfoListWithCustomDepth(jsFrames);
    std::stringstream ssRawStack;
    for (size_t i = 0; i < jsFrames.size() && i < g_hookJsConfig->maxJsStackDepth; i++) {
        ssRawStack << jsFrames[i].functionName << JS_SYMBOL_FILEPATH_SEP << jsFrames[i].fileName << ":" <<
            jsFrames[i].pos;
        if (i < jsFrames.size() - 1) {
            ssRawStack << JS_CALL_STACK_DEPTH_SEP;
        }
    }
    rawStack = ssRawStack.str();
    OHOS::HiviewDFX::HiTraceChain::Begin("ArkNativeFunctionCallBack", 0);
    OHOS::HiviewDFX::HiTraceId hitraceId = OHOS::HiviewDFX::HiTraceChain::GetId();
    // resolve nested calls to napi and ts
    if (hitraceId.IsValid()) {
        nestChainId = hitraceId.GetChainId();
    }
    uint64_t chainId = ++g_chainId;
    hitraceId.SetChainId(chainId);
    OHOS::HiviewDFX::HiTraceChain::SetId(hitraceId);
    __send_hook_misc_data(chainId, rawStack.c_str(), rawStack.size() + 1, 1);
    return nestChainId;

#endif
    return 0;
}

static inline void FinishNapiProfilerTrace(uint64_t value)
{
#ifdef ENABLE_HITRACE
    if (ArkNativeEngine::napiProfilerEnabled) {
        FinishTrace(HITRACE_TAG_ACE);
    }
    bool hookFlag = __get_hook_flag() && __get_global_hook_flag();
    if (!hookFlag) {
        return;
    }
    BlockHookScope blockHook; // block hook
    OHOS::HiviewDFX::HiTraceId hitraceId = OHOS::HiviewDFX::HiTraceChain::GetId();
    if (hitraceId.IsValid()) {
        OHOS::HiviewDFX::HiTraceChain::End(hitraceId);
        OHOS::HiviewDFX::HiTraceChain::ClearId();
    }
    // resolve nested calls to napi and ts
    if (value) {
        hitraceId.SetChainId(value);
        OHOS::HiviewDFX::HiTraceChain::SetId(hitraceId);
    }

#endif
}

template <bool changeState>
panda::JSValueRef ArkNativeFunctionCallBack(JsiRuntimeCallInfo *runtimeInfo)
{
    EcmaVM *vm = runtimeInfo->GetVM();
    panda::LocalScope scope(vm);
    bool getStackBeforeCallNapiSuccess = false;
    JSNApi::GetStackBeforeCallNapiSuccess(vm, getStackBeforeCallNapiSuccess);
    auto info = reinterpret_cast<NapiFunctionInfo*>(runtimeInfo->GetData());
    napi_env env = nullptr;
    if (info->isSendable) {
        env = reinterpret_cast<napi_env>(JSNApi::GetEnv(vm));
    } else {
        env = info->env;
    }
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto cb = info->callback;
    if (engine == nullptr) {
        HILOG_ERROR("native engine is null");
        return **JSValueRef::Undefined(vm);
    }

    uint64_t nestChainId = StartNapiProfilerTrace(runtimeInfo, engine, reinterpret_cast<void *>(cb));

    if (JSNApi::IsMixedDebugEnabled(vm)) {
        JSNApi::NotifyNativeCalling(vm, reinterpret_cast<void *>(cb));
    }

    napi_value result = nullptr;
    if (cb != nullptr) {
        if constexpr (changeState) {
            panda::JsiNativeScope nativeScope(vm);
#if ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE
#ifdef ENABLE_HITRACE
                        StartTrace(HITRACE_TAG_ACE, "Developer::NativeCallBack::One");
#endif
#endif
            result = cb(env, runtimeInfo);
#if ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE
#ifdef ENABLE_HITRACE
                        FinishTrace(HITRACE_TAG_ACE);
#endif
#endif
        } else {
#if ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE
#ifdef ENABLE_HITRACE
                        StartTrace(HITRACE_TAG_ACE, "Developer::NativeCallBack::Two");
#endif
#endif
            result = cb(env, runtimeInfo);
#if ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE
#ifdef ENABLE_HITRACE
                        FinishTrace(HITRACE_TAG_ACE);
#endif
#endif
        }
    }

    if (JSNApi::IsMixedDebugEnabled(vm)) {
        JSNApi::NotifyNativeReturn(vm, reinterpret_cast<void *>(cb));
    }

    Local<panda::JSValueRef> localRet = panda::JSValueRef::Undefined(vm);
    if (result != nullptr) {
        localRet = LocalValueFromJsValue(result);
    }

    FinishNapiProfilerTrace(nestChainId);
    // Fixme: Rethrow error to engine while clear lastException_
    if (!engine->lastException_.IsEmpty()) {
        engine->lastException_.Empty();
    }

    if (localRet.IsEmpty()) {
        return **JSValueRef::Undefined(vm);
    }
    if (getStackBeforeCallNapiSuccess) {
        JSNApi::GetStackAfterCallNapi(vm);
    }
    return **localRet;
}

static Local<JSValueRef> GetProperty(EcmaVM* vm, Local<panda::ObjectRef> &obj, const char* name)
{
    Local<StringRef> key = StringRef::NewFromUtf8(vm, name);
    Local<JSValueRef> val = obj->Get(vm, key);
    return val;
}

void GetCString(EcmaVM* vm, Local<StringRef> str, char* buffer, size_t size, size_t* length)
{
    if (length == nullptr) {
        return;
    }
    if (buffer == nullptr) {
        *length = str->Utf8Length(vm, true) - 1;
    } else if (size != 0) {
        uint32_t copied = str->WriteUtf8(vm, buffer, size - 1, true) - 1;
        buffer[copied] = '\0';
        *length = copied;
    } else {
        *length = 0;
    }
}

std::string NapiGetModuleName(napi_env env, Local<panda::ObjectRef> &obj)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto vm = const_cast<EcmaVM*>(engine->GetEcmaVm());
    std::string moduleName("");
    auto nativeModuleName = GetProperty(vm, obj, PANDA_MODULE_NAME);
    if (nativeModuleName->IsString(vm)) {
        char arrayName[PANDA_MODULE_NAME_LEN] = {0};
        size_t len = 0;
        GetCString(vm, nativeModuleName, arrayName, PANDA_MODULE_NAME_LEN, &len);
        moduleName += arrayName;
        moduleName += ".";
    }
    return moduleName;
}

void NapiDefinePropertyInner(napi_env env,
                             Local<panda::ObjectRef> &obj,
                             NapiPropertyDescriptor &propertyDescriptor,
                             Local<panda::JSValueRef> &propertyName,
                             bool &result)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto vm = engine->GetEcmaVm();
    bool writable = (propertyDescriptor.attributes & NATIVE_WRITABLE) != 0;
    bool enumable = (propertyDescriptor.attributes & NATIVE_ENUMERABLE) != 0;
    bool configable = (propertyDescriptor.attributes & NATIVE_CONFIGURABLE) != 0;
    std::string fullName("");
    if (propertyDescriptor.getter != nullptr || propertyDescriptor.setter != nullptr) {
#ifdef ENABLE_HITRACE
        fullName += NapiGetModuleName(env, obj);
#endif
        Local<panda::JSValueRef> localGetter = panda::JSValueRef::Undefined(vm);
        Local<panda::JSValueRef> localSetter = panda::JSValueRef::Undefined(vm);

        if (propertyDescriptor.getter != nullptr) {
            fullName += "getter";
            localGetter = NapiNativeCreateFunction(env, fullName.c_str(),
                                                   propertyDescriptor.getter, propertyDescriptor.data);
        }
        if (propertyDescriptor.setter != nullptr) {
            fullName += "setter";
            localSetter = NapiNativeCreateFunction(env, fullName.c_str(),
                                                   propertyDescriptor.setter, propertyDescriptor.data);
        }

        PropertyAttribute attr(panda::JSValueRef::Undefined(vm), false, enumable, configable);
        result = obj->SetAccessorProperty(vm, propertyName, localGetter, localSetter, attr);
    } else if (propertyDescriptor.method != nullptr) {
#ifdef ENABLE_HITRACE
        fullName += NapiGetModuleName(env, obj);
#endif
        if (propertyDescriptor.utf8name != nullptr) {
            fullName += propertyDescriptor.utf8name;
        } else {
            fullName += propertyName->IsString(vm)
                        ? Local<panda::StringRef>(propertyName)->ToString(vm)
                        : Local<panda::SymbolRef>(propertyName)->GetDescription(vm)->ToString(vm);
        }
        Local<panda::JSValueRef> cbObj = NapiNativeCreateFunction(env, fullName.c_str(),
                                                                  propertyDescriptor.method, propertyDescriptor.data);
        PropertyAttribute attr(cbObj, writable, enumable, configable);
        result = obj->DefineProperty(vm, propertyName, attr);
    } else {
        Local<panda::JSValueRef> val = LocalValueFromJsValue(propertyDescriptor.value);

        PropertyAttribute attr(val, writable, enumable, configable);
        result = obj->DefineProperty(vm, propertyName, attr);
    }
}

bool NapiDefineProperty(napi_env env, Local<panda::ObjectRef> &obj, NapiPropertyDescriptor propertyDescriptor)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto vm = engine->GetEcmaVm();
    bool result = false;
    Local<panda::JSValueRef> propertyName;
    if (propertyDescriptor.utf8name != nullptr) {
        propertyName = panda::StringRef::NewFromUtf8(vm, propertyDescriptor.utf8name);
    } else {
        propertyName = LocalValueFromJsValue(propertyDescriptor.name);
    }
    if (obj->IsJSShared(vm)) {
        NativeSendable::NapiDefineSendabledProperty(env, obj, propertyDescriptor, propertyName, result);
    } else {
        NapiDefinePropertyInner(env, obj, propertyDescriptor, propertyName, result);
    }
    Local<panda::ObjectRef> excep = JSNApi::GetUncaughtException(vm);
    if (!excep.IsNull()) {
        HILOG_DEBUG("ArkNativeObject::DefineProperty occur Exception");
        JSNApi::GetAndClearUncaughtException(vm);
    }
    return result;
}

panda::Local<panda::ObjectRef> NapiCreateObjectWithProperties(napi_env env, size_t propertyCount,
                                                              const napi_property_descriptor *properties,
                                                              Local<panda::JSValueRef> *keys,
                                                              PropertyAttribute *attrs)
{
    auto vm = reinterpret_cast<NativeEngine*>(env)->GetEcmaVm();
    panda::EscapeLocalScope scope(vm);
    for (size_t i = 0; i < propertyCount; ++i) {
        const napi_property_descriptor &property = properties[i];

        const char* utf8name = property.utf8name;
        uint32_t attributes = property.attributes;
        bool writable = (attributes & NATIVE_WRITABLE) != 0;
        bool enumable = (attributes & NATIVE_ENUMERABLE) != 0;
        bool configable = (attributes & NATIVE_CONFIGURABLE) != 0;
        NapiNativeCallback method = reinterpret_cast<NapiNativeCallback>(property.method);
        NapiNativeCallback getter = reinterpret_cast<NapiNativeCallback>(property.getter);
        NapiNativeCallback setter = reinterpret_cast<NapiNativeCallback>(property.setter);
        napi_value value = property.value;
        void *data = property.data;

        Local<panda::JSValueRef> val = panda::JSValueRef::Undefined(vm);

        std::string fullName("");
        if (getter != nullptr || setter != nullptr) {
            Local<panda::JSValueRef> localGetter = panda::JSValueRef::Undefined(vm);
            Local<panda::JSValueRef> localSetter = panda::JSValueRef::Undefined(vm);

            if (getter != nullptr) {
                fullName += "getter";
                localGetter = NapiNativeCreateFunction(env, fullName.c_str(), getter, data);
            }
            if (setter != nullptr) {
                fullName += "setter";
                localSetter = NapiNativeCreateFunction(env, fullName.c_str(), setter, data);
            }

            val = panda::ObjectRef::CreateAccessorData(vm, localGetter, localSetter);
            writable = false;
        } else if (method != nullptr) {
            fullName += utf8name;
            val = NapiNativeCreateFunction(env, fullName.c_str(), method, data);
        } else {
            val = LocalValueFromJsValue(value);
        }
        new (reinterpret_cast<void *>(&attrs[i])) PropertyAttribute(val, writable, enumable, configable);
        keys[i] = panda::StringRef::NewFromUtf8(vm, utf8name);
    }
    Local<panda::ObjectRef> object = panda::ObjectRef::NewWithProperties(vm, propertyCount, keys, attrs);
    return scope.Escape(object);
}

panda::Local<panda::ObjectRef> NapiCreateSObjectWithProperties(napi_env env,
                                                               size_t propertyCount,
                                                               const NapiPropertyDescriptor* properties)
{
    auto vm = reinterpret_cast<NativeEngine*>(env)->GetEcmaVm();
    panda::EscapeLocalScope scope(vm);
    FunctionRef::SendablePropertiesInfo info;
    for (size_t i = 0; i < propertyCount; ++i) {
        NativeSendable::InitSendablePropertiesInfo(env, info, properties[i]);
    }
    Local<panda::ObjectRef> object = panda::ObjectRef::NewSWithProperties(vm, info);
    return scope.Escape(object);
}

void CommonDeleter(void *env, void *externalPointer, void *data)
{
    auto info = reinterpret_cast<NapiFunctionInfo *>(data);
    if (info != nullptr) {
        delete info;
    }
}

panda::Local<panda::ObjectRef> ArkNativeEngine::GetModuleFromName(
    const std::string& moduleName, bool isAppModule, const std::string& id, const std::string& param,
    const std::string& instanceName, void** instance)
{
    panda::EscapeLocalScope scope(vm_);
    Local<ObjectRef> exports(JSValueRef::Undefined(vm_));
    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    std::string errInfo = "";
    NativeModule* module = moduleManager->LoadNativeModule(moduleName.c_str(), nullptr, isAppModule, errInfo);
    if (module != nullptr) {
        Local<StringRef> idStr = StringRef::NewFromUtf8(vm_, id.c_str(), id.size());
        napi_value idValue = JsValueFromLocalValue(idStr);
        Local<StringRef> paramStr = StringRef::NewFromUtf8(vm_, param.c_str(), param.size());
        napi_value paramValue = JsValueFromLocalValue(paramStr);
        Local<ObjectRef> exportObj = ObjectRef::New(vm_);
        NapiPropertyDescriptor idProperty;
        NapiPropertyDescriptor paramProperty;
        idProperty.utf8name = "id";
        idProperty.value = idValue;
        paramProperty.utf8name = "param";
        paramProperty.value = paramValue;
        SetModuleName(exportObj, module->name);
        NapiDefineProperty(reinterpret_cast<napi_env>(this), exportObj, idProperty);
        NapiDefineProperty(reinterpret_cast<napi_env>(this), exportObj, paramProperty);
        MoudleNameLocker nameLocker(module->name);
        module->registerCallback(reinterpret_cast<napi_env>(this), JsValueFromLocalValue(exportObj));
        napi_value nExport = JsValueFromLocalValue(exportObj);
        napi_value exportInstance = nullptr;
        napi_status status = napi_get_named_property(
            reinterpret_cast<napi_env>(this), nExport, instanceName.c_str(), &exportInstance);
        if (status != napi_ok) {
            HILOG_ERROR("GetModuleFromName napi_get_named_property status != napi_ok");
        }

        status = napi_unwrap(reinterpret_cast<napi_env>(this), exportInstance, reinterpret_cast<void**>(instance));
        if (status != napi_ok) {
            HILOG_ERROR("GetModuleFromName napi_unwrap status != napi_ok");
        }
        exports = exportObj;
    }
    return scope.Escape(exports);
}

napi_value ArkNativeEngine::CreatePromise(NativeDeferred** deferred)
{
    panda::EscapeLocalScope scope(vm_);
    Local<PromiseCapabilityRef> capability = PromiseCapabilityRef::New(vm_);
    if (capability->IsUndefined()) {
        return JsValueFromLocalValue(scope.Escape(JSValueRef::Undefined(vm_)));
    }
    *deferred = new ArkNativeDeferred(this, capability);
    Local<JSValueRef> promiseValue = capability->GetPromise(vm_);
    return JsValueFromLocalValue(scope.Escape(promiseValue));
}

panda::Local<panda::ObjectRef> ArkNativeEngine::LoadModuleByName(const std::string& moduleName, bool isAppModule,
    const std::string& param, const std::string& instanceName, void* instance, const std::string& path)
{
    panda::EscapeLocalScope scope(vm_);
    Local<ObjectRef> exports(JSValueRef::Undefined(vm_));
    NativeModuleManager* moduleManager = NativeModuleManager::GetInstance();
    std::string errInfo = "";
    NativeModule* module = moduleManager->LoadNativeModule(moduleName.c_str(),
        path.empty() ? nullptr : path.c_str(), isAppModule, errInfo);
    if (module != nullptr) {
        Local<ObjectRef> exportObj = ObjectRef::New(vm_);
        NapiPropertyDescriptor instanceProperty;
        NapiPropertyDescriptor paramProperty;
        Local<StringRef> paramStr = StringRef::NewFromUtf8(vm_, param.c_str(), param.size());
        napi_value paramValue = JsValueFromLocalValue(paramStr);
        paramProperty.utf8name = "param";
        paramProperty.value = paramValue;
        Local<ObjectRef> instanceValue = ObjectRef::New(vm_);
        Local<StringRef> key = StringRef::GetNapiWrapperString(vm_);
        if (instance == nullptr && instanceValue->Has(vm_, key)) {
            Local<ObjectRef> wrapper = instanceValue->Get(vm_, key);
            auto ref = reinterpret_cast<NativeReference*>(wrapper->GetNativePointerField(vm_, 0));
            wrapper->SetNativePointerField(vm_, 0, nullptr, nullptr, nullptr, 0);
            instanceValue->Delete(vm_, key);
            delete ref;
        } else {
            Local<ObjectRef> object = ObjectRef::New(vm_);
            NativeReference* ref = nullptr;
            Local<JSValueRef> value(instanceValue);
            ref = new ArkNativeReference(this, value, 0, true, nullptr, instance, nullptr);

            object->SetNativePointerFieldCount(vm_, 1);
            object->SetNativePointerField(vm_, 0, ref, nullptr, nullptr, 0);
            PropertyAttribute attr(object, true, false, true);
            instanceValue->DefineProperty(vm_, key, attr);
        }
        instanceProperty.utf8name = instanceName.c_str();
        instanceProperty.value = JsValueFromLocalValue(instanceValue);
        SetModuleName(exportObj, module->name);
        NapiDefineProperty(reinterpret_cast<napi_env>(this), exportObj, paramProperty);
        NapiDefineProperty(reinterpret_cast<napi_env>(this), exportObj, instanceProperty);

        MoudleNameLocker nameLocker(module->name);
        module->registerCallback(reinterpret_cast<napi_env>(this), JsValueFromLocalValue(exportObj));
        exports = exportObj;
    }
    return scope.Escape(exports);
}

void ArkNativeEngine::Loop(LoopMode mode, bool needSync)
{
    LocalScope scope(vm_);
    NativeEngine::Loop(mode, needSync);
    JSNApi::ExecutePendingJob(vm_);
}

void ArkNativeEngine::SetModuleName(Local<ObjectRef> &nativeObj, std::string moduleName)
{
#ifdef ENABLE_HITRACE
    if (ArkNativeEngine::napiProfilerEnabled) {
        Local<StringRef> moduleNameStr = StringRef::NewFromUtf8(vm_, moduleName.c_str(), moduleName.size());
        Local<StringRef> key = StringRef::NewFromUtf8(vm_, PANDA_MODULE_NAME);
        nativeObj->Set(vm_, key, moduleNameStr);
    }
#endif
}

static void ConcurrentCallbackFunc(Local<JSValueRef> result, bool success, void *taskInfo, void *data)
{
    if (data == nullptr) {
        return;
    }
    auto engine = static_cast<ArkNativeEngine *>(data);
    auto concurrentCallbackFunc = engine->GetConcurrentCallbackFunc();
    if (concurrentCallbackFunc == nullptr) {
        return;
    }
    napi_env env = reinterpret_cast<napi_env>(engine);
    concurrentCallbackFunc(env, ArkNativeEngine::ArkValueToNapiValue(env, result), success, taskInfo);
}

bool ArkNativeEngine::InitTaskPoolThread(NativeEngine* engine, NapiConcurrentCallback callback)
{
    concurrentCallbackFunc_ = callback;
    return JSNApi::InitForConcurrentThread(vm_, ConcurrentCallbackFunc, static_cast<void *>(this));
}

bool ArkNativeEngine::InitTaskPoolThread(napi_env env, NapiConcurrentCallback callback)
{
    concurrentCallbackFunc_ = callback;
    return JSNApi::InitForConcurrentThread(vm_, ConcurrentCallbackFunc, static_cast<void *>(this));
}

bool ArkNativeEngine::InitTaskPoolFunc(napi_env env, napi_value func, void* taskInfo)
{
    LocalScope scope(vm_);
    Local<JSValueRef> function = LocalValueFromJsValue(func);
    return JSNApi::InitForConcurrentFunction(vm_, function, taskInfo);
}

bool ArkNativeEngine::HasPendingJob() const
{
    return JSNApi::HasPendingJob(vm_);
}

bool ArkNativeEngine::IsProfiling() const
{
    return JSNApi::IsProfiling(vm_);
}

bool ArkNativeEngine::IsExecutingPendingJob() const
{
    return JSNApi::IsExecutingPendingJob(vm_);
}

void* ArkNativeEngine::GetCurrentTaskInfo() const
{
    return JSNApi::GetCurrentTaskInfo(vm_);
}

void ArkNativeEngine::ClearCurrentTaskInfo()
{
    JSNApi::ClearCurrentTaskInfo(vm_);
}

void ArkNativeEngine::TerminateExecution() const
{
    DFXJSNApi::TerminateExecution(vm_);
}

void ArkNativeEngine::NotifyTaskBegin() const
{
    JSNApi::NotifyTaskBegin(vm_);
}

void ArkNativeEngine::NotifyTaskFinished() const
{
    JSNApi::NotifyTaskFinished(vm_);
}

napi_value ArkNativeEngine::CallFunction(
    napi_value thisVar, napi_value function, napi_value const* argv, size_t argc)
{
    if (function == nullptr) {
        return nullptr;
    }
    panda::EscapeLocalScope scope(vm_);
    Local<JSValueRef> thisObj = JSValueRef::Undefined(vm_);
    if (thisVar != nullptr) {
        thisObj = LocalValueFromJsValue(thisVar);
    }
    Local<FunctionRef> funcObj = LocalValueFromJsValue(function);
    std::vector<Local<JSValueRef>> args;
    args.reserve(argc);
    for (size_t i = 0; i < argc; i++) {
        if (argv[i] != nullptr) {
            args.emplace_back(LocalValueFromJsValue(argv[i]));
        } else {
            args.emplace_back(JSValueRef::Undefined(vm_));
        }
    }

    Local<JSValueRef> value = funcObj->Call(vm_, thisObj, args.data(), argc);
    if (JSNApi::HasPendingException(vm_)) {
        HILOG_ERROR("pending exception when js function called");
        HILOG_ERROR("print exception info: ");
        JSNApi::PrintExceptionInfo(vm_);
        return nullptr;
    }

    return JsValueFromLocalValue(scope.Escape(value));
}

bool ArkNativeEngine::NapiNewTypedArray(NativeTypedArrayType typedArrayType,
                                        Local<panda::ArrayBufferRef> arrayBuf,
                                        size_t byte_offset, size_t length, napi_value* result)
{
    Local<panda::TypedArrayRef> typedArray;
    switch (typedArrayType) {
        case NATIVE_INT8_ARRAY:
            typedArray = panda::Int8ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT8_ARRAY:
            typedArray = panda::Uint8ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT8_CLAMPED_ARRAY:
            typedArray = panda::Uint8ClampedArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_INT16_ARRAY:
            typedArray = panda::Int16ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT16_ARRAY:
            typedArray = panda::Uint16ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_INT32_ARRAY:
            typedArray = panda::Int32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT32_ARRAY:
            typedArray = panda::Uint32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_FLOAT32_ARRAY:
            typedArray = panda::Float32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_FLOAT64_ARRAY:
            typedArray = panda::Float64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_BIGINT64_ARRAY:
            typedArray = panda::BigInt64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_BIGUINT64_ARRAY:
            typedArray = panda::BigUint64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        default:
            *result = nullptr;
            return false;
    }
    *result = JsValueFromLocalValue(typedArray);
    return true;
}

bool ArkNativeEngine::NapiNewSendableTypedArray(NativeTypedArrayType typedArrayType,
                                                Local<panda::SendableArrayBufferRef> arrayBuf,
                                                size_t byte_offset, size_t length, napi_value* result)
{
    Local<panda::SendableTypedArrayRef> typedArray;
    switch (typedArrayType) {
        case NATIVE_INT8_ARRAY:
            typedArray = panda::SharedInt8ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT8_ARRAY:
            typedArray = panda::SharedUint8ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_INT16_ARRAY:
            typedArray = panda::SharedInt16ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT16_ARRAY:
            typedArray = panda::SharedUint16ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_INT32_ARRAY:
            typedArray = panda::SharedInt32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT32_ARRAY:
            typedArray = panda::SharedUint32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_FLOAT32_ARRAY:
            typedArray = panda::SharedFloat32ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_UINT8_CLAMPED_ARRAY:
            typedArray = panda::SharedUint8ClampedArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_FLOAT64_ARRAY:
            typedArray = panda::SharedFloat64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_BIGINT64_ARRAY:
            typedArray = panda::SharedBigInt64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        case NATIVE_BIGUINT64_ARRAY:
            typedArray = panda::SharedBigUint64ArrayRef::New(vm_, arrayBuf, byte_offset, length);
            break;
        default:
            *result = nullptr;
            return false;
    }
    *result = JsValueFromLocalValue(typedArray);
    return true;
}

NativeTypedArrayType ArkNativeEngine::GetTypedArrayType(panda::Local<panda::TypedArrayRef> typedArray)
{
    NativeTypedArrayType thisType = NATIVE_INT8_ARRAY;
    if (typedArray->IsInt32Array(vm_)) {
        thisType = NATIVE_INT32_ARRAY;
    } else if (typedArray->IsInt8Array(vm_)) {
        thisType = NATIVE_INT8_ARRAY;
    } else if (typedArray->IsUint8Array(vm_)) {
        thisType = NATIVE_UINT8_ARRAY;
    } else if (typedArray->IsUint8ClampedArray(vm_)) {
        thisType = NATIVE_UINT8_CLAMPED_ARRAY;
    } else if (typedArray->IsInt16Array(vm_)) {
        thisType = NATIVE_INT16_ARRAY;
    } else if (typedArray->IsUint16Array(vm_)) {
        thisType = NATIVE_UINT16_ARRAY;
    } else if (typedArray->IsUint32Array(vm_)) {
        thisType = NATIVE_UINT32_ARRAY;
    } else if (typedArray->IsFloat32Array(vm_)) {
        thisType = NATIVE_FLOAT32_ARRAY;
    } else if (typedArray->IsFloat64Array(vm_)) {
        thisType = NATIVE_FLOAT64_ARRAY;
    } else if (typedArray->IsBigInt64Array(vm_)) {
        thisType = NATIVE_BIGINT64_ARRAY;
    } else if (typedArray->IsBigUint64Array(vm_)) {
        thisType = NATIVE_BIGUINT64_ARRAY;
    }

    return thisType;
}

NativeTypedArrayType ArkNativeEngine::GetSendableTypedArrayType(panda::Local<panda::SendableTypedArrayRef> typedArray)
{
    NativeTypedArrayType thisType = NATIVE_INT8_ARRAY;
    if (typedArray->IsJSSharedInt8Array(vm_)) {
        thisType = NATIVE_INT8_ARRAY;
    } else if (typedArray->IsJSSharedUint8Array(vm_)) {
        thisType = NATIVE_UINT8_ARRAY;
    } else if (typedArray->IsJSSharedInt16Array(vm_)) {
        thisType = NATIVE_INT16_ARRAY;
    } else if (typedArray->IsJSSharedUint16Array(vm_)) {
        thisType = NATIVE_UINT16_ARRAY;
    } else if (typedArray->IsJSSharedInt32Array(vm_)) {
        thisType = NATIVE_INT32_ARRAY;
    } else if (typedArray->IsJSSharedUint32Array(vm_)) {
        thisType = NATIVE_UINT32_ARRAY;
    } else if (typedArray->IsJSSharedFloat32Array(vm_)) {
        thisType = NATIVE_FLOAT32_ARRAY;
    } else if (typedArray->IsJSSharedUint8ClampedArray(vm_)) {
        thisType = NATIVE_UINT8_CLAMPED_ARRAY;
    } else if (typedArray->IsJSSharedFloat64Array(vm_)) {
        thisType = NATIVE_FLOAT64_ARRAY;
    } else if (typedArray->IsJSSharedBigInt64Array(vm_)) {
        thisType = NATIVE_BIGINT64_ARRAY;
    } else if (typedArray->IsJSSharedBigUint64Array(vm_)) {
        thisType = NATIVE_BIGUINT64_ARRAY;
    }

    return thisType;
}

/*
 * Before: input: 1. @ohos.hilog
                  2. @system.app (NATIVE_MODULE contains this name)
 * After: return: 1.@ohos:hilog
 *                2.@native:system.app
 */
std::string ArkNativeEngine::GetOhmurl(std::string path)
{
    const std::regex reg("@(ohos|system)\\.(\\S+)");
    path.erase(0, path.find_first_not_of(" "));
    path.erase(path.find_last_not_of(" ") + 1);
    bool ret = std::regex_match(path, reg);
    if (!ret) {
        HILOG_ERROR("ArkNativeEngine:The module name doesn't comply with the naming rules");
        return "";
    }
    std::string systemModule = path.substr(1);
    if (NATIVE_MODULE.count(systemModule)) {
        return NATIVE_MODULE_PREFIX + systemModule;
    } else {
        int pos = static_cast<int>(path.find('.'));
        std::string systemKey = path.substr(pos + 1, systemModule.size());
        return OHOS_MODULE_PREFIX + systemKey;
    }
}

Local<JSValueRef> ArkNativeEngine::NapiLoadNativeModule(std::string path)
{
    std::string key = GetOhmurl(path);
    if (key.size() == 0) {
        return JSValueRef::Undefined(vm_);
    }
    return JSNApi::ExecuteNativeModule(vm_, key);
}

napi_value ArkNativeEngine::NapiLoadModule(const char* path)
{
    if (path == nullptr) {
        HILOG_ERROR("ArkNativeEngine:The module name is empty");
        return nullptr;
    }
    HILOG_INFO("ArkNativeEngine::NapiLoadModule path:%{public}s", path);
    panda::EscapeLocalScope scope(vm_);
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    Local exportObj(undefObj);
    std::string inputPath(path);
    std::string ohmurl = GetOhmurl(inputPath);
    if (!ohmurl.empty()) {
        exportObj = JSNApi::ExecuteNativeModule(vm_, ohmurl);
    } else {
        exportObj = JSNApi::GetModuleNameSpaceFromFile(vm_, inputPath);
    }
    if (!exportObj->IsObject(vm_)) {
        ThrowException("ArkNativeEngine:NapiLoadModule failed.");
        return JsValueFromLocalValue(scope.Escape(undefObj));
    }
    return JsValueFromLocalValue(scope.Escape(exportObj));
}

napi_value ArkNativeEngine::NapiLoadModuleWithInfo(const char* path, const char* module_info)
{
    if (path == nullptr) {
        HILOG_ERROR("ArkNativeEngine:The module name is empty");
        return nullptr;
    }
    panda::EscapeLocalScope scope(vm_);
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    Local<ObjectRef> exportObj(undefObj);
    std::string inputPath(path);
    std::string modulePath;
    if (module_info != nullptr) {
        modulePath = module_info;
        exportObj = panda::JSNApi::GetModuleNameSpaceWithModuleInfoForNormalApp(vm_, inputPath, modulePath);
    } else {
        exportObj = NapiLoadNativeModule(inputPath);
    }

    if (panda::JSNApi::HasPendingException(vm_)) {
        HILOG_WARN("ArkNativeEngine:NapiLoadModuleWithInfo failed.");
        panda::JSNApi::PrintExceptionInfo(vm_);
        panda::JSNApi::GetAndClearUncaughtException(vm_); // clear exception here
        return JsValueFromLocalValue(scope.Escape(undefObj));
    }
    return JsValueFromLocalValue(scope.Escape(exportObj));
}

napi_value ArkNativeEngine::NapiLoadModuleWithInfoForHybridApp(const char* path, const char* module_info)
{
    if (path == nullptr) {
        HILOG_ERROR("ArkNativeEngine:The module name is empty");
        return nullptr;
    }
    panda::EscapeLocalScope scope(vm_);
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    Local<ObjectRef> exportObj(undefObj);
    std::string inputPath(path);
    std::string modulePath;
    if (module_info != nullptr) {
        modulePath = module_info;
        exportObj = panda::JSNApi::GetModuleNameSpaceWithModuleInfoForHybridApp(vm_, inputPath, modulePath);
    } else {
        exportObj = NapiLoadNativeModule(inputPath);
    }

    if (JSNApi::HasPendingException(vm_)) {
        HILOG_WARN("ArkNativeEngine:NapiLoadModuleWithInfo failed.");
        JSNApi::PrintExceptionInfo(vm_);
        JSNApi::GetAndClearUncaughtException(vm_); // clear exception here
        return JsValueFromLocalValue(scope.Escape(undefObj));
    }
    return JsValueFromLocalValue(scope.Escape(exportObj));
}

bool ArkNativeEngine::SuspendVMById(uint32_t tid)
{
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    return DFXJSNApi::SuspendVMById(vm_, tid);
#else
    HILOG_WARN("ARK does not support dfx on windows");
    return false;
#endif
}

void ArkNativeEngine::ResumeVMById(uint32_t tid)
{
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    DFXJSNApi::ResumeVMById(vm_, tid);
#else
    HILOG_WARN("ARK does not support dfx on windows");
    return;
#endif
}

void ArkNativeEngine::SetPackagePath(const std::string appLibPathKey, const std::vector<std::string>& packagePath)
{
    auto moduleManager = NativeModuleManager::GetInstance();
    if (moduleManager && !packagePath.empty()) {
        moduleManager->SetAppLibPath(appLibPathKey, packagePath);
    }
}

napi_value ArkNativeEngine::CreateInstance(napi_value constructor, napi_value const *argv, size_t argc)
{
    if (constructor == nullptr) {
        return nullptr;
    }
    panda::EscapeLocalScope scope(vm_);
    Local<FunctionRef> value = LocalValueFromJsValue(constructor);
    std::vector<Local<JSValueRef>> args;
    args.reserve(argc);
    for (size_t i = 0; i < argc; i++) {
        if (argv[i] != nullptr) {
            args.emplace_back(LocalValueFromJsValue(argv[i]));
        } else {
            args.emplace_back(JSValueRef::Undefined(vm_));
        }
    }
    Local<JSValueRef> instance = value->Constructor(vm_, args.data(), argc);
    Local<ObjectRef> excep = JSNApi::GetUncaughtException(vm_);
    if (!excep.IsNull()) {
        HILOG_ERROR("ArkNativeEngineImpl::CreateInstance occur Exception");
        return nullptr;
    }
    return JsValueFromLocalValue(scope.Escape(instance));
}

NativeReference* ArkNativeEngine::CreateReference(napi_value value, uint32_t initialRefcount,
    bool flag, NapiNativeFinalize callback, void* data, void* hint, size_t nativeBindingSize)
{
    return new ArkNativeReference(this, value, initialRefcount, flag, callback, data, hint, false, nativeBindingSize);
}

NativeReference* ArkNativeEngine::CreateXRefReference(napi_value value, uint32_t initialRefcount,
    bool flag, NapiNativeFinalize callback, void* data)
{
    ArkNativeReferenceConfig config(initialRefcount, true, flag, callback, data);
    return new ArkNativeReference(this, value, config);
}

NativeReference* ArkNativeEngine::CreateAsyncReference(napi_value value, uint32_t initialRefcount,
    bool flag, NapiNativeFinalize callback, void* data, void* hint)
{
    return new ArkNativeReference(this, value, initialRefcount, flag, callback, data, hint, true);
}

__attribute__((optnone)) void ArkNativeEngine::RunCallbacks(TriggerGCData *triggerGCData)
{
#ifdef ENABLE_HITRACE
    StartTrace(HITRACE_TAG_ACE, "RunTriggerGCTaskCallback");
#endif
    std::pair<void *, uint8_t> &param = *triggerGCData;
    JSNApi::TriggerIdleGC(reinterpret_cast<EcmaVM *>(param.first),
        static_cast<JSNApi::TRIGGER_IDLE_GC_TYPE>(param.second));
#ifdef ENABLE_HITRACE
    FinishTrace(HITRACE_TAG_ACE);
#endif
}

__attribute__((optnone)) void ArkNativeEngine::RunCallbacks(ArkFinalizersPack *finalizersPack)
{
#ifdef ENABLE_HITRACE
    StartTrace(HITRACE_TAG_ACE, "RunFinalizeCallbacks:" + std::to_string(finalizersPack->GetNumFinalizers()));
#endif
    finalizersPack->ProcessAll();
#ifdef ENABLE_HITRACE
    FinishTrace(HITRACE_TAG_ACE);
#endif
}

__attribute__((optnone)) void ArkNativeEngine::RunAsyncCallbacks(std::vector<RefAsyncFinalizer> *finalizers)
{
#ifdef ENABLE_HITRACE
    StartTrace(HITRACE_TAG_ACE, "RunFinalizeCallbacks:" + std::to_string(finalizers->size()));
#endif
    INIT_CRASH_HOLDER(holder, "NAPI");
    for (auto iter : (*finalizers)) {
        NapiNativeFinalize callback = iter.first;
        std::pair<void*, void*> &param = iter.second;
        holder.UpdateCallbackPtr(reinterpret_cast<uintptr_t>(callback));
        callback(nullptr, std::get<0>(param), std::get<1>(param)); // 1 is the param.
    }
#ifdef ENABLE_HITRACE
    FinishTrace(HITRACE_TAG_ACE);
#endif
}

void ArkNativeEngine::PostFinalizeTasks()
{
    if (IsInDestructor()) {
        return;
    }
    if (!pendingAsyncFinalizers_.empty()) {
        uv_work_t *asynWork = new uv_work_t;
        std::vector<RefAsyncFinalizer> *asyncFinalizers = new std::vector<RefAsyncFinalizer>();
        asyncFinalizers->swap(pendingAsyncFinalizers_);
        asynWork->data = reinterpret_cast<void *>(asyncFinalizers);

        int ret = uv_queue_work_with_qos(GetUVLoop(), asynWork, [](uv_work_t *asynWork) {
            std::vector<RefAsyncFinalizer> *finalizers =
                reinterpret_cast<std::vector<RefAsyncFinalizer> *>(asynWork->data);
            RunAsyncCallbacks(finalizers);
            HILOG_DEBUG("uv_queue_work async running ");
            delete finalizers;
        }, [](uv_work_t *asynWork, int32_t) {
            delete asynWork;
        }, uv_qos_t(napi_qos_background));
        if (ret != 0) {
            HILOG_ERROR("uv_queue_work fail ret '%{public}d'", ret);
            RunAsyncCallbacks(asyncFinalizers);
            delete asynWork;
            delete asyncFinalizers;
        }
    }
    if (arkFinalizersPack_.Empty()) {
        return;
    }
    ArkFinalizersPack *finalizersPack = new ArkFinalizersPack();
    std::swap(arkFinalizersPack_, *finalizersPack);
    if (!IsMainThread()) {
        panda::JsiNativeScope nativeScope(vm_);
        RunCallbacks(finalizersPack);
        delete finalizersPack;
        return;
    }
    size_t bindingSize = finalizersPack->GetTotalNativeBindingSize();
    if (pendingFinalizersPackNativeBindingSize_ > FINALIZERS_PACK_PENDING_NATIVE_BINDING_SIZE_THRESHOLD &&
        bindingSize > 0) {
        HILOG_DEBUG("Pending Finalizers NativeBindingSize '%{public}zu' large than '%{public}zu', process sync.",
            pendingFinalizersPackNativeBindingSize_, FINALIZERS_PACK_PENDING_NATIVE_BINDING_SIZE_THRESHOLD);
        panda::JsiNativeScope nativeScope(vm_);
        RunCallbacks(finalizersPack);
        delete finalizersPack;
        return;
    }
    uv_work_t *syncWork = new uv_work_t;
    finalizersPack->RegisterFinishNotify([this] (size_t totalNativeBindingSize) {
        this->DecreasePendingFinalizersPackNativeBindingSize(totalNativeBindingSize);
    });
    IncreasePendingFinalizersPackNativeBindingSize(bindingSize);

    syncWork->data = reinterpret_cast<void *>(finalizersPack);
    int ret = uv_queue_work_with_qos(GetUVLoop(), syncWork, [](uv_work_t *) {}, [](uv_work_t *syncWork, int32_t) {
        ArkFinalizersPack *finalizersPack = reinterpret_cast<ArkFinalizersPack*>(syncWork->data);
        RunCallbacks(finalizersPack);
        delete syncWork;
        delete finalizersPack;
    }, uv_qos_t(napi_qos_background));
    if (ret != 0) {
        HILOG_ERROR("uv_queue_work fail ret '%{public}d'", ret);
        panda::JsiNativeScope nativeScope(vm_);
        RunCallbacks(finalizersPack);
        delete syncWork;
        delete finalizersPack;
    }
}

__attribute__((optnone)) void ArkNativeEngine::RunCallbacks(AsyncNativeCallbacksPack *callbacksPack)
{
#ifdef ENABLE_HITRACE
    StartTrace(HITRACE_TAG_ACE, "RunNativeCallbacks:" + std::to_string(callbacksPack->GetNumCallBacks()));
#endif
    callbacksPack->ProcessAll("NAPI");
#ifdef ENABLE_HITRACE
    FinishTrace(HITRACE_TAG_ACE);
#endif
}

void ArkNativeEngine::PostAsyncTask(AsyncNativeCallbacksPack *callBacksPack)
{
    if (IsInDestructor()) {
        delete callBacksPack;
        return;
    }
    uv_work_t *syncWork = new uv_work_t;
    syncWork->data = reinterpret_cast<void *>(callBacksPack);

    int ret = uv_queue_work_with_qos(GetUVLoop(), syncWork, [](uv_work_t *) {}, [](uv_work_t *syncWork, int32_t) {
        AsyncNativeCallbacksPack *finalizers = reinterpret_cast<AsyncNativeCallbacksPack*>(syncWork->data);
        RunCallbacks(finalizers);
        HILOG_DEBUG("uv_queue_work running");
        delete syncWork;
        delete finalizers;
    }, uv_qos_t(napi_qos_background));
    if (ret != 0) {
        HILOG_ERROR("uv_queue_work fail ret '%{public}d'", ret);
        panda::JsiNativeScope nativeScope(vm_);
        RunCallbacks(callBacksPack);
        delete callBacksPack;
        delete syncWork;
    }
}

void ArkNativeEngine::PostTriggerGCTask(TriggerGCData& data)
{
    TriggerGCData *triggerGCData = new TriggerGCData(data);
    uv_work_t *syncWork = new uv_work_t;
    syncWork->data = reinterpret_cast<void *>(triggerGCData);
    int ret = uv_queue_work_with_qos(GetUVLoop(), syncWork, [](uv_work_t *) {}, [](uv_work_t *syncWork, int32_t) {
            auto triggerGCData = reinterpret_cast<TriggerGCData *>(syncWork->data);
            RunCallbacks(triggerGCData);
            delete syncWork;
            delete triggerGCData;
        }, uv_qos_t(napi_qos_user_initiated));
    if (ret != 0) {
        HILOG_ERROR("uv_queue_work fail ret '%{public}d'", ret);
        RunCallbacks(triggerGCData);
        delete syncWork;
        delete triggerGCData;
    }
}

#if defined(OHOS_PLATFORM) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
void ArkNativeEngine::SetAttribute(bool isLimitedWorker, panda::RuntimeOption &option)
{
    int arkProperties = OHOS::system::GetIntParameter<int>("persist.ark.properties", -1);
    std::string bundleName = OHOS::system::GetParameter("persist.ark.arkbundlename", "");
    std::string memConfigProperty = OHOS::system::GetParameter("persist.ark.mem_config_property", "");
    size_t gcThreadNum = OHOS::system::GetUintParameter<size_t>("persist.ark.gcthreads", 7);
    size_t longPauseTime = OHOS::system::GetUintParameter<size_t>("persist.ark.longpausetime", 40);
    bool asmInterpreterEnabled = OHOS::system::GetBoolParameter("persist.ark.asminterpreter", true);
    std::string asmOpcodeDisableRange = OHOS::system::GetParameter("persist.ark.asmopcodedisablerange", "");
    bool builtinsLazyEnabled = OHOS::system::GetBoolParameter("persist.ark.enablebuiltinslazy", true);
    option.SetEnableBuiltinsLazy(builtinsLazyEnabled);
    option.SetArkProperties(arkProperties);
    option.SetArkBundleName(bundleName);
    option.SetMemConfigProperty(memConfigProperty);
    option.SetGcThreadNum(gcThreadNum);
    option.SetLongPauseTime(longPauseTime);
    option.SetEnableAsmInterpreter(asmInterpreterEnabled);
    option.SetAsmOpcodeDisableRange(asmOpcodeDisableRange);
    option.SetIsWorker();
    option.SetIsRestrictedWorker(isLimitedWorker);
    HILOG_DEBUG("ArkNativeEngineImpl::CreateRuntimeFunc ark properties = %{public}d, bundlename = %{public}s",
        arkProperties, bundleName.c_str());
}
#endif

NativeEngine* ArkNativeEngine::CreateRuntimeFunc(NativeEngine* engine, void* jsEngine, bool isLimitedWorker)
{
    panda::RuntimeOption option;
#if defined(OHOS_PLATFORM) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    SetAttribute(isLimitedWorker, option);
#endif
    option.SetGcType(panda::RuntimeOption::GC_TYPE::GEN_GC);
    const int64_t poolSize = 0x1000000;
    option.SetGcPoolSize(poolSize);
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    option.SetLogLevel(panda::RuntimeOption::LOG_LEVEL::INFO);
#endif
    option.SetDebuggerLibraryPath("");
    EcmaVM* vm = JSNApi::CreateJSVM(option);
    if (vm == nullptr) {
        return nullptr;
    }
    // worker adaptation mergeabc
    const EcmaVM* hostVM = reinterpret_cast<ArkNativeEngine*>(engine)->GetEcmaVm();
    JSNApi::SynchronizVMInfo(vm, hostVM);
    ArkNativeEngine* arkEngine = new ArkNativeEngine(vm, jsEngine, isLimitedWorker);
    // init callback
    arkEngine->RegisterWorkerFunction(engine);
    arkEngine->SetHostEngine(engine);
    // sync apiVersion
    arkEngine->SetApiVersion(engine->GetApiVersion());

    auto cleanEnv = [vm]() {
        if (vm != nullptr) {
            HILOG_DEBUG("cleanEnv is called");
            JSNApi::DestroyJSVM(vm);
        }
    };
    arkEngine->SetCleanEnv(cleanEnv);
    if (hostVM != nullptr) {
        JSNApi::AddWorker(const_cast<EcmaVM*>(hostVM), vm);
    }
    return arkEngine;
}

void* ArkNativeEngine::CreateRuntime(bool isLimitedWorker)
{
    return ArkNativeEngine::CreateRuntimeFunc(this, jsEngine_, isLimitedWorker);
}

void ArkNativeEngine::SetJsDumpThresholds(size_t thresholds)
{
    DFXJSNApi::SetJsDumpThresholds(vm_, thresholds);
}

void ArkNativeEngine::SetAppFreezeFilterCallback(AppFreezeFilterCallback callback)
{
    DFXJSNApi::SetAppFreezeFilterCallback(vm_, callback);
}

void ArkNativeEngine::SetRawHeapTrimLevel(uint32_t level)
{
    DFXJSNApi::SetJsRawHeapCropLevel(static_cast<panda::CropLevel>(level));
    return;
}

void ArkNativeEngine::StartCpuProfiler(const std::string& fileName)
{
    JSNApi::SetNativePtrGetter(vm_, reinterpret_cast<void*>(ArkNativeEngine::GetNativePtrCallBack));
    DFXJSNApi::StartCpuProfilerForFile(vm_, fileName);
}

void ArkNativeEngine::StopCpuProfiler()
{
    DFXJSNApi::StopCpuProfilerForFile(vm_);
    JSNApi::SetNativePtrGetter(vm_, nullptr);
}

void ArkNativeEngine::ResumeVM()
{
    DFXJSNApi::ResumeVM(vm_);
}

bool ArkNativeEngine::SuspendVM()
{
    return DFXJSNApi::SuspendVM(vm_);
}

bool ArkNativeEngine::IsSuspended()
{
    return DFXJSNApi::IsSuspended(vm_);
}

bool ArkNativeEngine::CheckSafepoint()
{
    return DFXJSNApi::CheckSafepoint(vm_);
}

void ArkNativeEngine::GetCurrentModuleInfo(std::string& moduleName, std::string& fileName, bool needRecordName)
{
    LocalScope scope(vm_);
    std::pair<std::string, std::string> moduleInfo = JSNApi::GetCurrentModuleInfo(vm_, needRecordName);
    moduleName = moduleInfo.first; // if needRecordName is true, then moduleName is recordName.
    fileName = moduleInfo.second;
}

bool ArkNativeEngine::GetIsBundle()
{
    LocalScope scope(vm_);
    return JSNApi::IsBundle(vm_);
}

bool ArkNativeEngine::GetIsNormalizedOhmUrlPack()
{
    LocalScope scope(vm_);
    return JSNApi::IsNormalizedOhmUrlPack(vm_);
}

bool ArkNativeEngine::GetIsDebugModeEnabled()
{
    LocalScope scope(vm_);
    return JSNApi::IsDebugModeEnabled(vm_);
}

std::string ArkNativeEngine::GetBundleName()
{
    LocalScope scope(vm_);
    return JSNApi::GetBundleName(vm_);
}

std::string ArkNativeEngine::GetPkgName(const std::string &moduleName)
{
    LocalScope scope(vm_);
    return JSNApi::GetPkgName(vm_, moduleName);
}

int ArkNativeEngine::GetProcessStartRealTime()
{
    LocalScope scope(vm_);
    return JSNApi::GetStartRealTime(vm_);
}

bool ArkNativeEngine::IsExecuteModuleInAbcFile(std::string bundleName, std::string moduleName, std::string ohmurl)
{
    LocalScope scope(vm_);
    return JSNApi::IsExecuteModuleInAbcFile(vm_, bundleName, moduleName, ohmurl);
}

napi_value ArkNativeEngine::ValueToNapiValue(JSValueWrapper& value)
{
    Global<JSValueRef> arkValue = value;
    return JsValueFromLocalValue(arkValue.ToLocal(vm_));
}

napi_value ArkNativeEngine::ArkValueToNapiValue(napi_env env, Local<JSValueRef> value)
{
    return JsValueFromLocalValue(value);
}

std::string ArkNativeEngine::GetSourceCodeInfo(napi_value value, ErrorPos pos)
{
    if (value == nullptr || pos.first == 0) {
        return "";
    }

    LocalScope scope(vm_);
    Local<panda::FunctionRef> func = LocalValueFromJsValue(value);
    uint32_t line = pos.first;
    uint32_t column = pos.second;
    Local<panda::StringRef> sourceCode = func->GetSourceCode(vm_, line);
    std::string sourceCodeStr = sourceCode->ToString(vm_);
    if (sourceCodeStr.empty()) {
        return "";
    }
    std::string sourceCodeInfo = "SourceCode:\n";
    sourceCodeInfo.append(sourceCodeStr).append("\n");
    for (uint32_t k = 1; k < column; k++) {
        sourceCodeInfo.push_back(' ');
    }
    sourceCodeInfo.append("^\n");
    return sourceCodeInfo;
}

void ArkNativeEngine::TriggerFatalException(panda::Local<panda::JSValueRef> exceptionValue)
{
    JSNApi::ThrowException(GetEcmaVm(), exceptionValue);
    HandleUncaughtException();
}

bool ArkNativeEngine::AdjustExternalMemory(int64_t ChangeInBytes, int64_t* AdjustedValue)
{
    return true;
}

void ArkNativeEngine::SetPromiseRejectCallback(NativeReference* rejectCallbackRef, NativeReference* checkCallbackRef)
{
    if (rejectCallbackRef == nullptr || checkCallbackRef == nullptr) {
        HILOG_ERROR("rejectCallbackRef or checkCallbackRef is nullptr");
        return;
    }
    promiseRejectCallbackRef_ = rejectCallbackRef;
    checkCallbackRef_ = checkCallbackRef;
}

void ArkNativeEngine::PromiseRejectCallback(void* info)
{
    panda::PromiseRejectInfo* promiseRejectInfo = reinterpret_cast<panda::PromiseRejectInfo*>(info);
    ArkNativeEngine* env = reinterpret_cast<ArkNativeEngine*>(promiseRejectInfo->GetData());

    if (env == nullptr) {
        HILOG_ERROR("engine is nullptr");
        return;
    }
    bool hasRef = env->promiseRejectCallbackRef_ != nullptr;
    auto hasCallback = NapiErrorManager::GetInstance()->GetHasAllUnhandledRejectionCallback();
    if (!hasRef && !hasCallback) {
        return;
    }

    EcmaVM* vm = const_cast<EcmaVM*>(env->GetEcmaVm());
    LocalScope scope(vm);
    Local<JSValueRef> promise = promiseRejectInfo->GetPromise();
    Local<JSValueRef> reason = promiseRejectInfo->GetReason();
    panda::PromiseRejectInfo::PROMISE_REJECTION_EVENT operation = promiseRejectInfo->GetOperation();

    Local<JSValueRef> type(IntegerRef::New(vm, static_cast<int32_t>(operation)));

    Local<JSValueRef> args[] = {type, promise, reason};
    if (env->allPromiseRejectCallback_) {
        if (hasCallback && hasCallback()) {
            napi_value *callErrorArgs = new napi_value[ARGC_THREE];
            for (int i = 0; i < ARGC_THREE; i++) {
                callErrorArgs[i] = JsValueFromLocalValue(args[i]);
            }
            env->allPromiseRejectCallback_(callErrorArgs);
            delete[] callErrorArgs;
        }
    }
    if (env->promiseRejectCallbackRef_ == nullptr || env->checkCallbackRef_ == nullptr) {
        HILOG_ERROR("promiseRejectCallbackRef or checkCallbackRef is nullptr");
        return;
    }
    napi_value promiseNapiRejectCallback = env->promiseRejectCallbackRef_->Get(env);
    Local<FunctionRef> promiseRejectCallback = LocalValueFromJsValue(promiseNapiRejectCallback);
    if (!promiseRejectCallback.IsEmpty()) {
        promiseRejectCallback->Call(vm, JSValueRef::Undefined(vm), args, 3); // 3 args size
    }

    if (operation == panda::PromiseRejectInfo::PROMISE_REJECTION_EVENT::REJECT) {
        Local<JSValueRef> checkCallback = LocalValueFromJsValue(env->checkCallbackRef_->Get(env));
        if (!checkCallback.IsEmpty()) {
            JSNApi::SetHostEnqueueJob(vm, checkCallback, panda::QueueType::QUEUE_SCRIPT);
        }
    }
}

void ArkNativeEngine::DumpHeapSnapshot(const std::string &path, bool isVmMode, DumpFormat dumpFormat,
                                       bool isPrivate, bool captureNumericValue)
{
    panda::ecmascript::DumpSnapShotOption dumpOption;
    dumpOption.isVmMode = isVmMode;
    dumpOption.isSync = true;
    if (dumpFormat == DumpFormat::JSON) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::JSON;
        dumpOption.isPrivate = isPrivate;
        dumpOption.captureNumericValue = captureNumericValue;
        DFXJSNApi::DumpHeapSnapshot(vm_, path, dumpOption);
        return;
    }
    if (dumpFormat == DumpFormat::BINARY) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::BINARY;
        DFXJSNApi::DumpHeapSnapshot(vm_, path, dumpOption);
        return;
    }
    if (dumpFormat == DumpFormat::OTHER) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::OTHER;
        DFXJSNApi::DumpHeapSnapshot(vm_, path, dumpOption); // 2:enum is 2
    }
}

void ArkNativeEngine::DumpHeapSnapshot(bool isFullGC, const std::string &path,
                                       const std::function<void(uint8_t)> &callback)
{
    panda::ecmascript::DumpSnapShotOption dumpOption;
    dumpOption.isVmMode = true;
    dumpOption.isPrivate = false;
    dumpOption.isFullGC = isFullGC;
    dumpOption.isSync = false;
    dumpOption.dumpFormat = panda::ecmascript::DumpFormat::BINARY;
    DFXJSNApi::DumpHeapSnapshot(vm_, path, dumpOption, callback);
}

void ArkNativeEngine::DumpCpuProfile()
{
    DFXJSNApi::DumpCpuProfile(vm_);
}

void ArkNativeEngine::DumpHeapSnapshot(bool isVmMode, DumpFormat dumpFormat, bool isPrivate, bool isFullGC)
{
    panda::ecmascript::DumpSnapShotOption dumpOption;
    dumpOption.isVmMode = isVmMode;
    dumpOption.isPrivate = isPrivate;
    dumpOption.isFullGC = isFullGC;
    dumpOption.isSync = false;
    if (dumpFormat == DumpFormat::JSON) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::JSON;
        DFXJSNApi::DumpHeapSnapshot(vm_, dumpOption);
        return;
    }
    if (dumpFormat == DumpFormat::BINARY) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::BINARY;
        DFXJSNApi::DumpHeapSnapshot(vm_, dumpOption);
        return;
    }
    if (dumpFormat == DumpFormat::OTHER) {
        dumpOption.dumpFormat = panda::ecmascript::DumpFormat::OTHER;
        DFXJSNApi::DumpHeapSnapshot(vm_, dumpOption);
    }
}

bool ArkNativeEngine::BuildNativeAndJsStackTrace(std::string& stackTraceStr)
{
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    return DFXJSNApi::BuildNativeAndJsStackTrace(vm_, stackTraceStr);
#else
    HILOG_WARN("ARK does not support dfx on windows");
    return false;
#endif
}

bool ArkNativeEngine::BuildJsStackTrace(std::string& stackTraceStr)
{
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    return DFXJSNApi::BuildJsStackTrace(vm_, stackTraceStr);
#else
    HILOG_WARN("ARK does not support dfx on windows");
    return false;
#endif
}

bool ArkNativeEngine::BuildJsStackInfoListWithCustomDepth(std::vector<JsFrameInfo>& jsFrames)
{
#if !defined(PREVIEW) && !defined(IOS_PLATFORM)
    return DFXJSNApi::BuildJsStackInfoList(vm_, gettid(), jsFrames);
#else
    HILOG_WARN("ARK does not support dfx on windows");
    return false;
#endif
}

void ArkNativeEngine::GetMainThreadStackTrace(napi_env env, std::string &stackTraceStr)
{
    auto vm = reinterpret_cast<NativeEngine*>(env)->GetEcmaVm();
    DFXJSNApi::GetMainThreadStackTrace(vm, stackTraceStr);
}

bool ArkNativeEngine::DeleteWorker(NativeEngine* workerEngine)
{
    if (workerEngine != nullptr) {
#if !defined(PREVIEW)
        auto workerVM = const_cast<EcmaVM *>(workerEngine->GetEcmaVm());
        if (workerVM != nullptr) {
            return JSNApi::DeleteWorker(vm_, workerVM);
        }
#else
        HILOG_WARN("ARK does not support dfx on windows");
#endif
    }
    return false;
}

bool ArkNativeEngine::StartHeapTracking(double timeInterval, bool isVmMode)
{
    return DFXJSNApi::StartHeapTracking(vm_, timeInterval, isVmMode);
}

bool ArkNativeEngine::StopHeapTracking(const std::string &filePath)
{
    return DFXJSNApi::StopHeapTracking(vm_, filePath);
}

#if !defined(PREVIEW)
void ArkNativeEngine::PrintStatisticResult()
{
    DFXJSNApi::PrintStatisticResult(vm_);
}

void ArkNativeEngine::StartRuntimeStat()
{
    DFXJSNApi::StartRuntimeStat(vm_);
}

void ArkNativeEngine::StopRuntimeStat()
{
    DFXJSNApi::StopRuntimeStat(vm_);
}

size_t ArkNativeEngine::GetArrayBufferSize()
{
    return DFXJSNApi::GetArrayBufferSize(vm_);
}

size_t ArkNativeEngine::GetHeapTotalSize()
{
    return DFXJSNApi::GetHeapTotalSize(vm_);
}

size_t ArkNativeEngine::GetHeapUsedSize()
{
    return DFXJSNApi::GetHeapUsedSize(vm_);
}

size_t ArkNativeEngine::GetHeapObjectSize()
{
    return DFXJSNApi::GetHeapObjectSize(vm_);
}

size_t ArkNativeEngine::GetHeapLimitSize()
{
    return DFXJSNApi::GetHeapLimitSize(vm_);
}

size_t ArkNativeEngine::GetProcessHeapLimitSize()
{
    return DFXJSNApi::GetProcessHeapLimitSize();
}

size_t ArkNativeEngine::GetGCCount()
{
    return DFXJSNApi::GetGCCount(vm_);
}

size_t ArkNativeEngine::GetGCDuration()
{
    return DFXJSNApi::GetGCDuration(vm_);
}

size_t ArkNativeEngine::GetAccumulatedAllocateSize()
{
    return DFXJSNApi::GetAccumulatedAllocateSize(vm_);
}

size_t ArkNativeEngine::GetAccumulatedFreeSize()
{
    return DFXJSNApi::GetAccumulatedFreeSize(vm_);
}

size_t ArkNativeEngine::GetFullGCLongTimeCount()
{
    return DFXJSNApi::GetFullGCLongTimeCount(vm_);
}

void ArkNativeEngine::NotifyApplicationState(bool inBackground)
{
    DFXJSNApi::NotifyApplicationState(vm_, inBackground);
    ArkIdleMonitor::GetInstance()->NotifyChangeBackgroundState(inBackground);
    interopAppState_.Notify(inBackground ? NAPI_APP_STATE_BACKGROUND : NAPI_APP_STATE_FOREGROUND);
}

void ArkNativeEngine::NotifyIdleStatusControl(std::function<void(bool)> callback)
{
    DFXJSNApi::NotifyIdleStatusControl(vm_, callback);
}

void ArkNativeEngine::NotifyIdleTime(int idleMicroSec)
{
    DFXJSNApi::NotifyIdleTime(vm_, idleMicroSec);
}

void ArkNativeEngine::NotifyMemoryPressure(bool inHighMemoryPressure)
{
    DFXJSNApi::NotifyMemoryPressure(vm_, inHighMemoryPressure);
}

NativeEngine* ArkNativeEngine::GetArkNativeEngineByID(uint64_t tid)
{
#if defined(OHOS_PLATFORM) && !defined(MAC_PLATFORM) && !defined(IOS_PLATFORM)
    for (auto engine : g_alivedEngine_) {
        if (static_cast<uint64_t>(engine->GetTid()) == tid ||
            static_cast<uint64_t>(engine->GetSysTid()) == tid) {
                return engine;
        }
    }
    return nullptr;
#else
    return nullptr;
#endif
}

void ArkNativeEngine::NotifyForceExpandState(int32_t value)
{
    switch (ForceExpandState(value)) {
        case ForceExpandState::FINISH_COLD_START:
            DFXJSNApi::NotifyFinishColdStart(vm_, true);
            interopAppState_.Notify(NAPI_APP_STATE_COLD_START_FINISHED);
            break;
        case ForceExpandState::START_HIGH_SENSITIVE:
            DFXJSNApi::NotifyHighSensitive(vm_, true);
            interopAppState_.Notify(NAPI_APP_STATE_SENSITIVE_START);
            break;
        case ForceExpandState::FINISH_HIGH_SENSITIVE:
            DFXJSNApi::NotifyHighSensitive(vm_, false);
            interopAppState_.Notify(NAPI_APP_STATE_SENSITIVE_END);
            break;
        case ForceExpandState::WARM_START:
            DFXJSNApi::NotifyWarmStart(vm_);
            interopAppState_.Notify(NAPI_APP_STATE_WARM_START);
        default:
            HILOG_ERROR("Invalid Force Expand State: %{public}d.", value);
            break;
    }
}

void ArkNativeEngine::NotifyForceExpandState(uint64_t tid, int32_t value)
{
    std::lock_guard<std::mutex> alivedEngLock(GetAliveEngineMutex());
    NativeEngine *nativeEngine = GetArkNativeEngineByID(tid);
    if (nativeEngine == nullptr) {
        return;
    }
    nativeEngine->NotifyForceExpandState(value);
}
#else
void ArkNativeEngine::PrintStatisticResult()
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::StartRuntimeStat()
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::StopRuntimeStat()
{
    HILOG_WARN("ARK does not support dfx on windows");
}

size_t ArkNativeEngine::GetArrayBufferSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetHeapTotalSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetHeapUsedSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetHeapObjectSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetHeapLimitSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetProcessHeapLimitSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetGCCount()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetGCDuration()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetAccumulatedAllocateSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetAccumulatedFreeSize()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

size_t ArkNativeEngine::GetFullGCLongTimeCount()
{
    HILOG_WARN("ARK does not support dfx on windows");
    return 0;
}

void ArkNativeEngine::NotifyApplicationState([[maybe_unused]] bool inBackground)
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::NotifyIdleStatusControl([[maybe_unused]] std::function<void(bool)> callback)
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::NotifyIdleTime([[maybe_unused]] int idleMicroSec)
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::NotifyMemoryPressure([[maybe_unused]] bool inHighMemoryPressure)
{
    HILOG_WARN("ARK does not support dfx on windows");
}

void ArkNativeEngine::NotifyForceExpandState([[maybe_unused]] int32_t value)
{
    HILOG_WARN("ARK does not support dfx on windows");
}

NativeEngine* ArkNativeEngine::GetArkNativeEngineByID([[maybe_unused]] uint64_t tid)
{
    HILOG_WARN("ARK does not support dfx on windows");
    return nullptr;
}

void ArkNativeEngine::NotifyForceExpandState([[maybe_unused]] uint64_t tid, [[maybe_unused]] int32_t value)
{
    HILOG_WARN("ARK does not support dfx on windows");
}
#endif

void ArkNativeEngine::SetMockModuleList(const std::map<std::string, std::string> &list)
{
    JSNApi::SetMockModuleList(vm_, list);
}

static void OnErrorCallbackForThreadFunc(Local<ObjectRef> value, void *data)
{
    if (data == nullptr) {
        return;
    }
    auto engine = static_cast<ArkNativeEngine *>(data);
    auto napiUncaughtExceptionCallback = engine->GetNapiUncaughtExceptionCallback();
    if (napiUncaughtExceptionCallback == nullptr) {
        return;
    }
    napi_env env = reinterpret_cast<napi_env>(engine);
    napiUncaughtExceptionCallback(ArkNativeEngine::ArkValueToNapiValue(env, value));
}

void ArkNativeEngine::RegisterNapiUncaughtExceptionHandler(NapiUncaughtExceptionCallback callback)
{
    JSNApi::EnableUserUncaughtErrorHandler(vm_);
    JSNApi::SetOnErrorCallback(vm_, OnErrorCallbackForThreadFunc, static_cast<void *>(this));
    napiUncaughtExceptionCallback_ = callback;
}

void ArkNativeEngine::HandleUncaughtException()
{
    if (napiUncaughtExceptionCallback_ == nullptr) {
        return;
    }
    LocalScope scope(vm_);
    lastException_.Empty();
    Local<ObjectRef> exception = JSNApi::GetAndClearUncaughtException(vm_);
    if (!exception.IsEmpty() && !exception->IsHole()) {
        if (napiUncaughtExceptionCallback_ != nullptr) {
            napiUncaughtExceptionCallback_(ArkValueToNapiValue(reinterpret_cast<napi_env>(this), exception));
        }
    }
}

bool ArkNativeEngine::HasPendingException()
{
    return JSNApi::HasPendingException(vm_);
}

void ArkNativeEngine::RegisterPermissionCheck(PermissionCheckCallback callback)
{
    if (permissionCheckCallback_ == nullptr) {
        permissionCheckCallback_ = callback;
    }
}

bool ArkNativeEngine::ExecutePermissionCheck()
{
    if (permissionCheckCallback_ != nullptr) {
        return permissionCheckCallback_();
    } else {
        HILOG_INFO("permissionCheckCallback_ is still nullptr when executing permission check!");
        return true;
    }
}

void ArkNativeEngine::RegisterTranslateBySourceMap(SourceMapCallback callback)
{
    if (SourceMapCallback_ == nullptr) {
        SourceMapCallback_ = callback;
    }
    // regedit SourceMapCallback to ark_js_runtime
    JSNApi::SetSourceMapCallback(vm_, callback);
}

void ArkNativeEngine::RegisterSourceMapTranslateCallback(SourceMapTranslateCallback callback)
{
    JSNApi::SetSourceMapTranslateCallback(vm_, callback);
}

std::string ArkNativeEngine::ExecuteTranslateBySourceMap(const std::string& rawStack)
{
    if (SourceMapCallback_ != nullptr) {
        return SourceMapCallback_(rawStack);
    } else {
        return rawStack;
    }
}

bool ArkNativeEngine::IsMixedDebugEnabled()
{
    return JSNApi::IsMixedDebugEnabled(vm_);
}

void ArkNativeEngine::NotifyNativeCalling(const void *nativeAddress)
{
    JSNApi::NotifyNativeCalling(vm_, nativeAddress);
}

void ArkNativeEngine::AllowCrossThreadExecution() const
{
    JSNApi::AllowCrossThreadExecution(vm_);
}

bool DumpHybridStack(const EcmaVM* vm, std::string &stack, uint32_t ignore, int32_t deepth)
{
#ifdef OHOS_STANDARD_PLATFORM
    constexpr size_t minSkiped = 2; // 2: skiped frames, include current func and unwinder
    const size_t skipedFrames = minSkiped + ignore;
    const int backtraceDeepth = (deepth < 0 || deepth > DEFAULT_MAX_FRAME_NUM) ? DEFAULT_MAX_FRAME_NUM : deepth;
    auto unwinder = std::make_shared<OHOS::HiviewDFX::Unwinder>();
    std::vector<OHOS::HiviewDFX::DfxFrame> frames;
    if (unwinder->UnwindLocal(false, false, backtraceDeepth, skipedFrames)) {
        frames = unwinder->GetFrames();
    } else {
        HILOG_ERROR("Failed to unwind local");
    }

    for (auto &frame : frames) {
        if (frame.isJsFrame) {
            DFXJSNApi::TranslateJSStackInfo(vm, frame.mapName, frame.line, frame.column, frame.packageName);
        }
    }

    stack = OHOS::HiviewDFX::Unwinder::GetFramesStr(frames);
    return true;
#endif
    return false;
}

void ArkNativeEngine::RegisterAppStateCallback(NapiAppStateCallback callback)
{
    interopAppState_.SetCallback(callback);
}

int32_t ArkNativeEngine::GetObjectHash(napi_env env, napi_value src)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    auto vm = engine->GetEcmaVm();
    auto nativeValue = LocalValueFromJsValue(src);
    return DFXJSNApi::GetObjectHash(vm, nativeValue);
}
// LCOV_EXCL_STOP

bool ArkNativeEngine::RunScriptPath(const char* path, bool checkPath)
{
    if (checkPath && !IsValidPandaFile(path)) {
        HILOG_ERROR("file is not exist or format is invalid");
        return false;
    }
    // LCOV_EXCL_START
    panda::JSExecutionScope executionScope(vm_);
    LocalScope scope(vm_);
    [[maybe_unused]] bool ret = JSNApi::Execute(vm_, path, PANDA_MAIN_FUNCTION);
    if (JSNApi::HasPendingException(vm_)) {
        HandleUncaughtException();
        return false;
    }
    // LCOV_EXCL_STOP
    return true;
}

bool ArkNativeEngine::IsValidPandaFile(const char* path)
{
    if (path == nullptr) {
        HILOG_ERROR("file path is nullptr");
        return false;
    }

    char filePath[PATH_MAX + 1] = { 0 };
    if (!RealPath(path, filePath, PATH_MAX + 1)) {
        HILOG_ERROR("failed to format path");
        return false;
    }
    struct stat fileStat;
    int ret = stat(filePath, &fileStat);
    if (ret != 0) {
        HILOG_ERROR("script file \"%{public}s\" is not exist", filePath);
        return false;
    }

    if (!(fileStat.st_mode & S_IFREG)) {
        HILOG_ERROR("script path \"%{public}s\" is a directory", filePath);
        return false;
    }
    std::ifstream abcStream(filePath, std::ios::in | std::ios::binary);

    constexpr size_t fileHeaderLength = sizeof(uint64_t);
    uint8_t fileHeader[fileHeaderLength] = { 0 };
    if (abcStream.is_open()) {
        size_t fileSize = fileStat.st_size;
        if (fileSize < fileHeaderLength) {
            HILOG_ERROR("faild to read file header, invalid format \"%{public}s\"", filePath);
            abcStream.close();
            return false;
        }
        abcStream.read(reinterpret_cast<char*>(fileHeader), fileHeaderLength);
        abcStream.close();
        return IsValidScriptBuffer(fileHeader, fileHeaderLength);
    }
    return false;
}

bool ArkNativeEngine::IsValidScriptBuffer(uint8_t* scriptBuffer, size_t bufferSize)
{
    if (scriptBuffer == nullptr) {
        HILOG_ERROR("buffer is nullptr");
        return false;
    }
    constexpr size_t headerLen = sizeof(uint64_t);
    if (bufferSize < headerLen) {
        HILOG_ERROR("invalid buffer");
        return false;
    }
    constexpr char pandaFileHeader[headerLen] = "PANDA";
    const uint64_t bytePandaHeader = *reinterpret_cast<const uint64_t*>(pandaFileHeader);
    char fileHeader[headerLen] = { 0 };
    // Ensure destMax parameter is set correctly to avoid buffer overflows
    if (memcpy_s(fileHeader, sizeof(fileHeader), scriptBuffer, sizeof(fileHeader)) != 0) {
        HILOG_ERROR("faild to read file header of buffer");
        return false;
    }

    uint64_t byteFileHeader = *reinterpret_cast<uint64_t*>(fileHeader);
    if (byteFileHeader != bytePandaHeader) {
        HILOG_ERROR("invalid format of file buffer");
        return false;
    }
    return true;
}

// The security interface needs to be modified accordingly.
napi_value ArkNativeEngine::RunScriptBuffer(const char* path, std::vector<uint8_t>& buffer, bool isBundle,
    bool needUpdate)
{
    if (!IsValidScriptBuffer(buffer.data(), buffer.size())) {
        HILOG_ERROR("invalid script buffer");
        return nullptr;
    }

    // LCOV_EXCL_START
    panda::EscapeLocalScope scope(vm_);
    [[maybe_unused]] bool ret = false;
    if (isBundle) {
        ret = JSNApi::Execute(vm_, buffer.data(), buffer.size(), PANDA_MAIN_FUNCTION, path);
    } else {
        ret = JSNApi::ExecuteModuleBuffer(vm_, buffer.data(), buffer.size(), path, needUpdate);
    }

    if (JSNApi::HasPendingException(vm_)) {
        HandleUncaughtException();
        return nullptr;
    }
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    // LCOV_EXCL_STOP
    return JsValueFromLocalValue(scope.Escape(undefObj));
}

bool ArkNativeEngine::RunScriptBuffer(const std::string& path, uint8_t* buffer, size_t size, bool isBundle)
{
    if (!IsValidScriptBuffer(buffer, size)) {
        HILOG_ERROR("invalid script buffer");
        return false;
    }

    // LCOV_EXCL_START
    panda::JSExecutionScope executionScope(vm_);
    LocalScope scope(vm_);
    bool ret = false;
    if (isBundle) {
        ret = JSNApi::ExecuteSecure(vm_, buffer, size, PANDA_MAIN_FUNCTION, path);
    } else {
        ret = JSNApi::ExecuteModuleBufferSecure(vm_, buffer, size, path);
    }

    if (JSNApi::HasPendingException(vm_)) {
        HandleUncaughtException();
        return false;
    }
    return ret;
    // LCOV_EXCL_STOP
}

napi_value ArkNativeEngine::RunBufferScript(std::vector<uint8_t>& buffer)
{
    if (!IsValidScriptBuffer(buffer.data(), buffer.size())) {
        HILOG_ERROR("invalid script buffer");
        return nullptr;
    }

    // LCOV_EXCL_START
    panda::EscapeLocalScope scope(vm_);
    [[maybe_unused]] bool ret = JSNApi::Execute(vm_, buffer.data(), buffer.size(), PANDA_MAIN_FUNCTION);

    if (JSNApi::HasPendingException(vm_)) {
        HandleUncaughtException();
        return nullptr;
    }
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    return JsValueFromLocalValue(scope.Escape(undefObj));
    // LCOV_EXCL_STOP
}

#define EXECUTE_BUFFER(functionName)                                                                      \
    if (JSNApi::IsBundle(vm_)) {                                                                          \
        /* FA doesn't enable securemem */                                                                 \
        ret = JSNApi::Execute(vm_, buffer, bufferSize, PANDA_MAIN_FUNCTION, desc);                        \
    } else if (bufferSize != 0) {                                                                         \
        if (entryPoint == nullptr) {                                                                      \
            HILOG_DEBUG("Input entryPoint is nullptr, please input entryPoint for merged ESModule");      \
            /* this path for bundle and abc compiled by single module js */                               \
            ret = JSNApi::functionName(vm_, buffer, bufferSize, PANDA_MAIN_FUNCTION, desc, false, fileMapper);    \
        } else {                                                                                          \
            /* this path for mergeabc with specific entryPoint */                                         \
            ret = JSNApi::functionName(vm_, buffer, bufferSize, entryPoint, desc, false, fileMapper);     \
        }                                                                                                 \
    } else {                                                                                              \
        /* this path for worker */                                                                        \
        ret = JSNApi::Execute(vm_, desc, PANDA_MAIN_FUNCTION);                                            \
    }

napi_value ArkNativeEngine::RunActor(uint8_t* buffer, size_t bufferSize,
    const char* descriptor, char* entryPoint, bool checkPath, void* fileMapper)
{
    if (buffer == nullptr && descriptor == nullptr) {
        HILOG_ERROR("invalid param, both buffer and descriptor are nullptr");
        return nullptr;
    }

    if ((buffer != nullptr && !IsValidScriptBuffer(buffer, bufferSize)) &&
        (checkPath && descriptor != nullptr && !IsValidPandaFile(descriptor))) {
        HILOG_ERROR("invalid param");
        return nullptr;
    }

    // LCOV_EXCL_START
    panda::EscapeLocalScope scope(vm_);
    std::string desc(descriptor);
    [[maybe_unused]] bool ret = false;
    // if apiVersion > API11, use secure path.
    if (IsApplicationApiVersionAPI11Plus()) {
        EXECUTE_BUFFER(ExecuteSecure);
    } else {
        EXECUTE_BUFFER(Execute);
    }
    if (JSNApi::HasPendingException(vm_)) {
        HandleUncaughtException();
        return nullptr;
    }
    Local<JSValueRef> undefObj = JSValueRef::Undefined(vm_);
    return JsValueFromLocalValue(scope.Escape(undefObj));
    // LCOV_EXCL_STOP
}

panda::Local<panda::ObjectRef> ArkNativeEngine::LoadArkModule(const void* buffer,
    int32_t len, const std::string& fileName)
{
    panda::EscapeLocalScope scope(vm_);
    Local<ObjectRef> undefObj(JSValueRef::Undefined(vm_));
    if (buffer == nullptr || len <= 0 || fileName.empty()) {
        HILOG_ERROR("fileName is nullptr or source code is nullptr");
        return scope.Escape(undefObj);
    }
    if (!IsValidScriptBuffer(reinterpret_cast<uint8_t*>(const_cast<void*>(buffer)), len)) {
        HILOG_ERROR("invalid script buffer");
        return scope.Escape(undefObj);
    }

    // LCOV_EXCL_START
    bool res = JSNApi::ExecuteModuleFromBuffer(vm_, buffer, len, fileName);
    if (!res) {
        HILOG_ERROR("Execute module failed");
        return scope.Escape(undefObj);
    }

    Local<ObjectRef> exportObj = JSNApi::GetExportObjectFromBuffer(vm_, fileName, "default");
    if (exportObj->IsNull()) {
        HILOG_ERROR("Get export object failed");
        return scope.Escape(undefObj);
    }

    HILOG_DEBUG("ArkNativeEngineImpl::LoadModule end");
    return scope.Escape(exportObj);
    // LCOV_EXCL_STOP
}

bool ArkNativeEngine::ExecuteJsBin(const std::string& fileName, bool checkPath)
{
    if (checkPath && !IsValidPandaFile(fileName.c_str())) {
        HILOG_ERROR("faild to execute js bin, file is not exist or format is invalid");
        return false;
    }
    // LCOV_EXCL_START
    panda::JSExecutionScope executionScope(vm_);
    LocalScope scope(vm_);
    bool ret = JSNApi::Execute(vm_, fileName, PANDA_MAIN_FUNCTION);
    return ret;
    // LCOV_EXCL_STOP
}

void ArkNativeEngine::EnableNapiProfiler()
{
#ifdef ENABLE_HITRACE
    if (!ArkNativeEngine::napiProfilerParamReaded) {
        char napiProfilerParam[NAPI_PROFILER_PARAM_SIZE] = {0};
        int ret = GetParameter("persist.hiviewdfx.napiprofiler.enabled", "false",
            napiProfilerParam, sizeof(napiProfilerParam));
        if (ret > 0 && strcmp(napiProfilerParam, "true") == 0) {
            ArkNativeEngine::napiProfilerEnabled = true;
        }
        ArkNativeEngine::napiProfilerParamReaded = true;
    }
#endif
}

napi_status ArkNativeEngine::SwitchContext()
{
    if (context_.IsEmpty()) {
        HILOG_ERROR("no env context exists");
        return napi_generic_failure;
    }

    panda::JSNApi::SwitchContext(vm_, context_.ToLocal(vm_));
    return napi_ok;
}

napi_status ArkNativeEngine::DestroyContext()
{
    if (engineState_ != ArkNativeEngineState::RUNNING) {
        HILOG_ERROR("Attempt to release an env that is already marked for deletion");
        return napi_invalid_arg;
    }

    // the original context cannot be destroyed
    if (isMainEnvContext_) {
        HILOG_ERROR("main env context cannot be destroyed");
        return napi_invalid_arg;
    }

    // the current running context also cannot be destroyed
    Local<JSValueRef> currentContext = panda::JSNApi::GetCurrentContext(vm_);
    if (context_.ToLocal(vm_) == currentContext) {
        HILOG_ERROR("running env context cannot be destroyed");
        return napi_invalid_arg;
    }

    engineState_ = ArkNativeEngineState::STOPPED;
    if (!HasRuntimeOwnedRef()) {
        Delete();
    }
    // waiting all ref callbacks done.
    return napi_ok;
}
