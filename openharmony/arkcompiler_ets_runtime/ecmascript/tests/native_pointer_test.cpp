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

#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;

using FunctionPtr = void (*)(panda::ecmascript::JSHandle<EcmaString> &);

namespace panda::test {
class NativePointerTest : public BaseTestWithScope<false> {
};

HWTEST_F_L0(NativePointerTest, Print)
{
    // mock object needed in test
    char array[] = "Hello World!";
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<EcmaString> str = factory->NewFromASCII(array);
    EXPECT_TRUE(*str != nullptr);

    JSHandle<JSFunction> jsFunction = factory->NewJSFunction(env);
    EXPECT_TRUE(*jsFunction != nullptr);

    JSHandle<Method> target = factory->NewMethodForNativeFunction(nullptr);
    jsFunction->SetMethod(thread, target.GetTaggedValue());

    // run cpp methed 'Print'
    ASSERT_EQ(Method::Cast(target.GetTaggedValue().GetTaggedObject()), jsFunction->GetCallTarget());
}
}  // namespace panda::test
