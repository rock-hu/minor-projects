/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_VERIFICATION_UTIL_PANDA_OR_STD_H
#define PANDA_VERIFICATION_UTIL_PANDA_OR_STD_H

#ifdef PANDA_RAPIDCHECK
#include <memory>
#else
#include "runtime/include/mem/panda_containers.h"
#include "runtime/include/mem/panda_smart_pointers.h"
#endif

namespace ark::verifier {

// Panda's allocator by default, standard allocator if Rapidcheck is enabled because it doesn't link Panda's libraries
#ifdef PANDA_RAPIDCHECK
template <typename T>
using MPandaAllocator = std::allocator<T>;

template <typename T>
using MPandaVector = std::vector<T>;

template <typename T>
using MPandaUniquePtr = std::unique_ptr<T>;

// much simpler than trying to reproduce the full API
#define M_MAKE_PANDA_UNIQUE std::make_unique
#else
template <typename T>
using MPandaAllocator = ark::mem::AllocatorAdapter<T>;

template <typename T>
using MPandaVector = PandaVector<T>;

template <typename T>
using MPandaUniquePtr = PandaUniquePtr<T>;

#define M_MAKE_PANDA_UNIQUE MakePandaUnique
#endif

}  // namespace ark::verifier

#endif  // PANDA_VERIFICATION_UTIL_PANDA_OR_STD_H
