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

#include "ecmascript/builtins/builtins_cjs_require.h"
#if ECMASCRIPT_ENABLE_CAST_CHECK
#include "ecmascript/js_tagged_value-inl.h"
#endif

namespace panda::ecmascript::builtins {
JSTaggedValue BuiltinsCjsRequire::CjsRequireConstructor(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, CjsRequire, CjsRequireConstructor);
    [[maybe_unused]] EcmaHandleScope scope(thread);

    // 1. If NewTarget is not undefined, throw a TypeError exception.
    JSHandle<JSTaggedValue> newTarget = GetNewTarget(argv);
    if (!newTarget->IsUndefined()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "newTarget is undefined", JSTaggedValue::Exception());
    }
    uint32_t length = argv->GetArgsNumber();
    JSHandle<JSTaggedValue> result(thread, JSTaggedValue::Undefined());
    if (length != 1) {  // strange arg's number
        LOG_ECMA(FATAL) << "BuiltinsCjsRequire::CjsRequireConstructor : can only accept one argument";
        UNREACHABLE();
    }
    JSHandle<EcmaString> requestName = JSHandle<EcmaString>::Cast(GetCallArg(argv, 0));
    result = CjsModule::Load(thread, requestName);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}

JSTaggedValue BuiltinsCjsRequire::Main(EcmaRuntimeCallInfo *msg)
{
    ASSERT(msg);
    JSThread *thread = msg->GetThread();
    BUILTINS_API_TRACE(thread, CjsRequire, Main);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    return JSTaggedValue(0);
}

JSTaggedValue BuiltinsCjsRequire::Resolve(EcmaRuntimeCallInfo *msg)
{
    ASSERT(msg);
    JSThread *thread = msg->GetThread();
    BUILTINS_API_TRACE(thread, CjsRequire, Resolve);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    return JSTaggedValue::Hole();
}
}  // namespace panda::ecmascript::builtins

