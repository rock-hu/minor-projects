/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef LIBPANDAFILE_PANDA_CACHE_H
#define LIBPANDAFILE_PANDA_CACHE_H

#include "file.h"
#include "os/mutex.h"
#include "libpandabase/utils/math_helpers.h"

#include <atomic>
#include <vector>

namespace panda {

class Method;
class Field;
class Class;

namespace panda_file {

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
class PandaCache {
public:
    struct MethodCachePair {
        File::EntityId id_;
        Method *ptr_ {nullptr};
    };

    struct FieldCachePair {
        File::EntityId id_;
        Field *ptr_ {nullptr};
    };

    struct ClassCachePair {
        File::EntityId id_;
        Class *ptr_ {nullptr};
    };

    PandaCache()
        : METHOD_CACHE_SIZE(DEFAULT_METHOD_CACHE_SIZE),
          FIELD_CACHE_SIZE(DEFAULT_FIELD_CACHE_SIZE),
          CLASS_CACHE_SIZE(DEFAULT_CLASS_CACHE_SIZE)
    {
        method_cache_.resize(METHOD_CACHE_SIZE, MethodCachePair());
        field_cache_.resize(FIELD_CACHE_SIZE, FieldCachePair());
        class_cache_.resize(CLASS_CACHE_SIZE, ClassCachePair());
    }

    ~PandaCache() = default;

    inline uint32_t GetMethodIndex(File::EntityId id) const
    {
        return panda::helpers::math::PowerOfTwoTableSlot(id.GetOffset(), METHOD_CACHE_SIZE);
    }

    inline uint32_t GetFieldIndex(File::EntityId id) const
    {
        // lowest one or two bits is very likely same between different fields
        return panda::helpers::math::PowerOfTwoTableSlot(id.GetOffset(), FIELD_CACHE_SIZE, 2U);
    }

    inline uint32_t GetClassIndex(File::EntityId id) const
    {
        return panda::helpers::math::PowerOfTwoTableSlot(id.GetOffset(), CLASS_CACHE_SIZE);
    }

    inline Method *GetMethodFromCache(File::EntityId id) const
    {
        uint32_t index = GetMethodIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<MethodCachePair> *>(reinterpret_cast<uintptr_t>(&(method_cache_[index])));
        // Atomic with acquire order reason: fixes a data race with method_cache_
        auto pair = pair_ptr->load(std::memory_order_acquire);
        TSAN_ANNOTATE_HAPPENS_AFTER(pair_ptr);
        if (pair.id_ == id) {
            return pair.ptr_;
        }
        return nullptr;
    }

    inline void SetMethodCache(File::EntityId id, Method *method)
    {
        MethodCachePair pair;
        pair.id_ = id;
        pair.ptr_ = method;
        uint32_t index = GetMethodIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<MethodCachePair> *>(reinterpret_cast<uintptr_t>(&(method_cache_[index])));
        TSAN_ANNOTATE_HAPPENS_BEFORE(pair_ptr);
        // Atomic with release order reason: fixes a data race with method_cache_
        pair_ptr->store(pair, std::memory_order_release);
    }

    inline Field *GetFieldFromCache(File::EntityId id) const
    {
        uint32_t index = GetFieldIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<FieldCachePair> *>(reinterpret_cast<uintptr_t>(&(field_cache_[index])));
        // Atomic with acquire order reason: fixes a data race with field_cache_
        auto pair = pair_ptr->load(std::memory_order_acquire);
        TSAN_ANNOTATE_HAPPENS_AFTER(pair_ptr);
        if (pair.id_ == id) {
            return pair.ptr_;
        }
        return nullptr;
    }

    inline void SetFieldCache(File::EntityId id, Field *field)
    {
        uint32_t index = GetFieldIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<FieldCachePair> *>(reinterpret_cast<uintptr_t>(&(field_cache_[index])));
        FieldCachePair pair;
        pair.id_ = id;
        pair.ptr_ = field;
        TSAN_ANNOTATE_HAPPENS_BEFORE(pair_ptr);
        // Atomic with release order reason: fixes a data race with field_cache_
        pair_ptr->store(pair, std::memory_order_release);
    }

    inline Class *GetClassFromCache(File::EntityId id) const
    {
        uint32_t index = GetClassIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<ClassCachePair> *>(reinterpret_cast<uintptr_t>(&(class_cache_[index])));
        // Atomic with acquire order reason: fixes a data race with class_cache_
        auto pair = pair_ptr->load(std::memory_order_acquire);
        TSAN_ANNOTATE_HAPPENS_AFTER(pair_ptr);
        if (pair.id_ == id) {
            return pair.ptr_;
        }
        return nullptr;
    }

    inline void SetClassCache(File::EntityId id, Class *clazz)
    {
        ClassCachePair pair;
        pair.id_ = id;
        pair.ptr_ = clazz;
        uint32_t index = GetClassIndex(id);
        auto *pair_ptr =
            reinterpret_cast<std::atomic<ClassCachePair> *>(reinterpret_cast<uintptr_t>(&(class_cache_[index])));
        TSAN_ANNOTATE_HAPPENS_BEFORE(pair_ptr);
        // Atomic with release order reason: fixes a data race with class_cache_
        pair_ptr->store(pair, std::memory_order_release);
    }

    inline void Clear()
    {
        method_cache_.clear();
        field_cache_.clear();
        class_cache_.clear();

        method_cache_.resize(METHOD_CACHE_SIZE, MethodCachePair());
        field_cache_.resize(FIELD_CACHE_SIZE, FieldCachePair());
        class_cache_.resize(CLASS_CACHE_SIZE, ClassCachePair());
    }

    template <class Callback>
    bool EnumerateCachedClasses(const Callback &cb)
    {
        for (uint32_t i = 0; i < CLASS_CACHE_SIZE; i++) {
            auto *pair_ptr =
                reinterpret_cast<std::atomic<ClassCachePair> *>(reinterpret_cast<uintptr_t>(&(class_cache_[i])));
            // Atomic with acquire order reason: fixes a data race with class_cache_
            auto pair = pair_ptr->load(std::memory_order_acquire);
            TSAN_ANNOTATE_HAPPENS_AFTER(pair_ptr);
            if (pair.ptr_ != nullptr) {
                if (!cb(pair.ptr_)) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    static constexpr uint32_t DEFAULT_FIELD_CACHE_SIZE = 1024U;
    static constexpr uint32_t DEFAULT_METHOD_CACHE_SIZE = 1024U;
    static constexpr uint32_t DEFAULT_CLASS_CACHE_SIZE = 1024U;
    static_assert(panda::helpers::math::IsPowerOfTwo(DEFAULT_FIELD_CACHE_SIZE));
    static_assert(panda::helpers::math::IsPowerOfTwo(DEFAULT_METHOD_CACHE_SIZE));
    static_assert(panda::helpers::math::IsPowerOfTwo(DEFAULT_CLASS_CACHE_SIZE));

    const uint32_t METHOD_CACHE_SIZE;
    const uint32_t FIELD_CACHE_SIZE;
    const uint32_t CLASS_CACHE_SIZE;

    std::vector<MethodCachePair> method_cache_;
    std::vector<FieldCachePair> field_cache_;
    std::vector<ClassCachePair> class_cache_;
};

}  // namespace panda_file
}  // namespace panda

#endif  // LIBPANDAFILE_PANDA_CACHE_H
