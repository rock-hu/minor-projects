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


#include "ecmascript/compiler/assembler/aarch64/assembler_aarch64.h"


namespace panda::ecmascript::aarch64 {
using namespace panda::ecmascript::base;
static const uint64_t HWORD_MASK = 0xFFFF;

LogicalImmediate LogicalImmediate::Create(uint64_t imm, int width)
{
    if ((imm == 0ULL) || (imm == ~0ULL) ||
        ((width != RegXSize) && (((imm >> width) != 0) || (imm == (~0ULL >> (RegXSize - width)))))) {
        return LogicalImmediate(InvalidLogicalImmediate);
    }

    // First, determine the element size.
    unsigned int size = static_cast<uint32_t>(width);
    do {
        size /= 2; // 2: Divide by 2
        uint64_t mask = (1ULL << size) - 1;

        if ((imm & mask) != ((imm >> size) & mask)) {
            size *= 2; // 2: Multiply by 2
            break;
        }
    } while (size > 2); // 2: Greater than 2

    // Second, determine the rotation to make the element be: 0^m 1^n.
    unsigned int cto = 0;
    unsigned int i = 0;
    uint64_t mask = ((uint64_t)-1LL) >> (RegXSize - size);
    imm &= mask;

    if (IsShiftedMask_64(imm)) {
        i = CountTrailingZeros64(imm);
        ASSERT_PRINT(i < RegXSize, "undefined behavior");
        cto = CountTrailingOnes64(imm >> i);
    } else {
        imm |= ~mask;
        if (!IsShiftedMask_64(~imm)) {
            return LogicalImmediate(InvalidLogicalImmediate);
        }

        uint32_t clo = CountLeadingOnes64(imm);
        i = static_cast<uint32_t>(RegXSize) - clo;
        cto = clo + CountTrailingOnes64(imm) - (static_cast<uint32_t>(RegXSize) - size);
    }

    // Encode in Immr the number of RORs it would take to get *from* 0^m 1^n
    // to our target value, where I is the number of RORs to go the opposite
    // direction.
    ASSERT_PRINT(size > i, "i should be smaller than element size");
    unsigned immr = (size - i) & (size - 1);

    // If size has a 1 in the n'th bit, create a value that has zeroes in
    // bits [0, n] and ones above that.
    uint64_t nImms = ~(size - 1) << 1;

    // Or the CTO value into the low bits, which must be below the Nth bit
    // bit mentioned above.
    ASSERT(cto > 0);
    nImms |= (cto - 1);

    // Extract the seventh bit and toggle it to create the N field.
    // 6 means the topmost bit in nImms
    unsigned int n = ((nImms >> 6) & 1) ^ 1;
    return LogicalImmediate((n << BITWISE_OP_N_LOWBITS) | (immr << BITWISE_OP_Immr_LOWBITS) |
                            ((nImms << BITWISE_OP_Imms_LOWBITS) & BITWISE_OP_Imms_MASK));
}

void AssemblerAarch64::Ldp(const Register &rt, const Register &rt2, const MemoryOperand &operand)
{
    uint32_t op = 0;
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET:
                op = LoadStorePairOpCode::LDP_Offset;
                break;
            case PREINDEX:
                op = LoadStorePairOpCode::LDP_Pre;
                break;
            case POSTINDEX:
                op = LoadStorePairOpCode::LDP_Post;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        bool sf = !rt.IsW();
        uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
        if (sf) {
            imm >>= 3;  // 3: 64 RegSise, imm/8 to remove trailing zeros
        } else {
            imm >>= 2;  // 2: 32 RegSise, imm/4 to remove trailing zeros
        }
        uint32_t instructionCode = Sf(sf) | op | LoadAndStorePairImm(imm) | Rt2(rt2.GetId()) |
                                   Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
        EmitU32(instructionCode);
        return;
    }
    LOG_ECMA(FATAL) << "this branch is unreachable";
    UNREACHABLE();
}

void AssemblerAarch64::Stp(const Register &rt, const Register &rt2, const MemoryOperand &operand)
{
    uint32_t op = 0;
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET:
                op = LoadStorePairOpCode::STP_Offset;
                break;
            case PREINDEX:
                op = LoadStorePairOpCode::STP_Pre;
                break;
            case POSTINDEX:
                op = LoadStorePairOpCode::STP_Post;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        bool sf = !rt.IsW();
        uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
        if (sf) {
            imm >>= 3;  // 3: 64 RegSise, imm/8 to remove trailing zeros
        } else {
            imm >>= 2;  // 2: 32 RegSise, imm/4 to remove trailing zeros
        }
        uint32_t instructionCode = Sf(sf) | op | LoadAndStorePairImm(imm) | Rt2(rt2.GetId()) |
                                   Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
        EmitU32(instructionCode);
        return;
    }
    LOG_ECMA(FATAL) << "this branch is unreachable";
    UNREACHABLE();
}

void AssemblerAarch64::Ldp(const VectorRegister &vt, const VectorRegister &vt2, const MemoryOperand &operand)
{
    uint32_t op = 0;
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET:
                op = LoadStorePairOpCode::LDP_V_Offset;
                break;
            case PREINDEX:
                op = LoadStorePairOpCode::LDP_V_Pre;
                break;
            case POSTINDEX:
                op = LoadStorePairOpCode::LDP_V_Post;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
        switch (vt.GetScale()) {
            case S:
                // 2 : 2 means remove trailing zeros
                imm >>= 2;
                break;
            case D:
                // 3 : 3 means remove trailing zeros
                imm >>= 3;
                break;
            case Q:
                // 4 : 4 means remove trailing zeros
                imm >>= 4;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        uint32_t opc = GetOpcFromScale(vt.GetScale(), true);
        uint32_t instructionCode = opc | op | LoadAndStorePairImm(imm) | Rt2(vt2.GetId()) |
                                   Rn(operand.GetRegBase().GetId()) | Rt(vt.GetId());
        EmitU32(instructionCode);
        return;
    }
    LOG_ECMA(FATAL) << "this branch is unreachable";
    UNREACHABLE();
}

void AssemblerAarch64::Stp(const VectorRegister &vt, const VectorRegister &vt2, const MemoryOperand &operand)
{
    uint32_t op = 0;
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET:
                op = LoadStorePairOpCode::STP_V_Offset;
                break;
            case PREINDEX:
                op = LoadStorePairOpCode::STP_V_Pre;
                break;
            case POSTINDEX:
                op = LoadStorePairOpCode::STP_V_Post;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
        switch (vt.GetScale()) {
            case S:
                // 2 : 2 means remove trailing zeros
                imm >>= 2;
                break;
            case D:
                // 3 : 3 means remove trailing zeros
                imm >>= 3;
                break;
            case Q:
                // 4 : 4 means remove trailing zeros
                imm >>= 4;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        uint32_t opc = GetOpcFromScale(vt.GetScale(), true);
        uint32_t instructionCode = opc | op | LoadAndStorePairImm(imm) | Rt2(vt2.GetId()) |
                                   Rn(operand.GetRegBase().GetId()) | Rt(vt.GetId());
        EmitU32(instructionCode);
        return;
    }
    LOG_ECMA(FATAL) << "this branch is unreachable";
    UNREACHABLE();
}

uint32_t AssemblerAarch64::GetOpcFromScale(Scale scale, bool ispair)
{
    uint32_t opc = 0;
    switch (scale) {
        case Scale::B:
        case Scale::H:
            ASSERT(!ispair);
            opc = 1;
            break;
        case Scale::S:
            opc = ispair ? 0 : 1;
            break;
        case Scale::D:
            opc = 1;
            break;
        case Scale::Q:
            // 3 : means opc bit is 11
            opc = ispair ? 1 : 3;
            break;
        default:
            LOG_ECMA(FATAL) << "this branch is unreachable";
            UNREACHABLE();
    }

    return (opc << LDP_STP_Opc_LOWBITS) & LDP_STP_Opc_MASK;
}

void AssemblerAarch64::Ldr(const Register &rt, const MemoryOperand &operand, Scale scale)
{
    bool regX = !rt.IsW();
    uint32_t op = GetOpcodeOfLdr(operand, scale);
    if (operand.IsImmediateOffset()) {
        uint64_t imm = GetImmOfLdr(operand, scale, regX);
        bool isSigned = operand.GetAddrMode() != AddrMode::OFFSET;
        // 30: 30bit indicate the size of LDR Reg, and Ldrb and Ldrh do not need it
        uint32_t instructionCode = ((regX && (scale == Scale::Q)) << 30) | op | LoadAndStoreImm(imm, isSigned) |
                                    Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
        EmitU32(instructionCode);
    } else {
        ASSERT(operand.GetExtendOption() != Extend::NO_EXTEND);
        uint32_t shift = GetShiftOfLdr(operand, scale, regX);
        Register rm = operand.GetRegisterOffset();
        Register rn = operand.GetRegBase();
        uint32_t extendField =
            (operand.GetExtendOption() << LDR_STR_Extend_LOWBITS) & LDR_STR_Extend_MASK;
        uint32_t shiftField = (shift << LDR_STR_S_LOWBITS) & LDR_STR_S_MASK;
        // 30: 30bit indicate the size of LDR Reg, and Ldrb and Ldrh do not need it
        uint32_t instructionCode = ((regX && (scale == Scale::Q)) << 30) | op | Rm(rm.GetId()) |
                                    extendField | shiftField | Rn(rn.GetId()) | Rt(rt.GetId());
        EmitU32(instructionCode);
    }
}

void AssemblerAarch64::Ldr(const Register &rt, const MemoryOperand &operand)
{
    Ldr(rt, operand, Scale::Q);
}

void AssemblerAarch64::Ldrh(const Register &rt, const MemoryOperand &operand)
{
    ASSERT(rt.IsW());
    Ldr(rt, operand, Scale::H);
}

void AssemblerAarch64::Ldrb(const Register &rt, const MemoryOperand &operand)
{
    ASSERT(rt.IsW());
    Ldr(rt, operand, Scale::B);
}

void AssemblerAarch64::Str(const Register &rt, const MemoryOperand &operand)
{
    uint32_t op = 0;
    bool regX = !rt.IsW();
    bool isSigned = true;
    uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET:
                op = LoadStoreOpCode::STR_Offset;
                if (regX) {
                    imm >>= 3;   // 3:  64 RegSise, imm/8 to remove trailing zeros
                } else {
                    imm >>= 2;  // 2: 32 RegSise, imm/4 to remove trailing zeros
                }
                isSigned = false;
                break;
            case PREINDEX:
                op = LoadStoreOpCode::STR_Pre;
                break;
            case POSTINDEX:
                op = LoadStoreOpCode::STR_Post;
                break;
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
        // 30: 30bit indicate the size of LDR Reg
        uint32_t instructionCode = (regX << 30) | op | LoadAndStoreImm(imm, isSigned) |
                                   Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
        EmitU32(instructionCode);
        return;
    }
    LOG_ECMA(FATAL) << "this branch is unreachable";
    UNREACHABLE();
}

void AssemblerAarch64::Ldur(const Register &rt, const MemoryOperand &operand)
{
    bool regX = !rt.IsW();
    uint32_t op = LDUR_Offset;
    ASSERT(operand.IsImmediateOffset());
    uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
    // 30: 30bit indicate the size of LDUR Reg
    uint32_t instructionCode = (regX << 30) | op | LoadAndStoreImm(imm, true) |
                               Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
    EmitU32(instructionCode);
}

void AssemblerAarch64::Stur(const Register &rt, const MemoryOperand &operand)
{
    bool regX = !rt.IsW();
    uint32_t op = STUR_Offset;
    ASSERT(operand.IsImmediateOffset());
    uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
    // 30: 30bit indicate the size of LDUR Reg
    uint32_t instructionCode = (regX << 30) | op | LoadAndStoreImm(imm, true) |
                               Rn(operand.GetRegBase().GetId()) | Rt(rt.GetId());
    EmitU32(instructionCode);
}

void AssemblerAarch64::Mov(const Register &rd, const Immediate &imm)
{
    ASSERT_PRINT(!rd.IsSp(), "sp can't load immediate, please use add instruction");
    const unsigned int HWORDSIZE = 16;
    uint64_t immValue = static_cast<uint64_t>(imm.Value());
    unsigned int allOneHalfWords = 0;
    unsigned int allZeroHalfWords = 0;
    unsigned int regSize = rd.IsW() ? RegWSize : RegXSize;
    unsigned int halfWords = regSize / HWORDSIZE;

    for (unsigned int shift = 0; shift < regSize; shift += HWORDSIZE) {
        const unsigned int halfWord = (immValue >> shift) & HWORD_MASK;
        if (halfWord == HWORD_MASK) {
            allOneHalfWords++;
        } else if (halfWord == 0) {
            allZeroHalfWords++;
        }
    }
    // use movz/movn over ORR.
    if (((halfWords - allOneHalfWords) <= 1) && ((halfWords - allZeroHalfWords) <= 1)) {
        EmitMovInstruct(rd, immValue, allOneHalfWords, allZeroHalfWords);
        return;
    }
    // Try a single ORR.
    uint64_t realImm = immValue << (RegXSize - regSize) >> (RegXSize - regSize);
    LogicalImmediate orrImm = LogicalImmediate::Create(realImm, regSize);
    if (orrImm.IsValid()) {
        Orr(rd, Register(Zero), orrImm);
        return;
    }
    // 2: One to up three instruction sequence.
    if (allOneHalfWords >= (halfWords - 2) || allZeroHalfWords >= (halfWords - 2)) {
        EmitMovInstruct(rd, immValue, allOneHalfWords, allZeroHalfWords);
        return;
    }
    ASSERT_PRINT(regSize == RegXSize, "all 32-bit Immediate will be transformed with a MOVZ/MOVK pair");

    for (unsigned int shift = 0; shift < regSize; shift += HWORDSIZE) {
        uint64_t shiftedMask = (HWORD_MASK << shift);
        uint64_t zeroChunk = realImm & ~shiftedMask;
        uint64_t oneChunk = realImm | shiftedMask;
        uint64_t rotatedImm = (realImm << 32) | (realImm >> 32);
        uint64_t replicateChunk = zeroChunk | (rotatedImm & shiftedMask);
        LogicalImmediate zeroImm = LogicalImmediate::Create(zeroChunk, regSize);
        LogicalImmediate oneImm = LogicalImmediate::Create(oneChunk, regSize);
        LogicalImmediate replicateImm = LogicalImmediate::Create(replicateChunk, regSize);
        if (!zeroImm.IsValid() && !oneImm.IsValid() && !replicateImm.IsValid()) {
            continue;
        }

        if (zeroImm.IsValid()) {
            Orr(rd, Register(Zero), zeroImm);
        } else if (oneImm.IsValid()) {
            Orr(rd, Register(Zero), oneImm);
        } else {
            Orr(rd, Register(Zero), replicateImm);
        }
        const uint64_t movkImm = (realImm & shiftedMask) >> shift;
        Movk(rd, movkImm, shift);
        return;
    }

    if (allOneHalfWords || allZeroHalfWords) {
        EmitMovInstruct(rd, immValue, allOneHalfWords, allZeroHalfWords);
        return;
    }

    if (regSize == RegXSize && TryReplicateHWords(rd, realImm)) {
        return;
    }

    if (regSize == RegXSize && TrySequenceOfOnes(rd, realImm)) {
        return;
    }
    EmitMovInstruct(rd, immValue, allOneHalfWords, allZeroHalfWords);
    return;
}

void AssemblerAarch64::Mov(const Register &rd, const Register &rm)
{
    if (rd.IsSp() || rm.IsSp()) {
        Add(rd, rm, Operand(Immediate(0)));
    } else {
        Orr(rd, Register(Zero), Operand(rm));
    }
}

/// Check whether this chunk matches the pattern '1...0...'. This pattern
/// starts a contiguous sequence of ones if we look at the bits from the LSB
/// towards the MSB.
static bool IsStartHWord(uint64_t hWord)
{
    if (hWord == 0 || hWord == std::numeric_limits<uint64_t>::max()) {
        return false;
    }
    return IsMask_64(~hWord);
}

/// Check whether this chunk matches the pattern '0...1...' This pattern
/// ends a contiguous sequence of ones if we look at the bits from the LSB
/// towards the MSB.
static bool IsEndHWord(uint64_t hWord)
{
    if (hWord == 0 || hWord == std::numeric_limits<uint64_t>::max()) {
        return false;
    }
    return IsMask_64(hWord);
}

/// Clear or set all bits in the chunk at the given index.
static uint64_t UpdateImm(uint64_t imm, unsigned idx, bool clear)
{
    if (clear) {
        // Clear chunk in the immediate.
        imm &= ~(HWORD_MASK << idx);
    } else {
        // Set all bits in the immediate for the particular chunk.
        imm |= HWORD_MASK << idx;
    }
    return imm;
}

bool AssemblerAarch64::TrySequenceOfOnes(const Register &rd, uint64_t imm)
{
    const int HWORDSIZE = 16;
    int startIdx = -1;
    int endIdx = -1;
    // Try to find the chunks which start/end a contiguous sequence of ones.
    for (int shift = 0; shift < RegXSize; shift += HWORDSIZE) {
        int64_t himm = (imm >> shift) & HWORD_MASK;
        // Sign extend the 16-bit chunk to 64-bit.
        // 48 : 48 means RegXSize - HWORDSIZE
        himm = (himm << 48) >> 48;

        if (IsStartHWord(himm)) {
            startIdx = shift;
        } else if (IsEndHWord(static_cast<uint64_t>(himm))) {
            endIdx = shift;
        }
    }
    // Early exit in case we can't find a start/end chunk.
    if (startIdx == -1 || endIdx == -1) {
        return false;
    }
    // Outside of the contiguous sequence of ones everything needs to be zero.
    uint64_t outside = 0;
    // Chunks between the start and end chunk need to have all their bits set.
    uint64_t inside = HWORD_MASK;

    // If our contiguous sequence of ones wraps around from the MSB into the LSB,
    // just swap indices and pretend we are materializing a contiguous sequence
    // of zeros surrounded by a contiguous sequence of ones.
    if (startIdx > endIdx) {
        std::swap(startIdx, endIdx);
        std::swap(outside, inside);
    }

    uint64_t orrImm = imm;
    int firstMovkShift = -1;
    int secondMovkShift = -1;
    for (int shift = 0; shift < RegXSize; shift += HWORDSIZE) {
        uint64_t himm = (imm >> shift) & HWORD_MASK;
        // Check whether we are looking at a chunk which is not part of the
        // contiguous sequence of ones.
        if ((shift < startIdx || endIdx < shift) && himm != outside) {
            orrImm = UpdateImm(orrImm, shift, outside == 0);
            if (firstMovkShift == -1) {
                firstMovkShift = shift;
            } else {
                secondMovkShift = shift;
            }
        } else if (shift > startIdx && shift < endIdx && himm != inside) {
            orrImm = UpdateImm(orrImm, shift, outside == 0);
            if (firstMovkShift == -1) {
                firstMovkShift = shift;
            } else {
                secondMovkShift = shift;
            }
        }
    }
    ASSERT_PRINT(firstMovkShift != -1, "constant materializable with single orr!");
    Orr(rd, rd, LogicalImmediate::Create(orrImm, RegXSize));
    Movk(rd, (imm >> firstMovkShift) & HWORD_MASK, firstMovkShift);
    if (secondMovkShift != -1) {
        Movk(rd, (imm >> secondMovkShift) & HWORD_MASK, secondMovkShift);
    }
    return true;
}

bool AssemblerAarch64::TryReplicateHWords(const Register &rd, uint64_t imm)
{
    const int HWORDSIZE = 16;
    std::map<uint64_t, int> repeatMaps;
    for (int idx = 0; idx < RegXSize; idx += HWORDSIZE) {
        uint64_t halfWord = (imm >> idx) & HWORD_MASK;
        if (repeatMaps.find(halfWord) != repeatMaps.end()) {
            repeatMaps[halfWord] += 1;
        } else {
            repeatMaps[halfWord] = 1;
        }
    }
    for (auto iter : repeatMaps) {
        const uint64_t hImm = iter.first;
        const int count = iter.second;
        uint64_t repeatImm = hImm | (hImm << 16) | (hImm << 32) | (hImm << 48);
        LogicalImmediate orrImm = LogicalImmediate::Create(repeatImm, 64);
        // if orrImm not valid, repeat count can't be 2 or 3, it can't be simplified with orr.
        if ((count != 2 && count != 3) || orrImm.IsValid()) {
            continue;
        }
        Orr(rd, rd, orrImm);
        int shift = 0;
        uint64_t imm16 = 0;
        // Find the first chunk not materialized with the ORR instruction.
        for (; shift < RegXSize; shift += HWORDSIZE) {
            imm16 = (imm >> shift) & HWORD_MASK;
            if (imm16 != hImm) {
                break;
            }
        }
        // Create the first MOVK instruction.
        Movk(rd, imm16, shift);
        // 3 : 3 means repeat 3 times, Imm encode has been done.
        if (count == 3) {
            return true;
        }
        // Find the remaining chunk which needs to be materialized.
        for (shift += HWORDSIZE; shift < RegXSize; shift += HWORDSIZE) {
            imm16 = (imm >> shift) & HWORD_MASK;
            if (imm16 != hImm) {
                break;
            }
        }
        Movk(rd, imm16, shift);
        return true;
    }
    return false;
}

void AssemblerAarch64::EmitMovInstruct(const Register &rd, uint64_t imm,
                                       unsigned int allOneHWords, unsigned int allZeroHWords)
{
    bool isNeg = false;
    if (allOneHWords > allZeroHWords) {
        isNeg = true;
        imm = ~imm;
    }
    int firstshift = 0;     // LSL amount for high bits with MOVZ/MOVN
    int lastshift = 0; // LSL amount for last MOVK
    if (imm != 0) {
        int lz = static_cast<int>(CountLeadingZeros64(imm));
        int tz = static_cast<int>(CountTrailingZeros64(imm));
        firstshift = (tz / 16) * 16;         // 16 : 16  means the operand of MOVK/N/Z is 16 bits Immediate
        // 63 : 63  means the topmost bits of RegXSize
        lastshift = ((63 - lz) / 16) * 16;   // 16 : 16  means the operand of MOVK/N/Z is 16 bits Immediate
    }
    uint64_t imm16 = (imm >> firstshift) & HWORD_MASK;
    if (isNeg) {
        Movn(rd, imm16, firstshift);
        imm = ~imm;
    } else {
        Movz(rd, imm16, firstshift);
    }
    if (firstshift == lastshift) {
        return;
    }
    while (firstshift < lastshift) {
        firstshift += 16;                   // 16 : 16  means the operand of MOVK is 16 bits Immediate
        imm16 = (imm >> firstshift) & HWORD_MASK;
        if (imm16 == (isNeg ? HWORD_MASK : 0)) {
            // skip movk because initial value is already set correctly.
            continue;
        }
        Movk(rd, imm16, firstshift);
    }
}

void AssemblerAarch64::Movz(const Register &rd, uint64_t imm, int shift)
{
    MovWide(MoveOpCode::MOVZ, rd, imm, shift);
}

void AssemblerAarch64::Movk(const Register &rd, uint64_t imm, int shift)
{
    MovWide(MoveOpCode::MOVK, rd, imm, shift);
}

void AssemblerAarch64::Movn(const Register &rd, uint64_t imm, int shift)
{
    MovWide(MoveOpCode::MOVN, rd, imm, shift);
}

void AssemblerAarch64::MovWide(uint32_t op, const Register &rd, uint64_t imm, int shift)
{
    uint32_t imm_field = (imm << MOV_WIDE_Imm16_LOWBITS) & MOV_WIDE_Imm16_MASK;
    uint32_t hw_field = ((shift / 16) << MOV_WIDE_Hw_LOWBITS) & MOV_WIDE_Hw_MASK;
    uint32_t code = Sf(!rd.IsW()) | op | imm_field | hw_field | Rd(rd.GetId());
    EmitU32(code);
}


void AssemblerAarch64::Orr(const Register &rd, const Register &rn, const LogicalImmediate &imm)
{
    BitWiseOpImm(ORR_Imm, rd, rn, imm.Value());
}

void AssemblerAarch64::And(const Register &rd, const Register &rn, const LogicalImmediate &imm)
{
    BitWiseOpImm(AND_Imm, rd, rn, imm.Value());
}

void AssemblerAarch64::Ands(const Register &rd, const Register &rn, const LogicalImmediate &imm)
{
    BitWiseOpImm(ANDS_Imm, rd, rn, imm.Value());
}

void AssemblerAarch64::Orr(const Register &rd, const Register &rn, const Operand &operand)
{
    ASSERT(operand.IsShifted());
    BitWiseOpShift(ORR_Shift, rd, rn, operand);
}

void AssemblerAarch64::And(const Register &rd, const Register &rn, const Operand &operand)
{
    ASSERT(operand.IsShifted());
    BitWiseOpShift(AND_Shift, rd, rn, operand);
}

void AssemblerAarch64::Ands(const Register &rd, const Register &rn, const Operand &operand)
{
    ASSERT(operand.IsShifted());
    BitWiseOpShift(ANDS_Shift, rd, rn, operand);
}

void AssemblerAarch64::BitWiseOpImm(BitwiseOpCode op, const Register &rd, const Register &rn, uint64_t imm)
{
    uint32_t code = Sf(!rd.IsW()) | op | imm | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::BitWiseOpShift(BitwiseOpCode op, const Register &rd, const Register &rn, const Operand &operand)
{
    uint32_t shift_field = (operand.GetShiftOption() << BITWISE_OP_Shift_LOWBITS) & BITWISE_OP_Shift_MASK;
    uint32_t shift_amount = (operand.GetShiftAmount() << BITWISE_OP_ShiftAmount_LOWBITS) & BITWISE_OP_ShiftAmount_MASK;
    uint32_t code = Sf(!rd.IsW()) | op | shift_field | Rm(operand.Reg().GetId()) |
                       shift_amount | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Lsl(const Register &rd, const Register &rn, const Register &rm)
{
    uint32_t code = Sf(!rd.IsW()) | LSL_Reg | Rm(rm.GetId()) | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Lsr(const Register &rd, const Register &rn, const Register &rm)
{
    uint32_t code = Sf(!rd.IsW()) | LSR_Reg | Rm(rm.GetId()) | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Ubfm(const Register &rd, const Register &rn, unsigned immr, unsigned imms)
{
    bool sf = !rd.IsW();
    uint32_t n = (sf << BITWISE_OP_N_LOWBITS) & BITWISE_OP_N_MASK;
    uint32_t immr_field = (immr << BITWISE_OP_Immr_LOWBITS) & BITWISE_OP_Immr_MASK;
    uint32_t imms_field = (imms << BITWISE_OP_Imms_LOWBITS) & BITWISE_OP_Imms_MASK;
    uint32_t code = Sf(sf) | UBFM | n | immr_field | imms_field | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Bfm(const Register &rd, const Register &rn, unsigned immr, unsigned imms)
{
    bool sf = !rd.IsW();
    uint32_t n = (sf << BITWISE_OP_N_LOWBITS) & BITWISE_OP_N_MASK;
    uint32_t immr_field = (immr << BITWISE_OP_Immr_LOWBITS) & BITWISE_OP_Immr_MASK;
    uint32_t imms_field = (imms << BITWISE_OP_Imms_LOWBITS) & BITWISE_OP_Imms_MASK;
    uint32_t code = Sf(sf) | BFM | n | immr_field | imms_field | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Lsr(const Register &rd, const Register &rn, unsigned shift)
{
    unsigned imms = 0;
    if (rd.IsW()) {
        imms = 31; // 31 : 31 32-bit variant Applies when sf == 0 && N == 0 && imms == 011111
        // LSR <Wd>, <Wn>, #<shift> is equivalent to UBFM <Wd>, <Wn>, #<shift>, #31
        // and is always the preferred disassembly
    } else {
        imms = 63; // 63 : 63 64-bit variant Applies when sf == 1 && N == 1 && imms == 111111
        // LSR <Xd>, <Xn>, #<shift> is equivalent to UBFM <Xd>, <Xn>, #<shift>, #63
        // and is always the preferred disassembly
    }
    Ubfm(rd, rn, shift, imms);
}

void AssemblerAarch64::Add(const Register &rd, const Register &rn, const Operand &operand)
{
    if (operand.IsImmediate()) {
        int64_t imm = static_cast<int64_t>(operand.ImmediateValue());
        if (imm < 0) {
            AddSubImm(SUB_Imm, rd, rn, false, -1 * imm);
        } else {
            AddSubImm(ADD_Imm, rd, rn, false, imm);
        }
    } else {
        if (operand.IsShifted()) {
            AddSubReg(ADD_Shift, rd, rn, false, operand);
        } else {
            AddSubReg(ADD_Extend, rd, rn, false, operand);
        }
    }
}

void AssemblerAarch64::Adds(const Register &rd, const Register &rn, const Operand &operand)
{
    if (operand.IsImmediate()) {
        AddSubImm(ADD_Imm, rd, rn, true, operand.ImmediateValue());
    } else {
        if (operand.IsShifted()) {
            AddSubReg(ADD_Shift, rd, rn, true, operand);
        } else {
            AddSubReg(ADD_Extend, rd, rn, true, operand);
        }
    }
}

void AssemblerAarch64::Sub(const Register &rd, const Register &rn, const Operand &operand)
{
    if (operand.IsImmediate()) {
        int64_t imm = static_cast<int64_t>(operand.ImmediateValue());
        if (imm < 0) {
            AddSubImm(ADD_Imm, rd, rn, false, -1 * imm);
        } else {
            AddSubImm(SUB_Imm, rd, rn, false, imm);
        }
    } else {
        if (operand.IsShifted()) {
            AddSubReg(SUB_Shift, rd, rn, false, operand);
        } else {
            AddSubReg(SUB_Extend, rd, rn, false, operand);
        }
    }
}

void AssemblerAarch64::Subs(const Register &rd, const Register &rn, const Operand &operand)
{
    if (operand.IsImmediate()) {
        AddSubImm(SUB_Imm, rd, rn, true, operand.ImmediateValue());
    } else {
        if (operand.IsShifted()) {
            AddSubReg(SUB_Shift, rd, rn, true, operand);
        } else {
            AddSubReg(SUB_Extend, rd, rn, true, operand);
        }
    }
}

bool AssemblerAarch64::IsAddSubImm(uint64_t imm)
{
    const uint64_t IMM12_MASK = (1 << ADD_SUB_Imm12_WIDTH) - 1;
    if (imm <= IMM12_MASK) {
        return true;
    }

    if (((imm & IMM12_MASK) == 0) && ((imm & ~IMM12_MASK) <= IMM12_MASK)) {
        return true;
    }
    return false;
}

void AssemblerAarch64::AddSubImm(AddSubOpCode op, const Register &rd, const Register &rn, bool setFlags, uint64_t imm)
{
    ASSERT(IsAddSubImm(imm));
    uint32_t shift = 0;
    const uint64_t IMM12_MASK = (1 << ADD_SUB_Imm12_WIDTH) - 1;
    uint64_t imm12 = imm & (~IMM12_MASK);
    if (imm12 != 0) {
        shift = 1;
    } else {
        imm12 = imm;
    }
    uint32_t flags_field = ((setFlags ? 1 : 0) << ADD_SUB_S_LOWBITS) & ADD_SUB_S_MASK;
    uint32_t imm_field = (imm12 << ADD_SUB_Imm12_LOWBITS) & ADD_SUB_Imm12_MASK;
    uint32_t shift_field = (shift << ADD_SUB_Sh_LOWBITS) & ADD_SUB_Sh_MASK;
    uint32_t code = Sf(!rd.IsW()) | op | flags_field | shift_field | imm_field | Rd(rd.GetId()) | Rn(rn.GetId());
    EmitU32(code);
}

void AssemblerAarch64::AddSubReg(AddSubOpCode op, const Register &rd, const Register &rn,
                                 bool setFlags, const Operand &operand)
{
    uint32_t flags_field = ((setFlags ? 1 : 0) << ADD_SUB_S_LOWBITS) & ADD_SUB_S_MASK;
    uint32_t code = 0;
    if (operand.IsShifted()) {
        uint32_t shift_field = ((operand.GetShiftOption()) << ADD_SUB_Shift_LOWBITS) & ADD_SUB_Shift_MASK;
        uint32_t shift_amount = ((operand.GetShiftAmount()) << ADD_SUB_ShiftAmount_LOWBITS) & ADD_SUB_ShiftAmount_MASK;
        ASSERT((op == ADD_Shift) | (op == SUB_Shift));
        code = Sf(!rd.IsW()) | op | flags_field | shift_field | Rm(operand.Reg().GetId()) |
                  shift_amount | Rn(rn.GetId()) | Rd(rd.GetId());
    } else {
        ASSERT((op == ADD_Extend) | (op == SUB_Extend));
        uint32_t extend_field =
            (operand.GetExtendOption() << ADD_SUB_ExtendOption_LOWBITS) & ADD_SUB_ExtendOption_MASK;
        uint32_t extend_shift = (operand.GetShiftAmount() << ADD_SUB_ExtendShift_LOWBITS) & ADD_SUB_ExtendShift_MASK;
        code = Sf(!rd.IsW()) | op | flags_field | Rm(operand.Reg().GetId()) | extend_field |
                  extend_shift | Rn(rn.GetId()) | Rd(rd.GetId());
    }
    EmitU32(code);
}

void AssemblerAarch64::Cmp(const Register &rd, const Operand &operand)
{
    Subs(Register(Zero, rd.GetType()), rd, operand);
}

void AssemblerAarch64::CMov(const Register &rd, const Register &rn, const Operand &operand, Condition cond)
{
    ASSERT(!operand.IsImmediate());
    uint32_t cond_field = (cond << CSEL_Cond_LOWBITS) & CSEL_Cond_MASK;
    uint32_t code = Sf(!rd.IsW()) | CSEL | Rm(operand.Reg().GetId()) | cond_field | Rn(rn.GetId()) | Rd(rd.GetId());
    EmitU32(code);
}

void AssemblerAarch64::B(Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    B(offsetImm);
}

void AssemblerAarch64::B(int32_t imm)
{
    uint32_t code = BranchOpCode::Branch | ((imm << BRANCH_Imm26_LOWBITS) & BRANCH_Imm26_MASK);
    EmitU32(code);
}

void AssemblerAarch64::Br(const Register &rn)
{
    uint32_t code = BranchOpCode::BR | Rn(rn.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Bl(Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    Bl(offsetImm);
}

void AssemblerAarch64::Bl(int32_t imm)
{
    uint32_t code = CallOpCode::BL | ((imm << BRANCH_Imm26_LOWBITS) & BRANCH_Imm26_MASK);
    EmitU32(code);
}

void AssemblerAarch64::Blr(const Register &rn)
{
    ASSERT(!rn.IsW());
    uint32_t code = CallOpCode::BLR | Rn(rn.GetId());
    EmitU32(code);
}

void AssemblerAarch64::B(Condition cond, Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    B(cond, offsetImm);
}

void AssemblerAarch64::B(Condition cond, int32_t imm)
{
    uint32_t code = BranchOpCode::BranchCond | BranchImm19(imm) | cond;
    EmitU32(code);
}

void AssemblerAarch64::Cbz(const Register &rt, Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    Cbz(rt, offsetImm);
}

void AssemblerAarch64::Cbnz(const Register &rt, Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    Cbnz(rt, offsetImm);
}

void AssemblerAarch64::Cbz(const Register &rt, int32_t imm)
{
    uint32_t code = Sf(!rt.IsW()) | BranchOpCode::CBZ | BranchImm19(imm) | rt.GetId();
    EmitU32(code);
}

void AssemblerAarch64::Cbnz(const Register &rt, int32_t imm)
{
    uint32_t code = Sf(!rt.IsW()) | BranchOpCode::CBNZ | BranchImm19(imm) | rt.GetId();
    EmitU32(code);
}

void AssemblerAarch64::Tbz(const Register &rt, int32_t bitPos, Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    Tbz(rt, bitPos, offsetImm);
}

void AssemblerAarch64::Tbz(const Register &rt, int32_t bitPos, int32_t imm)
{
    uint32_t b5 = (bitPos << (BRANCH_B5_LOWBITS - 5)) & BRANCH_B5_MASK;
    uint32_t b40 = (bitPos << BRANCH_B40_LOWBITS) & BRANCH_B40_MASK;
    uint32_t imm14 = (imm << BRANCH_Imm14_LOWBITS) & BRANCH_Imm14_MASK;
    uint32_t code = b5 | BranchOpCode::TBZ | b40 | imm14 | rt.GetId();
    EmitU32(code);
}

void AssemblerAarch64::Tbnz(const Register &rt, int32_t bitPos, Label *label)
{
    int32_t offsetImm = LinkAndGetInstOffsetToLabel(label);
    // 2 : 2 means 4 bytes aligned.
    offsetImm >>= 2;
    Tbnz(rt, bitPos, offsetImm);
}

void AssemblerAarch64::Tbnz(const Register &rt, int32_t bitPos, int32_t imm)
{
    uint32_t b5 = (bitPos << (BRANCH_B5_LOWBITS - 5)) & BRANCH_B5_MASK;
    uint32_t b40 = (bitPos << BRANCH_B40_LOWBITS) & BRANCH_B40_MASK;
    uint32_t imm14 = (imm <<BRANCH_Imm14_LOWBITS) & BRANCH_Imm14_MASK;
    uint32_t code = b5 | BranchOpCode::TBNZ | b40 | imm14 | rt.GetId();
    EmitU32(code);
}

void AssemblerAarch64::Tst(const Register& rn, const Operand& operand)
{
    Ands(Register(Zero, rn.GetType()), rn, operand);
}

void AssemblerAarch64::Tst(const Register &rn, const LogicalImmediate &imm)
{
    Ands(Register(Zero, rn.GetType()), rn, imm);
}

int32_t AssemblerAarch64::LinkAndGetInstOffsetToLabel(Label *label)
{
    int32_t offset = 0;
    if (label->IsBound()) {
        offset = static_cast<int32_t>(label->GetPos() - GetCurrentPosition());
    } else {
        if (label->IsLinked()) {
            offset = static_cast<int32_t>(label->GetLinkedPos() - GetCurrentPosition());
        } else {
            offset = 0;
        }
        label->LinkTo(GetCurrentPosition());
    }
    return offset;
}

void AssemblerAarch64::Bind(Label *target)
{
    size_t pos = GetCurrentPosition();
    ASSERT(!target->IsBound());
        if (target->IsLinked()) {
        uint32_t linkPos = target->GetLinkedPos();
        while (linkPos != 0) {
            int32_t offset = GetLinkOffsetFromBranchInst(linkPos);
            int32_t disp = static_cast<int32_t>(pos - linkPos);
            SetRealOffsetToBranchInst(linkPos, disp);
            if (offset == 0) {
                break;
            }
            linkPos = linkPos + offset;
        }
    }
    target->BindTo(pos);
}

int32_t AssemblerAarch64::GetLinkOffsetFromBranchInst(int32_t pos)
{
    uint32_t branchCode = GetU32(pos);
    // 2 : 2 means 4 bytes aligned.
    int32_t immOffSet = ImmBranch(branchCode) << 2;
    return immOffSet;
}

int32_t AssemblerAarch64::ImmBranch(uint32_t branchCode)
{
    int32_t immOffset = 0;
    if ((branchCode & BranchFMask) == BranchOpCode::Branch) {
        immOffset = (branchCode & BRANCH_Imm26_MASK) >> BRANCH_Imm26_LOWBITS;
        if (immOffset & (1 << (BRANCH_Imm26_WIDTH - 1))) {
            // 31 : 31 means topmost bits of instruction "uint32_t"
            immOffset |= ((1 << (31 - BRANCH_Imm26_WIDTH)) - 1) << BRANCH_Imm26_WIDTH;
        }
    } else if ((branchCode & BranchCondFMask) == BranchOpCode::BranchCond) {
        immOffset = (branchCode & BRANCH_Imm19_MASK) >> BRANCH_Imm19_LOWBITS;
        if (immOffset & (1 << (BRANCH_Imm19_WIDTH - 1))) {
            // 31 : 31 means topmost bits of instruction "uint32_t"
            immOffset |= ((1 << (31 - BRANCH_Imm19_WIDTH)) - 1) << BRANCH_Imm19_WIDTH;
        }
    } else if ((branchCode & BranchCompareFMask) == BranchOpCode::CBZ) {
        immOffset = (branchCode & BRANCH_Imm19_MASK) >> BRANCH_Imm19_LOWBITS;
        if (immOffset & (1 << (BRANCH_Imm19_WIDTH - 1))) {
            // 31 : 31 means topmost bits of instruction "uint32_t"
            immOffset |= ((1 << (31 - BRANCH_Imm19_WIDTH)) - 1) << BRANCH_Imm19_WIDTH;
        }
    } else if ((branchCode & BranchTestFMask) == BranchOpCode::TBZ) {
        immOffset = (branchCode & BRANCH_Imm14_MASK) >> BRANCH_Imm14_LOWBITS;
        if (immOffset & (1 << (BRANCH_Imm14_WIDTH - 1))) {
            // 31 : 31 means topmost bits of instruction "uint32_t"
            immOffset |= ((1 << (31 - BRANCH_Imm14_WIDTH)) - 1) << BRANCH_Imm14_WIDTH;
        }
    } else {
        UNREACHABLE();
    }
    return immOffset;
}

void AssemblerAarch64::SetRealOffsetToBranchInst(uint32_t linkPos, int32_t disp)
{
    uint32_t branchCode = GetU32(linkPos);
    // 2 : 2 means 4 bytes aligned.
    uint32_t immOffset = disp >> 2;

    if ((branchCode & BranchFMask) == BranchOpCode::Branch) {
        branchCode &= ~BRANCH_Imm26_MASK;
        branchCode |= (immOffset << BRANCH_Imm26_LOWBITS) & BRANCH_Imm26_MASK;
    } else if ((branchCode & BranchCondFMask) == BranchOpCode::BranchCond) {
        branchCode &= ~BRANCH_Imm19_MASK;
        branchCode |= (immOffset << BRANCH_Imm19_LOWBITS) & BRANCH_Imm19_MASK;
    } else if ((branchCode & BranchCompareFMask) == BranchOpCode::CBZ) {
        branchCode &= ~BRANCH_Imm19_MASK;
        branchCode |= (immOffset << BRANCH_Imm19_LOWBITS) & BRANCH_Imm19_MASK;
    } else if ((branchCode & BranchTestFMask) == BranchOpCode::TBZ) {
        branchCode &= ~BRANCH_Imm14_MASK;
        branchCode |= (immOffset << BRANCH_Imm14_LOWBITS) & BRANCH_Imm14_MASK;
    }
    PutI32(linkPos, branchCode);
}

void AssemblerAarch64::Ret()
{
    Ret(Register(X30));
}

void AssemblerAarch64::Ret(const Register &rn)
{
    uint32_t code = RetOpCode::Ret | Rn(rn.GetId());
    EmitU32(code);
}

void AssemblerAarch64::Brk(const Immediate &imm)
{
    uint32_t brk_number_field =
        (static_cast<uint32_t>(imm.Value()) << BRK_Imm16_LOWBITS) & BRK_Imm16_MASK;
    uint32_t code = BRKImm | brk_number_field;
    EmitU32(code);
}

uint64_t AssemblerAarch64::GetImmOfLdr(const MemoryOperand &operand, Scale scale, bool isRegX)
{
    ASSERT(operand.IsImmediateOffset());
    uint64_t imm = static_cast<uint64_t>(operand.GetImmediate().Value());
    if (operand.GetAddrMode() == OFFSET) {
        if (scale == Scale::H) {
            imm >>= 1;
        } else if (scale == Scale::Q) {
            if (isRegX) {
                imm >>= 3;  // 3:  64 RegSise, imm/8 to remove trailing zeros
            } else {
                imm >>= 2;  // 2: 32 RegSise, imm/4 to remove trailing zeros
            }
        }
    }
    return imm;
}

uint64_t AssemblerAarch64::GetOpcodeOfLdr(const MemoryOperand &operand, Scale scale)
{
    uint32_t op = 0;
    if (operand.IsImmediateOffset()) {
        switch (operand.GetAddrMode()) {
            case OFFSET: {
                if (scale == Scale::B) {
                    op = LoadStoreOpCode::LDRB_Offset;
                } else if (scale == Scale::H) {
                    op = LoadStoreOpCode::LDRH_Offset;
                } else if (scale == Scale::Q) {
                    op = LoadStoreOpCode::LDR_Offset;
                } else {
                    LOG_ECMA(FATAL) << "this branch is unreachable";
                    UNREACHABLE();
                }
                break;
            }
            case PREINDEX: {
                if (scale == Scale::B) {
                    op = LoadStoreOpCode::LDRB_Pre;
                } else if (scale == Scale::H) {
                    op = LoadStoreOpCode::LDRH_Pre;
                } else if (scale == Scale::Q) {
                    op = LoadStoreOpCode::LDR_Pre;
                } else {
                    LOG_ECMA(FATAL) << "this branch is unreachable";
                    UNREACHABLE();
                }
                break;
            }
            case POSTINDEX: {
                if (scale == Scale::B) {
                    op = LoadStoreOpCode::LDRB_Post;
                } else if (scale == Scale::H) {
                    op = LoadStoreOpCode::LDRH_Post;
                } else if (scale == Scale::Q) {
                    op = LoadStoreOpCode::LDR_Post;
                } else {
                    LOG_ECMA(FATAL) << "this branch is unreachable";
                    UNREACHABLE();
                }
                break;
            }
            default:
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
        }
    } else {
        if (scale == Scale::B) {
            op = LoadStoreOpCode::LDRB_Register;
        } else if (scale == Scale::H) {
            op = LoadStoreOpCode::LDRH_Register;
        } else if (scale == Scale::Q) {
            op = LoadStoreOpCode::LDR_Register;
        } else {
            LOG_ECMA(FATAL) << "this branch is unreachable";
            UNREACHABLE();
        }
    }
    return op;
}

uint32_t AssemblerAarch64::GetShiftOfLdr(const MemoryOperand &operand, Scale scale, bool isRegX)
{
    uint32_t shift = 0;
    if (scale == Scale::B) {
        shift = operand.GetShiftOption() != Shift::NO_SHIFT;
    } else if (scale == Scale::H) {
        shift = operand.GetShiftAmount();
        ASSERT(shift == 0 || shift == 1);
        shift = (shift == 0) ? 0 : 1;
    } else if (scale == Scale::Q) {
        shift = operand.GetShiftAmount();
        if (isRegX) {
            // 3 : 3 means address aligned with 8bytes
            ASSERT(shift == 0 || shift == 3);
        } else {
            // 2 : 2 means address aligned with 4bytes
            ASSERT(shift == 0 || shift == 2);
        }
        shift = (shift == 0) ? 0 : 1;
    }
    return shift;
}
}   // namespace panda::ecmascript::aarch64
