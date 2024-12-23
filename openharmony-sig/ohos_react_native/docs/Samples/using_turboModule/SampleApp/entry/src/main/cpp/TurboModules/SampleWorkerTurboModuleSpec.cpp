/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// NOTE: This entire file should be codegen'ed.

#include "SampleWorkerTurboModuleSpec.h"

using namespace rnoh;
using namespace facebook;

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_pushStringToHarmony(jsi::Runtime &rt,
                                                                                       react::TurboModule &turboModule,
                                                                                       const jsi::Value *args,
                                                                                       size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "pushStringToHarmony", args, count));
}

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_registerFunction(jsi::Runtime &rt,
                                                                                    react::TurboModule &turboModule,
                                                                                    const jsi::Value *args,
                                                                                    size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "registerFunction", args, count));
}

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_doAsyncJob(jsi::Runtime &rt,
                                                                              react::TurboModule &turboModule,
                                                                              const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).callAsync(rt, "doAsyncJob", args, count));
}

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_rnLog(jsi::Runtime &rt,
                                                                         react::TurboModule &turboModule,
                                                                         const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "rnLog", args, count));
}

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_getPreferencesData(jsi::Runtime &rt,
                                                                                      react::TurboModule &turboModule,
                                                                                      const jsi::Value *args,
                                                                                      size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getPreferencesData", args, count));
}

static jsi::Value __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_putPreferencesData(jsi::Runtime &rt,
                                                                                      react::TurboModule &turboModule,
                                                                                      const jsi::Value *args,
                                                                                      size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "putPreferencesData", args, count));
}

NativeSampleWorkerTurboModuleSpecJSI::NativeSampleWorkerTurboModuleSpecJSI(const ArkTSTurboModule::Context ctx,
                                                               const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_["pushStringToHarmony"] =
        MethodMetadata{1, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_pushStringToHarmony};
    methodMap_["registerFunction"] =
        MethodMetadata{0, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_registerFunction};
    methodMap_["doAsyncJob"] = MethodMetadata{1, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_doAsyncJob};
    methodMap_["rnLog"] = MethodMetadata{1, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_rnLog};
    methodMap_["putPreferencesData"] =
        MethodMetadata{1, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_putPreferencesData};
    methodMap_["getPreferencesData"] =
        MethodMetadata{0, __hostFunction_NativeSampleWorkerTurboCxxModuleSpecJSI_getPreferencesData};
}