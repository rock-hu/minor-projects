#include <ace/xcomponent/native_interface_xcomponent.h>
#include <cxxreact/JSExecutor.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "RNInstanceFactory.h"
#include "RNOH/ArkJS.h"
#include "RNOH/ArkTSBridge.h"
#include "RNOH/Inspector.h"
#include "RNOH/LogSink.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "RNOH/Performance/OHReactMarkerListener.h"
#include "RNOH/RNInstance.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/TaskExecutor/NapiTaskRunner.h"
#include "RNOH/TaskExecutor/ThreadTaskRunner.h"
#include "RNOH/UITicker.h"
#include "RNOH/arkui/ArkUINodeRegistry.h"

template <typename Map, typename K, typename V>
auto getOrDefault(const Map& map, K&& key, V&& defaultValue)
    -> std::common_type_t<decltype(map.at(std::forward<K>(key))), V&&> {
  auto it = map.find(std::forward<K>(key));
  if (it != map.end()) {
    return it->second;
  }
  return std::forward<V>(defaultValue);
}

template <typename Map, typename K, typename V>
auto extractOrDefault(Map& map, K&& key, V&& defaultValue)
    -> std::common_type_t<decltype(map.at(std::forward<K>(key))), V&&> {
  auto it = map.find(std::forward<K>(key));
  if (it != map.end()) {
    auto value = std::move(it->second);
    map.erase(it);
    return value;
  }
  return std::forward<V>(defaultValue);
}

std::mutex RN_INSTANCE_BY_ID_MTX;
std::unordered_map<size_t, std::shared_ptr<RNInstanceInternal>>
    RN_INSTANCE_BY_ID;

std::unordered_map<int, ArkTSBridge::Shared> ARK_TS_BRIDGE_BY_ENV_ID;

std::unordered_map<int, std::pair<napi_ref, napi_env>>
    WORKER_TURBO_MODULE_PROVIDER_REF_AND_ENV_BY_RN_INSTANCE_ID;
std::unordered_map<int, std::unique_ptr<NapiTaskRunner>>
    WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID;
std::mutex WORKER_DATA_MTX;

std::shared_ptr<RNInstanceInternal> maybeGetInstanceById(size_t instanceId) {
  auto lock = std::lock_guard<std::mutex>(RN_INSTANCE_BY_ID_MTX);
  auto it = RN_INSTANCE_BY_ID.find(instanceId);
  if (it == RN_INSTANCE_BY_ID.end()) {
    return nullptr;
  }
  return it->second;
}

auto UI_TICKER = std::make_shared<UITicker>();
static auto CLEANUP_RUNNER = std::make_unique<ThreadTaskRunner>("RNOH_CLEANUP");

napi_value invoke(napi_env env, std::function<napi_value()> operation) {
  ArkJS arkJS(env);
  try {
    return arkJS.createResult(Ok<napi_value>(operation()));
  } catch (...) {
    return arkJS.createResult(Err<napi_value>(std::current_exception()));
  }
}

static napi_value onInit(napi_env env, napi_callback_info info) {
  static int nextEnvId = 0;
  HarmonyReactMarker::setLogMarkerIfNeeded();
#ifdef WITH_HITRACE_REACT_MARKER
  HarmonyReactMarker::addListener(OHReactMarkerListener::getInstance());
#endif
  LogSink::initializeLogging();
  auto logVerbosityLevel = 0;
 
#ifdef LOG_VERBOSITY_LEVEL
    FLAGS_v = LOG_VERBOSITY_LEVEL;
    logVerbosityLevel = LOG_VERBOSITY_LEVEL;
#endif
  DLOG(INFO) << "onInit (LOG_VERBOSITY_LEVEL=" << logVerbosityLevel << ")";
  ArkJS arkJs(env);
  auto args = arkJs.getCallbackArgs(info, 1);
    nextEnvId++;
  auto shouldClearRNInstances = arkJs.getBoolean(args[0]);
    if (shouldClearRNInstances) {
      /**
       * This CPP code can survive closing an app. The app can be closed before
       * removing all RNInstances. As a workaround, all rnInstances are removed
       * on the start.
       */
      CLEANUP_RUNNER->runAsyncTask([] {
        decltype(RN_INSTANCE_BY_ID) instances;
        {
          std::lock_guard<std::mutex> lock(RN_INSTANCE_BY_ID_MTX);
          std::swap(RN_INSTANCE_BY_ID, instances);
        }
        instances.clear();
      });
      ARK_TS_BRIDGE_BY_ENV_ID.clear();
    }
    auto isDebugModeEnabled = false;
#ifdef REACT_NATIVE_DEBUG
  isDebugModeEnabled = true;
#endif
    return arkJs.createObjectBuilder()
        .addProperty("isDebugModeEnabled", isDebugModeEnabled)
        .addProperty("envId", nextEnvId)
        .build();
}

napi_value initializeArkTSBridge(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  auto args = arkJs.getCallbackArgs(info, 1);
  auto bridgeHandlerRef = arkJs.createReference(args[0]);
  ArkTSBridge::initializeInstance(env, bridgeHandlerRef);
#ifdef C_API_ARCH
  ArkUINodeRegistry::initialize(ArkTSBridge::getInstance());
#endif
  return arkJs.getUndefined();
}

/**
 * @thread: WORKER
 */
static napi_value registerWorkerTurboModuleProvider(
    napi_env env,
    napi_callback_info info) {
  return invoke(env, [&] {
    DLOG(INFO) << "registerWorkerTurboModuleProvider";
    ArkJS arkJS(env);
    auto args = arkJS.getCallbackArgs(info, 2);
    auto workerTurboModuleProviderRef = arkJS.createReference(args[0]);
    auto rnInstanceId = arkJS.getDouble(args[1]);
    auto lock = std::lock_guard(WORKER_DATA_MTX);
    WORKER_TURBO_MODULE_PROVIDER_REF_AND_ENV_BY_RN_INSTANCE_ID.emplace(
        rnInstanceId, std::make_pair(workerTurboModuleProviderRef, env));
    if (WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.find(rnInstanceId) ==
        WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.end()) {
      WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.emplace(
          rnInstanceId,
          std::make_unique<NapiTaskRunner>(
              "RNOH_WORKER_" + std::to_string(static_cast<int>(rnInstanceId)),
              env));
    }
    return arkJS.getUndefined();
  });
}

static napi_value getNextRNInstanceId(
    napi_env env,
    napi_callback_info /*info*/) {
  try {
    static std::atomic_size_t nextId = 0;
    auto id = nextId++;
    DLOG(INFO) << "getNextRNInstanceId: " << id;
    return ArkJS(env).createInt(id);
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
    return ArkJS(env).createInt(-1);
  }
}

static napi_value onCreateRNInstance(
    napi_env env,
    napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    DLOG(INFO) << "createReactNativeInstance";
    HarmonyReactMarker::setAppStartTime(
        facebook::react::JSExecutor::performanceNow());
    auto args = arkJs.getCallbackArgs(info, 14);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto mainArkTSTurboModuleProviderRef = arkJs.createReference(args[1]);
    auto arkTsTurboModuleProviderRef = arkJs.createReference(args[1]);
    auto mutationsListenerRef = arkJs.createReference(args[2]);
    auto commandDispatcherRef = arkJs.createReference(args[3]);
    auto eventDispatcherRef = arkJs.createReference(args[4]);
    auto measureTextFnRef = arkJs.createReference(args[5]);
    auto shouldEnableDebugger = arkJs.getBoolean(args[6]);
    auto shouldEnableBackgroundExecutor = arkJs.getBoolean(args[7]);
    auto featureFlagRegistry = std::make_shared<FeatureFlagRegistry>();
    for (auto featureFlagNameAndStatus : arkJs.getObjectProperties(args[8])) {
      featureFlagRegistry->setFeatureFlagStatus(
          arkJs.getString(featureFlagNameAndStatus.first),
          arkJs.getBoolean(featureFlagNameAndStatus.second));
    }
    auto frameNodeFactoryRef = arkJs.createReference(args[9]);
    auto jsResourceManager = args[10];
    auto arkTsComponentNamesDynamic = arkJs.getDynamic(args[11]);
    std::unordered_set<std::string> arkTsComponentNames = {};
    for (size_t i = 0; i < arkTsComponentNamesDynamic.size(); ++i) {
        arkTsComponentNames.emplace(arkTsComponentNamesDynamic[i].asString());
    }
    auto fontPathByFontFamilyEntries =
        arkJs.getObjectProperties(args[12]);
    std::unordered_map<std::string, std::string>
        fontPathByFontFamily;
    for (auto& [fontFamily, fontPathRelativeToRawfileDir] :
         fontPathByFontFamilyEntries) {
      fontPathByFontFamily.emplace(
          arkJs.getString(fontFamily),
          arkJs.getString(fontPathRelativeToRawfileDir));
    }
    int envId = arkJs.getDouble(args[13]);
    auto workerLock = std::lock_guard(WORKER_DATA_MTX);
    std::unique_ptr<NapiTaskRunner> workerTaskRunner = nullptr;
    auto workerTaskRunnerIt =
        WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.find(instanceId);
    if (workerTaskRunnerIt != WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.end()) {
      workerTaskRunner = std::move(
          WORKER_TASK_RUNNER_BY_RN_INSTANCE_ID.extract(workerTaskRunnerIt)
              .mapped());
    }
    auto workerTurboModuleProviderRefAndEnv = extractOrDefault(
        WORKER_TURBO_MODULE_PROVIDER_REF_AND_ENV_BY_RN_INSTANCE_ID,
        instanceId,
        std::make_pair(nullptr, nullptr));
    auto rnInstance = createRNInstance(
        instanceId,
        env,
        workerTurboModuleProviderRefAndEnv.second,
        std::move(workerTaskRunner),
        getOrDefault(ARK_TS_BRIDGE_BY_ENV_ID, envId, nullptr),
        mainArkTSTurboModuleProviderRef,
        workerTurboModuleProviderRefAndEnv.first,
        frameNodeFactoryRef,
        [env, instanceId, mutationsListenerRef](
            auto const& mutationsToNapiConverter, auto const& mutations) {
          {
            auto lock = std::lock_guard<std::mutex>(RN_INSTANCE_BY_ID_MTX);
            if (RN_INSTANCE_BY_ID.find(instanceId) == RN_INSTANCE_BY_ID.end()) {
              DLOG(WARNING) << "RNInstance with the following id " +
                      std::to_string(instanceId) + " does not exist";
              return;
            }
          }
          ArkJS arkJs(env);
          auto napiMutations = mutationsToNapiConverter.convert(env, mutations);
          std::array<napi_value, 1> args = {napiMutations};
          auto listener = arkJs.getReferenceValue(mutationsListenerRef);
          arkJs.call<1>(listener, args);
        },
        [env, instanceId, commandDispatcherRef](
            auto tag, auto const& commandName, auto args) {
          {
            auto lock = std::lock_guard<std::mutex>(RN_INSTANCE_BY_ID_MTX);
            if (RN_INSTANCE_BY_ID.find(instanceId) == RN_INSTANCE_BY_ID.end()) {
              DLOG(WARNING) << "RNInstance with the following id " +
                      std::to_string(instanceId) + " does not exist";
              return;
            }
          }
          ArkJS arkJs(env);
          auto napiArgs = arkJs.convertIntermediaryValueToNapiValue(args);
          std::array<napi_value, 3> napiArgsArray = {
              arkJs.createDouble(tag),
              arkJs.createString(commandName),
              napiArgs};
          auto commandDispatcher =
              arkJs.getReferenceValue(commandDispatcherRef);
          arkJs.call<3>(commandDispatcher, napiArgsArray);
        },
        measureTextFnRef,
        eventDispatcherRef,
        featureFlagRegistry,
        UI_TICKER,
        jsResourceManager,
        shouldEnableDebugger,
        shouldEnableBackgroundExecutor,
        arkTsComponentNames,
        std::move(fontPathByFontFamily));

    auto lock = std::lock_guard<std::mutex>(RN_INSTANCE_BY_ID_MTX);
    if (RN_INSTANCE_BY_ID.find(instanceId) != RN_INSTANCE_BY_ID.end()) {
      LOG(FATAL) << "RNInstance with the following id " +
              std::to_string(instanceId) + " has been already created";
    }
    auto [it, _inserted] =
        RN_INSTANCE_BY_ID.emplace(instanceId, std::move(rnInstance));
    it->second->start();
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value onDestroyRNInstance(
    napi_env env,
    napi_callback_info info) {
  DLOG(INFO) << "destroyReactNativeInstance";
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 1);
    size_t rnInstanceId = arkJs.getDouble(args[0]);
    CLEANUP_RUNNER->runAsyncTask([rnInstanceId] {
      std::shared_ptr<RNInstanceInternal> instance;
      {
        auto lock = std::lock_guard<std::mutex>(RN_INSTANCE_BY_ID_MTX);
        if (auto it = RN_INSTANCE_BY_ID.find(rnInstanceId);
            it != RN_INSTANCE_BY_ID.end()) {
          std::swap(it->second, instance);
          RN_INSTANCE_BY_ID.erase(rnInstanceId);
        }
      }
    });
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value loadScript(napi_env env, napi_callback_info info) {
  DLOG(INFO) << "loadScript";
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 4);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    auto onFinishRef = arkJs.createReference(args[3]);
    rnInstance->loadScript(
        arkJs.getArrayBuffer(args[1]),
        arkJs.getString(args[2]),
        [taskExecutor = rnInstance->getTaskExecutor(), env, onFinishRef](
            const std::string& errorMsg) {
          taskExecutor->runTask(
              TaskThread::MAIN, [env, onFinishRef, errorMsg]() {
                ArkJS arkJs(env);
                auto listener = arkJs.getReferenceValue(onFinishRef);
                arkJs.call<1>(listener, {arkJs.createString(errorMsg)});
                arkJs.deleteReference(onFinishRef);
              });
        });
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value updateSurfaceConstraints(
    napi_env env,
    napi_callback_info info) {
  DLOG(INFO) << "updateSurfaceConstraints\n";
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 10);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    rnInstance->updateSurfaceConstraints(
        arkJs.getDouble(args[1]),
        arkJs.getDouble(args[2]),
        arkJs.getDouble(args[3]),
        arkJs.getDouble(args[4]),
        arkJs.getDouble(args[5]),
        arkJs.getDouble(args[6]),
        arkJs.getDouble(args[7]),
        arkJs.getDouble(args[8]),
        arkJs.getBoolean(args[9]));
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }

  return arkJs.getUndefined();
}

static napi_value measureSurface(napi_env env, napi_callback_info info) {
  return invoke(env, [&] {
    DLOG(INFO) << "measureSurface";
    ArkJS arkJS(env);
    auto args = arkJS.getCallbackArgs(info, 10);
    size_t instanceId = arkJS.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJS.getUndefined();
    }
    auto size = rnInstance->measureSurface(
        arkJS.getDouble(args[1]),
        arkJS.getDouble(args[2]),
        arkJS.getDouble(args[3]),
        arkJS.getDouble(args[4]),
        arkJS.getDouble(args[5]),
        arkJS.getDouble(args[6]),
        arkJS.getDouble(args[7]),
        arkJS.getDouble(args[8]),
        arkJS.getBoolean(args[9]));
    return arkJS.createObjectBuilder()
        .addProperty("width", size.width)
        .addProperty("height", size.height)
        .build();
  });
}

static napi_value createSurface(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 3);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    facebook::react::Tag surfaceId = arkJs.getDouble(args[1]);
    DLOG(INFO) << "createSurface: surfaceId=" << surfaceId;
    auto appKey = arkJs.getString(args[2]);
    rnInstance->createSurface(surfaceId, appKey);
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value startSurface(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 11);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    facebook::react::Tag surfaceId = arkJs.getDouble(args[1]);
    DLOG(INFO) << "startSurface: surfaceId=" << surfaceId << "\n";
    rnInstance->startSurface(
        surfaceId,
        arkJs.getDouble(args[2]),
        arkJs.getDouble(args[3]),
        arkJs.getDouble(args[4]),
        arkJs.getDouble(args[5]),
        arkJs.getDouble(args[6]),
        arkJs.getDouble(args[7]),
        arkJs.getDouble(args[8]),
        arkJs.getBoolean(args[9]),
        arkJs.getDynamic(args[10]));
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value stopSurface(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 3);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    facebook::react::Tag surfaceId = arkJs.getDouble(args[1]);
    auto n_onStopRef = arkJs.createReference(args[2]);
    DLOG(INFO) << "stopSurface: surfaceId=" << surfaceId << "\n";
    rnInstance->stopSurface(
        surfaceId, [env, n_onStopRef = std::move(n_onStopRef)]() {
          ArkJS arkJS(env);
          arkJS.call(arkJS.getReferenceValue(n_onStopRef), {});
          arkJS.deleteReference(n_onStopRef);
        });
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value destroySurface(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 2);
    size_t instanceId = arkJs.getDouble(args[0]);
    facebook::react::Tag surfaceId = arkJs.getDouble(args[1]);
    DLOG(INFO) << "destroySurface: surfaceId=" << surfaceId << "\n";
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    rnInstance->destroySurface(surfaceId);
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value setSurfaceDisplayMode(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 3);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    facebook::react::Tag surfaceId = arkJs.getDouble(args[1]);
    DLOG(INFO) << "setSurfaceDisplayMode: surfaceId=" << surfaceId << "\n";
    rnInstance->setSurfaceDisplayMode(
        surfaceId,
        static_cast<facebook::react::DisplayMode>(arkJs.getDouble(args[2])));
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value emitComponentEvent(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 5);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    rnInstance->emitComponentEvent(
        env, arkJs.getDouble(args[1]), arkJs.getString(args[2]), args[3]);
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value callRNFunction(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 4);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    auto moduleString = arkJs.getString(args[1]);
    auto nameString = arkJs.getString(args[2]);
    auto argsDynamic = arkJs.getDynamic(args[3]);
    rnInstance->callJSFunction(
        std::move(moduleString), std::move(nameString), std::move(argsDynamic));
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }
  return arkJs.getUndefined();
}

static napi_value onMemoryLevel(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 1);
    auto memoryLevel = arkJs.getDouble(args[0]);
    auto lock = std::unique_lock<std::mutex>(RN_INSTANCE_BY_ID_MTX);
    auto instances = RN_INSTANCE_BY_ID;
    lock.unlock();
    for (auto& [id, instance] : instances) {
      if (instance != nullptr) {
        instance->onMemoryLevel(static_cast<size_t>(memoryLevel));
      }
    }
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }

  return arkJs.getUndefined();
}

static napi_value updateState(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 4);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    auto componentName = arkJs.getString(args[1]);
    auto tag = arkJs.getDouble(args[2]);
    auto state = args[3];
    rnInstance->updateState(
        env, componentName, static_cast<facebook::react::Tag>(tag), state);
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }

  return arkJs.getUndefined();
}

static napi_value logMarker(napi_env env, napi_callback_info info) {
  return invoke(env, [&] {
    ArkJS arkJS(env);
    auto args = arkJS.getCallbackArgs(info, 2);
    auto markerId = arkJS.getString(args[0]);
    auto rnInstanceId = std::to_string(arkJS.getDouble(args[1])).c_str();
    HarmonyReactMarker::logMarker(markerId, rnInstanceId);
    return arkJS.getNull();
  });
}

/**
 * @thread: MAIN/WORKER
 */
static napi_value onArkTSMessage(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 2);
    auto messageName = arkJs.getString(args[0]);
    auto wrappedPayload = arkJs.getDynamic(args[1]);
    auto rnInstanceId = wrappedPayload["rnInstanceId"].getDouble();
    auto messagePayload = wrappedPayload["payload"];
    auto rnInstance = maybeGetInstanceById(rnInstanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
	std::weak_ptr<RNInstanceInternal> weakRNInstance = rnInstance;
    auto taskExecutor = rnInstance->getTaskExecutor();
    if (taskExecutor->isOnTaskThread(TaskThread::MAIN)) {
      rnInstance->handleArkTSMessage(messageName, messagePayload);
    } else {
      taskExecutor->runTask(
          TaskThread::MAIN, [weakRNInstance, messageName, messagePayload] {
            auto rnInstance = weakRNInstance.lock();
            if (rnInstance == nullptr) {
              return;
            }
            rnInstance->handleArkTSMessage(messageName, messagePayload);
          });
    }
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }

  return arkJs.getUndefined();
}

static void registerNativeXComponent(napi_env env, napi_value exports) {
if ((env == nullptr) || (exports == nullptr)) {
    LOG(ERROR) << "registerNativeXComponent: env or exports is null"
               << "\n";
    return;
  }

  napi_value exportInstance = nullptr;
  if (napi_get_named_property(
          env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
    LOG(ERROR) << "registerNativeXComponent: napi_get_named_property fail"
               << "\n";
    return;
  }

  OH_NativeXComponent* nativeXComponent = nullptr;
  if (napi_unwrap(
          env, exportInstance, reinterpret_cast<void**>(&nativeXComponent)) !=
      napi_ok) {
    LOG(ERROR) << "registerNativeXComponent: napi_unwrap fail"
               << "\n";
    return;
  }

  char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
  uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
  if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) !=
      OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOG(ERROR)
        << "registerNativeXComponent: OH_NativeXComponent_GetXComponentId fail"
        << "\n";
    return;
  }
  std::string xcomponentStr(idStr);
  std::stringstream ss(xcomponentStr);
  std::string instanceId;
  std::getline(ss, instanceId, '_');
  std::string surfaceId;
  std::getline(ss, surfaceId, '_');
  size_t instanceIdNum = std::stod(instanceId, nullptr);
  if (RN_INSTANCE_BY_ID.find(instanceIdNum) == RN_INSTANCE_BY_ID.end()) {
    LOG(ERROR) << "RNInstance with the following id "
               << std::to_string(instanceIdNum) << " does not exist";
    return;
  }
  auto& rnInstance = RN_INSTANCE_BY_ID.at(instanceIdNum);

  auto* rnInstanceCPIRawPtr = dynamic_cast<RNInstanceCAPI*>(rnInstance.get());
  if (rnInstanceCPIRawPtr != nullptr) {
    rnInstanceCPIRawPtr->registerNativeXComponentHandle(
        nativeXComponent, std::stoi(surfaceId));
  }
  DLOG(INFO) << "registerNativeXComponent: id = " << instanceId << "\n";
}

static napi_value setBundlePath(napi_env env, napi_callback_info info)
{
  DLOG(INFO) << "setBundlePath";
  ArkJS arkJs(env);
  try {
    auto args = arkJs.getCallbackArgs(info, 2);
    size_t instanceId = arkJs.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJs.getUndefined();
    }
    rnInstance->setBundlePath(arkJs.getString(args[1]));
  } catch (...) {
    ArkTSBridge::getInstance()->handleError(std::current_exception());
  }

  return arkJs.getUndefined();
}

static napi_value registerFont(napi_env env, napi_callback_info info) {
  return invoke(env, [&] {
    ArkJS arkJS(env);
    auto args = arkJS.getCallbackArgs(info, 3);
    size_t instanceId = arkJS.getDouble(args[0]);
    auto rnInstance = maybeGetInstanceById(instanceId);
    if (!rnInstance) {
      return arkJS.getUndefined();
    }

    auto fontName = arkJS.getString(args[1]);
    auto fontPath = arkJS.getString(args[2]);

    rnInstance->registerFont(fontName, fontPath);

    return arkJS.getUndefined();
  });
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      {"onInit",
       nullptr,
       onInit,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"registerWorkerTurboModuleProvider",
       nullptr,
       registerWorkerTurboModuleProvider,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"setBundlePath",
       nullptr,
       setBundlePath,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"getNextRNInstanceId",
       nullptr,
       getNextRNInstanceId,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"onCreateRNInstance",
       nullptr,
       onCreateRNInstance,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"onDestroyRNInstance",
       nullptr,
       onDestroyRNInstance,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"loadScript",
       nullptr,
       loadScript,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"startSurface",
       nullptr,
       startSurface,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"stopSurface",
       nullptr,
       stopSurface,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"destroySurface",
       nullptr,
       destroySurface,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"createSurface",
       nullptr,
       createSurface,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"measureSurface",
       nullptr,
       measureSurface,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"updateSurfaceConstraints",
       nullptr,
       updateSurfaceConstraints,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"setSurfaceDisplayMode",
       nullptr,
       setSurfaceDisplayMode,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"logMarker",
       nullptr,
       logMarker,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"onArkTSMessage",
       nullptr,
       onArkTSMessage,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"emitComponentEvent",
       nullptr,
       emitComponentEvent,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"callRNFunction",
       nullptr,
       callRNFunction,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"onMemoryLevel",
       nullptr,
       onMemoryLevel,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"updateState",
       nullptr,
       updateState,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"getInspectorWrapper",
       nullptr,
       rnoh::getInspectorWrapper,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"initializeArkTSBridge",
       nullptr,
       ::initializeArkTSBridge,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"registerFont",
       nullptr,
       ::registerFont,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr}};

  napi_define_properties(
      env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc);
  registerNativeXComponent(
      env, exports); // NOTE: shouldn't this be called when creating surface?
  return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "rnoh_app",
    .nm_priv = ((void*)nullptr),
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
  napi_module_register(&demoModule);
}
