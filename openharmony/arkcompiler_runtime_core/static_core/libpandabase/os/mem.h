/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_LIBPANDABASE_PBASE_OS_MEM_H_
#define PANDA_LIBPANDABASE_PBASE_OS_MEM_H_

#include "file.h"
#include "macros.h"
#include "utils/expected.h"
#include "utils/span.h"
#include "libpandabase/mem/mem.h"

#include <cstddef>
#ifdef PANDA_TARGET_UNIX
#include "platforms/unix/libpandabase/unix_mem.h"
#elif PANDA_TARGET_WINDOWS
#include "platforms/windows/libpandabase/windows_mem.h"
#else
#error "Unsupported target: please provide mmap API"
#endif

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>

namespace ark::os::mem {

static constexpr uint64_t HIGH_BOUND_32BIT_ADDRESS = 4_GB;

PANDA_PUBLIC_API void MmapDeleter(std::byte *ptr, size_t size) noexcept;

/**
 * @brief Make memory region @param mem with size @param size with protection flags @param prot
 * @param mem  Pointer to memory region (should be aligned to page size)
 * @param size Size of memory region
 * @param prot Memory protection flags, a combination of MMAP_PROT_XXX values
 * @return Error object if any errors occur
 */
std::optional<Error> MakeMemWithProtFlag(void *mem, size_t size, int prot);

/**
 * @brief Make memory region @param mem with size @param size readable and executable
 * @param mem  Pointer to memory region (should be aligned to page size)
 * @param size Size of memory region
 * @return Error object if any errors occur
 */
std::optional<Error> MakeMemReadExec(void *mem, size_t size);

/**
 * @brief Make memory region @param mem with size @param size readable and writable
 * @param mem  Pointer to memory region (should be aligned to page size)
 * @param size Size of memory region
 * @return Error object if any errors occur
 */
PANDA_PUBLIC_API std::optional<Error> MakeMemReadWrite(void *mem, size_t size);

/**
 * @brief Make memory region @param mem with size @param size readable
 * @param mem  Pointer to memory region (should be aligned to page size)
 * @param size Size of memory region
 * @return Error object if any errors occur
 */
std::optional<Error> MakeMemReadOnly(void *mem, size_t size);

/**
 * @brief Make memory region @param mem with size @param size protected
 * @param mem  Pointer to memory region (should be aligned to page size)
 * @param size Size of memory region
 * @return Error object if any errors occur
 */
PANDA_PUBLIC_API std::optional<Error> MakeMemProtected(void *mem, size_t size);

/**
 * @brief Align addr @param addr to page size to pass it to MakeMem functions
 * @param addr Address to align
 * @return Aligned address
 */
PANDA_PUBLIC_API uintptr_t AlignDownToPageSize(uintptr_t addr);

/**
 * @brief Allocated aligned memory with alignment @param alignment_in_bytes and
 *  with size @param size. Use AlignedFree to free this memory.
 * @param alignment_in_bytes - alignment in bytes
 * @param size - min required size in bytes
 * @return
 */
PANDA_PUBLIC_API void *AlignedAlloc(size_t alignmentInBytes, size_t size);

/**
 * @brief Free memory, allocated by AlignedAlloc.
 * @param mem - Pointer to memory, allocated by AlignedAlloc
 */
PANDA_PUBLIC_API void AlignedFree(void *mem);

template <class T>
class MapRange {
public:
    MapRange(T *ptr, size_t size) : sp_(reinterpret_cast<std::byte *>(ptr), size) {}

    MapRange GetSubRange(size_t offset, size_t size)
    {
        return MapRange(sp_.SubSpan(offset, size));
    }

    Expected<const std::byte *, Error> MakeReadExec()
    {
        auto res = MakeMemReadExec(sp_.Data(), sp_.Size());
        if (res) {
            return Unexpected(res.value());
        }

        return sp_.Data();
    }

    Expected<const std::byte *, Error> MakeReadOnly()
    {
        auto res = MakeMemReadOnly(sp_.Data(), sp_.Size());
        if (res) {
            return Unexpected(res.value());
        }

        return sp_.Data();
    }

    Expected<std::byte *, Error> MakeReadWrite()
    {
        auto res = MakeMemReadWrite(sp_.Data(), sp_.Size());
        if (res) {
            return Unexpected(res.value());
        }

        return sp_.Data();
    }

    MapRange<T> Align() const
    {
        auto unaligned = reinterpret_cast<uintptr_t>(sp_.Data());
        auto aligned = AlignDownToPageSize(unaligned);
        Span<std::byte> sp(reinterpret_cast<std::byte *>(aligned), sp_.Size() + unaligned - aligned);
        return MapRange<T>(sp);
    }

    size_t GetSize() const
    {
        return sp_.Size();
    }

    std::byte *GetData()
    {
        return sp_.Data();
    }

    virtual ~MapRange() = default;

    DEFAULT_COPY_SEMANTIC(MapRange);
    NO_MOVE_SEMANTIC(MapRange);

private:
    explicit MapRange(const Span<std::byte> &sp) : sp_(sp) {}

    Span<std::byte> sp_;
};

enum class MapPtrType { CONST, NON_CONST };

template <class T, MapPtrType TYPE>
class MapPtr {
public:
    using Deleter = void (*)(T *, size_t) noexcept;

    MapPtr(T *ptr, size_t size, Deleter deleter) : ptr_(ptr), size_(size), pageOffset_(0), deleter_(deleter) {}
    MapPtr(T *ptr, size_t size, size_t pageOffset, Deleter deleter)
        : ptr_(ptr), size_(size), pageOffset_(pageOffset), deleter_(deleter)
    {
    }

    MapPtr(MapPtr &&other) noexcept
    {
        ptr_ = other.ptr_;
        pageOffset_ = other.pageOffset_;
        size_ = other.size_;
        deleter_ = other.deleter_;
        other.ptr_ = nullptr;
        other.deleter_ = nullptr;
    }

    MapPtr &operator=(MapPtr &&other) noexcept
    {
        ptr_ = other.ptr_;
        pageOffset_ = other.pageOffset_;
        size_ = other.size_;
        deleter_ = other.deleter_;
        other.ptr_ = nullptr;
        other.deleter_ = nullptr;
        return *this;
    }

    std::conditional_t<TYPE == MapPtrType::CONST, const T *, T *> Get() const
    {
        return ptr_;
    }

    size_t GetSize() const
    {
        return size_;
    }

    MapRange<T> GetMapRange() const
    {
        return MapRange(ptr_, size_);
    }

    MapRange<T> GetMapRange()
    {
        return MapRange(ptr_, size_);
    }

    static constexpr uint32_t GetPtrOffset()
    {
        return MEMBER_OFFSET(MapPtr, ptr_);
    }

    MapPtr<T, MapPtrType::CONST> ToConst()
    {
        MapPtr<T, MapPtrType::CONST> res(ptr_, size_, pageOffset_, deleter_);
        ptr_ = nullptr;
        return res;
    }

    /*
     * memory layout for mmap
     *
     *             addr(is )
     *              ^
     *          page_offset_ |   size_
     *              |--------|-----------|
     *  P0          P1       |  P2       |  P3          P4
     *  |           |        |  |        |  |           |   4 pages
     *  +-----------+--------S--+--------E--+-----------+
     *                       ^
     *                       |
     *                      ptr_
     *              |--------------------| mmap memory
     *                       size
     *
     * S: file start; E: file end
     * Available space: [ptr_...(ptr_ + size_ - 1)]
     * addr sould be page aligned for file map but it is not guaranteed for anonymous map
     * For anonymous map, page_offset_ = 0
     */
    ~MapPtr()
    {
        if (ptr_ == nullptr) {
            return;
        }
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr_) - pageOffset_;
        // LINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        size_t size = size_ + pageOffset_;
        deleter_(reinterpret_cast<T *>(addr), size);
    }

private:
    T *ptr_;
    size_t size_;
    size_t pageOffset_;
    Deleter deleter_;

    NO_COPY_SEMANTIC(MapPtr);
};

using ByteMapRange = MapRange<std::byte>;
using BytePtr = MapPtr<std::byte, MapPtrType::NON_CONST>;
using ConstBytePtr = MapPtr<std::byte, MapPtrType::CONST>;
static_assert(ConstBytePtr::GetPtrOffset() == 0);

/**
 * Map the specified file into memory.
 * The interface is similat to POSIX mmap.
 *
 * @param file - file to map
 * @param prot - memory protection flags, a combination of MMAP_PROT_XXX values.
 * @param flags - memory map flags, a combination of MMAP_FLAG_XXX values.
 * @param file_offset - an offset in the file. If the offset is not multiple of page size
 * the function handles this situatio. The resulting BytePtr will points to desired data.
 * @param hint - an desired address to map file to.
 */
PANDA_PUBLIC_API BytePtr MapFile(file::File file, uint32_t prot, uint32_t flags, size_t size, size_t fileOffset = 0,
                                 void *hint = nullptr);

/**
 * @brief allocates executed memory of size @param size
 * @param size
 * @return
 */
BytePtr MapExecuted(size_t size);

/**
 * Anonymous mmap with READ | WRITE protection for pages
 * Note: returned memory will be poisoned in ASAN targets,
 * if you need other behavior - consider to change interface, or use manual unpoisoning.
 * @param size - size in bytes, should be multiple of PAGE_SIZE
 * @param force_poison - poison mmaped memory
 * @return
 */
PANDA_PUBLIC_API void *MapRWAnonymousRaw(size_t size, bool forcePoison = true);

/**
 * Anonymous mmap with READ | WRITE protection for pages.
 * Returned address will be aligned as @param aligment_in_bytes.
 * Note: returned memory will be poisoned in ASAN targets,
 * if you need other behavior - consider to change interface, or use manual unpoisoning.
 * @param size - size in bytes, should be multiple of PAGE_SIZE
 * @param aligment_in_bytes - alignment in bytes, should be multiple of PAGE_SIZE
 * @param force_poison - poison mmaped memory
 * @return
 */
PANDA_PUBLIC_API void *MapRWAnonymousWithAlignmentRaw(size_t size, size_t aligmentInBytes, bool forcePoison = true);

// ASAN mapped its structures at this magic address (shadow offset):
// see https://github.com/google/sanitizers/wiki/AddressSanitizerAlgorithm
// Therefore, we can successfully allocate memory at fixed address started somewhere at lower addresses
// and it can overlap sanitizer address space and mmap with MAP_FIXED flag finished successfully.
// (one can look at the MAP_FIXED flag description of Linux mmap)
// However, all load/store from this memory is prohibited.
// We can get an error during mmap call only if we use MAP_FIXED_NOREPLACE argument,
// but it is supported only since Linux 4.17 (Ubuntu 18 has 4.15)
// NOTE(aemelenko): Do smth with this constant.
#ifdef PANDA_TARGET_ARM64
static constexpr uint64_t MMAP_FIXED_MAGIC_ADDR_FOR_SANITIZERS = 1ULL << 36ULL;
#else
static constexpr uint64_t MMAP_FIXED_MAGIC_ADDR_FOR_SANITIZERS = 0x7fff8000ULL;
#endif

/**
 * Anonymous mmap in first 4GB address space with READ | WRITE protection for pages
 *
 * Try iterative mmap memory from min_mem to 4GB with iterative_step step
 *
 * @param min_mem minimal address for mmap attemps in first 4GB, should be multiple of PAGE_SIZE
 * @param size size in bytes, should be multiple of PAGE_SIZE
 * @param iterative_step size of step for mmap iteration, should be multiple of PAGE_SIZE
 *
 * @return pointer to the mapped area, nullptr if could't map
 *
 * @note returned memory will be poisoned in ASAN targets,
 * if you need other behavior - consider to change interface, or use manual unpoisoning
 */
void *MapRWAnonymousInFirst4GB(void *minMem, size_t size, size_t iterativeStep = 4_KB);

/**
 * Anonymous mmap with fixed address and READ | WRITE protection for pages
 * Note: returned memory will be poisoned in ASAN targets,
 * if you need other behavior - consider to change interface, or use manual unpoisoning.
 * @param mem used address
 * @param size size in bytes, should be multiple of PAGE_SIZE
 * @param force_poison poison mmaped memory
 * @return pointer to the mapped area
 */
void *MapRWAnonymousFixedRaw(void *mem, size_t size, bool forcePoison = true);

/**
 * Unmap previously mapped memory.
 * Note: memory will be unpoisoned before unmapping in ASAN targets.
 * @param mem - pointer to the memory
 * @param size - size of memory to unmap
 * @return Error object if any error occur
 */
PANDA_PUBLIC_API std::optional<Error> UnmapRaw(void *mem, size_t size);

/**
 * Unmap part of previously mapped memory.
 * Note: memory will be unpoisoned before unmapping in ASAN targets.
 * @param mem - pointer to the memory
 * @param size - size of memory piece which we want to unmap
 * @return Error object if any error occur
 */
std::optional<Error> PartiallyUnmapRaw(void *mem, size_t size);

/**
 * @brief returns page size for the system
 * @return
 */
PANDA_PUBLIC_API uint32_t GetPageSize();

/**
 * @brief returns cache line size for the system
 * @return
 */
PANDA_PUBLIC_API size_t GetCacheLineSize();

/**
 * Release pages [pages_start, pages_end] to os.
 * @param pages_start - address of pages beginning, should be multiple of PAGE_SIZE
 * @param pages_end - address of pages ending, should be multiple of PAGE_SIZE
 * @return
 */
inline int ReleasePages([[maybe_unused]] uintptr_t pagesStart, [[maybe_unused]] uintptr_t pagesEnd)
{
    ASSERT(pagesStart % os::mem::GetPageSize() == 0);
    ASSERT(pagesEnd % os::mem::GetPageSize() == 0);
    ASSERT(pagesEnd >= pagesStart);
#ifdef PANDA_TARGET_UNIX
    return madvise(ToVoidPtr(pagesStart), pagesEnd - pagesStart, MADV_DONTNEED);
#else
    // On Windows systems we can do nothing
    return 0;
#endif
}

/**
 * Tag anonymous memory with a debug name.
 * @param mem - pointer to the memory
 * @param size - size of memory to tag
 * @param tag - pointer to the debug name (must be a literal or heap object)
 * @return Error object if any error occur
 */
PANDA_PUBLIC_API std::optional<Error> TagAnonymousMemory(const void *mem, size_t size, const char *tag);

static constexpr size_t DEFAULT_NATIVE_BYTES_FROM_MALLINFO = 100000;

PANDA_PUBLIC_API size_t GetNativeBytesFromMallinfo();

#ifdef PANDA_TARGET_WINDOWS
PANDA_PUBLIC_API void *mmap([[maybe_unused]] void *addr, size_t len, uint32_t prot, int flags, int fildes, off_t off);
PANDA_PUBLIC_API int munmap(void *addr, [[maybe_unused]] size_t len);
#endif

}  // namespace ark::os::mem

#endif  // PANDA_LIBPANDABASE_PBASE_OS_MEM_H_
