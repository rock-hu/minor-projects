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

#include "ecmascript/base/error_helper.h"
#include "ecmascript/base/builtins_base.h"
#include "ecmascript/dfx/stackinfo/js_stackinfo.h"
#include "ecmascript/interpreter/frame_handler.h"
#include "ecmascript/platform/log.h"

namespace panda::ecmascript::base {
JSTaggedValue ErrorHelper::ErrorCommonToString(EcmaRuntimeCallInfo *argv, const ErrorType &errorType)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let O be the this value.
    // 2. If Type(O) is not Object, throw a TypeError exception
    JSHandle<JSTaggedValue> thisValue = BuiltinsBase::GetThis(argv);
    if (!thisValue->IsECMAObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "ErrorToString:not an object", JSTaggedValue::Exception());
    }
    // 3. Let name be Get(O, "name").
    // 4. ReturnIfAbrupt(name).
    auto globalConst = thread->GlobalConstants();
    JSHandle<JSTaggedValue> handleName = globalConst->GetHandledNameString();
    JSHandle<JSTaggedValue> name = JSObject::GetProperty(thread, thisValue, handleName).GetValue();
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. If name is undefined, let name be "Error"; otherwise let name be ToString(name).
    // 6. ReturnIfAbrupt(name).
    name = ErrorHelper::GetErrorName(thread, name, errorType);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 7. Let msg be Get(O, "message").
    // 8. ReturnIfAbrupt(msg).
    JSHandle<JSTaggedValue> handleMsg = globalConst->GetHandledMessageString();
    JSHandle<JSTaggedValue> msg = JSObject::GetProperty(thread, thisValue, handleMsg).GetValue();
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 9. If msg is undefined, let msg be the empty String; otherwise let msg be ToString(msg).
    // 10. ReturnIfAbrupt(msg).
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    if (msg->IsUndefined()) {
        msg = JSHandle<JSTaggedValue>::Cast(factory->GetEmptyString());
    } else {
        msg = JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToString(thread, msg));
    }
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 11. If name is the empty String, return msg.
    // 12. If msg is the empty String, return name.
    if (EcmaStringAccessor(JSHandle<EcmaString>::Cast(name)).GetLength() == 0) {
        return msg.GetTaggedValue();
    }
    if (EcmaStringAccessor(JSHandle<EcmaString>::Cast(msg)).GetLength() == 0) {
        return name.GetTaggedValue();
    }

    // 13. Return the result of concatenating name, the code unit 0x003A (COLON), the code unit 0x0020 (SPACE), and msg.
    JSHandle<EcmaString> space = factory->NewFromASCII(": ");
    JSHandle<EcmaString> jsHandleName = JSHandle<EcmaString>::Cast(name);
    JSHandle<EcmaString> jsHandleMsg = JSHandle<EcmaString>::Cast(msg);
    JSHandle<EcmaString> handleNameSpace = factory->ConcatFromString(jsHandleName, space);
    JSHandle<EcmaString> result = factory->ConcatFromString(handleNameSpace, jsHandleMsg);
    return result.GetTaggedValue();
}

JSHandle<JSTaggedValue> ErrorHelper::GetErrorName(JSThread *thread, const JSHandle<JSTaggedValue> &name,
                                                  const ErrorType &errorType)
{
    auto globalConst = thread->GlobalConstants();
    if (name->IsUndefined()) {
        JSHandle<JSTaggedValue> errorKey;
        switch (errorType) {
            case ErrorType::RANGE_ERROR:
                errorKey = globalConst->GetHandledRangeErrorString();
                break;
            case ErrorType::EVAL_ERROR:
                errorKey = globalConst->GetHandledEvalErrorString();
                break;
            case ErrorType::REFERENCE_ERROR:
                errorKey = globalConst->GetHandledReferenceErrorString();
                break;
            case ErrorType::TYPE_ERROR:
                errorKey = globalConst->GetHandledTypeErrorString();
                break;
            case ErrorType::AGGREGATE_ERROR:
                errorKey = globalConst->GetHandledAggregateErrorString();
                break;
            case ErrorType::URI_ERROR:
                errorKey = globalConst->GetHandledURIErrorString();
                break;
            case ErrorType::SYNTAX_ERROR:
                errorKey = globalConst->GetHandledSyntaxErrorString();
                break;
            case ErrorType::OOM_ERROR:
                errorKey = globalConst->GetHandledOOMErrorString();
                break;
            case ErrorType::TERMINATION_ERROR:
                errorKey = globalConst->GetHandledTerminationErrorString();
                break;
            default:
                errorKey = globalConst->GetHandledErrorString();
                break;
        }
        return errorKey;
    }
    return JSHandle<JSTaggedValue>::Cast(JSTaggedValue::ToString(thread, name));
}

JSTaggedValue ErrorHelper::ErrorCommonConstructor(EcmaRuntimeCallInfo *argv,
                                                  [[maybe_unused]] const ErrorType &errorType)
{
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. If NewTarget is undefined, let newTarget be the active function object, else let newTarget be NewTarget.
    auto ecmaVm = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVm->GetFactory();
    JSHandle<JSTaggedValue> ctor = BuiltinsBase::GetConstructor(argv);
    JSHandle<JSTaggedValue> newTarget = BuiltinsBase::GetNewTarget(argv);
    if (newTarget->IsUndefined()) {
        newTarget = ctor;
    }
    JSHandle<JSTaggedValue> message = BuiltinsBase::GetCallArg(argv, 0);

    // 2. Let O be OrdinaryCreateFromConstructor(newTarget, "%ErrorPrototype%", «[[ErrorData]]»).
    JSHandle<JSObject> nativeInstanceObj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(ctor), newTarget);

    // 3. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4. If message is not undefined, then
    //    a. Let msg be ToString(message).
    //    b. ReturnIfAbrupt(msg).
    //    c. Let msgDesc be the PropertyDescriptor{[[Value]]: msg, [[Writable]]: true, [[Enumerable]]: false,
    //       [[Configurable]]: true}.
    //    d. Let status be DefinePropertyOrThrow(O, "message", msgDesc).
    //    e. Assert: status is not an abrupt completion
    auto globalConst = thread->GlobalConstants();
    if (!message->IsUndefined()) {
        JSHandle<EcmaString> handleStr = JSTaggedValue::ToString(thread, message);
        if (errorType != ErrorType::OOM_ERROR) {
            LOG_ECMA(DEBUG) << "Throw error: " << EcmaStringAccessor(handleStr).ToCString(thread);
        }
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        JSHandle<JSTaggedValue> msgKey = globalConst->GetHandledMessageString();
        PropertyDescriptor msgDesc(thread, JSHandle<JSTaggedValue>::Cast(handleStr), true, false, true);
        [[maybe_unused]] bool status = JSObject::DefineOwnProperty(thread, nativeInstanceObj, msgKey, msgDesc);
        ASSERT_PRINT(status == true, "return result exception!");
    }
    // InstallErrorCause
    JSHandle<JSTaggedValue> options = BuiltinsBase::GetCallArg(argv, 1);
    // If options is an Object and ? HasProperty(options, "cause") is true, then
    //   a. Let cause be ? Get(options, "cause").
    //   b. Perform CreateNonEnumerableDataPropertyOrThrow(O, "cause", cause).
    if (options->IsECMAObject()) {
        JSHandle<JSTaggedValue> causeKey = globalConst->GetHandledCauseString();
        bool causePresent = JSTaggedValue::HasProperty(thread, options, causeKey);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        if (causePresent) {
            JSHandle<JSTaggedValue> cause = JSObject::GetProperty(thread, options, causeKey).GetValue();
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
            PropertyDescriptor causeDesc(thread, cause, true, false, true);
            [[maybe_unused]] bool status = JSObject::DefineOwnProperty(thread, nativeInstanceObj, causeKey, causeDesc);
            ASSERT_PRINT(status == true, "return result exception!");
        }
    }
    JSHandle<JSTaggedValue> errorFunc = GetErrorJSFunction(thread);
    if (!errorFunc->IsUndefined()) {
        JSHandle<JSTaggedValue> errorFunckey = globalConst->GetHandledErrorFuncString();
        PropertyDescriptor errorFuncDesc(thread, errorFunc, true, false, true);
        [[maybe_unused]] bool status = JSObject::DefineOwnProperty(thread,
                                                                   nativeInstanceObj, errorFunckey, errorFuncDesc);
        ASSERT_PRINT(status == true, "return result exception!");
    }

    std::string stack;
    JSHandle<EcmaString> handleStack = BuildEcmaStackTrace(thread, stack, nativeInstanceObj);
    JSHandle<JSTaggedValue> stackkey = globalConst->GetHandledStackString();
    PropertyDescriptor stackDesc(thread, JSHandle<JSTaggedValue>::Cast(handleStack), true, false, true);
    [[maybe_unused]] bool status = JSObject::DefineOwnProperty(thread, nativeInstanceObj, stackkey, stackDesc);
    ASSERT_PRINT(status == true, "return result exception!");

    // Uncaught exception parsing source code
    JSHandle<JSTaggedValue> topStackkey = globalConst->GetHandledTopStackString();
    PropertyDescriptor topStackDesc(thread, JSHandle<JSTaggedValue>::Cast(factory->NewFromStdString(stack)),
                                                                          true, false, true);
    [[maybe_unused]] bool topStackstatus = JSObject::DefineOwnProperty(thread, nativeInstanceObj,
                                                                       topStackkey, topStackDesc);
    ASSERT_PRINT(topStackstatus == true, "return result exception!");

    // 5. Return O.
    return nativeInstanceObj.GetTaggedValue();
}

// static
void ErrorHelper::PrintJSErrorInfo(JSThread *thread, const JSHandle<JSTaggedValue> exceptionInfo)
{
    CString nameBuffer = GetJSErrorInfo(thread, exceptionInfo, JSErrorProps::NAME);
    CString msgBuffer = GetJSErrorInfo(thread, exceptionInfo, JSErrorProps::MESSAGE);
    CString stackBuffer = GetJSErrorInfo(thread, exceptionInfo, JSErrorProps::STACK);
    LOG_NO_TAG(ERROR) << panda::ecmascript::previewerTag << nameBuffer << ": " << msgBuffer << "\n"
                      << (panda::ecmascript::previewerTag.empty()
                              ? stackBuffer
                              : std::regex_replace(stackBuffer, std::regex(".+(\n|$)"),
                                                   panda::ecmascript::previewerTag + "$0"));
}

// static
CString ErrorHelper::GetJSErrorInfo(JSThread *thread, const JSHandle<JSTaggedValue> exceptionInfo, JSErrorProps key)
{
    JSHandle<JSTaggedValue> keyStr(thread, JSTaggedValue::Undefined());
    switch (key) {
        case JSErrorProps::NAME:
            keyStr = thread->GlobalConstants()->GetHandledNameString();
            break;
        case JSErrorProps::MESSAGE:
            keyStr = thread->GlobalConstants()->GetHandledMessageString();
            break;
        case JSErrorProps::STACK:
            keyStr = thread->GlobalConstants()->GetHandledStackString();
            break;
        default:
            LOG_ECMA(FATAL) << "this branch is unreachable " << key;
            UNREACHABLE();
    }
    JSHandle<JSTaggedValue> value = JSObject::GetProperty(thread, exceptionInfo, keyStr).GetValue();
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, CString());
    JSHandle<EcmaString> errStr = JSTaggedValue::ToString(thread, value);
    // JSTaggedValue::ToString may cause exception. In this case, do not return, use "<error>" instead.
    if (thread->HasPendingException()) {
        thread->ClearException();
        errStr = thread->GetEcmaVM()->GetFactory()->NewFromStdString("<error>");
    }
    return ConvertToString(thread, *errStr);
}

JSHandle<JSTaggedValue> ErrorHelper::GetErrorJSFunction(JSThread *thread)
{
    FrameHandler frameHandler(thread);
    for (; frameHandler.HasFrame(); frameHandler.PrevJSFrame()) {
        if (!frameHandler.IsJSFrame()) {
            continue;
        }

        auto function = frameHandler.GetFunction();
        if (function.IsJSFunctionBase() || function.IsJSProxy()) {
            Method *method = ECMAObject::Cast(function.GetTaggedObject())->GetCallTarget(thread);
            if (!method->IsNativeWithCallField()) {
                return JSHandle<JSTaggedValue>(thread, function);
            }
        }
    }
    return thread->GlobalConstants()->GetHandledUndefined();
}

JSHandle<EcmaString> ErrorHelper::BuildEcmaStackTrace(JSThread *thread, std::string &stack,
                                                      const JSHandle<JSObject> &jsErrorObj)
{
    std::string data = JsStackInfo::BuildJsStackTrace(thread, false, jsErrorObj, true);
    if (data.size() > MAX_ERROR_SIZE) {
        // find last line break from 0 to MAX_ERROR_SIZE
        size_t pos = data.rfind('\n', MAX_ERROR_SIZE);
        if (pos != std::string::npos) {
            data = data.substr(0, pos);
        }
    }
    LOG_ECMA(DEBUG) << data;
    // unconverted stack
    stack = data;
    auto ecmaVm = thread->GetEcmaVM();
    // sourceMap callback
    auto sourceMapcb = ecmaVm->GetSourceMapCallback();
    if (sourceMapcb != nullptr && !data.empty()) {
        data = sourceMapcb(data.c_str());
    }

    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    return factory->NewFromStdString(data);
}
}  // namespace panda::ecmascript::base
