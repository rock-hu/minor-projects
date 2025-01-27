/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_MEM_SPACE_INL_H
#define ECMASCRIPT_MEM_SPACE_INL_H

#include "ecmascript/mem/space.h"

namespace panda::ecmascript {
void Space::AddRegion(Region *region)
{
    ASSERT(region != nullptr);
    LOG_ECMA_MEM(DEBUG) << "Add region:" << region << " to " << ToSpaceTypeName(spaceType_);
    regionList_.AddNode(region);
    IncreaseCommitted(region->GetCapacity());
    IncreaseObjectSize(region->GetSize());
}

void Space::RemoveRegion(Region *region)
{
    LOG_ECMA_MEM(DEBUG) << "Remove region:" << region << " to " << ToSpaceTypeName(spaceType_);
    regionList_.RemoveNode(region);
    DecreaseCommitted(region->GetCapacity());
    DecreaseObjectSize(region->GetSize());
}

template<class Callback>
void Space::EnumerateRegions(const Callback &cb, Region *end) const
{
    Region *current = regionList_.GetFirst();
    if (current == nullptr) {
        return;
    }
    if (end == nullptr) {
        end = regionList_.GetLast();
    }
    while (current != end) {
        auto next = current->GetNext();
        cb(current);
        current = next;
    }

    if (current != nullptr) {
        cb(current);
    }
}

template<class Callback>
void Space::EnumerateRegionsWithRecord(const Callback &cb) const
{
    if (recordRegion_ != nullptr) {
        EnumerateRegions(cb, recordRegion_);
    }
}

RegionSpaceFlag Space::GetRegionFlag() const
{
    RegionSpaceFlag flags = RegionSpaceFlag::UNINITIALIZED;
    switch (spaceType_) {
        case MemSpaceType::OLD_SPACE:
        case MemSpaceType::LOCAL_SPACE:
            flags = RegionSpaceFlag::IN_OLD_SPACE;
            break;
        case MemSpaceType::SEMI_SPACE:
            flags = RegionSpaceFlag::IN_YOUNG_SPACE;
            break;
        case MemSpaceType::HUGE_OBJECT_SPACE:
            flags = RegionSpaceFlag::IN_HUGE_OBJECT_SPACE;
            break;
        case MemSpaceType::MACHINE_CODE_SPACE:
            flags = RegionSpaceFlag::IN_MACHINE_CODE_SPACE;
            break;
        case MemSpaceType::HUGE_MACHINE_CODE_SPACE:
            flags = RegionSpaceFlag::IN_HUGE_MACHINE_CODE_SPACE;
            break;
        case MemSpaceType::NON_MOVABLE:
            flags = RegionSpaceFlag::IN_NON_MOVABLE_SPACE;
            break;
        case MemSpaceType::SNAPSHOT_SPACE:
            flags = RegionSpaceFlag::IN_SNAPSHOT_SPACE;
            break;
        case MemSpaceType::READ_ONLY_SPACE:
            flags = RegionSpaceFlag::IN_READ_ONLY_SPACE;
            break;
        case MemSpaceType::APPSPAWN_SPACE:
            flags = RegionSpaceFlag::IN_APPSPAWN_SPACE;
            break;
        case MemSpaceType::SHARED_APPSPAWN_SPACE:
            flags = RegionSpaceFlag::IN_SHARED_APPSPAWN_SPACE;
            break;
        case MemSpaceType::SHARED_NON_MOVABLE:
            flags = RegionSpaceFlag::IN_SHARED_NON_MOVABLE;
            break;
        case MemSpaceType::SHARED_OLD_SPACE:
        case MemSpaceType::SHARED_LOCAL_SPACE:
        case MemSpaceType::SHARED_COMPRESS_SPACE:
            flags = RegionSpaceFlag::IN_SHARED_OLD_SPACE;
            break;
        case MemSpaceType::SHARED_READ_ONLY_SPACE:
            flags = RegionSpaceFlag::IN_SHARED_READ_ONLY_SPACE;
            break;
        case MemSpaceType::SHARED_HUGE_OBJECT_SPACE:
            flags = RegionSpaceFlag::IN_SHARED_HUGE_OBJECT_SPACE;
            break;
        default:
            LOG_ECMA(FATAL) << "this branch is unreachable";
            UNREACHABLE();
            break;
    }
    return flags;
}
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_MEM_SPACE_INL_H
