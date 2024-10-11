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

#ifndef LIBPANDABASE_UTILS_CFRAME_LAYOUT_H
#define LIBPANDABASE_UTILS_CFRAME_LAYOUT_H

#include "arch.h"

namespace panda {

enum FrameBridgeKind {
    INTERPRETER_TO_COMPILED_CODE = 1,
    COMPILED_CODE_TO_INTERPRETER = 2,
    BYPASS = 3,
};

template <ssize_t START, ssize_t SIZE>
class StackRegion {
public:
    static_assert(SIZE >= 0);
    template <size_t SZ>
    using NextStackRegion = StackRegion<START + SIZE, SZ>;

    using NextStackSlot = StackRegion<START + SIZE, 1>;

    static constexpr ssize_t Start()
    {
        return START;
    }
    static constexpr ssize_t End()
    {
        return Start() + GetSize();
    }
    static constexpr ssize_t GetSize()
    {
        return SIZE;
    }
    template <class CFrameLayoutT>
    static constexpr ssize_t GetOffsetFromSpInSlots(const CFrameLayoutT &fl)
    {
        return fl.template GetFrameSize<CFrameLayoutT::SLOTS>() - START - 2U;
    }
    template <class CFrameLayoutT>
    static constexpr ssize_t GetOffsetFromSpInBytes(const CFrameLayoutT &fl)
    {
        return GetOffsetFromSpInSlots(fl) * fl.GetSlotSize();
    }

private:
};

class CFrameLayout {
public:
    constexpr CFrameLayout(Arch arch, size_t spills_count)
        : arch_(arch), spills_count_(AlignSpillCount(arch, spills_count))
    {
    }
    ~CFrameLayout() = default;
    DEFAULT_COPY_SEMANTIC(CFrameLayout);
    DEFAULT_MOVE_SEMANTIC(CFrameLayout);

    enum OffsetOrigin { SP, FP };
    enum OffsetUnit { BYTES, SLOTS };

    using StackArgSlot = StackRegion<-2, 1>;               // -2 slot
    using LrSlot = StackArgSlot::NextStackSlot;            // -1 slot
    using PrevFrameSlot = LrSlot::NextStackSlot;           //  0 slot
    using MethodSlot = PrevFrameSlot::NextStackSlot;       //  1 slot
    using FlagsSlot = MethodSlot::NextStackSlot;           //  2 slot
    using DataRegion = FlagsSlot::NextStackRegion<2>;      // [3..4] slots
    using LocalsRegion = DataRegion::NextStackRegion<4>;   // [5..8] slots
    using SlotsRegion = LocalsRegion::NextStackRegion<0>;  // [9...] slots
    using RegsRegion = SlotsRegion;

    static constexpr ssize_t HEADER_SIZE = FlagsSlot::End() - LrSlot::Start();

    enum class FrameKind : uint8_t { DEFAULT = 0, OSR = 1, NATIVE = 2, LAST = NATIVE };

    using ShouldDeoptimizeFlag = BitField<bool, 0, 1>;
    using HasFloatRegsFlag = ShouldDeoptimizeFlag::NextFlag;
    using FrameKindField = HasFloatRegsFlag::NextField<FrameKind, MinimumBitsToStore(FrameKind::LAST)>;

    // Current usage of the locals:
    //  [0..1] slots: internal spill slots for codegen
    //  [2..3] slots: fp and lr in osr mode
    // TODO(msherstennikov): need to make flexible machinery to handle locals
    static constexpr size_t LOCALS_START_SLOT = 5;
    static constexpr size_t STACK_START_SLOT = 9;
    static constexpr size_t CALLEE_REGS_START_SLOT = STACK_START_SLOT;

    // NB! This 4 static constants are for cframe_test and stack_walker_test
    // Use getters below in other code.
    static constexpr size_t CALLEE_FP_REGS_START_SLOT =
        CALLEE_REGS_START_SLOT + GetCalleeRegsCount(RUNTIME_ARCH, false);
    static constexpr size_t CALLER_REGS_START_SLOT = CALLEE_FP_REGS_START_SLOT + GetCalleeRegsCount(RUNTIME_ARCH, true);
    static constexpr size_t CALLER_FP_REGS_START_SLOT =
        CALLER_REGS_START_SLOT + GetCallerRegsCount(RUNTIME_ARCH, false);
    static constexpr size_t SPILLS_START_SLOT = CALLER_FP_REGS_START_SLOT + GetCallerRegsCount(RUNTIME_ARCH, true);

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr size_t GetCalleeRegsStartSlot() const
    {
        return STACK_START_SLOT;
    }

    constexpr size_t GetCalleeFpRegsStartSlot() const
    {
        return GetCalleeRegsStartSlot() + ::panda::GetCalleeRegsCount(GetArch(), false);
    }

    constexpr size_t GetCallerRegsStartSlot() const
    {
        return GetCalleeFpRegsStartSlot() + ::panda::GetCalleeRegsCount(GetArch(), true);
    }

    constexpr size_t GetCallerFpRegsStartSlot() const
    {
        return GetCallerRegsStartSlot() + ::panda::GetCallerRegsCount(GetArch(), false);
    }

    constexpr size_t GetSpillsStartSlot() const
    {
        return GetCallerFpRegsStartSlot() + ::panda::GetCallerRegsCount(GetArch(), true);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr ptrdiff_t GetStackArgsStartSlot() const
    {
        return StackArgSlot::Start();
    }

    constexpr Arch GetArch() const
    {
        return arch_;
    }

    template <OffsetUnit unit>
    constexpr size_t GetFrameSize() const
    {
        // +1 for LR slot
        size_t size_in_slots = STACK_START_SLOT + GetFirstSpillSlot() + spills_count_ + 1U;
        return unit == BYTES ? (size_in_slots * GetSlotSize()) : size_in_slots;
    }

    template <OffsetOrigin origin, OffsetUnit unit>
    constexpr ssize_t GetOffset(ssize_t slot) const
    {
        if constexpr (origin == SP) {  // NOLINT(readability-braces-around-statements)
            const auto OFFSET = GetFrameSize<SLOTS>() - slot - 2U;
            if constexpr (unit == BYTES) {  // NOLINT
                return OFFSET * GetSlotSize();
            }
            return OFFSET;
        } else {                            // NOLINT
            if constexpr (unit == BYTES) {  // NOLINT
                return slot * PointerSize(arch_);
            }
            return slot;
        }
    }

    template <OffsetOrigin origin, OffsetUnit unit>
    constexpr ssize_t GetMethodOffset() const
    {
        return GetOffset<origin, unit>(MethodSlot::Start());
    }

    template <OffsetOrigin origin, OffsetUnit unit>
    constexpr ssize_t GetReturnAddressOffset() const
    {
        return GetOffset<origin, unit>(LrSlot::Start());
    }

    template <OffsetOrigin origin, OffsetUnit unit>
    constexpr ssize_t GetFreeSlotOffset() const
    {
        return GetOffset<origin, unit>(LOCALS_START_SLOT);
    }

    template <OffsetOrigin origin, OffsetUnit unit>
    constexpr ssize_t GetSpillOffset(size_t spill_slot) const
    {
        size_t shift = Is64BitsArch(arch_) ? 0 : 1;  // in arm32 one slot is 2 word and shifted by 1
        return GetOffset<origin, unit>(STACK_START_SLOT + GetFirstSpillSlot() + (spill_slot << shift) + shift);
    }

    constexpr ssize_t GetSpillOffsetFromSpInBytes(size_t spill_slot) const
    {
        return GetSpillOffset<CFrameLayout::SP, CFrameLayout::BYTES>(spill_slot);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    static constexpr ssize_t GetStackStartSlot()
    {
        return STACK_START_SLOT;
    }

    constexpr size_t GetCalleeSlotsCount() const
    {
        return GetCalleeRegistersCount(false) + GetCalleeRegistersCount(true);
    }

    constexpr size_t GetCallerSlotsCount() const
    {
        return GetCallerRegistersCount(false) + GetCallerRegistersCount(true);
    }

    constexpr size_t GetFirstSpillSlot() const
    {
        return GetCalleeSlotsCount() + GetCallerSlotsCount();
    }
    constexpr size_t GetLastSpillSlot() const
    {
        return GetFirstSpillSlot() + spills_count_ - 1;
    }

    constexpr size_t GetCalleeFirstSlot(bool is_fp) const
    {
        return is_fp ? GetCalleeRegistersCount(false) : 0;
    }

    constexpr size_t GetCalleeLastSlot(bool is_fp) const
    {
        return GetCalleeFirstSlot(is_fp) + GetCalleeRegistersCount(is_fp) - 1;
    }

    constexpr size_t GetCallerFirstSlot(bool is_fp) const
    {
        return GetCalleeLastSlot(true) + 1 + (is_fp ? GetCallerRegistersCount(false) : 0);
    }

    constexpr size_t GetCallerLastSlot(bool is_fp) const
    {
        return GetCallerFirstSlot(is_fp) + GetCallerRegistersCount(is_fp) - 1;
    }

    constexpr size_t GetCalleeRegistersCount(bool is_fp) const
    {
        return panda::GetCalleeRegsCount(arch_, is_fp);
    }

    constexpr size_t GetCallerRegistersCount(bool is_fp) const
    {
        return panda::GetCallerRegsCount(arch_, is_fp);
    }

    constexpr size_t GetSlotSize() const
    {
        return PointerSize(arch_);
    }

    constexpr size_t GetSpillsCount() const
    {
        return spills_count_;
    }

    constexpr size_t GetRegsSlotsCount() const
    {
        return GetCalleeSlotsCount() + GetCallerSlotsCount() + GetSpillsCount();
    }

    static constexpr size_t GetLocalsCount()
    {
        return STACK_START_SLOT - LOCALS_START_SLOT;
    }

    static constexpr ssize_t GetOsrFpLrSlot()
    {
        return LocalsRegion::Start() + static_cast<ssize_t>(GetLocalsCount()) - 1;
    }

    constexpr ssize_t GetOsrFpLrOffset() const
    {
        return GetOffset<CFrameLayout::FP, CFrameLayout::BYTES>(GetOsrFpLrSlot());
    }

    static constexpr size_t GetFpLrSlotsCount()
    {
        static_assert(MethodSlot::Start() > LrSlot::Start());
        return MethodSlot::Start() - LrSlot::Start();
    }

    static constexpr size_t GetTopToRegsSlotsCount()
    {
        static_assert(SlotsRegion::Start() > LrSlot::Start());
        return SlotsRegion::Start() - LrSlot::Start();
    }

private:
    constexpr size_t AlignSpillCount(Arch arch, size_t spills_count)
    {
        // Allign by odd-number, because GetSpillsStartSlot begins from fp (+1 slot for lr)
        if (arch == Arch::AARCH64 || arch == Arch::X86_64) {
            // 2 is to calculate the spills_count on the AARCH32 processor
            if (((GetSpillsStartSlot() + spills_count) % 2) == 0) {
                spills_count++;
            }
        } else if (arch == Arch::AARCH32) {
            // Additional slot for spill/fill <-> sf-registers ldrd miscorp
            // 2 is to calculate the spills_count on the AARCH32 processor
            spills_count = (spills_count + 1) * 2;
            if (((GetSpillsStartSlot() + spills_count) % 2) == 0) {
                spills_count++;
            }
        }
        return spills_count;
    }

private:
    Arch arch_;
    size_t spills_count_ {0};
};

static_assert(CFrameLayout::GetLocalsCount() >= 2U);
static_assert(CFrameLayout::GetFpLrSlotsCount() == 2U);

enum class FrameKind { NONE, INTERPRETER, COMPILER };

template <FrameKind kind>
struct BoundaryFrame;

template <>
struct BoundaryFrame<FrameKind::INTERPRETER> {
    static constexpr ssize_t METHOD_OFFSET = 1;
    static constexpr ssize_t FP_OFFSET = 0;
    static constexpr ssize_t RETURN_OFFSET = 2;
    static constexpr ssize_t CALLEES_OFFSET = -1;
};

template <>
struct BoundaryFrame<FrameKind::COMPILER> {
    static constexpr ssize_t METHOD_OFFSET = -1;
    static constexpr ssize_t FP_OFFSET = 0;
    static constexpr ssize_t RETURN_OFFSET = 1;
    static constexpr ssize_t CALLEES_OFFSET = -2;
};

using CFrameReturnAddr = CFrameLayout::LrSlot;
using CFramePrevFrame = CFrameLayout::PrevFrameSlot;
using CFrameMethod = CFrameLayout::MethodSlot;
using CFrameFlags = CFrameLayout::FlagsSlot;
using CFrameData = CFrameLayout::DataRegion;
using CFrameLocals = CFrameLayout::LocalsRegion;
using CFrameSlots = CFrameLayout::SlotsRegion;
using CFrameRegs = CFrameLayout::RegsRegion;

}  // namespace panda

#endif  // LIBPANDABASE_UTILS_CFRAME_LAYOUT_H
