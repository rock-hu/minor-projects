/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "libpandabase/os/mem.h"
#include "plugins/ets/runtime/interop_js/ets_proxy/mem/items_pool.h"

namespace ark::ets::interop::js::ets_proxy::testing {

class ItemsPoolTest {
    static constexpr size_t ARRAY_SIZE = 2045;
    static constexpr size_t PADDED_ITEM_SIZE = 2048;

public:
    using Item = std::array<char, ARRAY_SIZE>;
    using Pool = ItemsPool<Item, 3U>;
    using PaddedItem = Pool::PaddedItem;
    static constexpr size_t MAX_INDEX = (1U << 3U);
    static_assert(sizeof(PaddedItem) == PADDED_ITEM_SIZE);

    static PaddedItem *GetData(const std::unique_ptr<Pool> &pool)
    {
        return pool->data_;
    }

    static PaddedItem *&GetCurrentPos(const std::unique_ptr<Pool> &pool)
    {
        return pool->currentPos_;
    }

    static PaddedItem *&GetFreeList(const std::unique_ptr<Pool> &pool)
    {
        return pool->freeList_;
    }

    static std::unique_ptr<Pool> CreatePool()
    {
        size_t size = Pool::MAX_POOL_SIZE;

        void *data = os::mem::MapRWAnonymousRaw(size);
        if (data == nullptr) {
            std::cerr << "Cannot allocate Pool, size=" << std::hex << size << std::dec << std::endl;
            std::abort();
        }
        return std::make_unique<Pool>(data, size);
    }
};

template <typename T>
static ItemsPoolTest::PaddedItem *GetData(const T &pool)
{
    return ItemsPoolTest::GetData(pool);
}

template <typename T>
static ItemsPoolTest::PaddedItem *&GetCurrentPos(const T &pool)
{
    return ItemsPoolTest::GetCurrentPos(pool);
}

template <typename T>
static ItemsPoolTest::PaddedItem *&GetFreeList(const T &pool)
{
    return ItemsPoolTest::GetFreeList(pool);
}

class ItemsPoolGTest : public ::testing::Test {};

TEST_F(ItemsPoolGTest, test_1)
{
    auto pool = ItemsPoolTest::CreatePool();
    auto data = GetData(pool);
    ASSERT_NE(data, nullptr);

    auto &freeList = GetFreeList(pool);
    ASSERT_EQ(freeList, nullptr);

    auto &currentPos = GetCurrentPos(pool);
    ASSERT_EQ(currentPos, data);
}

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
TEST_F(ItemsPoolGTest, test_2)
{
    // clang-format off
// |  INDEX  | Step1 | Step2 | Step3 | Step4 | Step5 | Step6 | Step7 | Step8 | Step9 | Step10| Step11| Step12| Step13| Step14| Step15| Step16| Step17| Step18| Step19|
// +=========+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+
// |   000   | item0 | item0 | item0 | item0 | item0 | item0 | item0 | item0 | item0 | item0 | item0 | item0 |       |       |       |       |       | item13| item13|
// |   001   |       | item1 | item1 |       | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 | item3 |
// |   010   |       |       | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 | item2 |
// |   011   |       |       |       |       |       | item4 | item4 | item4 | item4 | item4 | item4 | item4 | item4 | item4 |       | item11| item11| item11| item11|
// |   100   |       |       |       |       |       |       | item5 | item5 | item5 | item5 | item5 | item5 | item5 |       |       |       | item12| item12| item12|
// |   101   |       |       |       |       |       |       |       | item6 | item6 | item6 | item6 | item6 | item6 | item6 | item6 | item6 | item6 | item6 | item6 |
// |   110   |       |       |       |       |       |       |       |       | item7 | item7 | item7 | item7 | item7 | item7 | item7 | item7 | item7 | item7 | item7 |
// |   111   |       |       |       |       |       |       |       |       |       | item8 | item8 | item8 | item8 | item8 | item8 | item8 | item8 | item8 | item8 |
// +=========+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+=======+
// |free_list|       |       |       |       |       |       |       |       |       |       |       |       |   0   |   4   |   3   |   4   |   0   |       |       |
// |         |       |       |       |       |       |       |       |       |       |       |       |       |       |   0   |   4   |   0   |       |       |       |
// |         |       |       |       |       |       |       |       |       |       |       |       |       |       |       |   0   |       |       |       |       |
    // clang-format on

    auto pool = ItemsPoolTest::CreatePool();
    auto data = GetData(pool);
    auto &currentPos = GetCurrentPos(pool);
    auto &freeList = GetFreeList(pool);

    ASSERT_NE(data, nullptr);
    ASSERT_EQ(currentPos, data);
    ASSERT_EQ(freeList, nullptr);

    // Step1, Alloc
    auto item0 = pool->AllocItem();
    ASSERT_NE(item0, nullptr);
    ASSERT_EQ(item0, pool->GetItemByIndex(0));
    ASSERT_EQ(pool->GetIndexByItem(item0), 0b000);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 1);

    // Step2, Alloc
    auto item1 = pool->AllocItem();
    ASSERT_NE(item1, nullptr);
    ASSERT_EQ(item1, pool->GetItemByIndex(1));
    ASSERT_EQ(pool->GetIndexByItem(item1), 0b001);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 2U);

    // Step3, Alloc
    auto item2 = pool->AllocItem();
    ASSERT_NE(item2, nullptr);
    ASSERT_EQ(item2, pool->GetItemByIndex(2U));
    ASSERT_EQ(pool->GetIndexByItem(item2), 0b010);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 3U);

    // Step4, Free
    pool->FreeItem(item1);
    ASSERT_EQ(freeList, data + 1);
    ASSERT_EQ(currentPos, data + 3U);

    // Step5, Alloc
    auto item3 = pool->AllocItem();
    ASSERT_NE(item3, nullptr);
    ASSERT_EQ(item3, pool->GetItemByIndex(1));
    ASSERT_EQ(pool->GetIndexByItem(item3), 0b001);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 3U);

    // Step6, Alloc
    auto item4 = pool->AllocItem();
    ASSERT_NE(item4, nullptr);
    ASSERT_EQ(item4, pool->GetItemByIndex(3U));
    ASSERT_EQ(pool->GetIndexByItem(item4), 0b011);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 4U);

    // Step7, Alloc
    auto item5 = pool->AllocItem();
    ASSERT_NE(item5, nullptr);
    ASSERT_EQ(item5, pool->GetItemByIndex(4U));
    ASSERT_EQ(pool->GetIndexByItem(item5), 0b100);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 5U);

    // Step8, Alloc
    auto item6 = pool->AllocItem();
    ASSERT_NE(item6, nullptr);
    ASSERT_EQ(item6, pool->GetItemByIndex(5U));
    ASSERT_EQ(pool->GetIndexByItem(item6), 0b101);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 6U);

    // Step9, Alloc
    auto item7 = pool->AllocItem();
    ASSERT_NE(item7, nullptr);
    ASSERT_EQ(item7, pool->GetItemByIndex(6U));
    ASSERT_EQ(pool->GetIndexByItem(item7), 0b110);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 7U);

    // Step10, Alloc
    auto item8 = pool->AllocItem();
    ASSERT_NE(item8, nullptr);
    ASSERT_EQ(item8, pool->GetItemByIndex(7U));
    ASSERT_EQ(pool->GetIndexByItem(item8), 0b111);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Step11, Alloc, out of memory
    auto item9 = pool->AllocItem();
    ASSERT_EQ(item9, nullptr);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Step12, Alloc, out of memory
    auto item10 = pool->AllocItem();
    ASSERT_EQ(item10, nullptr);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Step13, Free
    ASSERT_EQ(pool->GetIndexByItem(item0), 0b000);
    pool->FreeItem(item0);
    ASSERT_EQ(freeList, data + 0);
    ASSERT_EQ(currentPos, data + 8U);

    // Step14, Free
    ASSERT_EQ(pool->GetIndexByItem(item5), 0b100);
    pool->FreeItem(item5);
    ASSERT_EQ(freeList, data + 4U);
    ASSERT_EQ(currentPos, data + 8U);

    // Step15, Free
    ASSERT_EQ(pool->GetIndexByItem(item4), 0b011);
    pool->FreeItem(item4);
    ASSERT_EQ(freeList, data + 3U);
    ASSERT_EQ(currentPos, data + 8U);

    // Step16, Alloc
    auto item11 = pool->AllocItem();
    ASSERT_NE(item11, nullptr);
    ASSERT_EQ(item11, pool->GetItemByIndex(3U));
    ASSERT_EQ(pool->GetIndexByItem(item11), 0b011);
    ASSERT_EQ(freeList, data + 4U);
    ASSERT_EQ(currentPos, data + 8U);

    // Step17, Alloc
    auto item12 = pool->AllocItem();
    ASSERT_NE(item12, nullptr);
    ASSERT_EQ(item12, pool->GetItemByIndex(4U));
    ASSERT_EQ(pool->GetIndexByItem(item12), 0b100);
    ASSERT_EQ(freeList, data + 0);
    ASSERT_EQ(currentPos, data + 8U);

    // Step18, Alloc
    auto item13 = pool->AllocItem();
    ASSERT_NE(item13, nullptr);
    ASSERT_EQ(item13, pool->GetItemByIndex(0));
    ASSERT_EQ(pool->GetIndexByItem(item13), 0b000);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Step19, Alloc, out of memory
    auto item14 = pool->AllocItem();
    ASSERT_EQ(item14, nullptr);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Check allocated items possitions
    ASSERT_EQ(uintptr_t(item13), uintptr_t(data + 0U));
    ASSERT_EQ(uintptr_t(item3), uintptr_t(data + 1U));
    ASSERT_EQ(uintptr_t(item2), uintptr_t(data + 2U));
    ASSERT_EQ(uintptr_t(item11), uintptr_t(data + 3U));
    ASSERT_EQ(uintptr_t(item12), uintptr_t(data + 4U));
    ASSERT_EQ(uintptr_t(item6), uintptr_t(data + 5U));
    ASSERT_EQ(uintptr_t(item7), uintptr_t(data + 6U));
    ASSERT_EQ(uintptr_t(item8), uintptr_t(data + 7U));
}

TEST_F(ItemsPoolGTest, test_3)
{
    // clang-format off
// |  INDEX  | Step1 | Step2 | Step3 | Step4 |
// +=========+=======+=======+=======+=======+
// |   000   | item0 |       | item6 | item6 |
// |   001   | item1 | item1 | item1 | item1 |
// |   010   | item2 | item2 | item2 | item2 |
// |   011   | item3 |       | item5 | item5 |
// |   100   | item4 | item4 | item4 | item4 |
// |   101   |       |       | item7 | item7 |
// |   110   |       |       |       | item8 |
// |   111   |       |       |       | item9 |
// +=========+=======+=======+=======+=======+
// |free_list|       |   3   |       |       |
// |         |       |   0   |       |       |
    // clang-format on

    auto pool = ItemsPoolTest::CreatePool();
    auto data = GetData(pool);
    auto &currentPos = GetCurrentPos(pool);
    auto &freeList = GetFreeList(pool);

    ASSERT_NE(data, nullptr);
    ASSERT_EQ(currentPos, data + 0);
    ASSERT_EQ(freeList, nullptr);

    // Step1, Alloc 5 items
    auto item0 = pool->AllocItem();
    ASSERT_NE(item0, nullptr);
    ASSERT_EQ(item0, pool->GetItemByIndex(0));
    ASSERT_EQ(pool->GetIndexByItem(item0), 0b000);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 1);

    auto item1 = pool->AllocItem();
    ASSERT_NE(item1, nullptr);
    ASSERT_EQ(item1, pool->GetItemByIndex(1));
    ASSERT_EQ(pool->GetIndexByItem(item1), 0b001);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 2U);

    auto item2 = pool->AllocItem();
    ASSERT_NE(item2, nullptr);
    ASSERT_EQ(item2, pool->GetItemByIndex(2U));
    ASSERT_EQ(pool->GetIndexByItem(item2), 0b010);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 3U);

    auto item3 = pool->AllocItem();
    ASSERT_NE(item3, nullptr);
    ASSERT_EQ(item3, pool->GetItemByIndex(3U));
    ASSERT_EQ(pool->GetIndexByItem(item3), 0b011);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 4U);

    auto item4 = pool->AllocItem();
    ASSERT_NE(item4, nullptr);
    ASSERT_EQ(item4, pool->GetItemByIndex(4U));
    ASSERT_EQ(pool->GetIndexByItem(item4), 0b100);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 5U);

    // Step2, Free item0 and item3
    ASSERT_EQ(pool->GetIndexByItem(item0), 0b000);
    pool->FreeItem(item0);
    ASSERT_EQ(freeList, data + 0);
    ASSERT_EQ(currentPos, data + 5U);

    ASSERT_EQ(pool->GetIndexByItem(item3), 0b011);
    pool->FreeItem(item3);
    ASSERT_EQ(freeList, data + 3U);
    ASSERT_EQ(currentPos, data + 5U);

    // Step3, Alloc 3 items
    auto item5 = pool->AllocItem();
    ASSERT_NE(item5, nullptr);
    ASSERT_EQ(item5, pool->GetItemByIndex(3U));
    ASSERT_EQ(pool->GetIndexByItem(item5), 0b011);
    ASSERT_EQ(freeList, data + 0);
    ASSERT_EQ(currentPos, data + 5U);

    auto item6 = pool->AllocItem();
    ASSERT_NE(item6, nullptr);
    ASSERT_EQ(item6, pool->GetItemByIndex(0));
    ASSERT_EQ(pool->GetIndexByItem(item6), 0b000);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 5U);

    auto item7 = pool->AllocItem();
    ASSERT_NE(item7, nullptr);
    ASSERT_EQ(item7, pool->GetItemByIndex(5U));
    ASSERT_EQ(pool->GetIndexByItem(item7), 0b101);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 6U);

    // Step4, Alloc 3 items
    auto item8 = pool->AllocItem();
    ASSERT_NE(item8, nullptr);
    ASSERT_EQ(item8, pool->GetItemByIndex(6U));
    ASSERT_EQ(pool->GetIndexByItem(item8), 0b110);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 7U);

    auto item9 = pool->AllocItem();
    ASSERT_NE(item9, nullptr);
    ASSERT_EQ(item9, pool->GetItemByIndex(7U));
    ASSERT_EQ(pool->GetIndexByItem(item9), 0b111);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    auto item10 = pool->AllocItem();
    ASSERT_EQ(item10, nullptr);
    ASSERT_EQ(freeList, nullptr);
    ASSERT_EQ(currentPos, data + 8U);

    // Check allocated items possitions
    ASSERT_EQ(uintptr_t(item6), uintptr_t(data + 0U));
    ASSERT_EQ(uintptr_t(item1), uintptr_t(data + 1U));
    ASSERT_EQ(uintptr_t(item2), uintptr_t(data + 2U));
    ASSERT_EQ(uintptr_t(item5), uintptr_t(data + 3U));
    ASSERT_EQ(uintptr_t(item4), uintptr_t(data + 4U));
    ASSERT_EQ(uintptr_t(item7), uintptr_t(data + 5U));
    ASSERT_EQ(uintptr_t(item8), uintptr_t(data + 6U));
    ASSERT_EQ(uintptr_t(item9), uintptr_t(data + 7U));
}

TEST_F(ItemsPoolGTest, test_IsValidItem)
{
    auto pool = ItemsPoolTest::CreatePool();
    auto data = GetData(pool);
    auto dataEnd = data + ItemsPoolTest::MAX_INDEX;

    auto item = pool->AllocItem();

    ASSERT_EQ(pool->IsValidItem(item), true);
    ASSERT_EQ(pool->IsValidItem(reinterpret_cast<decltype(item)>(data)), true);
    ASSERT_EQ(pool->IsValidItem(reinterpret_cast<decltype(item)>(dataEnd)), false);
    ASSERT_EQ(pool->IsValidItem(reinterpret_cast<decltype(item)>(data - 1)), false);
    ASSERT_EQ(pool->IsValidItem(reinterpret_cast<decltype(item)>(dataEnd - 1)), true);
}
// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace ark::ets::interop::js::ets_proxy::testing
