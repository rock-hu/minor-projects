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

#include "ecmascript/ic/ic_runtime.h"
#include "ecmascript/ic/ic_handler.h"
#include "ecmascript/dfx/stackinfo/js_stackinfo.h"
#include "ecmascript/interpreter/interpreter.h"
#include "ecmascript/interpreter/slow_runtime_stub.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/shared_objects/js_shared_array.h"

namespace panda::ecmascript {

void ICRuntime::UpdateLoadHandler(const ObjectOperator &op, JSHandle<JSTaggedValue> key,
                                  JSHandle<JSTaggedValue> receiver)
{
    if (icAccessor_.GetICState() == ProfileTypeAccessor::ICState::MEGA) {
        return;
    }
    if (IsNamedIC(GetICKind())) {
        key = JSHandle<JSTaggedValue>();
    }
    JSHandle<JSTaggedValue> handlerValue;
    ObjectFactory *factory = thread_->GetEcmaVM()->GetFactory();
    JSHandle<JSHClass> originhclass;
    if (receiver->IsNumber()) {
        receiver = JSHandle<JSTaggedValue>::Cast(factory->NewJSPrimitiveRef(PrimitiveType::PRIMITIVE_NUMBER, receiver));
    } else if (receiver->IsString()) {
        originhclass = JSHandle<JSHClass>(thread_, receiver->GetTaggedObject()->GetClass());
        receiver = JSHandle<JSTaggedValue>::Cast(factory->NewJSPrimitiveRef(PrimitiveType::PRIMITIVE_STRING, receiver));
    }
    JSHandle<JSHClass> hclass(GetThread(), receiver->GetTaggedObject()->GetClass());
    // When a transition occurs without the shadow property, AOT does not trigger the
    // notifyprototypechange behavior, so for the case where the property does not
    // exist and the Hclass is AOT, IC needs to be abandoned.
    if (hclass->IsAOT() && !op.IsFound()) {
        return;
    }
    if (op.IsElement()) {
        if (!op.IsFound() && hclass->IsDictionaryElement()) {
            return;
        }
        handlerValue = LoadHandler::LoadElement(thread_, op);
    } else {
        if (!op.IsFound()) {
            JSTaggedValue proto = hclass->GetPrototype();
            if (!proto.IsECMAObject()) {
                handlerValue = LoadHandler::LoadProperty(thread_, op);
            } else {
                handlerValue = PrototypeHandler::LoadPrototype(thread_, op, hclass);
            }
        } else if (!op.IsOnPrototype()) {
            handlerValue = LoadHandler::LoadProperty(thread_, op);
        } else {
            // do not support global prototype ic
            if (IsGlobalLoadIC(GetICKind())) {
                return;
            }
            handlerValue = PrototypeHandler::LoadPrototype(thread_, op, hclass);
        }
    }

    if (!originhclass.GetTaggedValue().IsUndefined()) {
        hclass = originhclass;
    }
    if (key.IsEmpty()) {
        icAccessor_.AddHandlerWithoutKey(JSHandle<JSTaggedValue>::Cast(hclass), handlerValue);
    } else if (op.IsElement()) {
        // do not support global element ic
        if (IsGlobalLoadIC(GetICKind())) {
            return;
        }
        icAccessor_.AddElementHandler(JSHandle<JSTaggedValue>::Cast(hclass), handlerValue);
    } else {
        icAccessor_.AddHandlerWithKey(key, JSHandle<JSTaggedValue>::Cast(hclass), handlerValue);
    }
}

void ICRuntime::UpdateLoadStringHandler(JSHandle<JSTaggedValue> receiver)
{
    if (icAccessor_.GetICState() == ProfileTypeAccessor::ICState::MEGA) {
        return;
    }
    JSHandle<JSTaggedValue> handlerValue = LoadHandler::LoadStringElement(thread_);
    JSHandle<JSHClass> hclass(GetThread(), receiver->GetTaggedObject()->GetClass());
    icAccessor_.AddElementHandler(JSHandle<JSTaggedValue>::Cast(hclass), handlerValue);
}

void ICRuntime::UpdateTypedArrayHandler(JSHandle<JSTaggedValue> receiver)
{
    if (icAccessor_.GetICState() == ProfileTypeAccessor::ICState::MEGA) {
        return;
    }
    JSHandle<JSTaggedValue> handlerValue =
        LoadHandler::LoadTypedArrayElement(thread_, JSHandle<JSTypedArray>(receiver));
    JSHandle<JSHClass> hclass(GetThread(), receiver->GetTaggedObject()->GetClass());
    icAccessor_.AddElementHandler(JSHandle<JSTaggedValue>::Cast(hclass), handlerValue);
}

void ICRuntime::UpdateStoreHandler(const ObjectOperator &op, JSHandle<JSTaggedValue> key,
                                   JSHandle<JSTaggedValue> receiver)
{
    if (icAccessor_.GetICState() == ProfileTypeAccessor::ICState::MEGA) {
        return;
    }
    if (IsNamedIC(GetICKind())) {
        key = JSHandle<JSTaggedValue>();
    }
    JSHandle<JSTaggedValue> handlerValue;
    ASSERT(op.IsFound());

    if (op.IsTransition()) {
        if (op.IsOnPrototype()) {
            JSHandle<JSHClass> hclass(thread_, JSHandle<JSObject>::Cast(receiver)->GetClass());
            handlerValue = TransWithProtoHandler::StoreTransition(thread_, op, hclass);
        } else {
            handlerValue = TransitionHandler::StoreTransition(thread_, op);
        }
    } else if (op.IsOnPrototype()) {
        // do not support global prototype ic
        if (IsGlobalStoreIC(GetICKind())) {
            return;
        }
        JSHandle<JSHClass> hclass(thread_, JSHandle<JSObject>::Cast(receiver)->GetClass());
        handlerValue = PrototypeHandler::StorePrototype(thread_, op, hclass);
    } else {
        handlerValue = StoreHandler::StoreProperty(thread_, op);
    }

    if (key.IsEmpty()) {
        icAccessor_.AddHandlerWithoutKey(receiverHClass_, handlerValue);
    } else if (op.IsElement()) {
        // do not support global element ic
        if (IsGlobalStoreIC(GetICKind())) {
            return;
        }
        icAccessor_.AddElementHandler(receiverHClass_, handlerValue);
    } else {
        icAccessor_.AddHandlerWithKey(key, receiverHClass_, handlerValue);
    }
}

JSTaggedValue LoadICRuntime::LoadValueMiss(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key)
{
    JSTaggedValue::RequireObjectCoercible(thread_, receiver, "Cannot load property of null or undefined");
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);

    if ((!receiver->IsJSObject() || receiver->HasOrdinaryGet()) && !receiver->IsString()) {
        icAccessor_.SetAsMega();
        JSHandle<JSTaggedValue> propKey = JSTaggedValue::ToPropertyKey(thread_, key);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
        return JSTaggedValue::GetProperty(thread_, receiver, propKey).GetValue().GetTaggedValue();
    }
    if (receiver->IsTypedArray() || receiver->IsSharedTypedArray()) {
        return LoadTypedArrayValueMiss(receiver, key);
    }
    // fixme(hzzhouzebin) Open IC for SharedArray later.
    if (receiver->IsJSSharedArray()) {
        return JSSharedArray::GetProperty(thread_, receiver, key, SCheckMode::CHECK).GetValue().GetTaggedValue();
    }
    ObjectOperator op(GetThread(), receiver, key);
    auto result = JSHandle<JSTaggedValue>(thread_, JSObject::GetProperty(GetThread(), &op));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);

    if (receiver->IsString()) {
         // do not cache element
        if (!op.IsFastMode()) {
            icAccessor_.SetAsMega();
            return result.GetTaggedValue();
        }
        UpdateLoadStringHandler(receiver);
    } else {
        if (op.GetValue().IsAccessor()) {
            op = ObjectOperator(GetThread(), receiver, key);
        }
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
        // ic-switch
        if (!GetThread()->GetEcmaVM()->ICEnabled()) {
            icAccessor_.SetAsMega();
            return result.GetTaggedValue();
        }
        TraceIC(GetThread(), receiver, key);
        // do not cache element
        if (!op.IsFastMode()) {
            icAccessor_.SetAsMega();
            return result.GetTaggedValue();
        }
        UpdateLoadHandler(op, key, receiver);
    }

    return result.GetTaggedValue();
}

JSTaggedValue LoadICRuntime::LoadMiss(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key)
{
    if ((!receiver->IsJSObject() || receiver->HasOrdinaryGet()) &&
         !receiver->IsString() && !receiver->IsNumber()) {
        return LoadOrdinaryGet(receiver, key);
    }

    ICKind kind = GetICKind();
    // global variable find from global record firstly
    if (kind == ICKind::NamedGlobalLoadIC || kind == ICKind::NamedGlobalTryLoadIC) {
        JSTaggedValue box = SlowRuntimeStub::LdGlobalRecord(thread_, key.GetTaggedValue());
        if (!box.IsUndefined()) {
            ASSERT(box.IsPropertyBox());
            if (icAccessor_.GetICState() != ProfileTypeAccessor::ICState::MEGA) {
                icAccessor_.AddGlobalRecordHandler(JSHandle<JSTaggedValue>(thread_, box));
            }
            return PropertyBox::Cast(box.GetTaggedObject())->GetValue();
        }
    }

    if (key->IsJSFunction()) { // key is a private getter
        return CallPrivateGetter(receiver, key);
    }

    ObjectOperator op(GetThread(), receiver, key);
    auto result = JSHandle<JSTaggedValue>(thread_, JSObject::GetProperty(GetThread(), &op));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
    if (op.GetValue().IsAccessor()) {
        op = ObjectOperator(GetThread(), receiver, key);
    }
    if (!op.IsFound() && kind == ICKind::NamedGlobalTryLoadIC) {
        return SlowRuntimeStub::ThrowReferenceError(GetThread(), key.GetTaggedValue(), " is not defined");
    }
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
    // ic-switch
    if (!GetThread()->GetEcmaVM()->ICEnabled()) {
        icAccessor_.SetAsMega();
        return result.GetTaggedValue();
    }
    TraceIC(GetThread(), receiver, key);
    // do not cache element
    if (!op.IsFastMode()) {
        icAccessor_.SetAsMega();
        return result.GetTaggedValue();
    }

    UpdateLoadHandler(op, key, receiver);
    return result.GetTaggedValue();
}

JSTaggedValue LoadICRuntime::LoadOrdinaryGet(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key)
{
    icAccessor_.SetAsMega();
    JSHandle<JSTaggedValue> propKey = JSTaggedValue::ToPropertyKey(thread_, key);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
    return JSTaggedValue::GetProperty(thread_, receiver, propKey).GetValue().GetTaggedValue();
}

inline JSTaggedValue LoadICRuntime::CallPrivateGetter(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key)
{
    JSHandle<JSTaggedValue> undefined = thread_->GlobalConstants()->GetHandledUndefined();
    EcmaRuntimeCallInfo* info =
        EcmaInterpreter::NewRuntimeCallInfo(thread_, key, receiver, undefined, 0); // 0: getter has 0 argument
    JSTaggedValue resGetter = JSFunction::Call(info);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
    return resGetter;
}

JSTaggedValue LoadICRuntime::LoadTypedArrayValueMiss(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key)
{
    JSHandle<JSTaggedValue> propKey = JSTaggedValue::ToPropertyKey(GetThread(), key);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
    JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(GetThread(), propKey);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
    if (!numericIndex.IsUndefined()) {
        if (!JSTypedArray::IsValidIntegerIndex(receiver, numericIndex) || !GetThread()->GetEcmaVM()->ICEnabled()) {
            icAccessor_.SetAsMega();
            return JSTaggedValue::GetProperty(GetThread(), receiver, propKey).GetValue().GetTaggedValue();
        }
        UpdateTypedArrayHandler(receiver);
        JSHandle<JSTaggedValue> indexHandle(GetThread(), numericIndex);
        JSTaggedNumber integerValue = JSTaggedValue::ToInteger(GetThread(), indexHandle);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
        uint32_t index = static_cast<uint32_t>(integerValue.ToInt32());
        JSType type = receiver->GetTaggedObject()->GetClass()->GetObjectType();
        return JSTypedArray::FastGetPropertyByIndex(GetThread(), receiver.GetTaggedValue(), index, type);
    } else {
        ObjectOperator op(GetThread(), receiver, key);
        auto result = JSHandle<JSTaggedValue>(GetThread(), JSObject::GetProperty(GetThread(), &op));
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
        if (op.GetValue().IsAccessor()) {
            op = ObjectOperator(GetThread(), receiver, key);
        }
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
        // ic-switch
        if (!GetThread()->GetEcmaVM()->ICEnabled()) {
            icAccessor_.SetAsMega();
            return result.GetTaggedValue();
        }
        TraceIC(GetThread(), receiver, key);
        // do not cache element
        if (!op.IsFastMode()) {
            icAccessor_.SetAsMega();
            return result.GetTaggedValue();
        }
        UpdateLoadHandler(op, key, receiver);
        return result.GetTaggedValue();
    }
}

JSTaggedValue StoreICRuntime::StoreMiss(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key,
                                        JSHandle<JSTaggedValue> value, bool isOwn)
{
    ICKind kind = GetICKind();
    if (IsValueIC(kind)) {
        key = JSTaggedValue::ToPropertyKey(GetThread(), key);
    }
    if (!receiver->IsJSObject() || receiver->HasOrdinaryGet()) {
        icAccessor_.SetAsMega();
        JSTaggedValue::SetProperty(GetThread(), receiver, key, value, true);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
        return JSTaggedValue::Undefined();
    }
    if (receiver->IsTypedArray() || receiver->IsSharedTypedArray()) {
        return StoreTypedArrayValueMiss(receiver, key, value);
    }

    // global variable find from global record firstly
    if (kind == ICKind::NamedGlobalStoreIC || kind == ICKind::NamedGlobalTryStoreIC) {
        JSTaggedValue box = SlowRuntimeStub::LdGlobalRecord(thread_, key.GetTaggedValue());
        if (!box.IsUndefined()) {
            ASSERT(box.IsPropertyBox());
            SlowRuntimeStub::TryUpdateGlobalRecord(thread_, key.GetTaggedValue(), value.GetTaggedValue());
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
            if (icAccessor_.GetICState() != ProfileTypeAccessor::ICState::MEGA) {
                icAccessor_.AddGlobalRecordHandler(JSHandle<JSTaggedValue>(thread_, box));
            }
            return JSTaggedValue::Undefined();
        }
    }
    UpdateReceiverHClass(JSHandle<JSTaggedValue>(GetThread(), JSHandle<JSObject>::Cast(receiver)->GetClass()));

    // fixme(hzzhouzebin) Open IC for SharedArray later.
    if (receiver->IsJSSharedArray()) {
        bool success = JSSharedArray::SetProperty(thread_, receiver, key, value, true, SCheckMode::CHECK);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
        return success ? JSTaggedValue::Undefined() : JSTaggedValue::Exception();
    }
    if (key->IsJSFunction()) { // key is a private setter
        return CallPrivateSetter(receiver, key, value);
    }

    ObjectOperator op(GetThread(), receiver, key, isOwn ? OperatorType::OWN : OperatorType::PROTOTYPE_CHAIN);
    if (!op.IsFound()) {
        if (kind == ICKind::NamedGlobalStoreIC) {
            PropertyAttributes attr = PropertyAttributes::Default(true, true, false);
            op.SetAttr(attr);
        } else if (kind == ICKind::NamedGlobalTryStoreIC) {
            return SlowRuntimeStub::ThrowReferenceError(GetThread(), key.GetTaggedValue(), " is not defined");
        }
    }

    bool success = false;
    // If op is Accessor, it may change the properties of receiver or receiver's proto,
    // causing IC compute errors, so move SetPropertyForAccessor to be executed after UpdateStoreHandler.
    bool isAccessor = false;
    if (isOwn) {
        bool enumerable = !(receiver->IsClassPrototype() || receiver->IsClassConstructor());
        PropertyDescriptor desc(thread_, value, true, enumerable, true);
        success = JSObject::DefineOwnProperty(thread_, &op, desc);
    } else {
        success = JSObject::SetPropertyForData(&op, value, &isAccessor);
    }
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);

    // IC Disable
    if (!GetThread()->GetEcmaVM()->ICEnabled() || !op.IsFastMode()) {
        icAccessor_.SetAsMega();
        if (!success) {
            return JSTaggedValue::Exception();
        }
        if (isAccessor) {
            return HandleAccesor(&op, value);
        }
        return JSTaggedValue::Undefined();
    }

    TraceIC(GetThread(), receiver, key);
    if (success) {
        UpdateStoreHandler(op, key, receiver);
    }
    if (isAccessor) {
        return HandleAccesor(&op, value);
    }
    return success ? JSTaggedValue::Undefined() : JSTaggedValue::Exception();
}

inline JSTaggedValue StoreICRuntime::CallPrivateSetter(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key,
                                                       JSHandle<JSTaggedValue> value)
{
    JSHandle<JSTaggedValue> undefined = thread_->GlobalConstants()->GetHandledUndefined();
    EcmaRuntimeCallInfo* info =
        EcmaInterpreter::NewRuntimeCallInfo(thread_, key, receiver, undefined, 1); // 1: setter has 1 argument
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
    info->SetCallArg(value.GetTaggedValue());
    JSTaggedValue resSetter = JSFunction::Call(info);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);
    return resSetter;
}

JSTaggedValue StoreICRuntime::StoreTypedArrayValueMiss(JSHandle<JSTaggedValue> receiver, JSHandle<JSTaggedValue> key,
                                                       JSHandle<JSTaggedValue> value)
{
    JSHandle<JSTaggedValue> propKey = JSTaggedValue::ToPropertyKey(GetThread(), key);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
    JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(GetThread(), propKey);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
    if (!numericIndex.IsUndefined()) {
        if (!JSTypedArray::IsValidIntegerIndex(receiver, numericIndex) || value->IsECMAObject() ||
            !GetThread()->GetEcmaVM()->ICEnabled()) {
            icAccessor_.SetAsMega();
            bool success = JSTaggedValue::SetProperty(GetThread(), receiver, propKey, value, true);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
            return success ? JSTaggedValue::Undefined() : JSTaggedValue::Exception();
        }
        UpdateTypedArrayHandler(receiver);
        JSHandle<JSTaggedValue> indexHandle(GetThread(), numericIndex);
        JSTaggedNumber integerValue = JSTaggedValue::ToInteger(GetThread(), indexHandle);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(GetThread());
        uint32_t index = static_cast<uint32_t>(integerValue.ToInt32());
        JSType type = receiver->GetTaggedObject()->GetClass()->GetObjectType();
        return JSTypedArray::FastSetPropertyByIndex(GetThread(), receiver.GetTaggedValue(), index,
                                                    value.GetTaggedValue(), type);
    } else {
        UpdateReceiverHClass(JSHandle<JSTaggedValue>(GetThread(), JSHandle<JSObject>::Cast(receiver)->GetClass()));
        ObjectOperator op(GetThread(), receiver, key);

        // If op is Accessor, it may change the properties of receiver or receiver's proto,
        // causing IC compute errors, so move SetPropertyForAccessor to be executed after UpdateStoreHandler.
        bool isAccessor = false;
        bool success = JSObject::SetPropertyForData(&op, value, &isAccessor);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread_);

        // IC Disable
        if (!GetThread()->GetEcmaVM()->ICEnabled() || !op.IsFastMode()) {
            icAccessor_.SetAsMega();
            if (!success) {
                return JSTaggedValue::Exception();
            }
            if (isAccessor) {
                return HandleAccesor(&op, value);
            }
            return JSTaggedValue::Undefined();
        }

        TraceIC(GetThread(), receiver, key);
        if (success) {
            UpdateStoreHandler(op, key, receiver);
        }
        if (isAccessor) {
            return HandleAccesor(&op, value);
        }
        return success ? JSTaggedValue::Undefined() : JSTaggedValue::Exception();
    }
}

JSTaggedValue StoreICRuntime::HandleAccesor(ObjectOperator *op, const JSHandle<JSTaggedValue> &value)
{
    bool success = JSObject::SetPropertyForAccessor(op, value);
    if (thread_->HasPendingException()) {
        icAccessor_.SetAsMega();
        return JSTaggedValue::Exception();
    }
    return success ? JSTaggedValue::Undefined() : JSTaggedValue::Exception();
}

void ICRuntime::TraceIC([[maybe_unused]] JSThread *thread,
                        [[maybe_unused]] JSHandle<JSTaggedValue> receiver,
                        [[maybe_unused]] JSHandle<JSTaggedValue> key) const
{
#if ECMASCRIPT_ENABLE_TRACE_IC
    // If BackTrace affects IC, can choose not to execute it.
    std::string strTraceIC = "Miss Func BackTrace: ";
    std::vector<JsFrameInfo> jsStackInfo = JsStackInfo::BuildJsStackInfo(thread, true);
    if (jsStackInfo.empty()) {
        strTraceIC += "empty";
    } else {
        JsFrameInfo jsFrameInfo = jsStackInfo.front();
        size_t pos = jsFrameInfo.pos.find(':', 0);
        if (pos != CString::npos) {
            int lineNumber = std::stoi(jsFrameInfo.pos.substr(0, pos));
            int columnNumber = std::stoi(jsFrameInfo.pos.substr(pos + 1));
            auto sourceMapcb = thread->GetEcmaVM()->GetSourceMapTranslateCallback();
            if (sourceMapcb != nullptr && !jsFrameInfo.fileName.empty()) {
                sourceMapcb(jsFrameInfo.fileName, lineNumber, columnNumber);
            }
        }
        strTraceIC += "funcName: " + jsFrameInfo.functionName + ", url: " +
            jsFrameInfo.fileName + ":" + jsFrameInfo.pos;
    }
    LOG_ECMA(ERROR) << strTraceIC;

    auto kind = ICKindToString(GetICKind());
    bool primitiveIc = false;
    if (receiver->IsNumber() || receiver->IsString()) {
        primitiveIc = true;
    }
    auto state = ProfileTypeAccessor::ICStateToString(icAccessor_.GetICState());
    if (key->IsString()) {
        auto keyStrHandle = JSHandle<EcmaString>::Cast(key);
        LOG_ECMA(ERROR) << kind << " miss, key is: " << EcmaStringAccessor(keyStrHandle).ToCString()
                        << ", icstate is: " << state
                        << ", slotid is: " << GetSlotId();
    } else {
        LOG_ECMA(ERROR) << kind << " miss, "
                        << ", icstate is " << state
                        << ", slotid is:" << GetSlotId();
    }
    if (primitiveIc) {
        LOG_ECMA(ERROR) << "primitiveIc ";
    } else {
        JSHClass *jshclass = receiver->GetTaggedObject()->GetClass();
        LOG_ECMA(ERROR) << "receiver DictionaryMode is: " << jshclass->IsDictionaryMode()
                        << ", hclass is: "<< std::hex << jshclass;
    }
#endif
}
}  // namespace panda::ecmascript
