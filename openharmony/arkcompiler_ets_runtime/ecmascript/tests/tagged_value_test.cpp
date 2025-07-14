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

#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_bigint-inl.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_hclass.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/js_symbol.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory-inl.h"
#include "ecmascript/snapshot/mem/snapshot.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;

namespace panda::test {
constexpr uint32_t TEST_NUMBER = 1;
constexpr uint32_t TEST_TAGGED_ARRAY_LENGTH = 1;
const std::string TEST_STRING = "testString";

class JSTaggedValueTest : public BaseTestWithScope<false> {
public:
    JSTaggedNumber NumberFromASCII(std::string_view data)
    {
        JSHandle<JSTaggedValue> stringV(thread->GetEcmaVM()->GetFactory()->NewFromASCII(data));
        return JSTaggedValue::ToNumber(thread, stringV);
    }
};

HWTEST_F_L0(JSTaggedValueTest, Double)
{
    double d = 1.1;
    JSTaggedValue td(d);
    EXPECT_EQ(true, td.IsDouble());
    EXPECT_EQ(false, td.IsInt());
    EXPECT_EQ(false, td.IsObject());
    ASSERT_DOUBLE_EQ(td.GetDouble(), d);

    double nan = std::nan("");
    JSTaggedValue tNan(nan);
    EXPECT_EQ(true, tNan.IsDouble());
    EXPECT_EQ(false, tNan.IsInt());
    EXPECT_EQ(false, tNan.IsObject());
    EXPECT_EQ(JSTaggedValue::CastDoubleToTagged(tNan.GetDouble()), JSTaggedValue::CastDoubleToTagged(nan));

    double pureNaN = JSTaggedValue::CastTaggedToDouble(JSTaggedValue::TAG_INT - JSTaggedValue::DOUBLE_ENCODE_OFFSET);
    EXPECT_EQ(true, JSTaggedValue::IsImpureNaN(pureNaN));
}

HWTEST_F_L0(JSTaggedValueTest, Int)
{
    int i = 0x5c;
    JSTaggedValue t(0x5c);
    EXPECT_EQ(true, t.IsInt());
    EXPECT_EQ(false, t.IsObject());
    EXPECT_EQ(false, t.IsDouble());
    EXPECT_EQ(t.GetInt(), i);
}

HWTEST_F_L0(JSTaggedValueTest, IsObject)
{
    TaggedObject *p = reinterpret_cast<TaggedObject *>(0xffff0000UL);
    JSTaggedValue t(p);
    EXPECT_EQ(true, t.IsObject());
    EXPECT_EQ(false, t.IsInt());
    EXPECT_EQ(false, t.IsDouble());
    EXPECT_EQ(t.GetTaggedObject(), p);
}

HWTEST_F_L0(JSTaggedValueTest, WeakRef)
{
    TaggedObject *p = reinterpret_cast<TaggedObject *>(0xffff0000UL);
    JSTaggedValue tInt(0x5c);
    JSTaggedValue t(p);

    t.CreateWeakRef();
    EXPECT_EQ(false, tInt.IsWeak());
    EXPECT_EQ(true, t.IsObject());
    EXPECT_EQ(true, t.IsWeak());
    EXPECT_EQ(false, t.IsInt());
    EXPECT_EQ(false, t.IsDouble());
    EXPECT_EQ(t.GetTaggedWeakRef(), p);
}

HWTEST_F_L0(JSTaggedValueTest, False)
{
    JSTaggedValue t = JSTaggedValue::False();
    EXPECT_EQ(t.IsFalse(), true);
}

HWTEST_F_L0(JSTaggedValueTest, True)
{
    JSTaggedValue t = JSTaggedValue::True();
    EXPECT_EQ(t.IsTrue(), true);
}

HWTEST_F_L0(JSTaggedValueTest, Undefined)
{
    JSTaggedValue t = JSTaggedValue::Undefined();
    EXPECT_EQ(t.IsUndefined(), true);
}
HWTEST_F_L0(JSTaggedValueTest, Null)
{
    JSTaggedValue t = JSTaggedValue::Null();
    EXPECT_EQ(t.IsNull(), true);
}

HWTEST_F_L0(JSTaggedValueTest, Hole)
{
    JSTaggedValue t = JSTaggedValue::Hole();
    EXPECT_EQ(t.IsHole(), true);
}

HWTEST_F_L0(JSTaggedValueTest, ToPrimitive)
{
    JSTaggedValue result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result.GetInt(), 100);

    JSTaggedValue doubleV((double)100.0);
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, doubleV));
    EXPECT_EQ(result.GetDouble(), (double)100.0);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_TRUE(result.IsUndefined());

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_TRUE(result.IsHole());

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_TRUE(result.IsNull());

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_TRUE(result.IsFalse());

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToPrimitive(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_TRUE(result.IsTrue());
}

HWTEST_F_L0(JSTaggedValueTest, ToBoolean)
{
    EXPECT_TRUE(JSTaggedValue(100).ToBoolean());
    EXPECT_FALSE(JSTaggedValue(0).ToBoolean());

    EXPECT_TRUE(JSTaggedValue((double)100.0).ToBoolean());
    EXPECT_FALSE(JSTaggedValue(std::nan("")).ToBoolean());

    EXPECT_FALSE(JSTaggedValue::Undefined().ToBoolean());

    EXPECT_FALSE(JSTaggedValue::Hole().ToBoolean());

    EXPECT_FALSE(JSTaggedValue::Null().ToBoolean());

    EXPECT_FALSE(JSTaggedValue::False().ToBoolean());
    EXPECT_TRUE(JSTaggedValue::True().ToBoolean());

    EXPECT_FALSE(thread->GetEcmaVM()->GetFactory()->GetEmptyString().GetTaggedValue().ToBoolean());
    EXPECT_TRUE(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test").GetTaggedValue().ToBoolean());
}

HWTEST_F_L0(JSTaggedValueTest, ToNumber)
{
    JSTaggedNumber result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result.GetNumber(), 100);

    JSTaggedValue doubleV((double)100.0);
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, doubleV));
    EXPECT_EQ(result.GetNumber(), (double)100.0);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_TRUE(std::isnan(result.GetNumber()));

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_TRUE(std::isnan(result.GetNumber()));

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToNumber(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result.GetNumber(), 1);

    thread->ClearException();
    JSHandle<JSTaggedValue> symbolV1(thread->GetEcmaVM()->GetFactory()->NewJSSymbol());
    JSTaggedValue::ToNumber(thread, symbolV1);
    EXPECT_TRUE(thread->HasPendingException());
    EXPECT_TRUE(thread->GetException().IsJSError());
}

HWTEST_F_L0(JSTaggedValueTest, StringToNumber)
{
    EXPECT_EQ(NumberFromASCII(" 1234 ").GetNumber(), 1234);
    EXPECT_EQ(NumberFromASCII(" 0b1010 ").GetNumber(), 10);
    EXPECT_EQ(NumberFromASCII(" 0O11 ").GetNumber(), 9);
    EXPECT_EQ(NumberFromASCII(" 0x2d ").GetNumber(), 45);
    EXPECT_EQ(NumberFromASCII(" 0.000001 ").GetNumber(), 0.000001);
    EXPECT_EQ(NumberFromASCII(" 1.23 ").GetNumber(), 1.23);
    EXPECT_EQ(NumberFromASCII(" -1.23e2  ").GetNumber(), -123);
    EXPECT_EQ(NumberFromASCII(" -123e-2").GetNumber(), -1.23);
    EXPECT_TRUE(std::isinf(NumberFromASCII("  Infinity ").GetNumber()));
    EXPECT_TRUE(std::isinf(NumberFromASCII("100e307").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("  .").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("12e+").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII(".e3").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("23eE").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("a").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("0o12e3").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("0x12.3").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII(" 12.4.").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII("123test").GetNumber()));
    EXPECT_TRUE(std::isnan(NumberFromASCII(" 0b ").GetNumber()));
    EXPECT_EQ(NumberFromASCII(" 0b0000 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 0o0000 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 0X0000 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 000.00000 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII("").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII("   ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII("0").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 0 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII("00000000").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 00000000 ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 000. ").GetNumber(), 0);
    EXPECT_EQ(NumberFromASCII(" 123. ").GetNumber(), 123);
}

HWTEST_F_L0(JSTaggedValueTest, ToInteger)
{
    JSTaggedNumber result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result.GetNumber(), 100);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result.GetNumber(), (double)100.0);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result.GetNumber(), (double)100);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToInteger(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result.GetNumber(), 1);
}

HWTEST_F_L0(JSTaggedValueTest, ToInt32)
{
    int32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100);

    double input1 = (static_cast<uint64_t>(UINT32_MAX) + 1) + 12345;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 12345);

    double input2 = 100 * (static_cast<uint64_t>(UINT32_MAX) + 1) + 23456;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 23456);

    double input3 = 100 * (static_cast<uint64_t>(UINT32_MAX) + 1) + INT32_MAX + 1 + 23456;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, 23456 - static_cast<int32_t>(INT32_MAX) - 1);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToInt32(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1);
}

HWTEST_F_L0(JSTaggedValueTest, ToUint32)
{
    uint32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100U);

    double input1 = (static_cast<uint64_t>(UINT32_MAX) + 1) + 12345;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 12345U);

    double input2 = 100 * (static_cast<uint64_t>(UINT32_MAX) + 1) + 23456;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 23456U);

    double input3 = 100 * (static_cast<uint64_t>(UINT32_MAX) + 1) + INT32_MAX + 1 + 23456;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, static_cast<uint32_t>(static_cast<uint64_t>(INT32_MAX) + 1 + 23456));

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToUint32(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1U);
}

HWTEST_F_L0(JSTaggedValueTest, ToInt16)
{
    int32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100);

    double input1 = (static_cast<uint64_t>(UINT16_MAX) + 1) + 12345;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 12345);

    double input2 = 100 * (static_cast<uint64_t>(UINT16_MAX) + 1) + 23456;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 23456);

    double input3 = 100 * (static_cast<uint64_t>(UINT16_MAX) + 1) + INT16_MAX + 1 + 23456;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, 23456 - static_cast<int32_t>(INT16_MAX) - 1);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToInt16(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1);
}

HWTEST_F_L0(JSTaggedValueTest, ToUint16)
{
    uint32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100U);

    double input1 = (static_cast<uint64_t>(UINT16_MAX) + 1) + 12345;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 12345U);

    double input2 = 100 * (static_cast<uint64_t>(UINT16_MAX) + 1) + 23456;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 23456U);

    double input3 = 100 * (static_cast<uint64_t>(UINT16_MAX) + 1) + INT16_MAX + 1 + 23456;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, static_cast<uint64_t>(INT16_MAX + 1 + 23456));

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToUint16(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1U);
}

HWTEST_F_L0(JSTaggedValueTest, ToInt8)
{
    int32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100);

    double input1 = (static_cast<uint64_t>(UINT8_MAX) + 1) + 45;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 45);

    double input2 = 100 * (static_cast<uint64_t>(UINT8_MAX) + 1) + 56;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 56);

    double input3 = 100 * (static_cast<uint64_t>(UINT8_MAX) + 1) + INT8_MAX + 1 + 23;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, 23 - static_cast<int32_t>(INT8_MAX) - 1);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToInt8(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1);
}

HWTEST_F_L0(JSTaggedValueTest, ToUint8)
{
    uint32_t result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100U);

    double input1 = (static_cast<uint64_t>(UINT8_MAX) + 1) + 34;
    JSTaggedValue doubleV3(input1);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 34U);

    double input2 = 100 * (static_cast<uint64_t>(UINT8_MAX) + 1) + 45;
    JSTaggedValue doubleV4(input2);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 45U);

    double input3 = 100 * (static_cast<uint64_t>(UINT8_MAX) + 1) + INT8_MAX + 1 + 56;
    JSTaggedValue doubleV5(input3);
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, static_cast<uint64_t>(INT8_MAX + 1 + 56));

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToUint8(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1U);
}

HWTEST_F_L0(JSTaggedValueTest, ToUint8Clamp)
{
    uint32_t result;

    JSTaggedValue intV1(-100);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, intV1));
    EXPECT_EQ(result, 0U);

    JSTaggedValue intV2(100);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, intV2));
    EXPECT_EQ(result, 100U);

    JSTaggedValue intV3(300);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, intV3));
    EXPECT_EQ(result, 255U);

    JSTaggedValue doubleV1((double)-100.123);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result, 0U);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV3((double)100.55);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result, 101U);

    JSTaggedValue doubleV4((double)99.9);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result, 100U);

    JSTaggedValue doubleV5((double)300.5);
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, doubleV5));
    EXPECT_EQ(result, 255U);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result, 0U);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToUint8Clamp(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result, 1U);
}

HWTEST_F_L0(JSTaggedValueTest, ToPropertyKey)
{
    JSTaggedValue result;
    JSHandle<EcmaString> str = thread->GetEcmaVM()->GetFactory()->NewFromASCII("null");
    JSTaggedValue key = str.GetTaggedValue();
    result = JSTaggedValue::ToPropertyKey(thread, JSHandle<JSTaggedValue>(thread, key)).GetTaggedValue();
    EXPECT_TRUE(key == result);
}

void CheckOkString(JSThread *thread, const JSHandle<JSTaggedValue> &tagged, CString &rightCStr)
{
    JSHandle<EcmaString> result = JSTaggedValue::ToString(thread, tagged);
    JSHandle<EcmaString> rightString = thread->GetEcmaVM()->GetFactory()->NewFromASCII(rightCStr);
    EXPECT_TRUE(EcmaStringAccessor::StringsAreEqual(thread, EcmaString::Cast(result.GetObject<EcmaString>()),
                                                    EcmaString::Cast(rightString.GetObject<EcmaString>())));
}

HWTEST_F_L0(JSTaggedValueTest, ToString)
{
    CString rightCStr = "";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue()), rightCStr);

    rightCStr = "undefined";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined()), rightCStr);

    rightCStr = "null";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()), rightCStr);

    rightCStr = "true";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()), rightCStr);

    rightCStr = "false";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()), rightCStr);

    rightCStr = "hello world";
    CheckOkString(thread,
                  JSHandle<JSTaggedValue>(thread->GetEcmaVM()->GetFactory()->NewFromASCII(rightCStr)),
                  rightCStr);

    double num = 1;
    JSTaggedNumber numberNum = JSTaggedNumber(num);
    rightCStr = "1";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, numberNum), rightCStr);

    num = 1.23;
    numberNum = JSTaggedNumber(num);
    rightCStr = "1.23";
    CheckOkString(thread, JSHandle<JSTaggedValue>(thread, numberNum), rightCStr);

    int numInt = 2;
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(numInt));
    rightCStr = "2";
    CheckOkString(thread, JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToObject(thread, value1)), rightCStr);

    num = 1.23;
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(num));
    rightCStr = "1.23";
    CheckOkString(thread, JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToObject(thread, value2)), rightCStr);

    bool valueBool = true;
    JSHandle<JSTaggedValue> value3(thread, JSTaggedValue(valueBool));
    rightCStr = "true";
    CheckOkString(thread, JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToObject(thread, value3)), rightCStr);
}

void CheckOkExceptionString(JSThread *thread, const JSHandle<JSTaggedValue> &tagged, std::string &rightCStr)
{
    std::string result = JSTaggedValue::ExceptionToString(thread, tagged);
    EXPECT_EQ(result, rightCStr);
}

HWTEST_F_L0(JSTaggedValueTest, ExceptionToString)
{
    std::string rightCStr = "";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue()), rightCStr);

    JSHandle<JSSymbol> symbol = thread->GetEcmaVM()->GetFactory()->NewJSSymbol();
    CheckOkExceptionString(thread,  JSHandle<JSTaggedValue>::Cast(symbol), rightCStr);

    rightCStr = "undefined";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined()), rightCStr);

    rightCStr = "null";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()), rightCStr);

    rightCStr = "true";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()), rightCStr);

    rightCStr = "false";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()), rightCStr);

    rightCStr = "hello world";
    CheckOkExceptionString(thread,
                  JSHandle<JSTaggedValue>(thread->GetEcmaVM()->GetFactory()->NewFromASCII(rightCStr)),
                  rightCStr);
}

HWTEST_F_L0(JSTaggedValueTest, ExceptionToString1)
{
    std::string rightCStr = "1";
    double num = 1;
    JSTaggedNumber numberNum = JSTaggedNumber(num);
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, numberNum), rightCStr);

    num = 1.23;
    numberNum = JSTaggedNumber(num);
    rightCStr = "1.23";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>(thread, numberNum), rightCStr);

    int numInt = 2;
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(numInt));
    rightCStr = "2";
    CheckOkExceptionString(thread, value1, rightCStr);

    rightCStr = "4294967295";
    JSHandle<BigInt> newBigint = BigInt::CreateBigint(thread, 2);
    newBigint->SetDigit(0, std::numeric_limits<uint32_t>::max());
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>::Cast(newBigint), rightCStr);

    num = 1.23;
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(num));
    rightCStr = "1.23";
    CheckOkExceptionString(thread, value2, rightCStr);

    bool valueBool = true;
    JSHandle<JSTaggedValue> value3(thread, JSTaggedValue(valueBool));
    rightCStr = "true";
    CheckOkExceptionString(thread, value3, rightCStr);

    JSHandle<JSTaggedValue> value4(thread, JSTaggedValue(valueBool));
    rightCStr = "Object";
    CheckOkExceptionString(thread, JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToObject(thread, value4)), rightCStr);

    auto *newBitSetVector = new std::vector<std::bitset<JSAPIBitVector::BIT_SET_LENGTH>>();
    auto deleter = []([[maybe_unused]] void *env, void *pointer, [[maybe_unused]] void *data) {
        if (pointer == nullptr) {
            return;
        }
        delete reinterpret_cast<std::vector<std::bitset<JSAPIBitVector::BIT_SET_LENGTH>> *>(pointer);
    };
    JSHandle<JSNativePointer> pointer = thread->GetEcmaVM()->GetFactory()->NewSJSNativePointer(
        newBitSetVector, deleter, newBitSetVector);
    std::string result = JSTaggedValue::ExceptionToString(thread, JSHandle<JSTaggedValue>::Cast(pointer));
    EXPECT_EQ(result.find("[External:") != std::string::npos, true);
}

HWTEST_F_L0(JSTaggedValueTest, CanonicalNumericIndexString)
{
    JSTaggedValue result;
    JSHandle<EcmaString> str = thread->GetEcmaVM()->GetFactory()->NewFromASCII("-0");
    JSTaggedValue tmpStr = str.GetTaggedValue();
    result = JSTaggedValue::CanonicalNumericIndexString(thread, JSHandle<JSTaggedValue>(thread, tmpStr));
    EXPECT_EQ(result.GetDouble(), -0.0);

    JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());
    JSHandle<JSTaggedValue> key1 = JSTaggedValue::ToPropertyKey(thread, undefinedHandle);
    result = JSTaggedValue::CanonicalNumericIndexString(thread, key1);
    EXPECT_TRUE(result.IsUndefined());
}

HWTEST_F_L0(JSTaggedValueTest, ToObject)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(2));
    JSTaggedValue tagged1 =
        JSTaggedValue(JSHandle<JSPrimitiveRef>::Cast(JSTaggedValue::ToObject(thread, value1))->GetValue(thread));
    EXPECT_EQ(tagged1.GetRawData(), JSTaggedValue(2).GetRawData());

    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2.2));
    JSTaggedValue tagged2 =
        JSTaggedValue(JSHandle<JSPrimitiveRef>::Cast(JSTaggedValue::ToObject(thread, value2))->GetValue(thread));
    EXPECT_EQ(tagged2.GetRawData(), JSTaggedValue(static_cast<double>(2.2)).GetRawData());

    JSHandle<JSTaggedValue> value3(thread, JSTaggedValue::True());
    JSTaggedValue tagged3 =
        JSTaggedValue(JSHandle<JSPrimitiveRef>::Cast(JSTaggedValue::ToObject(thread, value3))->GetValue(thread));
    EXPECT_EQ(tagged3.GetRawData(), JSTaggedValue::True().GetRawData());

    JSHandle<JSTaggedValue> value4(factory->NewFromASCII("aaa"));
    JSTaggedValue tagged4 =
        JSTaggedValue(JSHandle<JSPrimitiveRef>::Cast(JSTaggedValue::ToObject(thread, value4))->GetValue(thread));
    EXPECT_TRUE(tagged4.IsString());
    EXPECT_EQ(EcmaStringAccessor::Compare(instance,
        JSHandle<EcmaString>(thread, tagged4), JSHandle<EcmaString>(value4)), 0);

    JSHandle<JSSymbol> symbol = factory->NewPublicSymbolWithChar("bbb");
    JSHandle<EcmaString> str = factory->NewFromASCII("bbb");
    JSHandle<JSTaggedValue> value5(symbol);
    JSTaggedValue tagged5 =
        JSTaggedValue(JSHandle<JSPrimitiveRef>::Cast(JSTaggedValue::ToObject(thread, value5))->GetValue(thread));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, JSHandle<EcmaString>(thread,
        EcmaString::Cast(reinterpret_cast<JSSymbol *>(tagged5.GetRawData())->GetDescription(thread))), str),
        0);
    EXPECT_TRUE(tagged5.IsSymbol());

    EcmaVM *ecma = thread->GetEcmaVM();
    JSHandle<JSTaggedValue> objectFun = ecma->GetGlobalEnv()->GetObjectFunction();
    JSHandle<JSObject> jsObj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objectFun), objectFun);
    JSHandle<JSTaggedValue> value(jsObj);
    EXPECT_EQ(*JSTaggedValue::ToObject(thread, value), *jsObj);
}

HWTEST_F_L0(JSTaggedValueTest, ToLength)
{
    JSTaggedNumber result;

    JSTaggedValue intV(100);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, intV));
    EXPECT_EQ(result.GetNumber(), 100);

    JSTaggedValue intV2(-1);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, intV2));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue doubleV1((double)100.0);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, doubleV1));
    EXPECT_EQ(result.GetNumber(), (double)100.0);

    JSTaggedValue doubleV2((double)100.123);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, doubleV2));
    EXPECT_EQ(result.GetNumber(), (double)100);

    JSTaggedValue doubleV3((double)-1.0);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, doubleV3));
    EXPECT_EQ(result.GetNumber(), (double)0);

    JSTaggedValue doubleV4((double)9007199254740992);
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, doubleV4));
    EXPECT_EQ(result.GetNumber(), (double)9007199254740991);

    JSTaggedValue undefinedV = JSTaggedValue::Undefined();
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, undefinedV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue holeV = JSTaggedValue::Hole();
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, holeV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue nullV = JSTaggedValue::Null();
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, nullV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue falseV = JSTaggedValue::False();
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, falseV));
    EXPECT_EQ(result.GetNumber(), 0);

    JSTaggedValue trueV = JSTaggedValue::True();
    result = JSTaggedValue::ToLength(thread, JSHandle<JSTaggedValue>(thread, trueV));
    EXPECT_EQ(result.GetNumber(), 1);
}

HWTEST_F_L0(JSTaggedValueTest, IsArray)
{
    EcmaVM *ecma = thread->GetEcmaVM();
    JSHandle<JSTaggedValue> objectFun = ecma->GetGlobalEnv()->GetArrayFunction();

    JSHandle<JSObject> jsObj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objectFun), objectFun);

    ASSERT_TRUE(jsObj->IsJSArray());
    ASSERT_FALSE(JSTaggedValue(1).IsArray(thread));

    ASSERT_FALSE(
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("test").GetTaggedValue().IsArray(thread));
}

HWTEST_F_L0(JSTaggedValueTest, IsCallable_IsConstructor_IsExtensible)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> jsFunction = thread->GetEcmaVM()->GetFactory()->NewJSFunction(env);
    JSHClass *jsHclass = jsFunction->GetJSHClass();
    jsHclass->SetCallable(true);
    ASSERT_TRUE(jsFunction->IsCallable());
    jsHclass->SetCallable(false);
    ASSERT_FALSE(jsFunction->IsCallable());
    jsHclass->SetConstructor(true);
    ASSERT_TRUE(jsFunction->IsConstructor());
    jsHclass->SetConstructor(false);
    ASSERT_FALSE(jsFunction->IsConstructor());
    jsHclass->SetExtensible(true);
    ASSERT_TRUE(jsFunction->IsExtensible());
    jsHclass->SetExtensible(false);
    ASSERT_FALSE(jsFunction->IsExtensible());
    ASSERT_FALSE(JSTaggedValue(1).IsExtensible(thread));
    ASSERT_FALSE(JSTaggedValue(1).IsConstructor());
    ASSERT_FALSE(JSTaggedValue(1).IsCallable());
}

HWTEST_F_L0(JSTaggedValueTest, IsInteger)
{
    ASSERT_TRUE(JSTaggedValue(1).IsInteger());
    ASSERT_TRUE(JSTaggedValue(1.0).IsInteger());
    ASSERT_TRUE(JSTaggedValue(-1.0).IsInteger());
    ASSERT_FALSE(JSTaggedValue(-1.1).IsInteger());
    ASSERT_FALSE(JSTaggedValue(1.1).IsInteger());
    ASSERT_FALSE(JSTaggedValue(std::numeric_limits<double>::infinity()).IsInteger());
    ASSERT_FALSE(JSTaggedValue((-1) * std::numeric_limits<double>::infinity()).IsInteger());
    ASSERT_TRUE(JSTaggedValue(0).IsInteger());
    ASSERT_TRUE(JSTaggedValue(0.0).IsInteger());
    ASSERT_FALSE(JSTaggedValue::True().IsInteger());
    ASSERT_FALSE(JSTaggedValue::Undefined().IsInteger());
    ASSERT_FALSE(JSTaggedValue::Null().IsInteger());
    ASSERT_FALSE(JSTaggedValue::False().IsInteger());
    ASSERT_FALSE(JSTaggedValue::Hole().IsInteger());
    ASSERT_FALSE(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test").GetTaggedValue().IsInteger());
}

HWTEST_F_L0(JSTaggedValueTest, IsPropertyKey)
{
    ASSERT_TRUE(JSTaggedValue::IsPropertyKey(
        JSHandle<JSTaggedValue>(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test"))));
}

HWTEST_F_L0(JSTaggedValueTest, IsRegExp)
{
    JSHandle<EcmaString> string = thread->GetEcmaVM()->GetFactory()->NewFromASCII("test");
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>::Cast(string);
    ASSERT_FALSE(JSObject::IsRegExp(thread, obj));
}

HWTEST_F_L0(JSTaggedValueTest, IsOnlyJSObject)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSArray> array = factory->NewJSArray();
    ASSERT_FALSE(array.GetTaggedValue().IsOnlyJSObject());
    ASSERT_TRUE(array.GetTaggedValue().IsJSObject());

    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> objectFuncPrototype = env->GetObjectFunctionPrototype();
    JSHandle<JSHClass> hclass = factory->NewEcmaHClass(JSObject::SIZE, JSType::JS_OBJECT, objectFuncPrototype);
    JSHandle<JSObject> object = thread->GetEcmaVM()->GetFactory()->NewJSObject(hclass);
    ASSERT_TRUE(object.GetTaggedValue().IsOnlyJSObject());
    ASSERT_TRUE(object.GetTaggedValue().IsJSObject());
}

HWTEST_F_L0(JSTaggedValueTest, SameValue)
{
    EcmaVM *ecma = thread->GetEcmaVM();
    JSHandle<JSTaggedValue> objectFun = ecma->GetGlobalEnv()->GetObjectFunction();

    JSHandle<JSObject> jsObj = ecma->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objectFun), objectFun);

    // not same type
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1), JSTaggedValue::False()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1.0), JSTaggedValue::True()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1),
                                          ecma->GetFactory()->NewFromASCII("test").GetTaggedValue()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1), JSTaggedValue(*jsObj)));
    JSHandle<JSTaggedValue> test(ecma->GetFactory()->NewFromASCII("test"));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, test.GetTaggedValue(), JSTaggedValue(*jsObj)));

    // number compare
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue(1), JSTaggedValue(1)));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue(1), JSTaggedValue(1.0)));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1.0), JSTaggedValue(2.0)));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread,
        JSTaggedValue(std::numeric_limits<int>::quiet_NaN()), JSTaggedValue(2.0)));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue(std::numeric_limits<int>::quiet_NaN()),
                                         JSTaggedValue(std::numeric_limits<int>::quiet_NaN())));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue(std::numeric_limits<double>::quiet_NaN()),
                                         JSTaggedValue(std::numeric_limits<double>::quiet_NaN())));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(0.0), JSTaggedValue(-0.0)));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue(0), JSTaggedValue(-0)));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue(1.0), JSTaggedValue(-1.0)));

    // string compare
    JSHandle<JSTaggedValue> test1(ecma->GetFactory()->NewFromASCII("test1"));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, test.GetTaggedValue(), test1.GetTaggedValue()));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, test.GetTaggedValue(), test.GetTaggedValue()));

    // bool compare
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue::True(), JSTaggedValue::True()));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue::False(), JSTaggedValue::False()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue::False(), JSTaggedValue::True()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue::True(), JSTaggedValue::False()));

    // js object compare
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, jsObj.GetTaggedValue(), jsObj.GetTaggedValue()));

    // undefined or null compare
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue::Undefined(), JSTaggedValue::Undefined()));
    ASSERT_TRUE(JSTaggedValue::SameValue(thread, JSTaggedValue::Null(), JSTaggedValue::Null()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue::Undefined(), JSTaggedValue::Null()));
    ASSERT_FALSE(JSTaggedValue::SameValue(thread, JSTaggedValue::Null(), JSTaggedValue::Undefined()));
}

HWTEST_F_L0(JSTaggedValueTest, SameValueZero)
{
    // SameValueZero differs from SameValue only in its treatment of +0 and -0.
    ASSERT_TRUE(JSTaggedValue::SameValueZero(thread, JSTaggedValue(0.0), JSTaggedValue(-0.0)));
}

HWTEST_F_L0(JSTaggedValueTest, Less)
{
    JSHandle<JSTaggedValue> test(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test"));
    JSHandle<JSTaggedValue> test1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test1"));
    JSHandle<JSTaggedValue> test2(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test2"));

    ASSERT_TRUE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0)),
                                    JSHandle<JSTaggedValue>(thread, JSTaggedValue(2.0))));
    ASSERT_FALSE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(-0.0))));
    ASSERT_TRUE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
                                    JSHandle<JSTaggedValue>(thread, JSTaggedValue(2))));

    ASSERT_TRUE(JSTaggedValue::Less(thread, test, test1));
    ASSERT_FALSE(JSTaggedValue::Less(thread, test2, test1));
    ASSERT_FALSE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)), test1));
    ASSERT_FALSE(JSTaggedValue::Less(thread, test2, JSHandle<JSTaggedValue>(thread, JSTaggedValue(2))));

    ASSERT_TRUE(JSTaggedValue::Less(
        thread, JSHandle<JSTaggedValue>(thread->GetEcmaVM()->GetFactory()->NewFromASCII("1")),
        JSHandle<JSTaggedValue>(thread, JSTaggedValue(2))));
    ASSERT_TRUE(JSTaggedValue::Less(
        thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
        JSHandle<JSTaggedValue>(thread->GetEcmaVM()->GetFactory()->NewFromASCII("2"))));

    ASSERT_TRUE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_FALSE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    ASSERT_FALSE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null())));
    ASSERT_FALSE(JSTaggedValue::Less(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
}

HWTEST_F_L0(JSTaggedValueTest, Equal)
{
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
}

HWTEST_F_L0(JSTaggedValueTest, NumberEqual)
{
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(2))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(-0.0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(-0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0)),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0)),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0)),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null())));
}

HWTEST_F_L0(JSTaggedValueTest, StringEqual)
{
    JSHandle<JSTaggedValue> test(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test"));
    JSHandle<JSTaggedValue> test1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test1"));
    JSHandle<JSTaggedValue> empty(thread->GetEcmaVM()->GetFactory()->NewFromASCII(""));
    JSHandle<JSTaggedValue> char0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("0"));
    JSHandle<JSTaggedValue> char1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("1"));
    JSHandle<JSTaggedValue> charM1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-1"));
    JSHandle<JSTaggedValue> charM0Point0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-0.0"));
    JSHandle<JSTaggedValue> charM0Point1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-0.1"));

    ASSERT_TRUE(JSTaggedValue::Equal(thread, test, test));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, test, test1));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, test, empty));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, empty, empty));

    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0)), char0));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)), char0));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0)), char1));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(-1.0)), charM1));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)), charM0Point0));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)), charM0Point1));

    ASSERT_TRUE(JSTaggedValue::Equal(thread, char1, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, char1, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, char0, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, char0, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Undefined())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, char0, JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null())));
}

HWTEST_F_L0(JSTaggedValueTest, BooleanEqual)
{
    JSHandle<JSTaggedValue> char0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("0"));
    JSHandle<JSTaggedValue> char0Point0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("0.0"));
    JSHandle<JSTaggedValue> char1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("1"));
    JSHandle<JSTaggedValue> char1Point0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("1.0"));
    JSHandle<JSTaggedValue> charM1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-1"));
    JSHandle<JSTaggedValue> charM0Point0(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-0.0"));
    JSHandle<JSTaggedValue> charM0Point1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("-0.1"));

    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(0))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                     JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()),
                                      JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0))));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()), char0));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()), char0Point0));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()), char1));
    ASSERT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()), char1Point0));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::True()), char0));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue::False()), char1));
}

HWTEST_F_L0(JSTaggedValueTest, SymbolEqual)
{
    JSHandle<JSTaggedValue> test(thread->GetEcmaVM()->GetFactory()->NewFromASCII("test"));
    JSHandle<JSTaggedValue> testSymbol(thread->GetEcmaVM()->GetFactory()->NewPublicSymbolWithChar("test"));
    ASSERT_FALSE(JSTaggedValue::Equal(thread, testSymbol, test));
}

HWTEST_F_L0(JSTaggedValueTest, StrictEqual)
{
    // This algorithm differs from the SameValue Algorithm in its treatment of signed zeroes and NaNs.
    ASSERT_TRUE(JSTaggedValue::StrictEqual(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(0.0)),
                                           JSHandle<JSTaggedValue>(thread, JSTaggedValue(-0.0))));
    ASSERT_FALSE(JSTaggedValue::StrictEqual(
        thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(std::numeric_limits<double>::quiet_NaN())),
        JSHandle<JSTaggedValue>(thread, JSTaggedValue(std::numeric_limits<double>::quiet_NaN()))));
    ASSERT_FALSE(JSTaggedValue::StrictEqual(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)),
                 JSHandle<JSTaggedValue>(thread, JSTaggedValue(2))));
    ASSERT_FALSE(JSTaggedValue::StrictEqual(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0)),
                 JSHandle<JSTaggedValue>(thread, JSTaggedValue(2.0))));
    ASSERT_TRUE(JSTaggedValue::StrictEqual(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1.0)),
                JSHandle<JSTaggedValue>(thread, JSTaggedValue(1))));
}

HWTEST_F_L0(JSTaggedValueTest, EqualHeapObject0)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 2. create a number
    JSHandle<JSTaggedValue> testNumber = JSHandle<JSTaggedValue>(thread, JSTaggedValue(TEST_NUMBER));

    // 3. compare heapobject not ecmaobject with number
    bool res = JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()), testNumber);
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualHeapObject1)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 2. create a boolean
    JSHandle<JSTaggedValue> testBoolean = JSHandle<JSTaggedValue>(thread, JSTaggedValue::True());

    // 3. compare heapobject not ecmaobject with boolean
    bool res = JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()), testBoolean);
    ASSERT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualHeapObject2)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 2. create a string
    JSHandle<JSTaggedValue> testString = JSHandle<JSTaggedValue>(factory->NewFromASCII(TEST_STRING.c_str()));

    // 3. compare heapobject not ecmaobject with string
    bool res = JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()), testString);
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualHeapObject3)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 2. create a symbol
    JSHandle<JSTaggedValue> testSymbol = JSHandle<JSTaggedValue>(factory->NewJSSymbol());

    // 3. compare heapobject not ecmaobject with symbol
    bool res = JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()), testSymbol);
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualHeapObject4)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 2. create a bigint
    JSHandle<JSTaggedValue> testBigInt = JSHandle<JSTaggedValue>(factory->NewBigInt<>(TEST_NUMBER));

    // 3. compare heapobject not ecmaobject with bigint
    bool res = JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()), testBigInt);
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualNumber0)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a number
    JSHandle<JSTaggedValue> testNumber = JSHandle<JSTaggedValue>(thread, JSTaggedValue(TEST_NUMBER));

    // 2. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 3. compare heapobject not ecmaobject with number
    bool res = JSTaggedValue::Equal(thread, testNumber, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()));
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualString0)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a string
    JSHandle<JSTaggedValue> testString = JSHandle<JSTaggedValue>(factory->NewFromASCII(TEST_STRING.c_str()));

    // 2. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 3. compare heapobject not ecmaobject with number
    bool res = JSTaggedValue::Equal(thread, testString, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()));
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualSymbol0)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a symbol
    JSHandle<JSTaggedValue> testSymbol = JSHandle<JSTaggedValue>(factory->NewJSSymbol());

    // 2. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 3. compare heapobject not ecmaobject with number
    bool res = JSTaggedValue::Equal(thread, testSymbol, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()));
    EXPECT_FALSE(res);
}

HWTEST_F_L0(JSTaggedValueTest, EqualBigInt0)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    // 1. create a bigint
    JSHandle<JSTaggedValue> testBigInt = JSHandle<JSTaggedValue>(factory->NewBigInt<>(TEST_NUMBER));

    // 2. create a heapobject which is not ecmaobject
    JSHandle<TaggedArray> array = factory->NewTaggedArray(TEST_TAGGED_ARRAY_LENGTH);

    // 3. compare heapobject not ecmaobject with number
    bool res = JSTaggedValue::Equal(thread, testBigInt, JSHandle<JSTaggedValue>(thread, array.GetTaggedValue()));
    EXPECT_FALSE(res);
}
}  // namespace panda::test
