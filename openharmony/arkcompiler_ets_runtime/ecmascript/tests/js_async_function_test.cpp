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

#include "ecmascript/js_async_function.h"
#include "ecmascript/js_generator_object.h"
#include "ecmascript/global_env.h"
#include "ecmascript/jobs/micro_job_queue.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda;
using namespace panda::ecmascript;

namespace panda::test {
class JSAsyncFunctionTest : public BaseTestWithScope<false> {
};

/**
 * @tc.name: SetAsyncContext
 * @tc.desc: Call "NewJSAsyncAwaitStatusFunction" function to construct class JSAsyncAwaitStatusFunction object, calling
 *           "SetAsyncContext" function to set AsyncContext and check the AsyncContext is within expectations by calling
 *           "GetAsyncContext" function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(JSAsyncFunctionTest, SetAsyncContext)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSAsyncFuncObject> asyncFuncObj = factory->NewJSAsyncFuncObject();
    JSHandle<JSTaggedValue> asyncFuncObjVal(thread, asyncFuncObj->GetGeneratorContext(thread));

    JSHandle<JSAsyncAwaitStatusFunction> fulFunc =
        factory->NewJSAsyncAwaitStatusFunction(MethodIndex::BUILTINS_PROMISE_HANDLER_ASYNC_AWAIT_FULFILLED);
    fulFunc->SetAsyncContext(thread, asyncFuncObjVal);
    EXPECT_EQ(JSTaggedValue::SameValue(thread, fulFunc->GetAsyncContext(thread), asyncFuncObjVal.GetTaggedValue()),
              true);

    JSHandle<JSAsyncAwaitStatusFunction> rejFunc =
        factory->NewJSAsyncAwaitStatusFunction(MethodIndex::BUILTINS_PROMISE_HANDLER_ASYNC_AWAIT_REJECTED);
    rejFunc->SetAsyncContext(thread, asyncFuncObj->GetGeneratorContext(thread));
    EXPECT_EQ(JSTaggedValue::SameValue(thread, rejFunc->GetAsyncContext(thread), asyncFuncObjVal.GetTaggedValue()),
              true);
}

/**
 * @tc.name: AsyncFunctionAwait
 * @tc.desc: Call "NewJSAsyncFuncObject" function to construct class JSAsyncFuncObject object, calling
 *           "AsyncFunctionAwait" function remove asyncContext and run execution context,but context is
 *           undefined so cannot execute, the promise is undefined.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(JSAsyncFunctionTest, AsyncFunctionAwait)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> valueHandle(thread, JSTaggedValue(33));
    // asyncContext is undefined
    JSHandle<JSAsyncFuncObject> asyncFuncObj = factory->NewJSAsyncFuncObject();
    JSAsyncFunction::AsyncFunctionAwait(thread, asyncFuncObj, valueHandle);

    // check AsyncPromise queue and queue cannot execute
    auto microJobQueue = thread->GetEcmaVM()->GetMicroJobQueue();
    EXPECT_FALSE(microJobQueue->GetPromiseJobQueue(thread).IsUndefined());
    // promiese is undefined.
    EXPECT_TRUE(asyncFuncObj->GetPromise(thread).IsUndefined());
}
}  // namespace panda::test
