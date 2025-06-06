# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.4.1)
cmake_policy(SET CMP0079 NEW) # Allow to link libraries from different
                              # directories than targets
project(rnoh)
set(RNOH_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(NATIVERENDER_ROOT_PATH "${RNOH_CPP_DIR}")
set(third_party_dir "${RNOH_CPP_DIR}/third-party")
set(patches_dir "${RNOH_CPP_DIR}/patches")
option(USE_HERMES "enables the Hermes JS Engine" ON)

option(WITH_HITRACE_SYSTRACE "enables HiTrace-based Systrace implementation" ON)
if(WITH_HITRACE_SYSTRACE)
    add_compile_definitions(WITH_HITRACE_SYSTRACE)
endif()
add_compile_options("-Wno-error=unused-command-line-argument")

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

# FMT
set(fmt_include_dir "${third_party_dir}/fmt/include")
set(fmt_src_dir "${third_party_dir}/fmt/src")
add_library(fmt_target)
target_include_directories(fmt_target PRIVATE
    "${fmt_include_dir}"
)
target_sources(fmt_target PRIVATE
    "${fmt_src_dir}/format.cc"
    "${fmt_src_dir}/os.cc"
)

# DOUBLE CONVERSION
set(double_conversion_include_dir "${third_party_dir}/double-conversion")
set(double_conversion_src_dir "${third_party_dir}/double-conversion/double-conversion")
add_library(double_conversion_target)
target_include_directories(double_conversion_target PRIVATE
    "${double_conversion_include_dir}"
)
target_sources(double_conversion_target PRIVATE
    "${double_conversion_src_dir}/bignum-dtoa.cc"
    "${double_conversion_src_dir}/bignum.cc"
    "${double_conversion_src_dir}/cached-powers.cc"
    "${double_conversion_src_dir}/diy-fp.cc"
    "${double_conversion_src_dir}/double-conversion.cc"
    "${double_conversion_src_dir}/fast-dtoa.cc"
    "${double_conversion_src_dir}/fixed-dtoa.cc"
    "${double_conversion_src_dir}/strtod.cc"
)

# GLOG
set(glog_include_dir "${third_party_dir}/glog/src")
set(glog_src_dir "${third_party_dir}/glog/src")
add_library(glog SHARED)
target_include_directories(glog PUBLIC
    "${glog_include_dir}"
    "${glog_include_dir}/base"
)
target_sources(glog PRIVATE
    "${glog_src_dir}/demangle.cc"
    "${glog_src_dir}/logging.cc"
    "${glog_src_dir}/raw_logging.cc"
    "${glog_src_dir}/signalhandler.cc"
    "${glog_src_dir}/symbolize.cc"
    "${glog_src_dir}/utilities.cc"
    "${glog_src_dir}/vlog_is_on.cc"
)
target_compile_options(glog PRIVATE
    -Wno-shorten-64-to-32
    -Wno-header-hygiene
    -Wno-deprecated-declarations
    -fdeclspec
)

# BOOST
set(boost_libs_dir "${third_party_dir}/boost/libs")
file(GLOB boost_include_dirs "${boost_libs_dir}/*/include")
set(BOOST_ENABLE_CMAKE On)
add_subdirectory("${boost_libs_dir}/core")
add_subdirectory("${boost_libs_dir}/config")
add_subdirectory("${boost_libs_dir}/assert")
add_subdirectory("${boost_libs_dir}/static_assert")
add_subdirectory("${boost_libs_dir}/type_traits")
add_subdirectory("${boost_libs_dir}/throw_exception")
add_subdirectory("${boost_libs_dir}/move")
add_subdirectory("${boost_libs_dir}/predef")
add_subdirectory("${boost_libs_dir}/pool")
add_subdirectory("${boost_libs_dir}/mp11")
add_subdirectory("${boost_libs_dir}/smart_ptr")
add_subdirectory("${boost_libs_dir}/winapi")
add_subdirectory("${boost_libs_dir}/integer")
add_subdirectory("${boost_libs_dir}/context")

# LIBEVENT
add_library(libevent)
set(libevent_src_dir "${third_party_dir}/libevent")
set(libevent_include_dirs
    "${patches_dir}/libevent/${OHOS_ARCH}"
    "${third_party_dir}/libevent/include"
    "${third_party_dir}/libevent/compat"
)
target_include_directories(libevent PUBLIC
    ${libevent_include_dirs}
)
target_sources(libevent PRIVATE
    "${libevent_src_dir}/event.c"
    "${libevent_src_dir}/buffer.c"
    "${libevent_src_dir}/bufferevent.c"
    "${libevent_src_dir}/bufferevent_filter.c"
    "${libevent_src_dir}/bufferevent_pair.c"
    "${libevent_src_dir}/bufferevent_ratelim.c"
    "${libevent_src_dir}/bufferevent_sock.c"
    "${libevent_src_dir}/epoll.c"
    "${libevent_src_dir}/evmap.c"
    "${libevent_src_dir}/evthread.c"
    "${libevent_src_dir}/evthread_pthread.c"
    "${libevent_src_dir}/evutil.c"
    "${libevent_src_dir}/evutil_rand.c"
    "${libevent_src_dir}/evutil_time.c"
    "${libevent_src_dir}/listener.c"
    "${libevent_src_dir}/log.c"
    "${libevent_src_dir}/poll.c"
    "${libevent_src_dir}/signal.c"
    "${libevent_src_dir}/strlcpy.c"
    "${libevent_src_dir}/select.c"
)


# FOLLY
set(folly_include_dir "${third_party_dir}/folly")
set(folly_src_dir "${third_party_dir}/folly/folly")
add_library(folly_runtime SHARED)
target_include_directories(folly_runtime PUBLIC
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${double_conversion_include_dir}"
    "${glog_include_dir}"
    "${fmt_include_dir}"
)
set(folly_compile_options
    -DFOLLY_NO_CONFIG=1
    -DFOLLY_MOBILE=1
    -DFOLLY_USE_LIBCPP=1
    -DFOLLY_HAVE_RECVMMSG=1
    -DFOLLY_HAVE_PTHREAD=1
    -Wno-comma
    -Wno-shorten-64-to-32
    -Wno-documentation
    -faligned-new
)
target_compile_options(folly_runtime PUBLIC ${folly_compile_options})
target_sources(folly_runtime PRIVATE
    "${folly_src_dir}/SharedMutex.cpp"
    "${folly_src_dir}/concurrency/CacheLocality.cpp"
    "${folly_src_dir}/detail/Futex.cpp"
    "${folly_src_dir}/portability/Malloc.cpp"
    "${folly_src_dir}/synchronization/ParkingLot.cpp"
    "${folly_src_dir}/system/ThreadId.cpp"
    "${folly_src_dir}/lang/SafeAssert.cpp"
    "${folly_src_dir}/lang/ToAscii.cpp"
    "${folly_src_dir}/dynamic.cpp"
    "${folly_src_dir}/hash/SpookyHashV2.cpp"
    "${folly_src_dir}/json_pointer.cpp"
    "${folly_src_dir}/Conv.cpp"
    "${folly_src_dir}/Format.cpp"
    "${folly_src_dir}/memory/detail/MallocImpl.cpp"
    "${folly_src_dir}/json.cpp"
    "${folly_src_dir}/Unicode.cpp"
    "${folly_src_dir}/lang/Assume.cpp"
    "${folly_src_dir}/ScopeGuard.cpp"
)
target_link_libraries(folly_runtime PUBLIC
    fmt_target
    glog
    double_conversion_target
)

# FOLLY FUTURES
add_library(folly_futures)
target_include_directories(folly_futures PUBLIC
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${double_conversion_include_dir}"
    "${glog_include_dir}"
    "${fmt_include_dir}"
    "${libevent_include_dirs}"
)
target_compile_options(folly_futures PRIVATE
    ${folly_compile_options}
    "-Wno-nullability-completeness"
)
target_compile_features(folly_futures PUBLIC cxx_std_17)

target_sources(folly_futures PRIVATE
    "${patches_dir}/folly/Demangle.cpp"
    "${folly_src_dir}/ExceptionString.cpp"
    "${folly_src_dir}/ExceptionWrapper.cpp"
    "${folly_src_dir}/Executor.cpp"
    "${folly_src_dir}/FileUtil.cpp"
    "${folly_src_dir}/ScopeGuard.cpp"
    "${folly_src_dir}/String.cpp"
    "${folly_src_dir}/Singleton.cpp"
    "${folly_src_dir}/Try.cpp"
    # "${folly_src_dir}/concurrency/CacheLocality.cpp"
    "${folly_src_dir}/concurrency/DeadlockDetector.cpp"
    "${folly_src_dir}/concurrency/QueueObserver.cpp"
    "${folly_src_dir}/detail/AsyncTrace.cpp"
    "${folly_src_dir}/detail/AtFork.cpp"
    # "${folly_src_dir}/detail/Futex.cpp"
    "${folly_src_dir}/detail/MemoryIdler.cpp"
    "${folly_src_dir}/detail/SingletonStackTrace.cpp"
    "${folly_src_dir}/detail/StaticSingletonManager.cpp"
    "${folly_src_dir}/detail/ThreadLocalDetail.cpp"
    "${folly_src_dir}/detail/UniqueInstance.cpp"
    "${folly_src_dir}/executors/CPUThreadPoolExecutor.cpp"
    "${folly_src_dir}/executors/Codel.cpp"
    "${folly_src_dir}/executors/EDFThreadPoolExecutor.cpp"
    "${folly_src_dir}/executors/ExecutorWithPriority.cpp"
    "${folly_src_dir}/executors/GlobalExecutor.cpp"
    "${folly_src_dir}/executors/GlobalThreadPoolList.cpp"
    "${folly_src_dir}/executors/IOThreadPoolDeadlockDetectorObserver.cpp"
    "${folly_src_dir}/executors/IOThreadPoolExecutor.cpp"
    "${folly_src_dir}/executors/InlineExecutor.cpp"
    "${folly_src_dir}/executors/ManualExecutor.cpp"
    "${folly_src_dir}/executors/MeteredExecutor.cpp"
    "${folly_src_dir}/executors/QueuedImmediateExecutor.cpp"
    "${folly_src_dir}/executors/SerialExecutor.cpp"
    "${folly_src_dir}/executors/StrandExecutor.cpp"
    "${folly_src_dir}/executors/ThreadPoolExecutor.cpp"
    "${folly_src_dir}/executors/ThreadedExecutor.cpp"
    "${folly_src_dir}/executors/TimedDrivableExecutor.cpp"
    "${folly_src_dir}/executors/TimekeeperScheduledExecutor.cpp"
    "${folly_src_dir}/fibers/BatchSemaphore.cpp"
    "${folly_src_dir}/fibers/Baton.cpp"
    "${folly_src_dir}/fibers/Fiber.cpp"
    "${folly_src_dir}/fibers/FiberManager.cpp"
    "${folly_src_dir}/fibers/GuardPageAllocator.cpp"
    "${folly_src_dir}/fibers/Semaphore.cpp"
    "${folly_src_dir}/fibers/SemaphoreBase.cpp"
    "${folly_src_dir}/fibers/SimpleLoopController.cpp"
    "${folly_src_dir}/futures/Barrier.cpp"
    "${folly_src_dir}/futures/Future.cpp"
    "${folly_src_dir}/futures/ManualTimekeeper.cpp"
    "${folly_src_dir}/futures/Promise.cpp"
    "${folly_src_dir}/futures/SharedPromise.cpp"
    "${folly_src_dir}/futures/ThreadWheelTimekeeper.cpp"
    "${folly_src_dir}/futures/detail/Core.cpp"
    "${folly_src_dir}/io/Cursor.cpp"
    "${folly_src_dir}/io/IOBuf.cpp"
    "${folly_src_dir}/io/IOBufQueue.cpp"
    "${folly_src_dir}/io/async/AsyncTimeout.cpp"
    "${folly_src_dir}/io/async/EventBase.cpp"
    "${folly_src_dir}/io/async/EventBaseBackendBase.cpp"
    "${folly_src_dir}/io/async/EventBaseLocal.cpp"
    "${folly_src_dir}/io/async/EventBaseManager.cpp"
    "${folly_src_dir}/io/async/EventHandler.cpp"
    "${folly_src_dir}/io/async/HHWheelTimer.cpp"
    "${folly_src_dir}/io/async/Request.cpp"
    "${folly_src_dir}/io/async/TimeoutManager.cpp"
    "${folly_src_dir}/io/async/VirtualEventBase.cpp"
    "${folly_src_dir}/lang/CString.cpp"
    "${folly_src_dir}/lang/Exception.cpp"
    "${folly_src_dir}/memory/MallctlHelper.cpp"
    "${folly_src_dir}/net/NetOps.cpp"
    "${folly_src_dir}/net/NetOpsDispatcher.cpp"
    "${folly_src_dir}/net/TcpInfo.cpp"
    "${folly_src_dir}/net/detail/SocketFileDescriptorMap.cpp"
    "${folly_src_dir}/portability/SysMembarrier.cpp"
    "${folly_src_dir}/portability/SysUio.cpp"
    "${folly_src_dir}/synchronization/AsymmetricMemoryBarrier.cpp"
    "${folly_src_dir}/synchronization/AtomicNotification.cpp"
    "${folly_src_dir}/synchronization/DistributedMutex.cpp"
    "${folly_src_dir}/synchronization/Hazptr.cpp"
    "${folly_src_dir}/synchronization/HazptrThreadPoolExecutor.cpp"
    # "${folly_src_dir}/synchronization/ParkingLot.cpp"
    "${folly_src_dir}/synchronization/Rcu.cpp"
    "${folly_src_dir}/synchronization/SanitizeThread.cpp"
    "${folly_src_dir}/synchronization/WaitOptions.cpp"
    "${folly_src_dir}/synchronization/detail/Hardware.cpp"
    "${folly_src_dir}/synchronization/detail/Sleeper.cpp"
    "${folly_src_dir}/system/HardwareConcurrency.cpp"
    "${folly_src_dir}/system/Pid.cpp"
    # "${folly_src_dir}/system/ThreadId.cpp"
    "${folly_src_dir}/system/ThreadName.cpp"
    "${folly_src_dir}/tracing/AsyncStack.cpp"
)
target_link_libraries(folly_futures libevent)

# -------- REACT COMMON --------
set(REACT_COMMON_DIR "${RNOH_CPP_DIR}/third-party/rn/ReactCommon")
set(REACT_COMMON_PATCH_DIR "${patches_dir}/react_native_core")

include_directories(${REACT_COMMON_PATCH_DIR})

# dummy targets added to avoid modyfing CMakeLists located in ReactCommon
add_library(boost INTERFACE)
add_library(log INTERFACE)
add_library(glog_init INTERFACE)
add_library(android INTERFACE)
add_library(fb INTERFACE)
add_library(fbjni INTERFACE)
add_library(reactnativejni INTERFACE)
add_library(mapbufferjni INTERFACE)

function(add_react_common_subdir relative_path)
  add_subdirectory(${REACT_COMMON_DIR}/${relative_path})
endfunction()

set(yoga_compile_options
    -fexceptions
    -frtti
    -O3
    -Wall
    -Wpedantic
    -Wno-gnu-zero-variadic-macro-arguments)
file(GLOB_RECURSE yogacore_SRC CONFIGURE_DEPENDS ${REACT_COMMON_DIR}/yoga/yoga/*.cpp)
add_library(yoga SHARED ${yogacore_SRC})
target_compile_options(yoga PUBLIC ${yoga_compile_options})
target_include_directories(yoga PUBLIC ${REACT_COMMON_DIR}/yoga)
target_link_libraries(yoga android log)

add_react_common_subdir(runtimeexecutor)
add_react_common_subdir(reactperflogger)
add_react_common_subdir(logger)
add_react_common_subdir(jsiexecutor)
add_react_common_subdir(cxxreact)
add_react_common_subdir(jsi)
add_react_common_subdir(butter)
add_react_common_subdir(callinvoker)
add_react_common_subdir(jsinspector)
add_react_common_subdir(react/renderer/runtimescheduler)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/debug)
add_react_common_subdir(react/config)
add_react_common_subdir(react/renderer/animations)
add_react_common_subdir(react/renderer/attributedstring)
add_react_common_subdir(react/renderer/componentregistry)
add_react_common_subdir(react/renderer/mounting)
add_react_common_subdir(react/renderer/scheduler)
add_react_common_subdir(react/renderer/telemetry)
add_react_common_subdir(react/renderer/templateprocessor)
add_react_common_subdir(react/renderer/uimanager)
add_react_common_subdir(react/renderer/core)
add_react_common_subdir(react/renderer/graphics)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/renderer/debug)
add_react_common_subdir(react/renderer/imagemanager)
add_react_common_subdir(react/renderer/components/view)
# add_react_common_subdir(react/renderer/components/switch)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/renderer/components/textinput)
# add_react_common_subdir(react/renderer/components/progressbar)
add_react_common_subdir(react/renderer/components/root)
add_react_common_subdir(react/renderer/components/image)
# add_react_common_subdir(react/renderer/components/legacyviewmanagerinterop)
# add_react_common_subdir(react/renderer/componentregistry/native)
#add_react_common_subdir(react/renderer/components/text)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/renderer/components/text)
# add_react_common_subdir(react/renderer/components/unimplementedview)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/renderer/components/rncore)
add_react_common_subdir(react/renderer/components/modal)
add_react_common_subdir(react/renderer/components/scrollview)
add_react_common_subdir(react/renderer/leakchecker)
add_subdirectory(${REACT_COMMON_PATCH_DIR}/react/renderer/textlayoutmanager)
add_react_common_subdir(react/utils)
add_react_common_subdir(react/bridging)
add_react_common_subdir(react/renderer/mapbuffer)
add_react_common_subdir(react/nativemodule/core)
target_include_directories(react_nativemodule_core PRIVATE "${REACT_COMMON_DIR}/callinvoker")

add_subdirectory("${RNOH_CPP_DIR}/RNOH/executor")
# ----------------------------------------------------------------------------------------------------------------------

add_library(rnoh SHARED
    "${RNOH_CPP_DIR}/RNOH/RNInstanceArkTS.cpp"
    "${RNOH_CPP_DIR}/RNOH/RNInstanceCAPI.cpp"
    "${RNOH_CPP_DIR}/RNOH/RNInstanceInternal.cpp"
    "${RNOH_CPP_DIR}/RNOH/JSBigStringHelpers.cpp"
    "${RNOH_CPP_DIR}/RNOH/SchedulerDelegate.cpp"
    "${RNOH_CPP_DIR}/RNOH/BlobCollector.cpp"
    "${RNOH_CPP_DIR}/RNOH/MessageQueueThread.cpp"
    "${RNOH_CPP_DIR}/RNOH/MutationsToNapiConverter.cpp"
    "${RNOH_CPP_DIR}/RNOH/LogSink.cpp"
    "${RNOH_CPP_DIR}/RNOH/ArkJS.cpp"
    "${RNOH_CPP_DIR}/RNOH/ArkTSBridge.cpp"
    "${RNOH_CPP_DIR}/RNOH/Inspector.cpp"
    "${RNOH_CPP_DIR}/RNOH/ComponentInstanceProvider.cpp"
    "${RNOH_CPP_DIR}/RNOH/MountingManagerArkTS.cpp"
    "${RNOH_CPP_DIR}/RNOH/MountingManagerCAPI.cpp"
    "${RNOH_CPP_DIR}/RNOH/ShadowViewRegistry.cpp"
    "${RNOH_CPP_DIR}/RNOH/TurboModuleProvider.cpp"
    "${RNOH_CPP_DIR}/RNOH/TurboModuleFactory.cpp"
    "${RNOH_CPP_DIR}/RNOH/ArkTSTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOH/JsiConversions.cpp"
    "${RNOH_CPP_DIR}/RNOH/Package.cpp"
    "${RNOH_CPP_DIR}/RNOH/ComponentInstancePreallocationRequestQueue.cpp"
    "${RNOH_CPP_DIR}/RNOH/UIManagerModule.cpp"
    "${RNOH_CPP_DIR}/RNOH/VSyncListener.cpp"
    "${RNOH_CPP_DIR}/RNOH/TouchTarget.cpp"
    "${RNOH_CPP_DIR}/RNOH/TextMeasurer.cpp"
    "${RNOH_CPP_DIR}/RNOH/TextMeasureRegistry.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/TaskExecutor.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/NapiTaskRunner.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/EventLoopTaskRunner.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/ThreadTaskRunner.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/DefaultExceptionHandler.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/uv/Async.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/uv/Timer.cpp"
    "${RNOH_CPP_DIR}/RNOH/TaskExecutor/uv/EventLoop.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/AccessibilityInfoTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/AlertManagerTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/AppearanceTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/AppStateTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/BlobTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/DeviceEventManagerTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/DeviceInfoTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/ExceptionsManagerTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/FileReaderTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/ImageLoaderTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/LogBoxTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/DevLoadingViewTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/LinkingManagerTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/NetworkingTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/PlatformColorTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/PlatformConstantsTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/SourceCodeTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/StatusBarTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/TimingTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/WebSocketTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/SafeAreaTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/DevSettingsTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/DevMenuTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/VibrationTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/ToastAndroidTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/ShareTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/NativeAnimatedTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Nodes/AnimatedNode.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Nodes/TransformAnimatedNode.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Nodes/InterpolationAnimatedNode.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Nodes/DiffClampAnimatedNode.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Nodes/TrackingAnimatedNode.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Drivers/AnimationDriver.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Drivers/FrameBasedAnimationDriver.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Drivers/SpringAnimationDriver.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Drivers/DecayAnimationDriver.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/Animated/Drivers/EventAnimationDriver.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/TurboModules/I18nManagerTurboModule.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/EventEmitRequestHandlers/TouchEventEmitRequestHandler.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/UIInputEventHandler.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/NativeNodeApi.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/NativeDialogApi.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ArkUINodeRegistry.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/NodeContentHandle.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ArkUISurface.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ArkUINode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ColumnNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ImageNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ScrollNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/StackNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/TextNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/TextAreaNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/TextInputNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/TextInputNodeBase.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ArkUIDialogHandler.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/TouchEventDispatcher.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/LoadingProgressNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/ToggleNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/RefreshNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/arkui/CustomNode.cpp"
    "${RNOH_CPP_DIR}/RNOH/ComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/ImageComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/ViewComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/TextComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/TextConversions.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/TextInputComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/ScrollViewComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/ActivityIndicatorComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/ModalHostViewComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/SwitchComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/PullToRefreshViewComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOHCorePackage/ComponentInstances/CustomNodeComponentInstance.cpp"
    "${RNOH_CPP_DIR}/RNOH/Performance/HarmonyReactMarker.cpp"
    "${RNOH_CPP_DIR}/RNOH/Performance/OHReactMarkerListener.cpp"
)
target_include_directories(rnoh PUBLIC
    "${RNOH_CPP_DIR}"
    "${REACT_COMMON_DIR}"
    "${REACT_COMMON_DIR}/jsiexecutor"
    "${folly_include_dir}"
    "${RNOH_APP_DIR}"
)
# add hitrace systrace implementation for debugging
if(DEFINED WITH_HITRACE_SYSTRACE)
    target_link_libraries(reactnative libhitrace_ndk.z.so)
    target_link_libraries(react_nativemodule_core libhitrace_ndk.z.so)
    target_link_libraries(react_render_debug libhitrace_ndk.z.so)
endif()

target_link_libraries(rnoh PUBLIC
    libace_napi.z.so
    libace_ndk.z.so
    libhilog_ndk.z.so
    librawfile.z.so
    libnative_vsync.so
    libnative_drawing.so
    uv
    Boost::context
    reactnative
    react_render_scheduler
    rrc_image
    rrc_text
    rrc_textinput
    rrc_scrollview
    rrc_modal
    react_codegen_rncore
    react_nativemodule_core
    react_bridging
    react_render_animations
)

message("USE_HERMES is set to: ${USE_HERMES}")
if(USE_HERMES)
    add_hermes_executor(ON)
else()
    add_jsvm_executor(ON)
endif()

target_compile_options(rnoh PUBLIC
    ${folly_compile_options}
    -DRAW_PROPS_ENABLED
)
target_compile_features(rnoh PUBLIC cxx_std_17)

if("$ENV{RNOH_C_API_ARCH}" STREQUAL "1")
    message("Experimental C-API architecture enabled")
    target_link_libraries(rnoh PUBLIC libqos.so)
    target_compile_definitions(rnoh PUBLIC C_API_ARCH)
endif()

if(DEFINED LOG_VERBOSITY_LEVEL)
  message("LOG_VERBOSITY_LEVEL is set to: ${LOG_VERBOSITY_LEVEL}")
  target_compile_definitions(rnoh PUBLIC LOG_VERBOSITY_LEVEL=${LOG_VERBOSITY_LEVEL})
endif()
