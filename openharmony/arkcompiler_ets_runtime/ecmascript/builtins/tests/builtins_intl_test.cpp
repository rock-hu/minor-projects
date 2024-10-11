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

#include "ecmascript/builtins/builtins_intl.h"

#include "ecmascript/js_array.h"
#include "ecmascript/global_env.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
class BuiltinsIntlTest : public BaseTestWithScope<true> {
};

HWTEST_F_L0(BuiltinsIntlTest, GetCanonicalLocales_001)
{
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue::Undefined());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue resultObj = BuiltinsIntl::GetCanonicalLocales(ecmaRuntimeCallInfo);
    JSHandle<JSArray> resultHandle(thread, resultObj);
    EXPECT_EQ(resultHandle->GetArrayLength(), 0U);
}

HWTEST_F_L0(BuiltinsIntlTest, GetCanonicalLocales_002)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<EcmaString> handleStr = factory->NewFromASCII("ko-kore-kr");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, handleStr.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue resultObj = BuiltinsIntl::GetCanonicalLocales(ecmaRuntimeCallInfo);
    JSHandle<JSArray> resultHandle(thread, resultObj);

    JSHandle<TaggedArray> elements(thread, resultHandle->GetElements());
    EXPECT_EQ(elements->GetLength(), 1U);
    JSHandle<EcmaString> handleEcmaStr(thread, elements->Get(0));
    EXPECT_STREQ("ko-Kore-KR", EcmaStringAccessor(handleEcmaStr).ToCString().c_str());
}

HWTEST_F_L0(BuiltinsIntlTest, GetCanonicalLocales_003)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSArray *arr = JSArray::ArrayCreate(thread, JSTaggedNumber(0)).GetObject<JSArray>();
    JSHandle<JSTaggedValue> obj(thread, arr);

    JSHandle<JSTaggedValue> handleStr(factory->NewFromASCII("zh-Hans-Cn"));
    PropertyDescriptor desc(thread, handleStr, true, true, true);
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("1"));
    JSArray::DefineOwnProperty(thread, JSHandle<JSObject>(obj), key, desc);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue resultObj = BuiltinsIntl::GetCanonicalLocales(ecmaRuntimeCallInfo);
    JSHandle<JSArray> resultHandle(thread, resultObj);

    JSHandle<TaggedArray> elements(thread, resultHandle->GetElements());
    EXPECT_EQ(elements->GetLength(), 1U);
    JSHandle<EcmaString> handleEcmaStr(thread, elements->Get(0));
    EXPECT_STREQ("zh-Hans-CN", EcmaStringAccessor(handleEcmaStr).ToCString().c_str());
}
} // namespace panda::test
