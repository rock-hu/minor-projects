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

#include "ecmascript/mem/native_area_allocator.h"

#include "ecmascript/platform/map.h"
#include "ecmascript/platform/os.h"

#if ECMASCRIPT_ENABLE_ZAP_MEM
#include "securec.h"
#endif
namespace panda::ecmascript {
Area *NativeAreaAllocator::AllocateArea(size_t capacity)
{
    size_t headerSize = sizeof(Area);
    if (capacity < headerSize) { // LOCV_EXCL_BR_LINE
        LOG_ECMA_MEM(FATAL) << "capacity must have a size not less than sizeof Area.";
        UNREACHABLE();
    }
    if (cachedArea_ != nullptr && capacity <= cachedArea_->GetSize()) {
        auto result = cachedArea_;
        cachedArea_ = nullptr;
        return result;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    void *mem = malloc(capacity);
    if (mem == nullptr) { // LOCV_EXCL_BR_LINE
        LOG_ECMA_MEM(FATAL) << "malloc failed, current alloc size = " << capacity
                            << ", total allocated size = " << nativeMemoryUsage_.load(std::memory_order_relaxed);
        UNREACHABLE();
    }
#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(mem, capacity, 0, capacity) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    IncreaseNativeMemoryUsage(capacity);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    uintptr_t begin = reinterpret_cast<uintptr_t>(mem) + headerSize;
    capacity -= headerSize;
    return new (mem) Area(begin, capacity);
}

void NativeAreaAllocator::FreeArea(Area *area)
{
    if (area == nullptr) {
        return;
    }
    if (cachedArea_ == nullptr && area->GetSize() <= MAX_CACHED_CHUNK_AREA_SIZE) {
        cachedArea_ = area;
        return;
    }
    auto size = area->GetSize() + sizeof(Area);
    DecreaseNativeMemoryUsage(size);
#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(area, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(reinterpret_cast<std::byte *>(area));
}

void NativeAreaAllocator::Free(void *mem, size_t size)
{
    if (mem == nullptr) {
        return;
    }
    DecreaseNativeMemoryUsage(size);
#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(mem, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(mem);
}

void *NativeAreaAllocator::AllocateBuffer(size_t size)
{
    if (size == 0) { // LOCV_EXCL_BR_LINE
        LOG_ECMA_MEM(FATAL) << "size must have a size bigger than 0";
        UNREACHABLE();
    }
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    void *ptr = malloc(size);
    if (ptr == nullptr) { // LOCV_EXCL_BR_LINE
        LOG_ECMA_MEM(FATAL) << "malloc failed, current alloc size = " << size
                            << ", total allocated size = " << nativeMemoryUsage_.load(std::memory_order_relaxed);
        UNREACHABLE();
    }
#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(ptr, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    IncreaseNativeMemoryUsage(MallocUsableSize(ptr));
    return ptr;
}

void NativeAreaAllocator::FreeBuffer(void *mem)
{
    if (mem == nullptr) {
        return;
    }
    size_t size = MallocUsableSize(mem);
    DecreaseNativeMemoryUsage(size);

#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(mem, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    free(mem);
}

void *NativeAreaAllocator::NativeAreaPageMap(size_t size)
{
    if (size == 0) { // LOCV_EXCL_BR_LINE
        LOG_ECMA_MEM(FATAL) << "size must have a size bigger than 0";
        UNREACHABLE();
    }
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    size = AlignUp(size, PageSize());
    void *ptr = PageMap(size, PAGE_PROT_READWRITE).GetMem();
    PageTag(ptr, size, PageTagType::METHOD_LITERAL);
#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(ptr, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    IncreaseNativeMemoryUsage(size);
    return ptr;
}

void NativeAreaAllocator::NativeAreaPageUnmap(void *mem, size_t size)
{
    if (mem == nullptr) {
        return;
    }
    size = AlignUp(size, PageSize());
    DecreaseNativeMemoryUsage(size);

#if ECMASCRIPT_ENABLE_ZAP_MEM
    if (memset_s(mem, size, INVALID_VALUE, size) != EOK) { // LOCV_EXCL_BR_LINE
        LOG_FULL(FATAL) << "memset_s failed";
        UNREACHABLE();
    }
#endif
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    PageClearTag(mem, size);
    PageUnmap(MemMap(mem, size));
}

void NativeAreaAllocator::FreeBufferFunc([[maybe_unused]] void *env, void *buffer, void *data)
{
    if (buffer == nullptr || data == nullptr) {
        return;
    }
    NativeAreaAllocator* allocator = reinterpret_cast<NativeAreaAllocator*>(data);
    allocator->FreeBuffer(buffer);
}
}  // namespace panda::ecmascript
