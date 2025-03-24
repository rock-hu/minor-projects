/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_BASE_ERROR_HELPER_H
#define ECMASCRIPT_BASE_ERROR_HELPER_H

#include "ecmascript/base/error_type.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/mem/c_string.h"

namespace panda::ecmascript::base {
constexpr char DEFAULT_EMPTY_STACK_TRACE[] = "stack is empty";  // NOLINT (modernize-avoid-c-arrays)
constexpr char INTRINSIC_METHOD_NAME[] = "Intrinsic method";    // NOLINT (modernize-avoid-c-arrays)

class ErrorHelper {
public:
    enum JSErrorProps {
        NAME,
        MESSAGE,
        STACK
    };

    static JSTaggedValue ErrorCommonToString(EcmaRuntimeCallInfo *argv, const ErrorType &errorType);

    static JSTaggedValue ErrorCommonConstructor(EcmaRuntimeCallInfo *argv, const ErrorType &errorType);

    static void PrintJSErrorInfo(JSThread *thread, const JSHandle<JSTaggedValue> exceptionInfo);

    static CString GetJSErrorInfo(JSThread *thread, const JSHandle<JSTaggedValue> exceptionInfo, JSErrorProps key);

private:
    static JSHandle<JSTaggedValue> GetErrorJSFunction(JSThread *thread);

    static JSHandle<EcmaString> BuildEcmaStackTrace(JSThread *thread, std::string &stack,
                                                    const JSHandle<JSObject> &jsErrorObj);

    static JSHandle<JSTaggedValue> GetErrorName(JSThread *thread, const JSHandle<JSTaggedValue> &name,
                                                const ErrorType &errorType);
    static constexpr uint32_t MAX_ERROR_SIZE = 10_KB;
};
}  // namespace panda::ecmascript::base

#endif  // ECMASCRIPT_BASE_ERROR_HELPER_H
