/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECMASCRIPT_TOOLING_INIT_H
#define ECMASCRIPT_TOOLING_INIT_H
#include "common/macros.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

TOOLCHAIN_EXPORT int StartDebugger(uint32_t port, bool breakOnStart);
TOOLCHAIN_EXPORT void InitializeInspector(std::shared_ptr<void> vm, bool breakOnStart);
TOOLCHAIN_EXPORT void HandleMessage(std::string &&msg);
TOOLCHAIN_EXPORT void StopInspector();
TOOLCHAIN_EXPORT void WaitForDebugger();
TOOLCHAIN_EXPORT int StopDebugger();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //ECMASCRIPT_TOOLING_INIT_H