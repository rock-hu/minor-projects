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

#include "ecmascript/builtins/builtins_proxy.h"

#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_array.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
class BuiltinsProxyTest : public BaseTestWithScope<false> {
};

JSHandle<JSObject> BuiltinsTestProxyCreate(JSThread *thread)
{
    EcmaVM *ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> globalEnv = ecmaVM->GetGlobalEnv();
    JSHandle<JSTaggedValue> objFun(globalEnv->GetObjectFunction());
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFun), objFun);
    return obj;
}

// 26.2.1.1 Proxy( [ value ] )
HWTEST_F_L0(BuiltinsProxyTest, ProxyConstructor)
{
    JSHandle<JSObject> target = BuiltinsTestProxyCreate(thread);
    JSHandle<JSObject> handler = BuiltinsTestProxyCreate(thread);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Null(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, target.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handler.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsProxy::ProxyConstructor(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsECMAObject());
    JSHandle<JSTaggedValue> resultHandle(thread, result);
    EXPECT_TRUE(resultHandle->IsJSProxy());
    TestHelper::TearDownFrame(thread, prev);
}

// 26.2.2.1 Proxy.revocable ( target, handler )
HWTEST_F_L0(BuiltinsProxyTest, Revocable)
{
    JSHandle<JSObject> target = BuiltinsTestProxyCreate(thread);
    JSHandle<JSObject> handler = BuiltinsTestProxyCreate(thread);

    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> proxyFun(globalEnv->GetProxyFunction());
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("prop"));
    PropertyDescriptor desc(thread);
    desc.SetWritable(false);
    JSObject::DefineOwnProperty(thread, handler, key, desc);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, target.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handler.GetTaggedValue());
    ecmaRuntimeCallInfo->SetNewTarget(JSTaggedValue(*proxyFun));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsProxy::Revocable(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsECMAObject());
    JSHandle<JSObject> resultHandle(thread, result);

    auto globalConst = thread->GlobalConstants();
    JSHandle<JSTaggedValue> proxyKey = globalConst->GetHandledProxyString();
    JSHandle<JSTaggedValue> revokeKey = globalConst->GetHandledRevokeString();

    JSHandle<TaggedArray> keys = JSObject::GetOwnPropertyKeys(thread, resultHandle);
    bool pflag = false;
    bool rflag = false;
    for (uint32_t i = 0; i < keys->GetLength(); i++) {
        if (JSTaggedValue::SameValue(thread, keys->Get(thread, i), proxyKey.GetTaggedValue())) {
            pflag = true;
        }
        if (JSTaggedValue::SameValue(thread, keys->Get(thread, i), revokeKey.GetTaggedValue())) {
            rflag = true;
        }
    }
    EXPECT_TRUE(pflag);
    EXPECT_TRUE(rflag);

    PropertyDescriptor descRes(thread);
    JSObject::GetOwnProperty(thread, resultHandle, revokeKey, descRes);
    EXPECT_TRUE(descRes.GetValue()->IsProxyRevocFunction());
    TestHelper::TearDownFrame(thread, prev);
}
}  // namespace panda::test
