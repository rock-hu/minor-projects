/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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


#include "ecmascript/compiler/typed_bytecode_lowering.h"

#include "ecmascript/compiler/circuit_builder_helper.h"
#include "ecmascript/compiler/variable_type.h"
#include "ecmascript/dfx/vmstat/opt_code_profiler.h"
#include "ecmascript/jit/jit.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_tagged_value_internals.h"

namespace panda::ecmascript::kungfu {
void TypedBytecodeLowering::RunTypedBytecodeLowering()
{
    std::vector<GateRef> gateList;
    circuit_->GetAllGates(gateList);
    ParseOptBytecodeRange();
    for (const auto &gate : gateList) {
        auto op = acc_.GetOpCode(gate);
        if (op == OpCode::JS_BYTECODE) {
            Lower(gate);
        }
    }

    if (IsTypeLogEnabled()) {
        pgoTypeLog_.PrintPGOTypeLog();
    }

    if (IsLogEnabled()) {
        LOG_COMPILER(INFO) << "";
        LOG_COMPILER(INFO) << "\033[34m"
                           << "===================="
                           << " After TypedBytecodeLowering "
                           << "[" << GetMethodName() << "]"
                           << "===================="
                           << "\033[0m";
        for (auto a : bytecodeMap_) {
            if (bytecodeHitTimeMap_.find(a.first) != bytecodeHitTimeMap_.end()) {
                double rate = static_cast<double>(bytecodeHitTimeMap_[a.first]) / static_cast<double>(a.second);
                LOG_COMPILER(INFO) << "\033[34m" << " =========================== End opHitRate: "
                                   << GetEcmaOpcodeStr(a.first) << " rate: " << std::to_string(rate)
                                   << "(" << std::to_string(bytecodeHitTimeMap_[a.first])
                                   << " / " << std::to_string(a.second) << ")"
                                   << " ===========================" << "\033[0m";
            } else {
                LOG_COMPILER(INFO) << "\033[34m" << " =========================== End opHitRate: "
                                   << GetEcmaOpcodeStr(a.first) << " rate: " << std::to_string(0)
                                   << "(" << std::to_string(0)
                                   << " / " << std::to_string(a.second) << ")"
                                   << " ===========================" << "\033[0m";
            }
        }
    }
}

void TypedBytecodeLowering::ParseOptBytecodeRange()
{
    std::vector<std::string> splitStrs = base::StringHelper::SplitString(optBCRange_, ",");
    for (const auto &optBCRange : splitStrs) {
        std::vector<std::string> splitRange = base::StringHelper::SplitString(optBCRange, ":");
        // 2:Used to determine whether the size of the split string array splitRange is as expected.
        if (splitRange.size() == 2) {
            std::vector<int32_t> range;
            std::string start = splitRange[0];
            std::string end = splitRange[1];
            uint32_t startNumber = std::strtoull(start.c_str(), nullptr, 10);
            uint32_t endNumber = std::strtoull(end.c_str(), nullptr, 10);
            range.push_back(static_cast<int32_t>(startNumber));
            range.push_back(static_cast<int32_t>(endNumber));
            optBCRangeList_.push_back(range);
        }
    }
}

bool TypedBytecodeLowering::CheckIsInOptBCIgnoreRange(int32_t index, EcmaOpcode ecmaOpcode)
{
    for (std::vector<int32_t> range : optBCRangeList_) {
        if (index >= range[0] && index <= range[1]) {
            LOG_COMPILER(INFO) << "TypedBytecodeLowering ignore opcode:" << GetEcmaOpcodeStr(ecmaOpcode);
            return true;
        }
    }
    return false;
}

void TypedBytecodeLowering::Lower(GateRef gate)
{
    [[maybe_unused]] auto scopedGate = circuit_->VisitGateBegin(gate);
    // not all opcode will visit heap, but now jit lock all opcode
    Jit::JitLockHolder lock(compilationEnv_, "TypedBytecodeLowering::Lower");

    EcmaOpcode ecmaOpcode = acc_.GetByteCodeOpcode(gate);
    // initialize label manager
    Environment env(gate, circuit_, &builder_);
    AddBytecodeCount(ecmaOpcode);
    // The order in the switch is referred to in ecmascript/compiler/ecma_opcode_des.h
    int32_t index = GetEcmaOpCodeListIndex(ecmaOpcode);
    if (optBCRangeList_.size() > 0 && CheckIsInOptBCIgnoreRange(index, ecmaOpcode)) {
        DeleteBytecodeCount(ecmaOpcode);
        allNonTypedOpCount_++;
        return;
    }
    switch (ecmaOpcode) {
        case EcmaOpcode::GETITERATOR_IMM8:
        case EcmaOpcode::GETITERATOR_IMM16:
            LowerGetIterator(gate);
            break;
        case EcmaOpcode::CREATEEMPTYOBJECT:
            LowerCreateEmptyObject(gate);
            break;
        case EcmaOpcode::CREATEOBJECTWITHBUFFER_IMM8_ID16:
        case EcmaOpcode::CREATEOBJECTWITHBUFFER_IMM16_ID16:
            LowerCreateObjectWithBuffer(gate);
            break;
        case EcmaOpcode::ADD2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_ADD>(gate);
            break;
        case EcmaOpcode::SUB2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_SUB>(gate);
            break;
        case EcmaOpcode::MUL2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_MUL>(gate);
            break;
        case EcmaOpcode::DIV2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_DIV>(gate);
            break;
        case EcmaOpcode::MOD2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_MOD>(gate);
            break;
        case EcmaOpcode::EQ_IMM8_V8:
            LowerTypedEqOrNotEq<TypedBinOp::TYPED_EQ>(gate);
            break;
        case EcmaOpcode::NOTEQ_IMM8_V8:
            LowerTypedEqOrNotEq<TypedBinOp::TYPED_NOTEQ>(gate);
            break;
        case EcmaOpcode::LESS_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_LESS>(gate);
            break;
        case EcmaOpcode::LESSEQ_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_LESSEQ>(gate);
            break;
        case EcmaOpcode::GREATER_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_GREATER>(gate);
            break;
        case EcmaOpcode::GREATEREQ_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_GREATEREQ>(gate);
            break;
        case EcmaOpcode::SHL2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_SHL>(gate);
            break;
        case EcmaOpcode::SHR2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_SHR>(gate);
            break;
        case EcmaOpcode::ASHR2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_ASHR>(gate);
            break;
        case EcmaOpcode::AND2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_AND>(gate);
            break;
        case EcmaOpcode::OR2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_OR>(gate);
            break;
        case EcmaOpcode::XOR2_IMM8_V8:
            LowerTypedBinOp<TypedBinOp::TYPED_XOR>(gate);
            break;
        case EcmaOpcode::TYPEOF_IMM8:
        case EcmaOpcode::TYPEOF_IMM16:
            LowerTypedTypeOf(gate);
            break;
        case EcmaOpcode::TONUMERIC_IMM8:
            LowerTypeToNumeric(gate);
            break;
        case EcmaOpcode::NEG_IMM8:
            LowerTypedUnOp<TypedUnOp::TYPED_NEG>(gate);
            break;
        case EcmaOpcode::NOT_IMM8:
            LowerTypedUnOp<TypedUnOp::TYPED_NOT>(gate);
            break;
        case EcmaOpcode::INC_IMM8:
            LowerTypedUnOp<TypedUnOp::TYPED_INC>(gate);
            break;
        case EcmaOpcode::DEC_IMM8:
            LowerTypedUnOp<TypedUnOp::TYPED_DEC>(gate);
            break;
        case EcmaOpcode::INSTANCEOF_IMM8_V8:
            LowerInstanceOf(gate);
            break;
        case EcmaOpcode::STRICTNOTEQ_IMM8_V8:
            LowerTypedEqOrNotEq<TypedBinOp::TYPED_STRICTNOTEQ>(gate);
            break;
        case EcmaOpcode::STRICTEQ_IMM8_V8:
            LowerTypedEqOrNotEq<TypedBinOp::TYPED_STRICTEQ>(gate);
            break;
        case EcmaOpcode::ISTRUE:
        case EcmaOpcode::CALLRUNTIME_ISTRUE_PREF_IMM8:
            LowerTypedIsTrueOrFalse(gate, true);
            break;
        case EcmaOpcode::ISFALSE:
        case EcmaOpcode::CALLRUNTIME_ISFALSE_PREF_IMM8:
            LowerTypedIsTrueOrFalse(gate, false);
            break;
        case EcmaOpcode::CALLARG0_IMM8:
            LowerTypedCallArg0(gate);
            break;
        case EcmaOpcode::CALLARG1_IMM8_V8:
            LowerTypedCallArg1(gate);
            break;
        case EcmaOpcode::CALLARGS2_IMM8_V8_V8:
            LowerTypedCallArg2(gate);
            break;
        case EcmaOpcode::CALLARGS3_IMM8_V8_V8_V8:
            LowerTypedCallArg3(gate);
            break;
        case EcmaOpcode::CALLRANGE_IMM8_IMM8_V8:
            LowerTypedCallrange(gate);
            break;
        case EcmaOpcode::CALLTHIS0_IMM8_V8:
            LowerTypedCallthis0(gate);
            break;
        case EcmaOpcode::CALLTHIS1_IMM8_V8_V8:
            LowerTypedCallthis1(gate);
            break;
        case EcmaOpcode::CALLTHIS2_IMM8_V8_V8_V8:
            LowerTypedCallthis2(gate);
            break;
        case EcmaOpcode::CALLTHIS3_IMM8_V8_V8_V8_V8:
            LowerTypedCallthis3(gate);
            break;
        case EcmaOpcode::CALLTHISRANGE_IMM8_IMM8_V8:
            LowerTypedCallthisrange(gate);
            break;
        case EcmaOpcode::CALLRUNTIME_CALLINIT_PREF_IMM8_V8:
            LowerTypedCallInit(gate);
            break;
        case EcmaOpcode::SUPERCALLTHISRANGE_IMM8_IMM8_V8:
        case EcmaOpcode::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8:
            LowerTypedSuperCall(gate);
            break;
        case EcmaOpcode::NEWOBJRANGE_IMM8_IMM8_V8:
        case EcmaOpcode::NEWOBJRANGE_IMM16_IMM8_V8:
        case EcmaOpcode::WIDE_NEWOBJRANGE_PREF_IMM16_V8:
            LowerTypedNewObjRange(gate);
            break;
        case EcmaOpcode::STPRIVATEPROPERTY_IMM8_IMM16_IMM16_V8:
            LowerTypedStPrivateProperty(gate);
            break;
        case EcmaOpcode::LDPRIVATEPROPERTY_IMM8_IMM16_IMM16:
            LowerTypedLdPrivateProperty(gate);
            break;
        case EcmaOpcode::LDOBJBYNAME_IMM8_ID16:
        case EcmaOpcode::LDOBJBYNAME_IMM16_ID16:
        case EcmaOpcode::LDTHISBYNAME_IMM8_ID16:
        case EcmaOpcode::LDTHISBYNAME_IMM16_ID16:
            LowerTypedLdObjByName(gate);
            break;
        case EcmaOpcode::STOBJBYNAME_IMM8_ID16_V8:
        case EcmaOpcode::STOBJBYNAME_IMM16_ID16_V8:
        case EcmaOpcode::STTHISBYNAME_IMM8_ID16:
        case EcmaOpcode::STTHISBYNAME_IMM16_ID16:
        case EcmaOpcode::DEFINEFIELDBYNAME_IMM8_ID16_V8:
        case EcmaOpcode::DEFINEPROPERTYBYNAME_IMM8_ID16_V8:
            LowerTypedStObjByName(gate);
            break;
        case EcmaOpcode::LDOBJBYVALUE_IMM8_V8:
        case EcmaOpcode::LDOBJBYVALUE_IMM16_V8:
        case EcmaOpcode::LDTHISBYVALUE_IMM8:
        case EcmaOpcode::LDTHISBYVALUE_IMM16:
            LowerTypedLdObjByValue(gate);
            break;
        case EcmaOpcode::STOBJBYVALUE_IMM8_V8_V8:
        case EcmaOpcode::STOBJBYVALUE_IMM16_V8_V8:
            LowerTypedStObjByValue(gate);
            break;
        case EcmaOpcode::STOWNBYVALUE_IMM8_V8_V8:
        case EcmaOpcode::STOWNBYVALUE_IMM16_V8_V8:
            LowerTypedStOwnByValue(gate);
            break;
        case EcmaOpcode::LDOBJBYINDEX_IMM8_IMM16:
        case EcmaOpcode::LDOBJBYINDEX_IMM16_IMM16:
        case EcmaOpcode::WIDE_LDOBJBYINDEX_PREF_IMM32:
            LowerTypedLdObjByIndex(gate);
            break;
        case EcmaOpcode::STOBJBYINDEX_IMM8_V8_IMM16:
        case EcmaOpcode::STOBJBYINDEX_IMM16_V8_IMM16:
        case EcmaOpcode::WIDE_STOBJBYINDEX_PREF_V8_IMM32:
            LowerTypedStObjByIndex(gate);
            break;
        case EcmaOpcode::TRYLDGLOBALBYNAME_IMM8_ID16:
        case EcmaOpcode::TRYLDGLOBALBYNAME_IMM16_ID16:
            LowerTypedTryLdGlobalByName(gate);
            break;
        case EcmaOpcode::STOWNBYNAME_IMM8_ID16_V8:
        case EcmaOpcode::STOWNBYNAME_IMM16_ID16_V8:
            LowerTypedStOwnByName(gate);
            break;
        case EcmaOpcode::JEQZ_IMM8:
        case EcmaOpcode::JEQZ_IMM16:
        case EcmaOpcode::JEQZ_IMM32:
            LowerConditionJump(gate, false);
            break;
        case EcmaOpcode::JNEZ_IMM8:
        case EcmaOpcode::JNEZ_IMM16:
        case EcmaOpcode::JNEZ_IMM32:
            LowerConditionJump(gate, true);
            break;
        default:
            DeleteBytecodeCount(ecmaOpcode);
            allNonTypedOpCount_++;
            break;
    }
}

int32_t TypedBytecodeLowering::GetEcmaOpCodeListIndex(EcmaOpcode ecmaOpCode)
{
    std::vector<EcmaOpcode> opcodeList = GetEcmaCodeListForRange();
    int32_t index =  static_cast<int32_t>(opcodeList.size());
    int32_t size = static_cast<int32_t>(opcodeList.size());
    for (int32_t i = 0; i < size; i++) {
        if (opcodeList[i] == ecmaOpCode) {
            index = i;
            break;
        }
    }
    if (index != size) {
        return index;
    } else {
        return -1;
    }
}

template<TypedBinOp Op>
void TypedBytecodeLowering::LowerTypedBinOp(GateRef gate)
{
    BinOpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    if (Op == TypedBinOp::TYPED_SHR && tacc.GetParamType().IsIntOverflowType()) {
        return;
    }
    if (tacc.HasNumberType()) {
        SpeculateNumbers<Op>(tacc);
    } else if (tacc.IsInternStringType()) {
        SpeculateInternStrings<Op>(tacc);
    } else if (tacc.IsStringType()) {
        SpeculateStrings<Op>(tacc);
    }
}

template<TypedUnOp Op>
void TypedBytecodeLowering::LowerTypedUnOp(GateRef gate)
{
    UnOpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    // NOTICE-PGO: wx add support for PrimitiveNumberType
    if (Op == TypedUnOp::TYPED_NEG && tacc.GetParamType().IsIntOverflowType()) {
        return;
    }
    if (tacc.HasNumberType()) {
        SpeculateNumber<Op>(tacc);
    }
}

template<TypedBinOp Op>
void TypedBytecodeLowering::LowerTypedEqOrNotEq(GateRef gate)
{
    BinOpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    if (tacc.LeftOrRightIsUndefinedOrNull()) {
        AddProfiling(gate);
        GateRef left = tacc.GetLeftGate();
        GateRef right = tacc.GetReightGate();
        GateRef result = builder_.TypedBinaryOp<Op>(left, right, tacc.GetParamType());
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
    } else {
        LowerTypedBinOp<Op>(gate);
    }
}

GateRef TypedBytecodeLowering::InternStringCheck(GateRef value)
{
    // Marking a string as an Intern String is a runtime behavior.
    // The tag of Intern String in JIT process will not become invalid and can be used during compilation.
    if (compilationEnv_->IsJitCompiler()) {
        // We believe that the Intern String tag of a constant string will not change during interpreted execution,
        // JIT compilation, and JIT code execution. Therefore, the check for the constant Intern String can be removed
        // during JIT compilation.
        JSTaggedValue stringObject = JSTaggedValue::Undefined();
        if (acc_.GetOpCode(value) == OpCode::JS_BYTECODE && acc_.GetByteCodeOpcode(value) == EcmaOpcode::LDA_STR_ID16) {
            uint16_t stringIndex = acc_.GetConstantValue(acc_.GetValueIn(value, 0));
            auto methodOffset = acc_.TryGetMethodOffset(value);
            stringObject = compilationEnv_->GetStringFromConstantPool(methodOffset, stringIndex);
        }
        if (stringObject.IsHeapObject()) {
            EcmaString* ecmaString = EcmaString::Cast(stringObject.GetTaggedObject());
            EcmaStringAccessor ecmaStringAccessor(ecmaString);
            if (ecmaStringAccessor.IsInternString()) {
                return builder_.True();
            }
        }
    }
    return builder_.InternStringCheck(value);
}

template<TypedBinOp Op>
void TypedBytecodeLowering::SpeculateInternStrings(const BinOpTypeInfoAccessor &tacc)
{
    if (Op != TypedBinOp::TYPED_STRICTEQ && Op != TypedBinOp::TYPED_STRICTNOTEQ) {
        SpeculateStrings<Op>(tacc);
        return;
    }
    AddProfiling(tacc.GetGate());
    GateRef left = tacc.GetLeftGate();
    GateRef right = tacc.GetReightGate();
    InternStringCheck(left);
    InternStringCheck(right);
    GateRef result = builder_.TypedBinaryOp<Op>(left, right, tacc.GetParamType());
    acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
}

template<TypedBinOp Op>
void TypedBytecodeLowering::SpeculateStrings(const BinOpTypeInfoAccessor &tacc)
{
    if (Op == TypedBinOp::TYPED_EQ || Op == TypedBinOp::TYPED_ADD) {
        AddProfiling(tacc.GetGate());
        GateRef left = tacc.GetLeftGate();
        GateRef right = tacc.GetReightGate();
        if (!TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, left)) {
            if (!Uncheck()) {
                builder_.EcmaStringCheck(left);
            }
        }
        if (!TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, right)) {
            if (!Uncheck()) {
                builder_.EcmaStringCheck(right);
            }
        }
        GateRef result = builder_.TypedBinaryOp<Op>(left, right, tacc.GetParamType());
        acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
    }
}

template<TypedBinOp Op>
void TypedBytecodeLowering::SpeculateNumbers(const BinOpTypeInfoAccessor &tacc)
{
    AddProfiling(tacc.GetGate());
    pgoTypeLog_.CollectGateTypeLogInfo(tacc.GetGate(), true);
    GateRef left = tacc.GetLeftGate();
    GateRef right = tacc.GetReightGate();
    GateRef result = builder_.TypedBinaryOp<Op>(left, right, tacc.GetParamType());
    acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
}

template<TypedUnOp Op>
void TypedBytecodeLowering::SpeculateNumber(const UnOpTypeInfoAccessor &tacc)
{
    AddProfiling(tacc.GetGate());
    pgoTypeLog_.CollectGateTypeLogInfo(tacc.GetGate(), false);
    GateRef result = builder_.TypedUnaryOp<Op>(tacc.GetValue(), tacc.GetParamType());
    acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
}

GateRef CheckedNumberToString(CircuitBuilder *builder, GateRef numOrStr, Label *exit)
{
    auto isNum = builder->TaggedIsNumber(numOrStr);
    Label numberBranch(builder);
    Label notNumberBranch(builder);

    DEFVALUE(res, builder, VariableType::JS_ANY(), numOrStr);
    builder->Branch(isNum, &numberBranch, &notNumberBranch, BranchWeight::ONE_WEIGHT, BranchWeight::ONE_WEIGHT,
                    "IsNumber");
    builder->Bind(&numberBranch);
    {
        res = builder->NumberToString(numOrStr);
        builder->Jump(exit);
    }
    builder->Bind(&notNumberBranch);
    {
        builder->EcmaStringCheck(numOrStr);
        res = numOrStr;
        builder->Jump(exit);
    }
    builder->Bind(exit);

    return *res;
}

template<TypedBinOp Op>
void TypedBytecodeLowering::SpeculateNumbersOrString(const BinOpTypeInfoAccessor &tacc)
{
    if (Op == TypedBinOp::TYPED_ADD) {
        AddProfiling(tacc.GetGate());
        GateRef left = tacc.GetLeftGate();
        GateRef right = tacc.GetReightGate();

        Label exit(&builder_);
        if (TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, left)) {
            right = CheckedNumberToString(&builder_, right, &exit);
            ASSERT(tacc.GetParamType() == ParamType::StringType());
            GateRef result = builder_.TypedBinaryOp<Op>(left, right, ParamType::StringType());
            acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
        } else if (TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, right)) {
            left = CheckedNumberToString(&builder_, left, &exit);
            ASSERT(tacc.GetParamType() == ParamType::StringType());
            GateRef result = builder_.TypedBinaryOp<Op>(left, right, ParamType::StringType());
            acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
        }
    }
}

void TypedBytecodeLowering::LowerTypeToNumeric(GateRef gate)
{
    UnOpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    if (tacc.HasNumberType()) {
        AddProfiling(gate);
        LowerPrimitiveTypeToNumber(tacc);
    }
}

void TypedBytecodeLowering::LowerPrimitiveTypeToNumber(const UnOpTypeInfoAccessor &tacc)
{
    GateRef result = builder_.PrimitiveToNumber(tacc.GetValue(), tacc.GetParamType());
    acc_.ReplaceHirAndDeleteIfException(tacc.GetGate(), builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerConditionJump(GateRef gate, bool flag)
{
    ConditionJumpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    if (TypeInfoAccessor::IsTrustedBooleanType(acc_, tacc.GetValue())) {
        AddProfiling(gate);
        SpeculateConditionJump(tacc, flag);
    }
}

void TypedBytecodeLowering::SpeculateConditionJump(const ConditionJumpTypeInfoAccessor &tacc, bool flag)
{
    GateRef value = tacc.GetValue();
    ParamType paramType = ParamType::BooleanType();
    uint32_t weight = tacc.GetBranchWeight();
    GateRef jump = Circuit::NullGate();
    if (flag) {
        jump = builder_.TypedConditionJump<TypedJumpOp::TYPED_JNEZ>(value, paramType, weight);
    } else {
        jump = builder_.TypedConditionJump<TypedJumpOp::TYPED_JEQZ>(value, paramType, weight);
    }
    acc_.ReplaceGate(tacc.GetGate(), jump, jump, Circuit::NullGate());
}

void TypedBytecodeLowering::DeleteConstDataIfNoUser(GateRef gate)
{
    auto uses = acc_.Uses(gate);
    if (uses.begin() == uses.end()) {
        builder_.ClearConstantCache(gate);
        acc_.DeleteGate(gate);
    }
}

static void GeneratePrimitiveTypeCheck(CircuitBuilder &builder, GateRef receiver, PrimitiveType primitiveType)
{
    switch (primitiveType) {
        case PrimitiveType::PRIMITIVE_BOOLEAN:
            builder.TryPrimitiveTypeCheck(GateType::BooleanType(), receiver);
            break;
        case PrimitiveType::PRIMITIVE_NUMBER:
            builder.TryPrimitiveTypeCheck(GateType::NumberType(), receiver);
            break;
        default:
            LOG_ECMA(FATAL) << "Implementation Error: Unreachable branch.";
            UNREACHABLE();
            break;
    }
}

GateRef TypedBytecodeLowering::GetPrimitiveTypeProto(PrimitiveType primitiveType)
{
    size_t index = -1;
    if (primitiveType == PrimitiveType::PRIMITIVE_BOOLEAN) {
        index = GlobalEnv::BOOLEAN_PROTOTYPE_INDEX;
    } else if (primitiveType == PrimitiveType::PRIMITIVE_NUMBER) {
        index = GlobalEnv::NUMBER_PROTOTYPE_INDEX;
    } else {
        LOG_ECMA(FATAL) << "Implementation Error: Unreachable branch, not supported primitiveType currently";
        UNREACHABLE();
    }
    ASSERT(index != static_cast<size_t>(-1));
    return builder_.GetGlobalEnvValue(VariableType::JS_ANY(), glue_, builder_.GetGlobalEnv(), index);
}

void TypedBytecodeLowering::PolyPrimitiveTypeCheckAndLoad(LoadObjByNameDataInfo &info,
    std::map<size_t, uint32_t> &typeIndex2HeapConstantIndex)
{
    GateRef receiver = info.tacc.GetReceiver();
    auto itr = typeIndex2HeapConstantIndex.begin();
    size_t typeCount = info.tacc.GetTypeCount();
    GateRef gate = info.tacc.GetGate();
    GateRef frameState = acc_.GetFrameState(gate);
    for (size_t i = 0; i < typeIndex2HeapConstantIndex.size(); ++i) {
        auto primitiveType = info.tacc.GetPrimitiveType(itr->first);
        // only boolean and number are supported currently
        ASSERT(primitiveType == PrimitiveType::PRIMITIVE_BOOLEAN || primitiveType == PrimitiveType::PRIMITIVE_NUMBER);
        Label isDesignatePrimitiveType(&builder_);
        if (i != typeCount - 1) {
            if (primitiveType == PrimitiveType::PRIMITIVE_NUMBER) {
                BRANCH_CIR(builder_.TaggedIsNumber(receiver), &isDesignatePrimitiveType, &info.fails[i]);
            } else {
                BRANCH_CIR(builder_.TaggedIsBoolean(receiver), &isDesignatePrimitiveType, &info.fails[i]);
            }
        } else {
            GeneratePrimitiveTypeCheck(builder_, receiver, primitiveType);
            builder_.Jump(&isDesignatePrimitiveType);
        }
        builder_.Bind(&isDesignatePrimitiveType);
        GateRef protoType = GetPrimitiveTypeProto(primitiveType);
        builder_.PrimitiveTypeProtoChangeMarkerCheck(protoType, frameState);
        GateRef protoConstant = builder_.HeapConstant(itr->second);
        info.result = BuildNamedPropertyAccess(gate, receiver, protoConstant, info.tacc.GetAccessInfo(i).Plr());
        builder_.Jump(&info.exit);

        if (i != typeCount - 1) {
            builder_.Bind(&info.fails[i]);
        }
        itr++;
    }
}

void TypedBytecodeLowering::LoadOnPrototypeForHeapObjectReceiver(const LoadObjPropertyTypeInfoAccessor &tacc,
    Variable &result, LoadObjByNameOnProtoTypeInfo ldProtoInfo)
{
    auto gate = tacc.GetGate();

    // prototype change marker check
    if (!TryLazyDeoptStableProtoChain(tacc, ldProtoInfo.typeIndex, gate)) {
        builder_.ProtoChangeMarkerCheck(tacc.GetReceiver(), ldProtoInfo.frameState);
    }
    // lookup from receiver for holder
    auto prototype = builder_.LoadConstOffset(VariableType::JS_ANY(),
                                              ldProtoInfo.receiverHC, JSHClass::PROTOTYPE_OFFSET);
    // lookup from receiver for holder
    ObjectAccessTypeInfoAccessor::ObjectAccessInfo info = tacc.GetAccessInfo(ldProtoInfo.typeIndex);
    bool protoConstantFound = false;
    if (compilationEnv_->SupportHeapConstant()) {
        auto *jitCompilationEnv = static_cast<JitCompilationEnv*>(compilationEnv_);
        const auto &holderHClassIndex2HeapConstantIndex =
            jitCompilationEnv->GetHolderHClassIndex2HeapConstantIndex();
        auto holderHClassIndex = info.HClassIndex();
        auto itr = holderHClassIndex2HeapConstantIndex.find(holderHClassIndex);
        if (itr != holderHClassIndex2HeapConstantIndex.end()) {
            // holder is HeapConstant, which is recorded at jit profiler. so don't need to visit protochain
            GateRef protoConstant = builder_.HeapConstant(itr->second);
            result = BuildNamedPropertyAccess(
                gate, tacc.GetReceiver(), protoConstant, tacc.GetAccessInfo(ldProtoInfo.typeIndex).Plr());
            builder_.Jump(ldProtoInfo.exit);
            protoConstantFound = true;
        }
    }
    if (!protoConstantFound) {
        auto holderHC = builder_.GetHClassGateFromIndex(gate, info.HClassIndex());
        DEFVALUE(current, (&builder_), VariableType::JS_ANY(), prototype);
        Label loopHead(&builder_);
        Label loadHolder(&builder_);
        Label lookUpProto(&builder_);
        builder_.Jump(&loopHead);

        builder_.LoopBegin(&loopHead);
        builder_.DeoptCheck(builder_.TaggedIsNotNull(*current),
                            ldProtoInfo.frameState, DeoptType::INCONSISTENTHCLASS2);
        auto curHC = builder_.LoadHClassByConstOffset(glue_, *current);
        BRANCH_CIR(builder_.Equal(curHC, holderHC), &loadHolder, &lookUpProto);

        builder_.Bind(&lookUpProto);
        current = builder_.LoadConstOffset(VariableType::JS_ANY(), curHC, JSHClass::PROTOTYPE_OFFSET);
        builder_.LoopEnd(&loopHead);

        builder_.Bind(&loadHolder);
        result = BuildNamedPropertyAccess(gate, tacc.GetReceiver(),
                                          *current, tacc.GetAccessInfo(ldProtoInfo.typeIndex).Plr());
        builder_.Jump(ldProtoInfo.exit);
    }
}

void TypedBytecodeLowering::LowerTypedMonoLdObjByNameOnProto(const LoadObjPropertyTypeInfoAccessor &tacc,
                                                             Variable &result)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef gate = tacc.GetGate();
    PropertyLookupResult plr = tacc.GetAccessInfo(0).Plr();
    GateRef plrGate = builder_.Int32(plr.GetData());
    GateRef unsharedConstPool = argAcc_.GetFrameArgsIn(gate, FrameArgIdx::UNSHARED_CONST_POOL);
    size_t holderHClassIndex = static_cast<size_t>(tacc.GetAccessInfo(0).HClassIndex());
    if (LIKELY(!plr.IsAccessor())) {
        result = builder_.MonoLoadPropertyOnProto(receiver, plrGate, unsharedConstPool, holderHClassIndex);
    } else {
        result = builder_.MonoCallGetterOnProto(gate, receiver, plrGate, unsharedConstPool, holderHClassIndex);
    }
}

void TypedBytecodeLowering::LowerTypedMonoLdObjByName(const LoadObjPropertyTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef gate = tacc.GetGate();
    PrimitiveType primitiveType = tacc.GetPrimitiveType(0);
    DEFVALUE(result, (&builder_), VariableType::JS_ANY(), builder_.Hole());
    GateRef frameState = acc_.GetFrameState(gate);
    if ((primitiveType != PrimitiveType::PRIMITIVE_TYPE_INVALID) && compilationEnv_->SupportHeapConstant()) {
        Label notPrimitive(&builder_);
        Label isPrimitive(&builder_);
        if (primitiveType == PrimitiveType::PRIMITIVE_NUMBER) {
            Label isNumber(&builder_);
            BRANCH_CIR(builder_.TaggedIsNumber(receiver), &isNumber, &notPrimitive);
            builder_.Bind(&isNumber);
            GateRef protoType = GetPrimitiveTypeProto(PrimitiveType::PRIMITIVE_NUMBER);
            builder_.PrimitiveTypeProtoChangeMarkerCheck(protoType, frameState);
            builder_.Jump(&isPrimitive);
        } else {
            ASSERT(primitiveType == PrimitiveType::PRIMITIVE_BOOLEAN);
            Label isBoolean(&builder_);
            BRANCH_CIR(builder_.TaggedIsBoolean(receiver), &isBoolean, &notPrimitive);
            builder_.Bind(&isBoolean);
            GateRef prototype = GetPrimitiveTypeProto(PrimitiveType::PRIMITIVE_BOOLEAN);
            builder_.PrimitiveTypeProtoChangeMarkerCheck(prototype, frameState);
            builder_.Jump(&isPrimitive);
        }
        builder_.Bind(&notPrimitive);
        {
            builder_.ObjectTypeCheck(false, receiver, tacc.GetExpectedHClassIndexList(0), frameState);
            builder_.ProtoChangeMarkerCheck(receiver, frameState);
            builder_.Jump(&isPrimitive);
        }
        builder_.Bind(&isPrimitive);
        LowerTypedMonoLdObjByNameOnProto(tacc, result);
    } else {
        builder_.ObjectTypeCheck(false, receiver, tacc.GetExpectedHClassIndexList(0), frameState);
        if (tacc.IsReceiverEqHolder(0)) {
            result = BuildNamedPropertyAccess(gate, receiver, receiver, tacc.GetAccessInfo(0).Plr());
        } else {
            if (!TryLazyDeoptStableProtoChain(tacc, 0, gate)) {
                builder_.ProtoChangeMarkerCheck(receiver, frameState);
            }
            LowerTypedMonoLdObjByNameOnProto(tacc, result);
        }
    }
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), *result);
    DeleteConstDataIfNoUser(tacc.GetKey());
}

void TypedBytecodeLowering::GenerateMergedHClassListCheck(LoadObjByNameDataInfo &info, Label &hclassCheckExit,
    Variable &checkResult, size_t index, GateRef receiverHC)
{
    std::vector<Label> ifFalse;
    Label resultIsTrue(&builder_);
    Label resultIsFalse(&builder_);
    const auto& hclassList = info.tacc.GetExpectedHClassIndexList(index);
    size_t hclassCount = hclassList.size();

    for (size_t j = 0; j < hclassCount - 1; j++) {
        ifFalse.emplace_back(Label(&builder_));
    }
    for (size_t j = 0; j < hclassCount; j++) {
        auto expected = builder_.GetHClassGateFromIndex(info.tacc.GetGate(), hclassList[j]);
        if (j != hclassCount - 1) {
            BRANCH_CIR(builder_.Equal(receiverHC, expected), &resultIsTrue, &ifFalse[j]);
            builder_.Bind(&ifFalse[j]);
        } else {
            BRANCH_CIR(builder_.Equal(receiverHC, expected), &resultIsTrue, &resultIsFalse);
        }
    }
    builder_.Bind(&resultIsFalse);
    {
        checkResult = builder_.Boolean(false);
        builder_.Jump(&hclassCheckExit);
    }
    builder_.Bind(&resultIsTrue);
    {
        checkResult = builder_.Boolean(true);
        builder_.Jump(&hclassCheckExit);
    }
}

void TypedBytecodeLowering::PolyHeapObjectCheckAndLoad(LoadObjByNameDataInfo &info,
    const std::map<size_t, uint32_t> &typeIndex2HeapConstantIndex)
{
    GateRef gate = info.tacc.GetGate();
    GateRef frameState = acc_.GetFrameState(gate);
    builder_.HeapObjectCheck(info.tacc.GetReceiver(), frameState);
    auto receiverHC = builder_.LoadHClassByConstOffset(glue_, info.tacc.GetReceiver());
    size_t labelIndex = typeIndex2HeapConstantIndex.size();
    size_t typeCount = info.tacc.GetTypeCount();
    for (size_t i = 0; i < typeCount; ++i) {
        if (typeIndex2HeapConstantIndex.find(i) != typeIndex2HeapConstantIndex.end()) {
            continue;
        }
        Label hclassCheckExit(&builder_);
        DEFVALUE(checkResult, (&builder_), VariableType::BOOL(), builder_.Boolean(false));
        GenerateMergedHClassListCheck(info, hclassCheckExit, checkResult, i, receiverHC);
        builder_.Bind(&hclassCheckExit);
        if (labelIndex != typeCount - 1) {
            BRANCH_CIR(*checkResult, &info.loaders[labelIndex], &info.fails[labelIndex]);
            builder_.Bind(&info.loaders[labelIndex]);
        } else {
            // Deopt if fails at last hclass compare
            builder_.DeoptCheck(*checkResult, frameState, DeoptType::INCONSISTENTHCLASS1);
        }

        if (info.tacc.IsReceiverEqHolder(i)) {
            info.result = BuildNamedPropertyAccess(gate, info.tacc.GetReceiver(), info.tacc.GetReceiver(),
                                                   info.tacc.GetAccessInfo(i).Plr());
            builder_.Jump(&info.exit);
        } else {
            LoadOnPrototypeForHeapObjectReceiver(info.tacc, info.result,
                { gate, frameState, receiverHC, &info.exit, i });
        }
        if (labelIndex != typeCount - 1) {
            builder_.Bind(&info.fails[labelIndex]);
        }
        labelIndex++;
    }
}

void TypedBytecodeLowering::LowerTypedPolyLdObjByName(const LoadObjPropertyTypeInfoAccessor &tacc)
{
    DEFVALUE(result, (&builder_), VariableType::JS_ANY(), builder_.Hole());
    size_t typeCount = tacc.GetTypeCount();
    std::vector<Label> loaders;
    std::vector<Label> fails;
    ASSERT(typeCount > 0);
    for (size_t i = 0; i < typeCount - 1; ++i) {
        loaders.emplace_back(Label(&builder_));
        fails.emplace_back(Label(&builder_));
    }
    Label exit(&builder_);
    GateRef gate = tacc.GetGate();
    std::map<size_t, uint32_t> typeIndex2HeapConstantIndex = tacc.CollectPrimitiveTypeInfo(compilationEnv_);
    LoadObjByNameDataInfo info(loaders, fails, tacc, result, exit);
    if (!typeIndex2HeapConstantIndex.empty()) {
        PolyPrimitiveTypeCheckAndLoad(info, typeIndex2HeapConstantIndex);
    }

    if (typeIndex2HeapConstantIndex.size() < typeCount) {
        PolyHeapObjectCheckAndLoad(info, typeIndex2HeapConstantIndex);
    }
    builder_.Bind(&exit);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), *result);
    DeleteConstDataIfNoUser(tacc.GetKey());
}

void TypedBytecodeLowering::LowerTypedLdObjByName(GateRef gate)
{
    LoadObjPropertyTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);

    if (TryLowerTypedLdobjBynameFromGloablBuiltin(gate)) {
        return;
    }
    if (TryLowerTypedLdObjByNameForBuiltin(gate)) {
        return;
    }
    if (tacc.TypesIsEmpty() || tacc.HasIllegalType()) {
        return;
    }
    if (enableMergePoly_) {
        tacc.TryMergeExpectedHClass();
    }
    AddProfiling(gate);
    if (tacc.IsMono()) {
        LowerTypedMonoLdObjByName(tacc);
        return;
    }
    LowerTypedPolyLdObjByName(tacc);
}

void TypedBytecodeLowering::LowerTypedLdPrivateProperty(GateRef gate)
{
    LoadPrivatePropertyTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);

    if (tacc.HasIllegalType()) {
        return;
    }

    AddProfiling(gate);
    Label exit(&builder_);

    GateRef receiver = tacc.GetReceiver();
    GateRef levelIndex = tacc.GetLevelIndex();
    GateRef slotIndex = tacc.GetSlotIndex();

    DEFVALUE(result, (&builder_), VariableType::JS_ANY(), builder_.Undefined());
    GateRef frameState = acc_.FindNearestFrameState(builder_.GetDepend());
    GateRef key = builder_.GetKeyFromLexivalEnv(glue_,
        tacc.GetLexicalEnv(), builder_.TaggedGetInt(levelIndex), builder_.TaggedGetInt(slotIndex));

    builder_.HeapObjectCheck(key, frameState);
    if (tacc.IsAccessor()) {
        builder_.DeoptCheck(builder_.IsJSFunction(glue_, key), frameState, DeoptType::NOTJSFUNCTION);
        result = builder_.CallPrivateGetter(gate, receiver, key);
        builder_.Jump(&exit);
    } else {
        builder_.DeoptCheck(builder_.TaggedIsSymbol(glue_, key), frameState, DeoptType::NOTSYMBOL);
        builder_.ObjectTypeCheck(false, receiver, builder_.Int32(tacc.GetExpectedHClassIndex(0)), frameState);
        result = BuildNamedPropertyAccess(gate, receiver, receiver, tacc.GetAccessInfo(0).Plr());
        builder_.Jump(&exit);
    }

    builder_.Bind(&exit);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), *result);
    DeleteConstDataIfNoUser(key);
}

void TypedBytecodeLowering::LowerTypedStPrivateProperty(GateRef gate)
{
    StorePrivatePropertyTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);

    if (tacc.HasIllegalType()) {
        return;
    }

    AddProfiling(gate);
    Label exit(&builder_);

    GateRef receiver = tacc.GetReceiver();
    GateRef levelIndex = tacc.GetLevelIndex();
    GateRef slotIndex = tacc.GetSlotIndex();
    GateRef value = tacc.GetValue();

    GateRef frameState = acc_.FindNearestFrameState(builder_.GetDepend());
    GateRef key = builder_.GetKeyFromLexivalEnv(glue_,
        tacc.GetLexicalEnv(), builder_.TaggedGetInt(levelIndex), builder_.TaggedGetInt(slotIndex));

    builder_.HeapObjectCheck(key, frameState);
    if (tacc.IsAccessor()) {
        builder_.DeoptCheck(builder_.IsJSFunction(glue_, key), frameState, DeoptType::NOTJSFUNCTION);
        builder_.CallPrivateSetter(gate, receiver, key, value);
        builder_.Jump(&exit);
    } else {
        builder_.DeoptCheck(builder_.TaggedIsSymbol(glue_, key), frameState, DeoptType::NOTSYMBOL);
        builder_.ObjectTypeCheck(false, receiver, builder_.Int32(tacc.GetExpectedHClassIndex(0)), frameState);
        BuildNamedPropertyAccess(
            gate, receiver, receiver, value, tacc.GetAccessInfo(0).Plr(), tacc.GetExpectedHClassIndex(0));
        builder_.Jump(&exit);
    }

    builder_.Bind(&exit);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
    DeleteConstDataIfNoUser(key);
}

static void RecordGate2HeapConstantIndex(CompilationEnv *compilationEnv, uint32_t methodOffset,
                                         uint32_t keyIndex, GateRef resultGate)
{
    if (!compilationEnv->SupportHeapConstant()) {
        return;
    }
    auto *jitCompilationEnv = static_cast<JitCompilationEnv*>(compilationEnv);
    JSTaggedValue strObj = jitCompilationEnv->GetStringFromConstantPool(methodOffset, keyIndex, false);
    if (strObj.IsUndefined()) {
        return;
    }
    JSHandle<JSTaggedValue> strObjHandle = jitCompilationEnv->NewJSHandle(strObj);
    auto constpool = jitCompilationEnv->GetConstantPoolByMethodOffset(methodOffset);
    ASSERT(!constpool.IsUndefined());
    auto constpoolId = static_cast<uint32_t>(
        ConstantPool::Cast(constpool.GetTaggedObject())->GetSharedConstpoolId().GetInt());
    uint32_t indexInConstantTable = jitCompilationEnv->RecordHeapConstant(
        { constpoolId, keyIndex, JitCompilationEnv::IN_SHARED_CONSTANTPOOL }, strObjHandle);
    jitCompilationEnv->RecordGate2HeapConstantIndex(resultGate, indexInConstantTable);
}

void TypedBytecodeLowering::LowerTypedStObjByName(GateRef gate)
{
    StoreObjByNameTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    if (tacc.TypesIsEmpty() || tacc.HasIllegalType()) {
        return;
    }
    size_t typeCount = tacc.GetTypeCount();
    std::vector<Label> loaders;
    std::vector<Label> fails;
    ASSERT(typeCount > 0);
    for (size_t i = 0; i < typeCount - 1; ++i) {
        loaders.emplace_back(Label(&builder_));
        fails.emplace_back(Label(&builder_));
    }
    Label exit(&builder_);
    AddProfiling(gate);
    GateRef frameState = Circuit::NullGate();
    auto opcode = acc_.GetByteCodeOpcode(gate);
    // The framestate of Call and Accessor related instructions directives is placed on IR. Using the depend edge to
    // climb up and find the nearest framestate for other instructions
    if (opcode == EcmaOpcode::STOWNBYNAME_IMM8_ID16_V8 ||
        opcode == EcmaOpcode::STOWNBYNAME_IMM16_ID16_V8) {
        frameState = acc_.FindNearestFrameState(builder_.GetDepend());
    } else if (opcode == EcmaOpcode::STOBJBYNAME_IMM8_ID16_V8 ||
               opcode == EcmaOpcode::STOBJBYNAME_IMM16_ID16_V8 ||
               opcode == EcmaOpcode::STTHISBYNAME_IMM8_ID16 ||
               opcode == EcmaOpcode::STTHISBYNAME_IMM16_ID16 ||
               opcode == EcmaOpcode::DEFINEFIELDBYNAME_IMM8_ID16_V8 ||
               opcode == EcmaOpcode::DEFINEPROPERTYBYNAME_IMM8_ID16_V8) {
        frameState = acc_.GetFrameState(gate);
    } else {
        UNREACHABLE();
    }
    if (tacc.IsMono()) {
        GateRef receiver = tacc.GetReceiver();
        builder_.ObjectTypeCheck(false, receiver,
                                 builder_.Int32(tacc.GetExpectedHClassIndex(0)), frameState);
        if (tacc.IsReceiverNoEqNewHolder(0)) {
            if (!TryLazyDeoptStableProtoChain(tacc, 0, gate)) {
                builder_.ProtoChangeMarkerCheck(receiver, frameState);
            }
            PropertyLookupResult plr = tacc.GetAccessInfo(0).Plr();
            GateRef plrGate = builder_.Int32(plr.GetData());
            GateRef unsharedConstPool = argAcc_.GetFrameArgsIn(gate, FrameArgIdx::UNSHARED_CONST_POOL);
            size_t holderHClassIndex = static_cast<size_t>(tacc.GetAccessInfo(0).HClassIndex());
            GateRef value = tacc.GetValue();
            if (tacc.IsHolderEqNewHolder(0)) {
                builder_.MonoStorePropertyLookUpProto(tacc.GetReceiver(), plrGate, unsharedConstPool, holderHClassIndex,
                                                      value);
            } else {
                GateRef ret = builder_.MonoStoreProperty(tacc.GetReceiver(), plrGate, unsharedConstPool,
                    holderHClassIndex, value, builder_.TruncInt64ToInt32(tacc.GetKey()),
                    builder_.Boolean(tacc.IsPrototypeHclass(0)), frameState);
                auto methodOffset = acc_.TryGetMethodOffset(gate);
                uint32_t keyIndex = static_cast<uint32_t>(acc_.GetConstantValue(tacc.GetKey()));
                RecordGate2HeapConstantIndex(compilationEnv_, methodOffset, keyIndex, ret);
            }
        } else if (tacc.IsReceiverEqHolder(0)) {
            BuildNamedPropertyAccess(gate, tacc.GetReceiver(), tacc.GetReceiver(),
                                     tacc.GetValue(), tacc.GetAccessInfo(0).Plr(), tacc.GetExpectedHClassIndex(0));
        }
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
        DeleteConstDataIfNoUser(tacc.GetKey());
        return;
    }
    builder_.HeapObjectCheck(tacc.GetReceiver(), frameState);
    auto receiverHC = builder_.LoadHClassByConstOffset(glue_, tacc.GetReceiver());
    for (size_t i = 0; i < typeCount; ++i) {
        auto expected = builder_.GetHClassGateFromIndex(gate, tacc.GetExpectedHClassIndex(i));
        if (i != typeCount - 1) {
            BRANCH_CIR(builder_.Equal(receiverHC, expected),
                &loaders[i], &fails[i]);
            builder_.Bind(&loaders[i]);
        } else {
            builder_.DeoptCheck(builder_.Equal(receiverHC, expected), frameState, DeoptType::INCONSISTENTHCLASS3);
        }
        if (tacc.IsReceiverNoEqNewHolder(i)) {
            if (!TryLazyDeoptStableProtoChain(tacc, i, gate)) {
                builder_.ProtoChangeMarkerCheck(tacc.GetReceiver(), frameState);
            }
            if (tacc.IsHolderEqNewHolder(i)) {
                // lookup from receiver for holder
                auto prototype = builder_.LoadConstOffset(VariableType::JS_ANY(), receiverHC,
                                                          JSHClass::PROTOTYPE_OFFSET);
                auto holderHC = builder_.GetHClassGateFromIndex(gate, tacc.GetAccessInfo(i).HClassIndex());
                DEFVALUE(current, (&builder_), VariableType::JS_ANY(), prototype);
                Label loopHead(&builder_);
                Label loadHolder(&builder_);
                Label lookUpProto(&builder_);
                builder_.Jump(&loopHead);

                builder_.LoopBegin(&loopHead);
                builder_.DeoptCheck(builder_.TaggedIsNotNull(*current), frameState, DeoptType::INCONSISTENTHCLASS4);
                auto curHC = builder_.LoadHClassByConstOffset(glue_, *current);
                BRANCH_CIR(builder_.Equal(curHC, holderHC), &loadHolder, &lookUpProto);

                builder_.Bind(&lookUpProto);
                current = builder_.LoadConstOffset(VariableType::JS_ANY(), curHC, JSHClass::PROTOTYPE_OFFSET);
                builder_.LoopEnd(&loopHead);

                builder_.Bind(&loadHolder);
                BuildNamedPropertyAccess(gate, tacc.GetReceiver(), *current, tacc.GetValue(),
                                         tacc.GetAccessInfo(i).Plr());
                builder_.Jump(&exit);
            } else {
                TypedStObjByNameTransition(gate, receiverHC, frameState, exit, tacc, i);
            }
        } else if (tacc.IsReceiverEqHolder(i)) {
            // Local
            BuildNamedPropertyAccess(gate, tacc.GetReceiver(), tacc.GetReceiver(),
                                     tacc.GetValue(), tacc.GetAccessInfo(i).Plr());
            builder_.Jump(&exit);
        } else {
            // find in prototype, same as transition
            UNREACHABLE();
            return;
        }
        if (i != typeCount - 1) {
            // process fastpath for next type
            builder_.Bind(&fails[i]);
        }
    }
    builder_.Bind(&exit);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
    DeleteConstDataIfNoUser(tacc.GetKey());
}

void TypedBytecodeLowering::TypedStObjByNameTransition(GateRef gate, GateRef receiverHC, GateRef frameState,
                                                       Label &exit, StoreObjByNameTypeInfoAccessor &tacc, size_t i)
{
    Label notProto(&builder_);
    Label isProto(&builder_);
    auto newHolderHC = builder_.GetHClassGateFromIndex(gate, tacc.GetAccessInfo(i).HClassIndex());
    if (compilationEnv_->IsAotCompiler()) {
        auto prototype = builder_.LoadConstOffset(VariableType::JS_ANY(), receiverHC, JSHClass::PROTOTYPE_OFFSET);
        builder_.StoreConstOffset(VariableType::JS_ANY(), newHolderHC, JSHClass::PROTOTYPE_OFFSET, prototype);
    }
    if (!tacc.IsPrototypeHclass(i)) {
        builder_.DeoptCheck(builder_.BoolNot(builder_.IsPrototypeHClass(receiverHC)), frameState,
                            DeoptType::PROTOTYPECHANGED3);
    } else {
        builder_.Branch(builder_.IsPrototypeHClass(receiverHC), &isProto, &notProto,
                        BranchWeight::ONE_WEIGHT, BranchWeight::DEOPT_WEIGHT, "isPrototypeHClass");
        builder_.Bind(&isProto);
        GateRef propKey =
            builder_.GetObjectByIndexFromConstPool(glue_, gate, frameState,
                                                   builder_.TruncInt64ToInt32(tacc.GetKey()), ConstPoolType::STRING);
        builder_.CallRuntime(glue_, RTSTUB_ID(UpdateAOTHClass), Gate::InvalidGateRef,
                             { receiverHC, newHolderHC, propKey }, gate);
        builder_.Jump(&notProto);
        builder_.Bind(&notProto);
    }
    MemoryAttribute mAttr = MemoryAttribute::NeedBarrierAndAtomic();
    builder_.TransitionHClass(glue_, tacc.GetReceiver(), newHolderHC, mAttr);
    if (!tacc.GetAccessInfo(i).Plr().IsInlinedProps()) {
        auto properties = builder_.LoadConstOffset(VariableType::JS_ANY(), tacc.GetReceiver(),
                                                   JSObject::PROPERTIES_OFFSET);
        auto capacity = builder_.LoadConstOffset(VariableType::INT32(), properties, TaggedArray::LENGTH_OFFSET);
        auto index = builder_.Int32(tacc.GetAccessInfo(i).Plr().GetOffset());
        Label needExtend(&builder_);
        Label notExtend(&builder_);
        BRANCH_CIR(builder_.Int32UnsignedLessThan(index, capacity), &notExtend, &needExtend);
        builder_.Bind(&notExtend);
        BuildNamedPropertyAccess(gate, tacc.GetReceiver(), tacc.GetReceiver(), tacc.GetValue(),
                                 tacc.GetAccessInfo(i).Plr());
        builder_.Jump(&exit);
        builder_.Bind(&needExtend);
        builder_.CallRuntime(glue_, RTSTUB_ID(PropertiesSetValue), Gate::InvalidGateRef,
                             { tacc.GetReceiver(), tacc.GetValue(), properties, builder_.Int32ToTaggedInt(capacity),
                               builder_.Int32ToTaggedInt(index) }, gate);
        builder_.Jump(&exit);
    } else {
        BuildNamedPropertyAccess(gate, tacc.GetReceiver(), tacc.GetReceiver(), tacc.GetValue(),
                                 tacc.GetAccessInfo(i).Plr());
        builder_.Jump(&exit);
    }
}

void TypedBytecodeLowering::LowerTypedStOwnByName(GateRef gate)
{
    LowerTypedStObjByName(gate);
}

GateRef TypedBytecodeLowering::BuildNamedPropertyAccess(
    GateRef hir, GateRef receiver, GateRef holder, PropertyLookupResult plr)
{
    GateRef plrGate = builder_.Int32(plr.GetData());
    GateRef result = Circuit::NullGate();
    if (LIKELY(!plr.IsAccessor())) {
        result = builder_.LoadProperty(holder, plrGate, plr.IsFunction());
    } else {
        result = builder_.CallGetter(hir, receiver, holder, plrGate);
    }
    return result;
}

GateRef TypedBytecodeLowering::BuildNamedPropertyAccess(
    GateRef hir, GateRef receiver, GateRef holder, GateRef value, PropertyLookupResult plr,
    uint32_t receiverHClassIndex)
{
    GateRef plrGate = builder_.Int32(plr.GetData());
    GateRef result = Circuit::NullGate();
    if (LIKELY(!plr.IsAccessor())) {
        builder_.StoreProperty(receiver, plrGate, value, receiverHClassIndex);
    } else {
        builder_.CallSetter(hir, receiver, holder, plrGate, value);
    }
    return result;
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByNameForBuiltin(GateRef gate)
{
    LoadBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    // Just supported mono.
    if (tacc.IsMono()) {
        if (tacc.IsBuiltinsType()) {
            auto builtinsId = tacc.GetBuiltinsTypeId();
            if (builtinsId.has_value()) {
                if (TryLowerTypedLdObjByNameForBuiltin(tacc)) {
                    return true;
                }
                return TryLowerTypedLdObjByNameForBuiltinMethod(tacc, builtinsId.value());
            }
        } else if (tacc.IsGlobalsType()) {
            auto globalsId = tacc.GetGlobalsId();
            if (globalsId.has_value()) {
                return TryLowerTypedLdObjByNameForGlobalsId(tacc, globalsId.value());
            }
        }
    } else if (tacc.GetTypeCount() > 0) {
        auto builtinsId = tacc.GetBuiltinsTypeId();
        if (builtinsId.has_value()) {
            return TryLowerTypedLdObjByNameForBuiltin(tacc);
        }
    }
    return false; // No lowering performed
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByNameForBuiltin(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    JSTaggedValue key = tacc.GetKeyTaggedValue();
    if (key.IsUndefined()) {
        return false;
    }
    EcmaString *propString = EcmaString::Cast(key.GetTaggedObject());
    // (1) get length
    EcmaString *lengthString =
        EcmaString::Cast(compilationEnv_->GlobalConstants()->GetLengthString().GetTaggedObject());
    if (propString == lengthString) {
        if (tacc.IsBuiltinsArray()) {
            LowerTypedLdArrayLength(tacc);
            return true;
        }
        if (tacc.IsBuiltinsString()) {
            LowerTypedLdStringLength(tacc);
            return true;
        }
        if (tacc.IsBuiltinsTypeArray()) {
            LowerTypedLdTypedArrayLength(tacc);
            return true;
        }
    }

    EcmaString *sizeString = EcmaString::Cast(compilationEnv_->GlobalConstants()->GetSizeString().GetTaggedObject());
    if (propString == sizeString) {
        if (tacc.IsBuiltinsMap()) {
            LowerTypedLdMapSize(tacc);
            return true;
        }
    }

    // (2) other functions
    return false;
}

static void SetPlrLdFromIterResult(GlobalEnvField index, PropertyLookupResult &plr)
{
    if (index == GlobalEnvField::ITERATOR_RESULT_CLASS_INDEX) {
        plr.SetIsLoadFromIterResult(true);
    }
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByNameForGlobalsId(const LoadBulitinObjTypeInfoAccessor &tacc,
                                                                 GlobalIndex globalsId)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef gate = tacc.GetGate();
    JSTaggedValue key = tacc.GetKeyTaggedValue();
    if (key.IsUndefined()) {
        return false;
    }
    GateRef frameState = acc_.FindNearestFrameState(gate);
    if (globalsId.IsGlobalConstId()) {
        ConstantIndex index = static_cast<ConstantIndex>(globalsId.GetGlobalConstId());
        JSHClass *hclass = JSHClass::Cast(compilationEnv_->GlobalConstants()->GetGlobalConstantObject(
            static_cast<size_t>(index)).GetTaggedObject());
        PropertyLookupResult plr = JSHClass::LookupPropertyInBuiltinHClass(compilationEnv_->GetJSThread(), hclass, key);
        if (!plr.IsFound() || plr.IsAccessor()) {
            return false;
        }
        AddProfiling(gate);
        // 1. check hclass
        builder_.HeapObjectCheck(receiver, frameState);
        GateRef receiverHClass = builder_.LoadHClassByConstOffset(glue_, receiver);
        GateRef expectedHClass = builder_.GetGlobalConstantValue(index);
        builder_.DeoptCheck(builder_.Equal(receiverHClass, expectedHClass), frameState,
                            DeoptType::INCONSISTENTHCLASS11);
        // 2. load property
        GateRef plrGate = builder_.Int32(plr.GetData());
        GateRef result = builder_.LoadProperty(receiver, plrGate, plr.IsFunction());
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
        DeleteConstDataIfNoUser(tacc.GetKey());
        return true;
    } else if (globalsId.IsGlobalEnvId()) { // ctor Hclass
        GlobalEnvField index = static_cast<GlobalEnvField>(globalsId.GetGlobalEnvId());
        JSHClass *hclass = JSHClass::Cast(compilationEnv_->GetGlobalEnv()->GetGlobalEnvObjectByIndex(
            static_cast<size_t>(index))->GetTaggedObject());
        PropertyLookupResult plr = JSHClass::LookupPropertyInBuiltinHClass(compilationEnv_->GetJSThread(), hclass, key);
        if (!plr.IsFound() || plr.IsAccessor()) {
            return false;
        }
        SetPlrLdFromIterResult(index, plr);
        AddProfiling(gate);
        // 1. check hclass
        builder_.HeapObjectCheck(receiver, frameState);
        GateRef receiverHClass = builder_.LoadHClassByConstOffset(glue_, receiver);
        GateRef globalEnvObj = builder_.GetGlobalEnvObj(builder_.GetGlobalEnv(), static_cast<size_t>(index));
        builder_.DeoptCheck(builder_.Equal(receiverHClass, globalEnvObj), frameState,
                            DeoptType::INCONSISTENTHCLASS12);
        // 2. load property
        GateRef plrGate = builder_.Int32(plr.GetData());
        GateRef result = builder_.LoadProperty(receiver, plrGate, plr.IsFunction());
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
        DeleteConstDataIfNoUser(tacc.GetKey());
        return true;
    }
    return false;
}

bool TypedBytecodeLowering::TryLowerTypedLdobjBynameFromGloablBuiltin(GateRef gate)
{
    LoadBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    GateRef receiver = tacc.GetReceiver();
    if (acc_.GetOpCode(receiver) != OpCode::LOAD_BUILTIN_OBJECT) {
        return false;
    }
    JSHandle<GlobalEnv> globalEnv = compilationEnv_->GetGlobalEnv();
    uint64_t index = acc_.TryGetValue(receiver);
    BuiltinType type = static_cast<BuiltinType>(index);
    if (type == BuiltinType::BT_MATH) {
        auto math = globalEnv->GetMathFunction();
        JSHClass *hclass = math.GetTaggedValue().GetTaggedObject()->GetClass();
        JSTaggedValue key = tacc.GetKeyTaggedValue();
        if (key.IsUndefined()) {
            return false;
        }
        PropertyLookupResult plr = JSHClass::LookupPropertyInBuiltinHClass(compilationEnv_->GetJSThread(), hclass, key);
        if (!plr.IsFound() || plr.IsAccessor()) {
            return false;
        }
        AddProfiling(gate);
        builder_.MathHClassConsistencyCheck(receiver);
        GateRef plrGate = builder_.Int32(plr.GetData());
        GateRef result = builder_.LoadProperty(receiver, plrGate, plr.IsFunction());
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
        DeleteConstDataIfNoUser(tacc.GetKey());
        return true;
    }
    return false;
}

void TypedBytecodeLowering::LowerTypedLdArrayLength(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef gate = tacc.GetGate();
    GateRef array = tacc.GetReceiver();
    ElementsKind kind = acc_.TryGetElementsKind(gate);
    AddProfiling(gate);
    if (!Uncheck()) {
        if (!acc_.IsCreateArray(array)) {
            builder_.StableArrayCheck(array, kind, ArrayMetaDataAccessor::Mode::LOAD_LENGTH);
        }
    }

    GateRef result = builder_.LoadArrayLength(array, kind, ArrayMetaDataAccessor::Mode::LOAD_LENGTH);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerTypedLdTypedArrayLength(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef gate = tacc.GetGate();
    GateRef array = tacc.GetReceiver();
    AddProfiling(gate);
    ParamType arrayType = tacc.GetParamType();
    OnHeapMode onHeap = acc_.TryGetOnHeapMode(gate);
    if (!Uncheck()) {
        builder_.TypedArrayCheck(array, arrayType, TypedArrayMetaDataAccessor::Mode::LOAD_LENGTH, onHeap);
    }
    GateRef result = builder_.LoadTypedArrayLength(array, arrayType, onHeap);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerTypedLdStringLength(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef gate = tacc.GetGate();
    GateRef str = tacc.GetReceiver();
    AddProfiling(gate);
    if (!TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, str)) {
        if (!Uncheck()) {
            builder_.EcmaStringCheck(str);
        }
    }
    GateRef result = builder_.LoadStringLength(str);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerTypedLdMapSize(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef gate = tacc.GetGate();
    GateRef jsMap = tacc.GetReceiver();
    AddProfiling(gate);
    if (!Uncheck()) {
        builder_.EcmaMapCheck(jsMap);
    }
    GateRef result = builder_.LoadMapSize(jsMap);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByNameForBuiltinMethod(const LoadBulitinObjTypeInfoAccessor &tacc,
                                                                     BuiltinTypeId type)
{
    GateRef gate = tacc.GetGate();
    JSTaggedValue key = tacc.GetKeyTaggedValue();
    std::optional<GlobalEnvField> protoField = ToGlobelEnvPrototypeField(type);
    if (key.IsUndefined() || !protoField.has_value()) {
        return false;
    }
    size_t protoFieldIndex = static_cast<size_t>(*protoField);
    JSHandle<GlobalEnv> globalEnv = compilationEnv_->GetGlobalEnv();
    JSHClass *prototypeHClass = globalEnv->GetGlobalEnvObjectByIndex(protoFieldIndex)->GetTaggedObject()->GetClass();
    PropertyLookupResult plr = JSHClass::LookupPropertyInBuiltinPrototypeHClass(compilationEnv_->GetJSThread(),
        prototypeHClass, key);
    bool isPrototypeOfPrototype = false;
    // Unable to handle accessor at the moment
    if (!plr.IsFound() || plr.IsAccessor()) {
        if (type == BuiltinTypeId::ARRAY_ITERATOR) {
            protoField = ToGlobelEnvPrototypeField(BuiltinTypeId::ITERATOR);
            if (!protoField.has_value()) {
                return false;
            }
            protoFieldIndex = static_cast<size_t>(*protoField);
            prototypeHClass = globalEnv->GetGlobalEnvObjectByIndex(protoFieldIndex)->GetTaggedObject()->GetClass();
            plr = JSHClass::LookupPropertyInBuiltinPrototypeHClass(compilationEnv_->GetJSThread(),
                prototypeHClass, key);
            if (!plr.IsFound() || plr.IsAccessor()) {
                return false;
            } else {
                isPrototypeOfPrototype = true;
            }
        } else {
            return false;
        }
    }
    AddProfiling(gate);
    GateRef receiver = acc_.GetValueIn(gate, 2);
    if (!Uncheck()) {
        // For Array type only: array stability shall be ensured.
        ElementsKind kind = ElementsKind::NONE;
        if (type == BuiltinTypeId::ARRAY) {
            builder_.StableArrayCheck(receiver, ElementsKind::GENERIC, ArrayMetaDataAccessor::CALL_BUILTIN_METHOD);
            kind = tacc.TryGetArrayElementsKind();
        }

        builder_.BuiltinPrototypeHClassCheck(receiver, type, kind, isPrototypeOfPrototype);
    }
    // Successfully goes to typed path
    GateRef plrGate = builder_.Int32(plr.GetData());
    GateRef prototype = builder_.GetGlobalEnvObj(builder_.GetGlobalEnv(), static_cast<size_t>(*protoField));
    GateRef result = builder_.LoadProperty(prototype, plrGate, plr.IsFunction());
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
    return true;
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByIndexForBuiltin(GateRef gate)
{
    LoadBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    GateRef result = Circuit::NullGate();
    // Just supported mono.
    if (tacc.IsMono()) {
        if (tacc.IsBuiltinsTypeArray()) {  // pgo need dump profile type
            AddProfiling(gate);
            result = LoadTypedArrayByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
            return true;
        }
    }
    return false;
}

void TypedBytecodeLowering::LowerTypedLdObjByIndex(GateRef gate)
{
    if (TryLowerTypedLdObjByIndexForBuiltin(gate)) {
        return;
    }
}

bool TypedBytecodeLowering::TryLowerTypedStObjByIndexForBuiltin(GateRef gate)
{
    StoreBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    if (tacc.HasNoType()) {
        return false;
    }
    if (tacc.GetBuiltinsJSType() != JSType::JS_FLOAT32_ARRAY) {
        return false;
    }
    AddProfiling(gate);
    GateRef receiver = tacc.GetReceiver();
    ParamType receiverType = tacc.GetParamType();
    if (!Uncheck()) {
        OnHeapMode onHeap = tacc.TryGetHeapMode();
        builder_.TypedArrayCheck(receiver, receiverType, TypedArrayMetaDataAccessor::Mode::ACCESS_ELEMENT, onHeap);
    }
    GateRef index = builder_.Int32(tacc.TryConvertKeyToInt());
    GateRef value = tacc.GetValue();
    OnHeapMode onHeap = tacc.TryGetHeapMode();
    auto length = builder_.LoadTypedArrayLength(receiver, receiverType, onHeap);
    if (!Uncheck()) {
        builder_.IndexCheck(length, index);
    }
    builder_.StoreElement<TypedStoreOp::FLOAT32ARRAY_STORE_ELEMENT>(receiver, index, value, onHeap);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
    return true;
}

void TypedBytecodeLowering::LowerTypedStObjByIndex(GateRef gate)
{
    if (TryLowerTypedStObjByIndexForBuiltin(gate)) {
        return;
    }
}

bool TypedBytecodeLowering::TryLowerTypedLdObjByValueForBuiltin(GateRef gate)
{
    LoadBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    GateRef result = Circuit::NullGate();
    // Just supported mono.
    if (tacc.IsMono()) {
        if (tacc.IsBuiltinsString()) {
            AddProfiling(gate);
            result = LoadStringByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
            return true;
        } else if (tacc.IsBuiltinsArray()) {
            AddProfiling(gate);
            result = LoadJSArrayByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
            return true;
        } else if (tacc.IsBuiltinsTypeArray()) {
            AddProfiling(gate);
            result = LoadTypedArrayByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
            return true;
        }
    }
    return false;
}

void TypedBytecodeLowering::LowerTypedLdObjByValue(GateRef gate)
{
    if (TryLowerTypedLdObjByValueForBuiltin(gate)) {
        return;
    }
    if (!compilationEnv_->SupportHeapConstant() || !compilationEnv_->GetJSOptions().IsLdObjValueOpt()) {
        return;
    }
    LoadObjPropertyTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_, true);
    // only support for string named key.
    if (tacc.TypesIsEmpty() || tacc.HasIllegalType()) {
        return;
    }
    DEFVALUE(result, (&builder_), VariableType::JS_ANY(), builder_.Hole());
    AddProfiling(gate);
    GateRef frameState = acc_.FindNearestFrameState(gate);
    if (tacc.IsMono()) {
        if (!tacc.GetName()->IsString()) {
            return;
        }
        GateRef receiver = tacc.GetReceiver();
        EcmaString* ecmaString = EcmaString::Cast(tacc.GetName()->GetTaggedObject());
        EcmaStringAccessor ecmaStringAccessor(ecmaString);
        if (ecmaStringAccessor.IsInternString()) {
            builder_.InternStringKeyCheck(tacc.GetKey(), builder_.HeapConstant(tacc.GetNameIdx()));
        } else {
            builder_.StringKeyCheck(tacc.GetKey(), builder_.HeapConstant(tacc.GetNameIdx()));
        }
        builder_.ObjectTypeCheck(false, receiver,
                                 builder_.Int32(tacc.GetExpectedHClassIndex(0)), frameState);
        if (tacc.IsReceiverEqHolder(0)) {
            result = BuildNamedPropertyAccess(gate, receiver, receiver, tacc.GetAccessInfo(0).Plr());
        } else {
            builder_.ProtoChangeMarkerCheck(receiver, frameState);
            PropertyLookupResult plr = tacc.GetAccessInfo(0).Plr();
            GateRef plrGate = builder_.Int32(plr.GetData());
            GateRef unsharedConstPoool = argAcc_.GetFrameArgsIn(gate, FrameArgIdx::UNSHARED_CONST_POOL);
            size_t holderHClassIndex = static_cast<size_t>(tacc.GetAccessInfo(0).HClassIndex());
            if (LIKELY(!plr.IsAccessor())) {
                result = builder_.MonoLoadPropertyOnProto(receiver, plrGate, unsharedConstPoool, holderHClassIndex);
            } else {
                result = builder_.MonoCallGetterOnProto(gate, receiver, plrGate, unsharedConstPoool, holderHClassIndex);
            }
        }
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), *result);
        return;
    }
}

GateRef TypedBytecodeLowering::LoadStringByIndex(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef propKey = tacc.GetKey();
    acc_.SetGateType(propKey, GateType::NumberType());
    if (!Uncheck()) {
        if (!TypeInfoAccessor::IsTrustedStringType(compilationEnv_, circuit_, chunk_, acc_, receiver)) {
            builder_.EcmaStringCheck(receiver);
        }
        GateRef length = builder_.LoadStringLength(receiver);
        propKey = builder_.IndexCheck(length, propKey);
        receiver = builder_.FlattenTreeStringCheck(receiver);
    }
    return builder_.LoadElement<TypedLoadOp::STRING_LOAD_ELEMENT>(receiver, propKey);
}

GateRef TypedBytecodeLowering::LoadJSArrayByIndex(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef propKey = tacc.GetKey();
    acc_.SetGateType(propKey, GateType::NumberType());
    ElementsKind kind = tacc.TryGetArrayElementsKind();
    if (!Uncheck()) {
        if (!acc_.IsCreateArray(receiver)) {
            builder_.StableArrayCheck(receiver, kind, ArrayMetaDataAccessor::Mode::LOAD_ELEMENT);
            builder_.ElementsKindCheck(receiver, kind, ArrayMetaDataAccessor::Mode::LOAD_ELEMENT);
        }
        GateRef length = builder_.LoadArrayLength(receiver, kind, ArrayMetaDataAccessor::Mode::LOAD_LENGTH);
        propKey = builder_.IndexCheck(length, propKey);
    }

    GateRef result = Circuit::NullGate();
    if (Elements::IsInt(kind)) {
        // When elementskind switch on, need to add retype for loadInt
        result = builder_.LoadElement<TypedLoadOp::ARRAY_LOAD_INT_ELEMENT>(receiver, propKey);
    } else if (Elements::IsNumber(kind)) {
        // When elementskind switch on, need to add retype for loadNumber
        result = builder_.LoadElement<TypedLoadOp::ARRAY_LOAD_DOUBLE_ELEMENT>(receiver, propKey);
    } else if (Elements::IsObject(kind)) {
        result = builder_.LoadElement<TypedLoadOp::ARRAY_LOAD_OBJECT_ELEMENT>(receiver, propKey);
    } else if (!Elements::IsHole(kind)) {
        result = builder_.LoadElement<TypedLoadOp::ARRAY_LOAD_TAGGED_ELEMENT>(receiver, propKey);
    } else {
        result = builder_.LoadElement<TypedLoadOp::ARRAY_LOAD_HOLE_TAGGED_ELEMENT>(receiver, propKey);
    }
    return result;
}

GateRef TypedBytecodeLowering::LoadElmentFromFloat64Array(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef propKey = tacc.GetKey();
    OnHeapMode onHeap = tacc.TryGetHeapMode();
    auto resTemp = builder_.LoadElement<TypedLoadOp::FLOAT64ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
    Label entry(&builder_);
    builder_.SubCfgEntry(&entry);
    Label ifTrue(&builder_);
    Label ifFalse(&builder_);
    Label exit(&builder_);
    Variable result(builder_.GetCurrentEnvironment(),
        VariableType::JS_ANY(), builder_.NextVariableId(), builder_.Undefined());

    builder_.Branch(builder_.DoubleIsImpureNaN(resTemp), &ifTrue, &ifFalse);
    builder_.Bind(&ifTrue);
    {
        result = builder_.Double(base::NAN_VALUE);
        builder_.Jump(&exit);
    }
    builder_.Bind(&ifFalse);
    {
        result = resTemp;
        builder_.Jump(&exit);
    }
    builder_.Bind(&exit);
    auto res = *result;
    builder_.SubCfgExit();
    return res;
}

GateRef TypedBytecodeLowering::LoadTypedArrayByIndex(const LoadBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    ParamType receiverType = tacc.GetParamType();
    GateRef propKey = tacc.GetKey();
    OnHeapMode onHeap = tacc.TryGetHeapMode();
    JSType builtinsType = tacc.GetBuiltinsJSType();
    if (!Uncheck()) {
        builder_.TypedArrayCheck(receiver, receiverType, TypedArrayMetaDataAccessor::Mode::ACCESS_ELEMENT, onHeap);
        GateRef length = builder_.LoadTypedArrayLength(receiver, receiverType, onHeap);
        propKey = builder_.IndexCheck(length, propKey);
    }
    switch (builtinsType) {
        case JSType::JS_INT8_ARRAY:
            return builder_.LoadElement<TypedLoadOp::INT8ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_UINT8_ARRAY:
            return builder_.LoadElement<TypedLoadOp::UINT8ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_UINT8_CLAMPED_ARRAY:
            return builder_.LoadElement<TypedLoadOp::UINT8CLAMPEDARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_INT16_ARRAY:
            return builder_.LoadElement<TypedLoadOp::INT16ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_UINT16_ARRAY:
            return builder_.LoadElement<TypedLoadOp::UINT16ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_INT32_ARRAY:
            return builder_.LoadElement<TypedLoadOp::INT32ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_UINT32_ARRAY:
            return builder_.LoadElement<TypedLoadOp::UINT32ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_FLOAT32_ARRAY:
            return builder_.LoadElement<TypedLoadOp::FLOAT32ARRAY_LOAD_ELEMENT>(receiver, propKey, onHeap);
        case JSType::JS_FLOAT64_ARRAY: {
            return LoadElmentFromFloat64Array(tacc);
        }
        default:
            LOG_ECMA(FATAL) << "this branch is unreachable";
            UNREACHABLE();
    }

    return Circuit::NullGate();
}

void TypedBytecodeLowering::StoreJSArrayByIndex(const StoreBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    GateRef propKey = tacc.GetKey();
    GateRef value = tacc.GetValue();
    ElementsKind kind = tacc.TryGetArrayElementsKind();
    if (!Uncheck()) {
        if (!acc_.IsCreateArray(receiver)) {
            builder_.StableArrayCheck(receiver, kind, ArrayMetaDataAccessor::Mode::STORE_ELEMENT);
        }
        GateRef length = builder_.LoadArrayLength(receiver, kind, ArrayMetaDataAccessor::Mode::LOAD_LENGTH);
        builder_.IndexCheck(length, propKey);
        builder_.COWArrayCheck(receiver);

        if (Elements::IsObject(kind)) {
            GateRef frameState = acc_.FindNearestFrameState(builder_.GetDepend());
            builder_.HeapObjectCheck(value, frameState);
        }
    }
    builder_.StoreElement<TypedStoreOp::ARRAY_STORE_ELEMENT>(receiver, propKey, value);
}

void TypedBytecodeLowering::StoreTypedArrayByIndex(const StoreBulitinObjTypeInfoAccessor &tacc)
{
    GateRef receiver = tacc.GetReceiver();
    ParamType receiverType = tacc.GetParamType();
    GateRef propKey = tacc.GetKey();
    GateRef value = tacc.GetValue();
    OnHeapMode onHeap = tacc.TryGetHeapMode();
    if (!Uncheck()) {
        builder_.TypedArrayCheck(receiver, receiverType, TypedArrayMetaDataAccessor::Mode::ACCESS_ELEMENT, onHeap);
        GateRef length = builder_.LoadTypedArrayLength(receiver, receiverType, onHeap);
        propKey = builder_.IndexCheck(length, propKey);
    }

    JSType builtinsType = tacc.GetBuiltinsJSType();
    switch (builtinsType) {
        case JSType::JS_INT8_ARRAY:
            builder_.StoreElement<TypedStoreOp::INT8ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_UINT8_ARRAY:
            builder_.StoreElement<TypedStoreOp::UINT8ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_UINT8_CLAMPED_ARRAY:
            builder_.StoreElement<TypedStoreOp::UINT8CLAMPEDARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_INT16_ARRAY:
            builder_.StoreElement<TypedStoreOp::INT16ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_UINT16_ARRAY:
            builder_.StoreElement<TypedStoreOp::UINT16ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_INT32_ARRAY:
            builder_.StoreElement<TypedStoreOp::INT32ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_UINT32_ARRAY:
            builder_.StoreElement<TypedStoreOp::UINT32ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_FLOAT32_ARRAY:
            builder_.StoreElement<TypedStoreOp::FLOAT32ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        case JSType::JS_FLOAT64_ARRAY:
            builder_.StoreElement<TypedStoreOp::FLOAT64ARRAY_STORE_ELEMENT>(receiver, propKey, value, onHeap);
            break;
        default:
            LOG_ECMA(FATAL) << "this branch is unreachable";
            UNREACHABLE();
    }
}

bool TypedBytecodeLowering::TryLowerTypedStObjByValueForBuiltin(GateRef gate)
{
    StoreBulitinObjTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    // Just supported mono.
    if (tacc.IsMono() && !tacc.IsStoreOutOfBounds()) {
        if (tacc.IsBuiltinsArray()) {
            AddProfiling(gate);
            StoreJSArrayByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
            return true;
        } else if (tacc.IsBuiltinsTypeArray()) {
            AddProfiling(gate);
            StoreTypedArrayByIndex(tacc);
            acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), Circuit::NullGate());
            return true;
        }
    }

    return false;
}

void TypedBytecodeLowering::LowerTypedStObjByValue(GateRef gate)
{
    if (TryLowerTypedStObjByValueForBuiltin(gate)) {
        return;
    }
}

bool TypedBytecodeLowering::IsTrueOrFalseHasProfileType(GateRef gate) const
{
    ASSERT(acc_.GetOpCode(gate) == OpCode::JS_BYTECODE);
    return acc_.GetByteCodeOpcode(gate) == EcmaOpcode::CALLRUNTIME_ISTRUE_PREF_IMM8 ||
           acc_.GetByteCodeOpcode(gate) == EcmaOpcode::CALLRUNTIME_ISFALSE_PREF_IMM8;
}

void TypedBytecodeLowering::LowerTypedIsTrueOrFalse(GateRef gate, bool flag)
{
    UnOpTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    ParamType paramType;
    if (TypeInfoAccessor::IsTrustedBooleanType(acc_, tacc.GetValue()) ||
    (IsTrueOrFalseHasProfileType(gate) && tacc.IsBooleanType())) {
        paramType = ParamType::BooleanType();
    } else if (TypeInfoAccessor::IsTrustedNumberType(acc_, tacc.GetValue()) ||
    (IsTrueOrFalseHasProfileType(gate) && tacc.HasNumberType())) {
        paramType = ParamType::NumberType();
    } else {
        return;
    }
    AddProfiling(gate);
    GateRef result;
    if (!flag) {
        result = builder_.TypedUnaryOp<TypedUnOp::TYPED_ISFALSE>(tacc.GetValue(), paramType);
    } else {
        result = builder_.TypedUnaryOp<TypedUnOp::TYPED_ISTRUE>(tacc.GetValue(), paramType);
    }

    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

bool TryLowerNewNumber(CircuitBuilder *builder, GateAccessor acc, GateRef gate)
{
    auto loadBuiltin = acc.GetValueIn(gate, 0);
    if ((acc.GetOpCode(loadBuiltin) == OpCode::LOAD_BUILTIN_OBJECT) &&
            (acc.GetIndex(loadBuiltin) == static_cast<size_t>(BuiltinType::BT_NUMBER))) {
        auto arg = builder->ToTaggedIntPtr(builder->Int32(0));
        if (acc.GetNumValueIn(gate) > 1) {
            arg = acc.GetValueIn(gate, 1);
        }

        auto currentLabel = builder->GetCurrentEnvironment()->GetCurrentLabel();
        auto currentControl = currentLabel->GetControl();
        auto currentDepend = currentLabel->GetDepend();
        GateRef frameState = acc.FindNearestFrameState(gate);
        GateRef newNumber = acc.GetCircuit()->NewGate(acc.GetCircuit()->NewNumber(),
            MachineType::I64,
            {currentControl, currentDepend, loadBuiltin, arg, frameState},
            GateType::TaggedPointer());

        currentLabel->SetControl(newNumber);
        currentLabel->SetDepend(newNumber);

        acc.ReplaceHirAndDeleteIfException(gate, builder->GetStateDepend(), newNumber);
        return true;
    }
    return false;
}

void TypedBytecodeLowering::LowerTypedNewObjRange(GateRef gate)
{
    if (TryLowerNewNumber(&builder_, acc_, gate) || TryLowerNewBuiltinConstructor(gate)) {
        return;
    }
    NewObjRangeTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    if (!tacc.FindHClass() || !tacc.IsValidCallMethodId()) {
        return;
    }
    size_t methodId = tacc.GetCallMethodId();
    MethodLiteral* method = ctx_->GetJSPandaFile()->FindMethodLiteral(methodId);
    JSTaggedValue value = tacc.GetHClass();
    JSHClass *hclass = JSHClass::Cast(value.GetTaggedObject());
    if (method == nullptr || !value.IsJSHClass() || hclass->GetObjectType() != JSType::JS_OBJECT) {
        return ;
    }
    AddProfiling(gate);
    GateRef ctor = tacc.GetValue();
    GateRef hclassIndex = tacc.GetHClassIndex();
    GateRef stateSplit = acc_.GetDep(gate);
    GateRef frameState = acc_.FindNearestFrameState(stateSplit);
    GateRef ihclass = builder_.GetHClassGateFromIndex(frameState, hclassIndex);
    GateRef size = builder_.IntPtr(hclass->GetObjectSize());

    auto heapConstantIndex = tacc.TryGetHeapConstantConstructorIndex(methodId);
    if (heapConstantIndex != JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        GateRef res = builder_.HeapConstant(heapConstantIndex);
        builder_.DeoptCheck(builder_.Equal(ctor, res), frameState, DeoptType::NOTCALLTARGETHEAPOBJECT);
    } else {
        // call target check
        builder_.JSCallTargetTypeCheck<TypedCallTargetCheckOp::JS_NEWOBJRANGE>(ctor,
            builder_.IntPtr(INVALID_INDEX), gate);
    }
    // check IHC
    GateRef protoOrHclass = builder_.LoadConstOffset(VariableType::JS_ANY(), ctor,
        JSFunction::PROTO_OR_DYNCLASS_OFFSET);
    GateRef checkProto = builder_.Equal(ihclass, protoOrHclass);
    builder_.DeoptCheck(checkProto, frameState, DeoptType::NOTNEWOBJ2);
    // construct
    GateRef thisObj = builder_.TypedNewAllocateThis(ctor, ihclass, size, frameState);
    size_t range = acc_.GetNumValueIn(gate);
    size_t expectedArgc = method->GetNumArgs();
    size_t actualArgc = static_cast<size_t>(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
        EcmaOpcode::NEWOBJRANGE_IMM8_IMM8_V8));
    std::vector<GateRef> args {glue_,  builder_.Int64(actualArgc), builder_.IntPtr(0), ctor, ctor,
                               thisObj};
    for (size_t i = 1; i < range; ++i) { // 1:skip ctor
        args.emplace_back(acc_.GetValueIn(gate, i));
    }
    bool needPushArgv = (expectedArgc != actualArgc);
    bool isFastCall = method->IsFastCall();
    GateRef result = builder_.CallNew(gate, args, needPushArgv, isFastCall);
    ReplaceGateWithPendingException(glue_, gate, builder_.GetState(), builder_.GetDepend(), result);
}

bool TypedBytecodeLowering::TryLowerNewBuiltinConstructor(GateRef gate)
{
    NewBuiltinCtorTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    auto id = tacc.GetPGOBuiltinMethodId();
    if (id == BuiltinsStubCSigns::ID::NONE) {
        return false;
    }

    GateRef ctor = tacc.GetValue();
    GateRef constructGate = Circuit::NullGate();
    if (tacc.IsBuiltinId(BuiltinsStubCSigns::ID::ObjectConstructor)) {
        AddProfiling(gate);
        if (!Uncheck()) {
            builder_.ObjectConstructorCheck(ctor);
        }
        constructGate = builder_.BuiltinConstructor(BuiltinsStubCSigns::ID::ObjectConstructor, gate);
    } else if (tacc.IsBuiltinId(BuiltinsStubCSigns::ID::BooleanConstructor)) {
        if (acc_.GetNumValueIn(gate) <= 2) { // 2: ctor and first arg
            AddProfiling(gate);
            if (!Uncheck()) {
                builder_.BooleanConstructorCheck(ctor);
            }
            constructGate = builder_.BuiltinConstructor(BuiltinsStubCSigns::ID::BooleanConstructor, gate);
        }
    } else if (tacc.IsBuiltinId(BuiltinsStubCSigns::ID::Float32ArrayConstructor)) {
        if (acc_.GetNumValueIn(gate) <= 2) { // 2: ctor and first arg
            AddProfiling(gate);
            if (!Uncheck()) {
                builder_.Float32ArrayConstructorCheck(ctor);
            }
            constructGate = builder_.BuiltinConstructor(BuiltinsStubCSigns::ID::Float32ArrayConstructor, gate);
        }
    }
    if (constructGate == Circuit::NullGate()) {
        // for other builtin constructor, do necessory check and just call runtime JSCallNew.
        AddProfiling(gate);
        if (!Uncheck()) {
            builder_.TypedConstructorCheck(ctor, GET_TYPED_GLOBAL_ENV_INDEX(id));
        }
        GateRef actualArgc = builder_.Int64(
            BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate), EcmaOpcode::NEWOBJRANGE_IMM8_IMM8_V8));
        GateRef actualArgv = builder_.IntPtr(0);
        GateRef thisObj = builder_.Undefined();
        size_t range = acc_.GetNumValueIn(gate);
        std::vector<GateRef> args {glue_, actualArgc, actualArgv, ctor, ctor, thisObj};
        for (size_t i = 1; i < range; ++i) {
            args.emplace_back(acc_.GetValueIn(gate, i));
        }
        constructGate = builder_.CallNewBuiltin(gate, args);
    }
    ReplaceGateWithPendingException(glue_, gate, builder_.GetState(), builder_.GetDepend(), constructGate);
    return true;
}

void TypedBytecodeLowering::LowerTypedSuperCall(GateRef gate)
{
    SuperCallTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);

    auto methodId = tacc.GetMethodId();
    if (methodId == 0) {
        return;
    }
    auto *methodLiteral = ctx_->GetJSPandaFile()->FindMethodLiteral(methodId);
    if (methodLiteral == nullptr || !methodLiteral->IsTypedCall()) {
        return;
    }
    if (!tacc.IsValidCallMethodId()) {
        return;
    }
    AddProfiling(gate);

    GateRef ctor = tacc.GetCtor();

    GateRef superCtor = builder_.GetSuperConstructor(ctor);
    GateRef newTarget = argAcc_.GetFrameArgsIn(gate, FrameArgIdx::NEW_TARGET);
    GateRef thisObj = builder_.TypedSuperAllocateThis(superCtor, newTarget);

    // call constructor
    size_t range = acc_.GetNumValueIn(gate);
    GateRef actualArgc = builder_.Int64(range + 3);  // 3: ctor, newTaget, this
    GateRef actualArgv = builder_.IntPtr(0);
    std::vector<GateRef> args { glue_, actualArgc, actualArgv, superCtor, newTarget, thisObj };
    for (size_t i = 0; i < range; ++i) {
        args.emplace_back(acc_.GetValueIn(gate, i));
    }

    GateRef constructGate = builder_.Construct(gate, args);
    ReplaceGateWithPendingException(glue_, gate, builder_.GetState(),
        builder_.GetDepend(), constructGate);
}

void TypedBytecodeLowering::SpeculateCallBuiltin(GateRef gate, GateRef func, const std::vector<GateRef> &args,
                                                 BuiltinsStubCSigns::ID id, bool isThrow)
{
    if (!Uncheck()) {
        builder_.CallTargetCheck(gate, func, builder_.IntPtr(static_cast<int64_t>(id)), {args[0]});
    }

    GateRef result = builder_.TypedCallBuiltin(gate, args, id, IS_SIDE_EFFECT_BUILTINS_ID(id));

    if (isThrow) {
        ReplaceGateWithPendingException(glue_, gate, builder_.GetState(),
            builder_.GetDepend(), result);
    } else {
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
    }
}

void TypedBytecodeLowering::SpeculateCallBuiltinFromGlobal(GateRef gate, const std::vector<GateRef> &args,
                                                           BuiltinsStubCSigns::ID id, bool isThrow, bool isSideEffect)
{
    GateRef result = builder_.TypedCallBuiltin(gate, args, id, isSideEffect);

    if (isThrow) {
        ReplaceGateWithPendingException(glue_, gate, builder_.GetState(),
            builder_.GetDepend(), result);
    } else {
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
    }
}

void TypedBytecodeLowering::LowerFastCall(GateRef gate, GateRef func,
    const std::vector<GateRef> &argsFastCall, bool isNoGC)
{
    builder_.StartCallTimer(glue_, gate, {glue_, func, builder_.True()}, true);
    GateRef result = builder_.TypedFastCall(gate, argsFastCall, isNoGC);
    builder_.EndCallTimer(glue_, gate, {glue_, func}, true);
    ReplaceGateWithPendingException(glue_, gate, builder_.GetState(),
        builder_.GetDepend(), result);
}

void TypedBytecodeLowering::LowerCall(GateRef gate, GateRef func,
    const std::vector<GateRef> &args, bool isNoGC)
{
    builder_.StartCallTimer(glue_, gate, {glue_, func, builder_.True()}, true);
    GateRef result = builder_.TypedCall(gate, args, isNoGC);
    builder_.EndCallTimer(glue_, gate, {glue_, func}, true);
    ReplaceGateWithPendingException(glue_, gate, builder_.GetState(),
        builder_.GetDepend(), result);
}

template<class TypeAccessor>
void TypedBytecodeLowering::CheckFastCallThisCallTarget(const TypeAccessor &tacc)
{
    if (noCheck_) {
        return;
    }
    GateRef func = tacc.GetFunc();
    GateRef gate = tacc.GetGate();
    GateRef methodIndex = builder_.IntPtr(tacc.GetFuncMethodOffset());
    if (tacc.IsNoGC()) {
        builder_.JSNoGCCallThisTargetTypeCheck<TypedCallTargetCheckOp::JSCALLTHIS_FAST_NOGC>(func, methodIndex, gate);
    } else {
        builder_.JSCallTargetTypeCheck<TypedCallTargetCheckOp::JSCALLTHIS_FAST>(func, methodIndex, gate);
    }
}

template<class TypeAccessor>
void TypedBytecodeLowering::CheckCallThisCallTarget(const TypeAccessor &tacc)
{
    if (noCheck_) {
        return;
    }
    GateRef func = tacc.GetFunc();
    GateRef gate = tacc.GetGate();
    GateRef methodIndex = builder_.IntPtr(tacc.GetFuncMethodOffset());
    if (tacc.IsNoGC()) {
        builder_.JSNoGCCallThisTargetTypeCheck<TypedCallTargetCheckOp::JSCALLTHIS_NOGC>(func, methodIndex, gate);
    } else {
        builder_.JSCallTargetTypeCheck<TypedCallTargetCheckOp::JSCALLTHIS>(func, methodIndex, gate);
    }
}

template<class TypeAccessor>
void TypedBytecodeLowering::CheckThisCallTargetAndLowerCall(const TypeAccessor &tacc,
    const std::vector<GateRef> &args, const std::vector<GateRef> &argsFastCall)
{
    GateRef func = tacc.GetFunc();
    GateRef gate = tacc.GetGate();
    bool isNoGC = tacc.IsNoGC();
    auto heapConstantIndex = tacc.TryGetHeapConstantFunctionIndex(tacc.GetMethodId());
    if (heapConstantIndex != JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        ConvertCallTargetCheckToHeapConstantCheckAndLowerCall(tacc, args, argsFastCall, heapConstantIndex, isNoGC);
        return;
    }
    if (tacc.CanFastCall()) {
        CheckFastCallThisCallTarget(tacc);
        LowerFastCall(gate, func, argsFastCall, isNoGC);
    } else {
        CheckCallThisCallTarget(tacc);
        LowerCall(gate, func, args, isNoGC);
    }
}

template<class TypeAccessor>
void TypedBytecodeLowering::CheckCallTargetFromDefineFuncAndLowerCall(const TypeAccessor &tacc,
    const std::vector<GateRef> &args, const std::vector<GateRef> &argsFastCall, bool isNoGC)
{
    GateRef func = tacc.GetFunc();
    GateRef gate = tacc.GetGate();
    auto heapConstantIndex = tacc.TryGetHeapConstantFunctionIndex(tacc.GetMethodId());
    if (heapConstantIndex != JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        ConvertCallTargetCheckToHeapConstantCheckAndLowerCall(tacc, args, argsFastCall, heapConstantIndex, isNoGC);
        return;
    }
    // NO CHECK
    if (!Uncheck()) {
        builder_.CallTargetIsCompiledCheck(func, gate);
    }
    if (tacc.CanFastCall()) {
        LowerFastCall(gate, func, argsFastCall, isNoGC);
    } else {
        LowerCall(gate, func, args, isNoGC);
    }
}

template<class TypeAccessor>
bool TypedBytecodeLowering::InSameConstPool(const TypeAccessor &tacc) const
{
    auto pandaFile = ctx_->GetJSPandaFile();
    auto targetPandaFile = tacc.GetPandaFile();
    if (pandaFile != targetPandaFile) {
        return false;
    }
    auto targetMethodId = tacc.GetMethodId();
    panda_file::IndexAccessor indexAccessor(*(targetPandaFile->GetPandaFile()),
                                            panda_file::File::EntityId(targetMethodId));
    auto targetCpId = static_cast<uint32_t>(indexAccessor.GetHeaderIndex());
    if (constPoolId_ != targetCpId) {
        return false;
    }
    return true;
}

template<class TypeAccessor>
void TypedBytecodeLowering::ConvertCallTargetCheckToHeapConstantCheckAndLowerCall(const TypeAccessor &tacc,
    const std::vector<GateRef> &args, const std::vector<GateRef> &argsFastCall,
    uint32_t heapConstantIndex, bool isNoGC)
{
    GateRef func = tacc.GetFunc();
    GateRef gate = tacc.GetGate();
    if (!Uncheck()) {
        GateRef frameState = acc_.GetFrameState(gate);
        GateRef res = builder_.HeapConstant(heapConstantIndex);
#if DUMP_HEAP_OBJECT_DFX
        Label exit(&builder_);
        Label notEqual(&builder_);
        buidler_.Branch(builder_.Equal(func, res), &exit, &notEqual,
            BranchWeight::ONE_WEIGHT, BranchWeight::ONE_WEIGHT, "isEqualObject");
        builder_.Bind(&notEqual);
        {
            std::vector<GateRef> params;
            params.push_back(func);
            builder_.CallRuntime(glue_, RTSTUB_ID(DumpHeapObjectAddress), Gate::InvalidGateRef, params, gate);
            params.clear();
            params.push_back(res);
            builder_.CallRuntime(glue_, RTSTUB_ID(DumpHeapObjectAddress), Gate::InvalidGateRef, params, gate);
            builder_.Jump(&exit);
        }
        builder_.Bind(&exit);
#endif
        builder_.DeoptCheck(builder_.Equal(func, res), frameState, DeoptType::NOTCALLTARGETHEAPOBJECT);
    }
    auto *jitCompilationEnv = static_cast<const JitCompilationEnv*>(compilationEnv_);
    JSHandle<JSTaggedValue> heapObject = jitCompilationEnv->GetHeapConstantHandle(heapConstantIndex);
    JSHandle<JSFunction> jsFunc = JSHandle<JSFunction>::Cast(heapObject);
    Method *calleeMethod = Method::Cast(jsFunc->GetMethod());
    ASSERT(calleeMethod->GetMethodLiteral() != nullptr);
    if (calleeMethod->GetMethodLiteral()->IsFastCall()) {
        LowerFastCall(gate, func, argsFastCall, isNoGC);
    } else {
        LowerCall(gate, func, args, isNoGC);
    }
}

template<class TypeAccessor>
void TypedBytecodeLowering::CheckCallTargetAndLowerCall(const TypeAccessor &tacc,
    const std::vector<GateRef> &args, const std::vector<GateRef> &argsFastCall)
{
    GateRef func = tacc.GetFunc();
    if (IsLoadVtable(func)) {
        CheckThisCallTargetAndLowerCall(tacc, args, argsFastCall); // func = a.foo, func()
        return;
    }
    bool isNoGC = tacc.IsNoGC();
    auto op = acc_.GetOpCode(func);
    if (op == OpCode::JS_BYTECODE && (acc_.GetByteCodeOpcode(func) == EcmaOpcode::DEFINEFUNC_IMM8_ID16_IMM8 ||
                                      acc_.GetByteCodeOpcode(func) == EcmaOpcode::DEFINEFUNC_IMM16_ID16_IMM8)) {
        CheckCallTargetFromDefineFuncAndLowerCall(tacc, args, argsFastCall, isNoGC);
        return;
    }

    if (!tacc.MethodOffsetIsVaild()) {
        return;
    }
    if (!InSameConstPool(tacc)) {
        return;
    }

    auto heapConstantIndex = tacc.TryGetHeapConstantFunctionIndex(tacc.GetMethodId());
    if (heapConstantIndex != JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        ConvertCallTargetCheckToHeapConstantCheckAndLowerCall(tacc, args, argsFastCall, heapConstantIndex, isNoGC);
        return;
    }

    int methodIndex = tacc.GetMethodIndex();
    if (methodIndex == -1) {
        return;
    }

    GateRef gate = tacc.GetGate();
    if (tacc.CanFastCall()) {
        if (!Uncheck()) {
            builder_.JSCallTargetTypeCheck<TypedCallTargetCheckOp::JSCALL_FAST>(func,
                builder_.IntPtr(methodIndex), gate);
        }
        LowerFastCall(gate, func, argsFastCall, isNoGC);
    } else {
        if (!Uncheck()) {
            builder_.JSCallTargetTypeCheck<TypedCallTargetCheckOp::JSCALL>(func,
                builder_.IntPtr(methodIndex), gate);
        }
        LowerCall(gate, func, args, isNoGC);
    }
}

template<EcmaOpcode Op, class TypeAccessor>
void TypedBytecodeLowering::LowerTypedCall(const TypeAccessor &tacc)
{
    auto heapConstantIndex = tacc.TryGetHeapConstantFunctionIndex(tacc.GetMethodId());
    if (!tacc.IsHotnessFunc() &&
        heapConstantIndex == JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        return;
    }
    auto methodId = tacc.GetMethodId();
    if (methodId == 0) {
        return;
    }
    auto *methodLiteral = ctx_->GetJSPandaFile()->FindMethodLiteral(methodId);
    if (methodLiteral == nullptr || !methodLiteral->IsTypedCall()) {
        return;
    }
    uint32_t argc = tacc.GetArgc();
    GateRef gate = tacc.GetGate();
    GateRef actualArgc = Circuit::NullGate();
    GateRef actualArgv = builder_.IntPtr(0);
    switch (Op) {
        case EcmaOpcode::CALLARG0_IMM8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLARG0_IMM8));
            break;
        }
        case EcmaOpcode::CALLARG1_IMM8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLARG1_IMM8_V8));
            break;
        }
        case EcmaOpcode::CALLARGS2_IMM8_V8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLARGS2_IMM8_V8_V8));
            break;
        }
        case EcmaOpcode::CALLARGS3_IMM8_V8_V8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLARGS3_IMM8_V8_V8_V8));
            break;
        }
        case EcmaOpcode::CALLRANGE_IMM8_IMM8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLRANGE_IMM8_IMM8_V8));
            break;
        }
        default:
            UNREACHABLE();
    }
    uint32_t len = tacc.GetFunctionTypeLength();
    if (len == tacc.INVALID_LEN) {
        return;
    }
    GateRef func = tacc.GetFunc();
    GateRef newTarget = builder_.Undefined();
    GateRef thisObj = builder_.Undefined();
    std::vector<GateRef> argsFastCall { glue_, func, thisObj};
    std::vector<GateRef> args { glue_, actualArgc, actualArgv, func, newTarget, thisObj };
    for (uint32_t i = 0; i < argc; i++) {
        GateRef value = acc_.GetValueIn(gate, i);
        argsFastCall.emplace_back(value);
        args.emplace_back(value);
    }
    for (uint32_t i = argc; i < len; i++) {
        argsFastCall.emplace_back(builder_.Undefined());
        args.emplace_back(builder_.Undefined());
    }
    if (argc != len) {
        return ;
    }
    AddProfiling(gate);
    CheckCallTargetAndLowerCall(tacc, args, argsFastCall);
}

const JSPandaFile* TypedBytecodeLowering::GetCalleePandaFile(GateRef gate)
{
    auto profileType = acc_.TryGetPGOType(gate).GetPGOSampleType();
    bool haveProfileType = profileType->IsProfileType() && !profileType->IsProfileTypeNone();
    if (haveProfileType) {
        if (compilationEnv_->IsJitCompiler()) {
            return compilationEnv_->GetJSPandaFile();
        }
        auto abcId = profileType->GetProfileType().GetAbcId();
        CString fileDesc;
        if (!decoder_->GetAbcNameById(abcId, fileDesc)) {
            UNREACHABLE();
        }
        fileDesc = JSPandaFile::GetNormalizedFileDesc(fileDesc);
        return JSPandaFileManager::GetInstance()->FindJSPandaFileByNormalizedName(fileDesc).get();
    }
    // nullptr if no pgo info
    return nullptr;
}

void TypedBytecodeLowering::LowerTypedCallArg0(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallArg0TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    if (!tacc.IsValidCallMethodId()) {
        return;
    }
    LowerTypedCall<EcmaOpcode::CALLARG0_IMM8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallArg1(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallArg1TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    GateRef func = tacc.GetFunc();
    GateRef a0Value = tacc.GetValue();
    BuiltinsStubCSigns::ID id = tacc.TryGetPGOBuiltinMethodId();
    if (!IS_INVALID_ID(id) && (IS_TYPED_BUILTINS_NUMBER_ID(id) || IS_TYPED_BUILTINS_GLOBAL_ID(id))) {
        if (IS_TYPED_INLINE_BUILTINS_ID(id)) {
            return;
        }
        AddProfiling(gate);
        if (IsFuncFromGlobal(func)) {
            // No need to do CallTargetCheck if func is from LOAD_BUILTIN_OBJECT.
            SpeculateCallBuiltinFromGlobal(gate, { a0Value }, id, true);
        } else {
            SpeculateCallBuiltin(gate, func, { a0Value }, id, true);
        }
    } else {
        if (!tacc.IsValidCallMethodId()) {
            return;
        }
        LowerTypedCall<EcmaOpcode::CALLARG1_IMM8_V8>(tacc);
    }
}

void TypedBytecodeLowering::LowerTypedCallArg2(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallArg2TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    if (!tacc.IsValidCallMethodId()) {
        return;
    }
    LowerTypedCall<EcmaOpcode::CALLARGS2_IMM8_V8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallArg3(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallArg3TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    if (!tacc.IsValidCallMethodId()) {
        return;
    }
    LowerTypedCall<EcmaOpcode::CALLARGS3_IMM8_V8_V8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallrange(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallRangeTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    if (!tacc.IsValidCallMethodId()) {
        return;
    }
    LowerTypedCall<EcmaOpcode::CALLRANGE_IMM8_IMM8_V8>(tacc);
}

bool TypedBytecodeLowering::IsLoadVtable(GateRef func)
{
    auto op = acc_.GetOpCode(func);
    if (op != OpCode::LOAD_PROPERTY) {
        return false;
    }
    return true;
}

template<EcmaOpcode Op, class TypeAccessor>
void TypedBytecodeLowering::LowerTypedThisCall(const TypeAccessor &tacc)
{
    auto methodId = tacc.GetMethodId();
    if (methodId == 0) {
        return;
    }
    auto *methodLiteral = ctx_->GetJSPandaFile()->FindMethodLiteral(methodId);
    if (methodLiteral == nullptr || !methodLiteral->IsTypedCall()) {
        return;
    }
    auto heapConstantIndex = tacc.TryGetHeapConstantFunctionIndex(methodId);
    if (!tacc.IsHotnessFunc() &&
        heapConstantIndex == JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
        return;
    }
    uint32_t argc = tacc.GetArgc();
    GateRef gate = tacc.GetGate();
    GateRef actualArgc = Circuit::NullGate();
    GateRef actualArgv = builder_.IntPtr(0);
    switch (Op) {
        case EcmaOpcode::CALLTHIS0_IMM8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLTHIS0_IMM8_V8));
            break;
        }
        case EcmaOpcode::CALLTHIS1_IMM8_V8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLTHIS1_IMM8_V8_V8));
            break;
        }
        case EcmaOpcode::CALLTHIS2_IMM8_V8_V8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLTHIS2_IMM8_V8_V8_V8));
            break;
        }
        case EcmaOpcode::CALLTHIS3_IMM8_V8_V8_V8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLTHIS3_IMM8_V8_V8_V8_V8));
            break;
        }
        case EcmaOpcode::CALLTHISRANGE_IMM8_IMM8_V8: {
            actualArgc = builder_.Int64(BytecodeCallArgc::ComputeCallArgc(acc_.GetNumValueIn(gate),
                EcmaOpcode::CALLTHISRANGE_IMM8_IMM8_V8));
            break;
        }
        default:
            UNREACHABLE();
    }

    uint32_t len = tacc.GetFunctionTypeLength();
    if (len == tacc.INVALID_LEN) {
        return;
    }
    GateRef func = tacc.GetFunc();
    GateRef newTarget = builder_.Undefined();
    GateRef thisObj = tacc.GetThisObj();
    std::vector<GateRef> argsFastCall { glue_, func, thisObj};
    std::vector<GateRef> args { glue_, actualArgc, actualArgv, func, newTarget, thisObj };
    for (uint32_t i = 0; i < argc; i++) {
        GateRef value = acc_.GetValueIn(gate, i + 1);
        argsFastCall.emplace_back(value);
        args.emplace_back(value);
    }
    for (uint32_t i = argc; i < len; i++) {
        argsFastCall.emplace_back(builder_.Undefined());
        args.emplace_back(builder_.Undefined());
    }
    AddProfiling(gate);
    CheckThisCallTargetAndLowerCall(tacc, args, argsFastCall);
}

void TypedBytecodeLowering::LowerTypedCallthis0(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallThis0TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    BuiltinsStubCSigns::ID pgoFuncId = tacc.TryGetPGOBuiltinMethodId();
    if (!IS_INVALID_ID(pgoFuncId) && IS_TYPED_BUILTINS_ID_CALL_THIS0(pgoFuncId)) {
        AddProfiling(gate);
        SpeculateCallBuiltin(gate, tacc.GetFunc(), {tacc.GetThisObj()}, pgoFuncId, true);
        return;
    }
    if (!tacc.CanOptimizeAsFastCall()) {
        return;
    }
    LowerTypedThisCall<EcmaOpcode::CALLTHIS0_IMM8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallthis1(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallThis1TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    BuiltinsStubCSigns::ID pgoFuncId = tacc.TryGetPGOBuiltinMethodId();
    if (!IS_INVALID_ID(pgoFuncId) && IS_TYPED_BUILTINS_ID_CALL_THIS1(pgoFuncId)) {
        AddProfiling(gate);
        SpeculateCallBuiltin(gate, tacc.GetFunc(), {tacc.GetArgs()}, pgoFuncId, true);
        return;
    }
    if (!tacc.CanOptimizeAsFastCall()) {
        return;
    }
    LowerTypedThisCall<EcmaOpcode::CALLTHIS1_IMM8_V8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallthis2(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallThis2TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    BuiltinsStubCSigns::ID pgoFuncId = tacc.TryGetPGOBuiltinMethodId();
    if (!IS_INVALID_ID(pgoFuncId) && IS_TYPED_BUILTINS_ID_CALL_THIS2(pgoFuncId)) {
        AddProfiling(gate);
        SpeculateCallBuiltin(gate, tacc.GetFunc(), { tacc.GetArgs() }, pgoFuncId, true);
        return;
    }
    if (!tacc.CanOptimizeAsFastCall()) {
        return;
    }
    LowerTypedThisCall<EcmaOpcode::CALLTHIS2_IMM8_V8_V8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallthis3(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallThis3TypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    BuiltinsStubCSigns::ID pgoFuncId = tacc.TryGetPGOBuiltinMethodId();
    if (!IS_INVALID_ID(pgoFuncId) && IS_TYPED_BUILTINS_ID_CALL_THIS3(pgoFuncId)) {
        AddProfiling(gate);
        SpeculateCallBuiltin(gate, tacc.GetFunc(), { tacc.GetArgs() }, pgoFuncId, true);
        return;
    }
    if (!tacc.CanOptimizeAsFastCall()) {
        return;
    }
    LowerTypedThisCall<EcmaOpcode::CALLTHIS3_IMM8_V8_V8_V8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallthisrange(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    CallThisRangeTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    if (!tacc.CanOptimizeAsFastCall()) {
        return;
    }
    LowerTypedThisCall<EcmaOpcode::CALLTHISRANGE_IMM8_IMM8_V8>(tacc);
}

void TypedBytecodeLowering::LowerTypedCallInit(GateRef gate)
{
    // same as callthis0
    LowerTypedCallthis0(gate);
}

void TypedBytecodeLowering::AddProfiling(GateRef gate)
{
    hitTypedOpCount_++;
    AddHitBytecodeCount();
    if (IsTraceBC()) {
        // see stateSplit as a part of JSByteCode if exists
        GateRef maybeStateSplit = acc_.GetDep(gate);
        GateRef current = Circuit::NullGate();
        if (acc_.GetOpCode(maybeStateSplit) == OpCode::STATE_SPLIT) {
            current = maybeStateSplit;
        } else {
            current = gate;
        }

        EcmaOpcode ecmaOpcode = acc_.GetByteCodeOpcode(gate);
        auto ecmaOpcodeGate = builder_.Int32(static_cast<uint32_t>(ecmaOpcode));
        GateRef constOpcode = builder_.Int32ToTaggedInt(ecmaOpcodeGate);
        GateRef typedPath = builder_.Int32ToTaggedInt(builder_.Int32(1));
        GateRef traceGate = builder_.CallRuntime(glue_, RTSTUB_ID(DebugAOTPrint), acc_.GetDep(current),
                                                 { constOpcode, typedPath }, gate);
        acc_.SetDep(current, traceGate);
        builder_.SetDepend(acc_.GetDep(gate));  // set gate depend: trace or STATE_SPLIT
    }

    if (IsProfiling()) {
        // see stateSplit as a part of JSByteCode if exists
        GateRef maybeStateSplit = acc_.GetDep(gate);
        GateRef current = Circuit::NullGate();
        if (acc_.GetOpCode(maybeStateSplit) == OpCode::STATE_SPLIT) {
            current = maybeStateSplit;
        } else {
            current = gate;
        }

        GateRef func = builder_.Undefined();
        if (acc_.HasFrameState(gate)) {
            func = argAcc_.GetFrameArgsIn(gate, FrameArgIdx::FUNC);
        }

        GateRef bcIndex = builder_.Int32ToTaggedInt(builder_.Int32(acc_.TryGetBcIndex(gate)));
        EcmaOpcode ecmaOpcode = acc_.GetByteCodeOpcode(gate);
        auto ecmaOpcodeGate = builder_.Int32(static_cast<uint32_t>(ecmaOpcode));
        GateRef constOpcode = builder_.Int32ToTaggedInt(ecmaOpcodeGate);
        GateRef mode =
            builder_.Int32ToTaggedInt(builder_.Int32(static_cast<int32_t>(OptCodeProfiler::Mode::TYPED_PATH)));
        GateRef profiling = builder_.CallRuntime(glue_, RTSTUB_ID(ProfileOptimizedCode), acc_.GetDep(current),
            { func, bcIndex, constOpcode, mode }, gate);
        acc_.SetDep(current, profiling);
        builder_.SetDepend(acc_.GetDep(gate));  // set gate depend: profiling or STATE_SPLIT
    }
}

void TypedBytecodeLowering::AddBytecodeCount(EcmaOpcode op)
{
    currentOp_ = op;
    if (bytecodeMap_.find(op) != bytecodeMap_.end()) {
        bytecodeMap_[op]++;
    } else {
        bytecodeMap_[op] = 1;
    }
}

void TypedBytecodeLowering::DeleteBytecodeCount(EcmaOpcode op)
{
    bytecodeMap_.erase(op);
}

void TypedBytecodeLowering::AddHitBytecodeCount()
{
    if (bytecodeHitTimeMap_.find(currentOp_) != bytecodeHitTimeMap_.end()) {
        bytecodeHitTimeMap_[currentOp_]++;
    } else {
        bytecodeHitTimeMap_[currentOp_] = 1;
    }
}

void TypedBytecodeLowering::LowerTypedTypeOf(GateRef gate)
{
    TypeOfTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    if (tacc.IsIllegalType()) {
        return;
    }
    AddProfiling(gate);
    if (!Uncheck()) {
        builder_.TypeOfCheck(tacc.GetValue(), tacc.GetParamType());
    }
    GateRef result = builder_.TypedTypeOf(tacc.GetParamType());
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerGetIterator(GateRef gate)
{
    if (GetCalleePandaFile(gate) == nullptr) {
        return;
    }
    GetIteratorTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, GetCalleePandaFile(gate), callMethodFlagMap_);
    BuiltinsStubCSigns::ID id = tacc.TryGetPGOBuiltinMethodId();
    if (IS_INVALID_ID(id) || id == BuiltinsStubCSigns::ID::NONE) {
        return;
    }
    AddProfiling(gate);
    GateRef obj = tacc.GetCallee();
    SpeculateCallBuiltin(gate, obj, { obj }, id, true);
}

void TypedBytecodeLowering::LowerTypedTryLdGlobalByName(GateRef gate)
{
    if (!enableLoweringBuiltin_) {
        return;
    }
    LoadGlobalObjByNameTypeInfoAccessor tacc(compilationEnv_, circuit_, gate);
    JSTaggedValue key = tacc.GetKeyTaggedValue();
    if (key.IsUndefined()) {
        return;
    }

    BuiltinIndex& builtin = BuiltinIndex::GetInstance();
    auto index = builtin.GetBuiltinIndex(key);
    if (index != builtin.NOT_FOUND) {
        AddProfiling(gate);
        GateRef result = builder_.LoadBuiltinObject(index);
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
        DeleteConstDataIfNoUser(tacc.GetKey());
        return;
    }

    if (compilationEnv_->IsJitCompiler()) {
        uint32_t pcOffset = acc_.TryGetPcOffset(gate);
        uint32_t methodOffset = acc_.TryGetMethodOffset(gate);
        uint32_t heapConstantIndex = static_cast<const JitCompilationEnv*>(compilationEnv_)->
                GetLdGlobalByNameBcOffset2HeapConstantIndex(methodOffset, pcOffset);
        if (heapConstantIndex == JitCompilationEnv::INVALID_HEAP_CONSTANT_INDEX) {
            return;
        }
        AddProfiling(gate);
        GateRef frameState = acc_.FindNearestFrameState(builder_.GetDepend());
        GateRef propertyBoxConst = builder_.HeapConstant(heapConstantIndex);
        GateRef boxValue = builder_.LoadConstOffset(
            VariableType::JS_ANY(), propertyBoxConst, PropertyBox::VALUE_OFFSET);
        builder_.DeoptCheck(builder_.TaggedIsNotHole(boxValue), frameState, DeoptType::PROPERTYBOXINVALID);
        acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), boxValue);
        DeleteConstDataIfNoUser(tacc.GetKey());
    }
}

void TypedBytecodeLowering::LowerInstanceOf(GateRef gate)
{
    InstanceOfTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, chunk_);
    if (tacc.TypesIsEmpty() || tacc.HasIllegalType()) {
        return;
    }
    AddProfiling(gate);
    size_t typeCount = tacc.GetTypeCount();
    std::vector<GateRef> expectedHCIndexes;
    for (size_t i = 0; i < typeCount; ++i) {
        GateRef temp = builder_.Int32(tacc.GetExpectedHClassIndex(i));
        expectedHCIndexes.emplace_back(temp);
    }
    DEFVALUE(result, (&builder_), VariableType::JS_ANY(), builder_.Hole());
    // RuntimeCheck -
    // 1. pgo.hclass == ctor.hclass
    // 2. ctor.hclass has a prototype chain up to Function.prototype
    GateRef obj = tacc.GetReceiver();
    GateRef target = tacc.GetTarget();

    builder_.ObjectTypeCheck(false, target, expectedHCIndexes[0]);
    builder_.ProtoChangeMarkerCheck(target);

    result = builder_.OrdinaryHasInstance(obj, target);
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), *result);
}

void TypedBytecodeLowering::LowerCreateEmptyObject(GateRef gate)
{
    AddProfiling(gate);
    GateRef globalEnv = builder_.GetGlobalEnv();
    GateRef hclass = builder_.GetGlobalEnvObjHClass(globalEnv, GlobalEnv::OBJECT_FUNCTION_INDEX);

    JSHandle<JSFunction> objectFunc(compilationEnv_->GetGlobalEnv()->GetObjectFunction());
    JSTaggedValue protoOrHClass = objectFunc->GetProtoOrHClass();
    JSHClass *objectHC = JSHClass::Cast(protoOrHClass.GetTaggedObject());
    size_t objectSize = objectHC->GetObjectSize();

    GateRef emptyArray = builder_.GetGlobalConstantValue(ConstantIndex::EMPTY_ARRAY_OBJECT_INDEX);
    GateRef size = builder_.IntPtr(objectHC->GetObjectSize());

    builder_.StartAllocate();
    GateRef object = builder_.HeapAlloc(glue_, size, GateType::TaggedValue(), RegionSpaceFlag::IN_YOUNG_SPACE);

    // initialization
    for (size_t offset = JSObject::SIZE; offset < objectSize; offset += JSTaggedValue::TaggedTypeSize()) {
        builder_.StoreConstOffset(VariableType::INT64(), object, offset, builder_.Undefined());
    }
    builder_.StoreHClass(glue_, object, hclass, MemoryAttribute::NeedBarrierAndAtomic());
    builder_.StoreConstOffset(VariableType::INT64(), object, JSObject::HASH_OFFSET,
                              builder_.Int64(JSTaggedValue(0).GetRawData()));
    builder_.StoreConstOffset(VariableType::JS_POINTER(), object, JSObject::PROPERTIES_OFFSET, emptyArray,
                              MemoryAttribute::NoBarrier());
    builder_.StoreConstOffset(VariableType::JS_POINTER(), object, JSObject::ELEMENTS_OFFSET, emptyArray,
                              MemoryAttribute::NoBarrier());
    GateRef result = builder_.FinishAllocate(object);

    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), result);
}

void TypedBytecodeLowering::LowerTypedStOwnByValue(GateRef gate)
{
    // StOwnByValue is rarely used, so the callruntime solution is used
    AddProfiling(gate);
    return;
}

void TypedBytecodeLowering::LowerCreateObjectWithBuffer(GateRef gate)
{
    CreateObjWithBufferTypeInfoAccessor tacc(compilationEnv_, circuit_, gate, recordName_, chunk_);
    if (!tacc.CanOptimize()) {
        return;
    }
    JSTaggedValue hclassVal = tacc.GetHClass();
    if (hclassVal.IsUndefined()) {
        return;
    }
    JSHClass *newClass = JSHClass::Cast(hclassVal.GetTaggedObject());
    GateRef index = tacc.GetIndex();
    JSTaggedValue obj = tacc.GetObject();
    if (obj.IsUndefined()) {
        return;
    }
    JSObject *objhandle = JSObject::Cast(obj);
    std::vector<uint64_t> inlinedProps;
    auto layout = LayoutInfo::Cast(newClass->GetLayout().GetTaggedObject());
    uint32_t numOfProps = newClass->NumberOfProps();
    uint32_t numInlinedProps = newClass->GetInlinedProperties();
    for (uint32_t i = 0; i < numOfProps; i++) {
        auto attr = layout->GetAttr(i);
        JSTaggedValue value = objhandle->GetPropertyInlinedProps(i);
        if ((!attr.IsTaggedRep()) || value.IsUndefinedOrNull() ||
            value.IsNumber() || value.IsBoolean() || value.IsException()) {
            auto converted = JSObject::ConvertValueWithRep(attr, value);
            if (!converted.first) {
                return;
            }
            // CanOptimize.GetObject had convert value, just used directly
            inlinedProps.emplace_back(value.GetRawData());
        } else {
            return;
        }
    }

    AddProfiling(gate);
    auto size = newClass->GetObjectSize();
    std::vector<GateRef> valueIn;
    valueIn.emplace_back(builder_.IntPtr(size));
    valueIn.emplace_back(index);
    valueIn.emplace_back(builder_.Int64(JSTaggedValue(newClass).GetRawData()));
    valueIn.emplace_back(acc_.GetValueIn(gate, 1));
    for (uint32_t i = 0; i < numOfProps; i++) {
        auto attr = layout->GetAttr(i);
        GateRef prop;
        if (attr.IsIntRep()) {
            prop = builder_.Int32(inlinedProps.at(i));
        } else if (attr.IsTaggedRep()) {
            prop = circuit_->NewGate(circuit_->GetMetaBuilder()->Constant(inlinedProps.at(i)),
                                     MachineType::I64, GateType::AnyType());
        } else if (attr.IsDoubleRep()) {
            prop = circuit_->NewGate(circuit_->GetMetaBuilder()->Constant(inlinedProps.at(i)),
                                     MachineType::F64, GateType::NJSValue());
        } else {
            prop = builder_.Int64(inlinedProps.at(i));
        }
        valueIn.emplace_back(prop);
        valueIn.emplace_back(builder_.Int32(newClass->GetInlinedPropertiesOffset(i)));
    }
    GateRef prop = newClass->IsAOT() ? builder_.Hole() : builder_.Undefined();
    for (uint32_t i = numOfProps; i < numInlinedProps; i++) {
        valueIn.emplace_back(prop);
        valueIn.emplace_back(builder_.Int32(newClass->GetInlinedPropertiesOffset(i)));
    }
    GateRef ret = builder_.TypedCreateObjWithBuffer(valueIn);
    if (compilationEnv_->SupportHeapConstant()) {
        auto *jitCompilationEnv = static_cast<JitCompilationEnv*>(compilationEnv_);
        JSHandle<JSTaggedValue> jsObjectHandle = jitCompilationEnv->NewJSHandle(obj);
        auto methodOffset = acc_.TryGetMethodOffset(gate);
        auto objIndex = acc_.GetConstantValue(index);
        auto constpool = jitCompilationEnv->GetConstantPoolByMethodOffset(methodOffset);
        ASSERT(!constpool.IsUndefined());
        auto constpoolId = static_cast<uint32_t>(
            ConstantPool::Cast(constpool.GetTaggedObject())->GetSharedConstpoolId().GetInt());
        uint32_t indexInConstantTable = jitCompilationEnv->RecordHeapConstant(
            { constpoolId, objIndex, JitCompilationEnv::IN_UNSHARED_CONSTANTPOOL }, jsObjectHandle);
        jitCompilationEnv->RecordGate2HeapConstantIndex(ret, indexInConstantTable);
    }
    acc_.ReplaceHirAndDeleteIfException(gate, builder_.GetStateDepend(), ret);
}

void TypedBytecodeLowering::ReplaceGateWithPendingException(GateRef glue, GateRef gate, GateRef state, GateRef depend,
                                                            GateRef value)
{
    auto condition = builder_.HasPendingException(glue);
    GateRef ifBranch = builder_.Branch(state, condition, 1, BranchWeight::DEOPT_WEIGHT, "checkException");
    GateRef ifTrue = builder_.IfTrue(ifBranch);
    GateRef ifFalse = builder_.IfFalse(ifBranch);
    GateRef eDepend = builder_.DependRelay(ifTrue, depend);
    GateRef sDepend = builder_.DependRelay(ifFalse, depend);

    StateDepend success(ifFalse, sDepend);
    StateDepend exception(ifTrue, eDepend);
    acc_.ReplaceHirWithIfBranch(gate, success, exception, value);
}
}  // namespace panda::ecmascript
