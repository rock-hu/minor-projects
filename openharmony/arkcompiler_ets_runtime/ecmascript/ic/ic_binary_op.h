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

#ifndef ECMASCRIPT_IC_IC_BINARY_OP_H_
#define ECMASCRIPT_IC_IC_BINARY_OP_H_

#include "ecmascript/ic/profile_type_info.h"
#include "ecmascript/interpreter/slow_runtime_stub.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/property_attributes.h"
#include "ecmascript/runtime_call_id.h"

namespace panda::ecmascript {
enum class BinaryType : uint8_t {
    NUMBER,
    NUMBER_GEN,
    STRING,
    STRING_GEN,
    GENERIC,
};

class ICBinaryOP {
public:
    static inline JSTaggedValue AddWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, AddWithTSType);
        BinaryType addType = static_cast<BinaryType>(argType.GetInt());
        switch (addType) {
            // Support cases, such as: int + double, int + int, double + double
            case BinaryType::NUMBER: {
                double a0Double = left.IsInt() ? left.GetInt() : left.GetDouble();
                double a1Double = right.IsInt() ? right.GetInt() : right.GetDouble();
                double ret = a0Double + a1Double;
                return JSTaggedValue(ret);
            }
            // Support cases, such as: number + null, undefined + null, boolean + number, etc.
            case BinaryType::NUMBER_GEN: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                JSHandle<JSTaggedValue> primitiveA0(thread, JSTaggedValue::ToPrimitive(thread, leftValue));
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                JSHandle<JSTaggedValue> primitiveA1(thread, JSTaggedValue::ToPrimitive(thread, rightValue));
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

                JSTaggedNumber taggedValueA0 = JSTaggedValue::ToNumber(thread, primitiveA0);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                JSTaggedNumber taggedValueA1 = JSTaggedValue::ToNumber(thread, primitiveA1);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                double a0Double = taggedValueA0.GetNumber();
                double a1Double = taggedValueA1.GetNumber();
                return JSTaggedValue(a0Double + a1Double);
            }
            // Support case: string + string.
            case BinaryType::STRING: {
                JSHandle<EcmaString> stringA0 = JSHandle<EcmaString>(JSHandle<JSTaggedValue>(thread, left));
                JSHandle<EcmaString> stringA1 = JSHandle<EcmaString>(JSHandle<JSTaggedValue>(thread, right));
                EcmaString *ret = EcmaStringAccessor::Concat(thread->GetEcmaVM(), stringA0, stringA1);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                return JSTaggedValue(ret);
            }
            // Support cases, such as: string + null, string + object, string + boolean, string + number, etc.
            case BinaryType::STRING_GEN: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                if (left.IsString()) {
                    JSHandle<EcmaString> stringA0 = JSHandle<EcmaString>(leftValue);
                    JSHandle<EcmaString> stringA1 = JSTaggedValue::ToString(thread, rightValue);
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                    EcmaString *ret = EcmaStringAccessor::Concat(thread->GetEcmaVM(), stringA0, stringA1);
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                    return JSTaggedValue(ret);
                } else {
                    JSHandle<EcmaString> stringA0 = JSTaggedValue::ToString(thread, leftValue);
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                    JSHandle<EcmaString> stringA1 = JSHandle<EcmaString>(rightValue);
                    EcmaString *ret = EcmaStringAccessor::Concat(thread->GetEcmaVM(), stringA0, stringA1);
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                    return JSTaggedValue(ret);
                }
            }
            // Some special cases, such as: object + undefined, object + boolean, etc.
            case BinaryType::GENERIC: {
                JSTaggedValue res = SlowRuntimeStub::Add2(thread, left, right);
                return res;
            }
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
    }

    static inline JSTaggedValue SubWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, SubWithTSType);
        BinaryType subType = static_cast<BinaryType>(argType.GetInt());
        switch (subType) {
            // Support int or number
            case BinaryType::NUMBER: {
                double a0Double = left.IsInt() ? left.GetInt() : left.GetDouble();
                double a1Double = right.IsInt() ? right.GetInt() : right.GetDouble();
                double ret = a0Double - a1Double;
                return JSTaggedValue(ret);
            }
            // Support cases, such as: string like '2333', boolean, null
            case BinaryType::GENERIC: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                JSTaggedNumber number0 = JSTaggedValue::ToNumber(thread, leftValue);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                JSTaggedNumber number1 = JSTaggedValue::ToNumber(thread, rightValue);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                auto ret = number0 - number1;
                return JSTaggedValue(ret);
            }
            case BinaryType::NUMBER_GEN:
            case BinaryType::STRING:
            case BinaryType::STRING_GEN:
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
    }
    static inline JSTaggedValue MulWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, MulWithTSType);
        BinaryType mulType = static_cast<BinaryType>(argType.GetInt());
        switch (mulType) {
            // Support int or number
            case BinaryType::NUMBER: {
                return JSTaggedValue(left.GetNumber() * right.GetNumber());
            }
            // Support cases, such as: string like '2333', boolean, null
            case BinaryType::GENERIC: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                // 6. Let lnum be ToNumber(leftValue).
                JSTaggedNumber primitiveA = JSTaggedValue::ToNumber(thread, leftValue);
                // 7. ReturnIfAbrupt(lnum).
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                // 8. Let rnum be ToNumber(rightValue).
                JSTaggedNumber primitiveB = JSTaggedValue::ToNumber(thread, rightValue);
                // 9. ReturnIfAbrupt(rnum).
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                // 12.6.3.1 Applying the * Operator
                return primitiveA * primitiveB;
            }
            case BinaryType::NUMBER_GEN:
            case BinaryType::STRING:
            case BinaryType::STRING_GEN:
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
    }

    static inline JSTaggedValue DivWithTSType(JSThread *thread, JSTaggedValue left,
                                             JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, DivWithTSType);
        BinaryType divType = static_cast<BinaryType>(argType.GetInt());
        switch (divType) {
            // Support int or number
            case BinaryType::NUMBER: {
                double dLeft = left.IsInt() ? left.GetInt() : left.GetDouble();
                double dRight = right.IsInt() ? right.GetInt() : right.GetDouble();
                if (UNLIKELY(dRight == 0.0)) {
                    if (dLeft == 0.0 || std::isnan(dLeft)) {
                        return JSTaggedValue(base::NAN_VALUE);
                    }
                    uint64_t flagBit = ((base::bit_cast<uint64_t>(dLeft)) ^ (base::bit_cast<uint64_t>(dRight))) &
                                        base::DOUBLE_SIGN_MASK;
                    return JSTaggedValue(base::bit_cast<double>(
                        flagBit ^ (base::bit_cast<uint64_t>(base::POSITIVE_INFINITY))));
                }
                return JSTaggedValue(dLeft / dRight);
            }
            // Support special cases, such as: string like '2333', boolean, null
            case BinaryType::GENERIC: {
                auto res = SlowRuntimeStub::Div2(thread, left, right);
                return res;
            }
            case BinaryType::NUMBER_GEN:
            case BinaryType::STRING:
            case BinaryType::STRING_GEN:
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
    }

    static inline JSTaggedValue ModWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, ModWithTSType);
        BinaryType modType = static_cast<BinaryType>(argType.GetInt());
        switch (modType) {
            // Support int or number
            case BinaryType::NUMBER: {
                double dLeft = left.IsInt() ? left.GetInt() : left.GetDouble();
                double dRight = right.IsInt() ? right.GetInt() : right.GetDouble();
                if (dRight == 0.0 || std::isnan(dRight) || std::isnan(dLeft) || std::isinf(dLeft)) {
                    return JSTaggedValue(base::NAN_VALUE);
                }
                if (dLeft == 0.0 || std::isinf(dRight)) {
                    return JSTaggedValue(dLeft);
                }
                return JSTaggedValue(std::fmod(dLeft, dRight));
            }
            // Support special cases, such as: string like '2333', boolean, null
            case BinaryType::GENERIC: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                JSTaggedNumber leftNumber = JSTaggedValue::ToNumber(thread, leftValue);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                double dLeft = leftNumber.GetNumber();
                JSTaggedNumber rightNumber = JSTaggedValue::ToNumber(thread, rightValue);
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                double dRight = rightNumber.GetNumber();
                // 12.6.3.3 Applying the % Operator
                if ((dRight == 0.0) || std::isnan(dRight) || std::isnan(dLeft) || std::isinf(dLeft)) {
                    return JSTaggedValue(base::NAN_VALUE);
                }
                if ((dLeft == 0.0) || std::isinf(dRight)) {
                    return JSTaggedValue(dLeft);
                }
                return JSTaggedValue(std::fmod(dLeft, dRight));
            }
            case BinaryType::NUMBER_GEN:
            case BinaryType::STRING:
            case BinaryType::STRING_GEN:
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
    }

    static inline void GetBitOPDate(JSThread *thread, JSTaggedValue left, JSTaggedValue right,
                                    int32_t &opNumber0, int32_t &opNumber1, BinaryType opType)
    {
        INTERPRETER_TRACE(thread, GetBitOPDate);
        switch (opType) {
            case BinaryType::NUMBER: {
                opNumber0 =
                    left.IsInt() ? left.GetInt() :
                                   base::NumberHelper::DoubleToInt(left.GetDouble(), base::INT32_BITS);
                opNumber1 =
                    right.IsInt() ? right.GetInt() :
                                    base::NumberHelper::DoubleToInt(right.GetDouble(), base::INT32_BITS);
                break;
            }
            // Support special cases, such as: string like '2333', boolean, null
            case BinaryType::GENERIC: {
                JSHandle<JSTaggedValue> leftValue(thread, left);
                JSHandle<JSTaggedValue> rightValue(thread, right);
                JSTaggedValue taggedNumber0 = SlowRuntimeStub::ToJSTaggedValueWithInt32(thread,
                                                                                        leftValue.GetTaggedValue());
                JSTaggedValue taggedNumber1 = SlowRuntimeStub::ToJSTaggedValueWithUint32(thread,
                                                                                         rightValue.GetTaggedValue());
                opNumber0 = taggedNumber0.GetInt();
                opNumber1 = taggedNumber1.GetInt();
                break;
            }
            case BinaryType::NUMBER_GEN:
            case BinaryType::STRING:
            case BinaryType::STRING_GEN:
            default: {
                LOG_ECMA(FATAL) << "this branch is unreachable";
                UNREACHABLE();
            }
        }
        return;
    }

    static inline JSTaggedValue ShlWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, ShlWithTSType);
        BinaryType shlType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, shlType);
        uint32_t shift =
                static_cast<uint32_t>(opNumber1) & 0x1f;  // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
        using unsigned_type = std::make_unsigned_t<int32_t>;
        auto ret =
            static_cast<int32_t>(static_cast<unsigned_type>(opNumber0) << shift);  // NOLINT(hicpp-signed-bitwise)
        return JSTaggedValue(ret);
    }

    static inline JSTaggedValue ShrWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, ShrWithTSType);
        BinaryType shrType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, shrType);
        uint32_t shift =
                static_cast<uint32_t>(opNumber1) & 0x1f;     // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
        auto ret = static_cast<int32_t>(opNumber0 >> shift);  // NOLINT(hicpp-signed-bitwise)
        return JSTaggedValue(ret);
    }

    static inline JSTaggedValue AshrWithTSType(JSThread *thread, JSTaggedValue left,
                                               JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, AshrWithTSType);
        BinaryType ashrType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, ashrType);
        uint32_t shift =
                static_cast<uint32_t>(opNumber1) & 0x1f;  // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
        using unsigned_type = std::make_unsigned_t<uint32_t>;
        auto ret =
            static_cast<uint32_t>(static_cast<unsigned_type>(opNumber0) >> shift);  // NOLINT(hicpp-signed-bitwise)
        return JSTaggedValue(ret);
    }

    static inline JSTaggedValue AndWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, AndWithTSType);
        BinaryType andType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, andType);
        // NOLINT(hicpp-signed-bitwise)
        auto ret = static_cast<uint32_t>(opNumber0) & static_cast<uint32_t>(opNumber1);
        return JSTaggedValue(ret);
    }

    static inline JSTaggedValue OrWithTSType(JSThread *thread, JSTaggedValue left,
                                             JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, OrWithTSType);
        BinaryType orType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, orType);
        // NOLINT(hicpp-signed-bitwise)
        auto ret = static_cast<uint32_t>(opNumber0) | static_cast<uint32_t>(opNumber1);
        return JSTaggedValue(ret);
    }

    static inline JSTaggedValue XorWithTSType(JSThread *thread, JSTaggedValue left,
                                              JSTaggedValue right, JSTaggedValue argType)
    {
        INTERPRETER_TRACE(thread, XorWithTSType);
        BinaryType xorType = static_cast<BinaryType>(argType.GetInt());
        int32_t opNumber0 = 0;
        int32_t opNumber1 = 0;
        GetBitOPDate(thread, left, right, opNumber0, opNumber1, xorType);
        // NOLINT(hicpp-signed-bitwise)
        auto ret = static_cast<uint32_t>(opNumber0) ^ static_cast<uint32_t>(opNumber1);
        return JSTaggedValue(ret);
    }
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_IC_IC_BINARY_OP_H_
