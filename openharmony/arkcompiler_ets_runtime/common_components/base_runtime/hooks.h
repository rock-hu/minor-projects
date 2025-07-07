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

#ifndef COMMON_COMPONENTS_BASE_RUNTIME_HOOKS_H
#define COMMON_COMPONENTS_BASE_RUNTIME_HOOKS_H

#include <cstdint>

#include "common_interfaces/heap/heap_visitor.h"
#include "common_interfaces/thread/mutator_base.h"

// Visitor that iterate all `RefField`s in a TaggedObject and add them to
// `WorkStack` Should be moved to BaseRT and panda namespace later
namespace common {
// Roots in BaseRuntime
PUBLIC_API void VisitBaseRoots(const RefFieldVisitor &visitor);
// Dynamic VM Roots scanning
PUBLIC_API void VisitDynamicGlobalRoots(const RefFieldVisitor &visitor);
PUBLIC_API void VisitDynamicWeakGlobalRoots(const WeakRefFieldVisitor &visitorFunc);
PUBLIC_API void VisitDynamicLocalRoots(const RefFieldVisitor &visitor);
PUBLIC_API void VisitDynamicWeakLocalRoots(const WeakRefFieldVisitor &visitorFunc);

// Visit roots of specific local thread.
PUBLIC_API void VisitDynamicThreadRoot(const RefFieldVisitor &visitorFunc, void *vm);
PUBLIC_API void VisitDynamicWeakThreadRoot(const WeakRefFieldVisitor &visitorFunc, void *vm);

PUBLIC_API void VisitJSThread(void *jsThread, CommonRootVisitor visitor);
PUBLIC_API void SynchronizeGCPhaseToJSThread(void *jsThread, GCPhase gcPhase);

// CMC-GC dependent interface
PUBLIC_API void FillFreeObject(void *object, size_t size);
PUBLIC_API void SetBaseAddress(uintptr_t base);
PUBLIC_API void JSGCCallback(void *ecmaVM);
PUBLIC_API bool IsPostForked();

// Jit interfaces
PUBLIC_API void SweepThreadLocalJitFort();
PUBLIC_API bool IsMachineCodeObject(uintptr_t obj);
PUBLIC_API void JitFortUnProt(size_t size, void* base);

} // namespace common
#endif // COMMON_COMPONENTS_BASE_RUNTIME_HOOKS_H
