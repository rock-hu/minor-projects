/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "ecmascript/compiler/lcr_circuit_builder.h"
#include "ecmascript/compiler/circuit_builder-inl.h"

namespace panda::ecmascript::kungfu {

GateRef CircuitBuilder::BinaryCmp(const GateMetaData* meta, GateRef left, GateRef right, const char* comment)
{
    return GetCircuit()->NewGate(meta, MachineType::I1, { left, right }, GateType::NJSValue(), comment);
}

MachineType CircuitBuilder::GetMachineTypeFromVariableType(VariableType type)
{
    return type.GetMachineType();
}

GateRef CircuitBuilder::Sqrt(GateRef param)
{
    return GetCircuit()->NewGate(circuit_->Sqrt(), MachineType::F64, {param}, GateType::DoubleType());
}

GateRef CircuitBuilder::AddWithOverflow(GateRef left, GateRef right)
{
    return GetCircuit()->NewGate(circuit_->AddWithOverflow(), MachineType::I64, {left, right}, GateType::AnyType());
}

GateRef CircuitBuilder::SubWithOverflow(GateRef left, GateRef right)
{
    return GetCircuit()->NewGate(circuit_->SubWithOverflow(), MachineType::I64, {left, right}, GateType::AnyType());
}

GateRef CircuitBuilder::MulWithOverflow(GateRef left, GateRef right)
{
    return GetCircuit()->NewGate(circuit_->MulWithOverflow(), MachineType::I64, {left, right}, GateType::AnyType());
}

GateRef CircuitBuilder::ExtractValue(MachineType mt, GateRef pointer, GateRef index)
{
    ASSERT(acc_.GetOpCode(index) == OpCode::CONSTANT);
    ASSERT(acc_.GetMachineType(index) == MachineType::I32);
    return GetCircuit()->NewGate(circuit_->ExtractValue(), mt, {pointer, index}, GateType::NJSValue());
}

GateRef CircuitBuilder::ReadSp()
{
    return circuit_->NewGate(circuit_->ReadSp(), MachineType::I64, GateType::NJSValue());
}

MachineType CircuitBuilder::GetMachineTypeOfValueType(ValueType type)
{
    switch (type) {
        case ValueType::BOOL:
            return MachineType::I1;
        case ValueType::INT32:
        case ValueType::UINT32:
            return MachineType::I32;
        case ValueType::FLOAT64:
            return MachineType::F64;
        case ValueType::TAGGED_BOOLEAN:
        case ValueType::TAGGED_INT:
        case ValueType::TAGGED_DOUBLE:
        case ValueType::TAGGED_NUMBER:
        case ValueType::TAGGED_NULL:
        case ValueType::CHAR:
        case ValueType::ECMA_STRING:
        case ValueType::UNDEFINED:
        case ValueType::HOLE_INT:
        case ValueType::HOLE_DOUBLE:
            return MachineType::I64;
        default:
            UNREACHABLE();
            break;
    }
    return MachineType::NOVALUE;
}

GateRef CircuitBuilder::BinaryArithmetic(const GateMetaData* meta, MachineType machineType,
                                         GateRef left, GateRef right, GateType gateType, const char* comment)
{
    auto circuit = GetCircuit();
    if (gateType == GateType::Empty()) {
        gateType = acc_.GetGateType(left);
    }
    return circuit->NewGate(meta, machineType, { left, right }, gateType, comment);
}

GateRef CircuitBuilder::Alloca(size_t size)
{
    return GetCircuit()->NewGate(circuit_->Alloca(size), MachineType::ARCH, GateType::NJSValue());
}

// memory
void CircuitBuilder::Store(VariableType type, GateRef glue, GateRef base, GateRef offset, GateRef value,
                           MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    if (mAttr.GetBarrier() == MemoryAttribute::Barrier::UNKNOWN_BARRIER && acc_.IsConstant(value)) {
        mAttr.SetBarrier(MemoryAttribute::Barrier::NO_BARRIER);
    }
    auto bit = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(circuit_->Store(bit),
        MachineType::NOVALUE, { depend, glue, base, offset, value, value }, type.GetGateType());
    label->SetDepend(result);
}

void CircuitBuilder::StoreHClass(VariableType type, GateRef glue, GateRef base, GateRef offset, GateRef value,
                                 GateRef compValue, MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    if (mAttr.GetBarrier() == MemoryAttribute::Barrier::UNKNOWN_BARRIER && acc_.IsConstant(value)) {
        mAttr.SetBarrier(MemoryAttribute::Barrier::NO_BARRIER);
    }
    auto bit = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(circuit_->Store(bit),
        MachineType::NOVALUE, { depend, glue, base, offset, value, compValue }, type.GetGateType());
    label->SetDepend(result);
}

void CircuitBuilder::StoreWithoutBarrier(VariableType type, GateRef addr, GateRef value, MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    auto bit = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(circuit_->StoreWithoutBarrier(bit),
        MachineType::NOVALUE, { depend, addr, value }, type.GetGateType());
    label->SetDepend(result);
}

// memory
GateRef CircuitBuilder::Load(VariableType type, GateRef glue, GateRef base, GateRef offset, MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    GateRef val = PtrAdd(base, offset);
    auto bits = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(GetCircuit()->Load(bits), type.GetMachineType(),
                                           { depend, glue, val }, type.GetGateType());
    label->SetDepend(result);
    return result;
}

GateRef CircuitBuilder::Load(VariableType type, GateRef glue, GateRef base, GateRef offset, GateRef depend,
                             MemoryAttribute mAttr)
{
    GateRef val = PtrAdd(base, offset);
    auto bits = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(GetCircuit()->Load(bits), type.GetMachineType(),
                                           { depend, glue, val }, type.GetGateType());
    return result;
}

GateRef CircuitBuilder::LoadWithoutBarrier(VariableType type, GateRef base, GateRef offset, MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    GateRef val = PtrAdd(base, offset);
    auto bits = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(GetCircuit()->LoadWithoutBarrier(bits), type.GetMachineType(),
                                           { depend, val }, type.GetGateType());
    label->SetDepend(result);
    return result;
}

GateRef CircuitBuilder::LoadWithoutBarrier(VariableType type, GateRef base, GateRef offset, GateRef depend,
    MemoryAttribute mAttr)
{
    GateRef val = PtrAdd(base, offset);
    auto bits = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(GetCircuit()->LoadWithoutBarrier(bits), type.GetMachineType(),
                                           { depend, val }, type.GetGateType());
    return result;
}

GateRef CircuitBuilder::LoadFromAddressWithoutBarrier(VariableType type, GateRef addr, MemoryAttribute mAttr)
{
    auto label = GetCurrentLabel();
    auto depend = label->GetDepend();
    auto bits = LoadStoreAccessor::ToValue(mAttr);
    GateRef result = GetCircuit()->NewGate(GetCircuit()->LoadWithoutBarrier(bits), type.GetMachineType(),
                                           { depend, addr }, type.GetGateType());
    label->SetDepend(result);
    return result;
}

GateRef CircuitBuilder::DoubleTrunc(GateRef gate, GateRef value, const char* comment)
{
    if (GetCompilationConfig()->IsAArch64()) {
        return DoubleTrunc(value, comment);
    }

    GateRef glue = acc_.GetGlueFromArgList();
    return CallNGCRuntime(glue, RTSTUB_ID(FloatTrunc), Gate::InvalidGateRef, {value}, gate, comment);
}

GateRef CircuitBuilder::GetDoubleOfTNumber(GateRef x)
{
    Label subentry(env_);
    SubCfgEntry(&subentry);
    Label isInt(env_);
    Label isDouble(env_);
    Label exit(env_);
    DEFVALUE(result, env_, VariableType::FLOAT64(), Double(0));
    BRANCH(TaggedIsInt(x), &isInt, &isDouble);
    Bind(&isInt);
    {
        result = ChangeInt32ToFloat64(GetInt32OfTInt(x));
        Jump(&exit);
    }
    Bind(&isDouble);
    {
        result = GetDoubleOfTDouble(x);
        Jump(&exit);
    }
    Bind(&exit);
    GateRef ret = *result;
    SubCfgExit();
    return ret;
}

GateRef CircuitBuilder::DoubleToIntOverflowCheck(GateRef x, size_t typeBits)
{
    GateRef xInt64 = CastDoubleToInt64(x);
    // exp = (u64 & DOUBLE_EXPONENT_MASK) >> DOUBLE_SIGNIFICAND_SIZE - DOUBLE_EXPONENT_BIAS
    GateRef exp = Int64And(xInt64, Int64(base::DOUBLE_EXPONENT_MASK));
    exp = TruncInt64ToInt32(Int64LSR(exp, Int64(base::DOUBLE_SIGNIFICAND_SIZE)));
    exp = Int32Sub(exp, Int32(base::DOUBLE_EXPONENT_BIAS));
    GateRef bits = Int32(typeBits - 1);
    // exp < 32 - 1
    return Int32LessThan(exp, bits);
}

GateRef CircuitBuilder::TruncDoubleToInt(GateRef glue, GateRef x, size_t typeBits)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label exit(env_);
    Label overflow(env_);

    GateRef xInt = ChangeFloat64ToInt32(x);
    DEFVALUE(result, env_, VariableType::INT32(), xInt);
    BRANCH(DoubleToIntOverflowCheck(x, typeBits), &exit, &overflow);

    Bind(&overflow);
    {
        result = CallNGCRuntime(glue, RTSTUB_ID(DoubleToInt), Circuit::NullGate(), { x, IntPtr(typeBits) },
                                Circuit::NullGate());
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env_->SubCfgExit();
    return ret;
}

GateRef CircuitBuilder::SaturateTruncDoubleToInt32(GateRef glue, GateRef x)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label exit(env_);
    Label overflow(env_);

    GateRef xInt = ChangeFloat64ToInt32(x);
    DEFVALUE(result, env_, VariableType::INT32(), xInt);
    BRANCH(DoubleToIntOverflowCheck(x, base::INT32_BITS), &exit, &overflow);

    Bind(&overflow);
    {
        result = CallNGCRuntime(glue, RTSTUB_ID(SaturateTruncDoubleToInt32), Circuit::NullGate(), { x },
                                Circuit::NullGate());
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env_->SubCfgExit();
    return ret;
}

GateRef CircuitBuilder::DoubleCheckINFInRangeInt32(GateRef x)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label exit(env_);
    Label isInfinity(env_);
    Label positiveInf(env_);
    Label negativeInf(env_);

    DEFVALUE(result, env_, VariableType::INT32(), DoubleInRangeInt32(x));
    GateRef Max = Double(INT32_MAX);
    GateRef Min = Double(INT32_MIN);
    GateRef pInfinity = Double(base::POSITIVE_INFINITY);
    Branch(DoubleIsINF(x), &isInfinity, &exit);
    Bind(&isInfinity);
    {
        Branch(DoubleEqual(x, pInfinity), &positiveInf, &negativeInf);
        Bind(&positiveInf);
        {
            result = ChangeFloat64ToInt32(Max);
            Jump(&exit);
        }
        Bind(&negativeInf);
        {
            result = ChangeFloat64ToInt32(Min);
            Jump(&exit);
        }
    }
    Bind(&exit);
    auto ret = *result;
    env_->SubCfgExit();
    return ret;
}

GateRef CircuitBuilder::DoubleInRangeInt32(GateRef x)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label exit(env_);
    Label overflow(env_);
    Label checkUnderflow(env_);
    Label underflow(env_);

    DEFVALUE(result, env_, VariableType::INT32(), ChangeFloat64ToInt32(x));
    GateRef Max = Double(INT32_MAX);
    GateRef Min = Double(INT32_MIN);
    Branch(DoubleGreaterThan(x, Max), &overflow, &checkUnderflow);
    Bind(&overflow);
    {
        result = ChangeFloat64ToInt32(Max);
        Jump(&exit);
    }
    Bind(&checkUnderflow);
    {
        Branch(DoubleLessThan(x, Min), &underflow, &exit);
        Bind(&underflow);
        {
            result = ChangeFloat64ToInt32(Min);
            Jump(&exit);
        }
    }
    Bind(&exit);
    auto ret = *result;
    env_->SubCfgExit();
    return ret;
}

// According to C++ standard: "If the truncated value cannot fit into the destination type, the behavior is undefined"
// Thus it's necessary to do bound checking before converting back and forth.
GateRef CircuitBuilder::DoubleIsWithinInt32(GateRef x)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label noOverflow(env_);
    Label noUnderflow(env_);
    Label isWithinInt32(env_);
    Label exit(env_);
    DEFVALUE(result, env_, VariableType::BOOL(), False());

    GateRef limitMax = Double(INT32_MAX);
    GateRef limitMin = Double(INT32_MIN);
    Branch(DoubleLessThanOrEqual(x, limitMax), &noOverflow, &exit); // NaN, +inf -> exit
    Bind(&noOverflow);
    Branch(DoubleGreaterThanOrEqual(x, limitMin), &noUnderflow, &exit); // NaN, -inf -> exit
    Bind(&noUnderflow);
    GateRef xToInt32 = ChangeFloat64ToInt32(x);
    Branch(DoubleEqual(ChangeInt32ToFloat64(xToInt32), x), &isWithinInt32, &exit);
    Bind(&isWithinInt32);
    result.WriteVariable(True());
    Jump(&exit);

    Bind(&exit);
    auto ret = *result;
    env_->SubCfgExit();
    return ret;
}

GateRef CircuitBuilder::ThreeInt64Min(GateRef first, GateRef second, GateRef third)
{
    Label entry(env_);
    env_->SubCfgEntry(&entry);
    Label exit(env_);
    DEFVALUE(min, env_, VariableType::INT64(), first);
    Label useSecond(env_);
    Label next(env_);
    Branch(Int64GreaterThan(*min, second), &useSecond, &next);
    Bind(&useSecond);
    {
        min = second;
        Jump(&next);
    }
    Bind(&next);
    Label useThird(env_);
    Branch(Int64GreaterThan(*min, third), &useThird, &exit);
    Bind(&useThird);
    {
        min = third;
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *min;
    env_->SubCfgExit();
    return ret;
}
}
