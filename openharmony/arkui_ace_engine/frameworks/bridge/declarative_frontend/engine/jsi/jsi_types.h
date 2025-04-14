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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_TYPES_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_TYPES_H

#include <string>

#include "ecmascript/napi/include/jsnapi.h"

#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_fwd.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_value_conversions.h"

#define FAKE_PTR_FOR_FUNCTION_ACCESS(klass) \
    const klass* operator->() const         \
    {                                       \
        return this;                        \
    }

namespace OHOS::Ace::Framework {

using JsiFunctionCallback = panda::Local<panda::JSValueRef> (*)(panda::JsiRuntimeCallInfo*);
using EcmaVM = panda::ecmascript::EcmaVM;

template<typename T>
class JsiType {
public:
    JsiType() = default;
    JsiType(const JsiType& rhs);
    JsiType(JsiType&& rhs);
    virtual ~JsiType() = default;

    explicit JsiType(panda::Local<T> val);
    explicit JsiType(const panda::CopyableGlobal<T>& other);
    explicit JsiType(const EcmaVM *vm, panda::Local<T> val);

    template<typename S>
    explicit JsiType(panda::Local<S> val);

    JsiType& operator=(const JsiType& rhs);
    JsiType& operator=(JsiType&& rhs);

    template<class... Args>
    static JsiType<T> New(Args &&... args);

    void SetWeakCallback(void *ref, panda::WeakRefClearCallBack callback);
    const panda::CopyableGlobal<T>& GetHandle() const;
    const panda::CopyableGlobal<T>& operator->() const;
    Local<T> GetLocalHandle() const;
    bool IsEmpty() const;
    bool IsWeak() const;
    void Reset();
    operator panda::CopyableGlobal<T>() const;

    const EcmaVM* GetEcmaVM() const;

private:
    panda::CopyableGlobal<T> handle_;
};

class ACE_FORCE_EXPORT JsiValue : public JsiType<panda::JSValueRef> {
public:
    JsiValue() = default;
    explicit JsiValue(const panda::CopyableGlobal<panda::JSValueRef>& val);
    explicit JsiValue(panda::Local<panda::JSValueRef> val);
    explicit JsiValue(const EcmaVM *vm, panda::Local<panda::JSValueRef> val);
    ~JsiValue() override = default;

    bool IsEmpty() const;
    bool IsFunction() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsBoolean() const;
    bool IsObject() const;
    bool IsArray() const;
    bool IsArrayBuffer() const;
    bool IsUint8ClampedArray() const;
    bool IsUndefined() const;
    bool IsNull() const;
    bool IsDate() const;
    std::string ToString() const;
    std::u16string ToU16String() const;
    bool ToBoolean() const;

    template<typename T>
    T ToNumber() const;

    static JsiRef<JsiValue> Undefined();
    static JsiRef<JsiValue> Null();
    static JsiRef<JsiValue> True();
    static JsiRef<JsiValue> False();

    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        return JsiValue(that.GetHandle());
    }

    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiValue)
};

/**
 * @brief A wrapper around a panda::StringRef
 *
 */
class JsiString : public JsiType<panda::StringRef> {
public:
    JsiString() = default;
    explicit JsiString(panda::Local<panda::StringRef> val);
    explicit JsiString(const panda::CopyableGlobal<panda::StringRef>& val);
    ~JsiString() override = default;

    static panda::Local<panda::StringRef> New(const char* str);
    static panda::Local<panda::StringRef> New(const std::string& str);
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsString())) {
            LOGF_ABORT("bad cast to JsiString.");
        }
        return JsiString(that.GetHandle());
    }
    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiString)
};

/**
 * @brief A wrapper around a panda::ArrayRef
 *
 */
class JsiArray : public JsiType<panda::ArrayRef> {
public:
    JsiArray();
    explicit JsiArray(panda::Local<panda::ArrayRef> val);
    explicit JsiArray(const EcmaVM *vm, panda::Local<panda::ArrayRef> val);
    explicit JsiArray(const panda::CopyableGlobal<panda::ArrayRef>& val);
    ~JsiArray() override = default;
    JsiRef<JsiValue> GetValueAt(size_t index) const;
    void SetValueAt(size_t index, JsiRef<JsiValue> value) const;
    JsiRef<JsiValue> GetProperty(const char* prop) const;
    JsiRef<JsiValue> GetProperty(int32_t propertyIndex) const;
    size_t Length() const;
    void SetLength(size_t length) const;
    bool IsArray() const;
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsArray())) {
            LOGF_ABORT("bad cast to JsiArray.");
        }
        return JsiArray(that.GetHandle());
    }
    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiArray)
};

/**
 * @brief A wrapper around a panda::ArrayBufferRef
 *
 */
class JsiArrayBuffer : public JsiType<panda::ArrayBufferRef> {
public:
    JsiArrayBuffer() = default;
    explicit JsiArrayBuffer(panda::Local<panda::ArrayBufferRef> val);
    explicit JsiArrayBuffer(const panda::CopyableGlobal<panda::ArrayBufferRef>& val);
    int32_t ByteLength() const;
    void* GetBuffer() const;
    void Detach() const;
    bool IsDetach() const;
    ~JsiArrayBuffer() override = default;
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsArrayBuffer())) {
            LOGF_ABORT("bad cast to JsiArrayBuffer.");
        }
        return JsiArrayBuffer(that.GetHandle());
    }
    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiArrayBuffer)
};

/**
 * @brief A wrapper around a panda::Uint8ClampedArrayRef
 *
 */
class JsiUint8ClampedArray : public JsiType<panda::Uint8ClampedArrayRef> {
public:
    JsiUint8ClampedArray() = default;
    explicit JsiUint8ClampedArray(panda::Local<panda::Uint8ClampedArrayRef> val);
    explicit JsiUint8ClampedArray(const panda::CopyableGlobal<panda::Uint8ClampedArrayRef>& val);
    ~JsiUint8ClampedArray() override = default;
    JsiRef<JsiArrayBuffer> GetArrayBuffer() const;
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsUint8ClampedArray())) {
            LOGF_ABORT("bad cast to JsiUint8ClampedArray.");
        }
        return JsiUint8ClampedArray(that.GetHandle());
    }
    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiUint8ClampedArray)
};

/**
 * @brief A wrapper around panda::ObjectRef
 *
 */
class JsiObject : public JsiType<panda::ObjectRef> {
public:
    JsiObject();
    explicit JsiObject(panda::Local<panda::ObjectRef> val);
    explicit JsiObject(const EcmaVM *vm, panda::Local<panda::ObjectRef> val);
    ACE_FORCE_EXPORT explicit JsiObject(const panda::CopyableGlobal<panda::ObjectRef>& val);
    bool IsUndefined() const;
    ~JsiObject() override = default;
    enum InternalFieldIndex { INSTANCE = 0 };

    template<typename T>
    T* Unwrap() const;

    template<typename T>
    void Wrap(T* data) const;

    JsiRef<JsiArray> GetPropertyNames() const;
    JsiRef<JsiValue> GetProperty(const char* prop) const;
    JsiRef<JsiValue> GetProperty(int32_t propertyIndex) const;
    bool HasProperty(int32_t propertyIndex) const;
    bool HasProperty(const char* prop) const;
    JsiRef<JsiValue> ToJsonObject(const char* value) const;

    template<typename T>
    T GetPropertyValue(int32_t propertyIndex, T defaultValue) const;

    template<typename T>
    T GetPropertyValue(const char* prop, T defaultValue) const;

    template<typename T>
    void SetProperty(int32_t propertyIndex, const T value) const;

    template<typename T>
    void SetProperty(const char* prop, const T value) const;
    void SetPropertyJsonObject(const char* prop, const char* value) const;
    void SetPropertyObject(const char* prop, JsiRef<JsiValue> value) const;
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsObject())) {
            LOGF_ABORT("bad cast to JsiObject.");
        }
        return JsiObject(that.GetHandle());
    }

    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiObject)
};

/**
 * @brief A wrapper around panda::FunctionRef
 *
 */
class JsiFunction : public JsiType<panda::FunctionRef> {
public:
    JsiFunction();
    explicit JsiFunction(panda::Local<panda::FunctionRef> val);
    explicit JsiFunction(const EcmaVM *vm, panda::Local<panda::FunctionRef> val);
    explicit JsiFunction(const panda::CopyableGlobal<panda::FunctionRef>& val);
    ~JsiFunction() override = default;

    JsiRef<JsiValue> Call(
        JsiRef<JsiValue> thisVal, int argc = 0, JsiRef<JsiValue> argv[] = nullptr, bool isAnimation = false) const;
    static panda::Local<panda::FunctionRef> New(JsiFunctionCallback func);
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsFunction())) {
            LOGF_ABORT("bad cast to JsiFunction.");
        }
        return JsiFunction(that.GetHandle());
    }

    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiFunction)
};

class JsiObjTemplate : public JsiObject {
public:
    JsiObjTemplate() = default;
    explicit JsiObjTemplate(panda::Local<panda::ObjectRef> val);
    explicit JsiObjTemplate(const panda::CopyableGlobal<panda::ObjectRef>& val);
    ~JsiObjTemplate() override = default;

    void SetInternalFieldCount(int32_t count) const;
    JsiRef<JsiObject> NewInstance() const;
    static panda::Local<panda::JSValueRef> New();
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsObject())) {
            LOGF_ABORT("bad cast to JsiObjTemplate.");
        }
        return JsiObjTemplate(that.GetHandle());
    }

    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiObjTemplate)
};

struct JsiExecutionContext {
    panda::ecmascript::EcmaVM* vm_ = nullptr;
};

class JsiCallbackInfo {
public:
    JsiCallbackInfo(panda::JsiRuntimeCallInfo* info);
    ~JsiCallbackInfo() = default;
    JsiCallbackInfo(const JsiCallbackInfo&) = delete;
    JsiCallbackInfo& operator=(const JsiCallbackInfo&) = delete;

    JsiRef<JsiValue> operator[](size_t index) const;
    JsiRef<JsiObject> This() const;
    uint32_t Length() const;

    template<typename T>
    void SetReturnValue(T* instance) const;

    template<typename T>
    void SetReturnValue(JsiRef<T> val) const;

    void ReturnSelf() const;

    std::variant<void*, panda::CopyableGlobal<panda::JSValueRef>> GetReturnValue()
    {
        return retVal_;
    }

    panda::JsiRuntimeCallInfo* GetJsiRuntimeCallInfo() const
    {
        return info_;
    }

    JsiExecutionContext GetExecutionContext() const
    {
        return JsiExecutionContext { info_->GetVM() };
    }

    panda::ecmascript::EcmaVM* GetVm() const
    {
        return info_->GetVM();
    }

    void SetSize(size_t size) const
    {
        size_ = size;
    }

    size_t GetSize() const
    {
        return size_;
    }

    template<typename T>
    T* UnwrapArg(size_t index) const;
    bool GetBooleanArg(size_t index, bool& value) const;
    bool GetInt32Arg(size_t index, int32_t& value) const;
    bool GetUint32Arg(size_t index, uint32_t& value) const;
    bool GetDoubleArg(size_t index, double& value, bool isJudgeSpecialValue = false) const;
    bool GetDoubleArrayArg(size_t index, std::vector<double>& valueArr) const;
    bool GetStringArg(size_t index, std::string& value) const;

private:
    mutable size_t size_ = 0;
    panda::JsiRuntimeCallInfo* info_ = nullptr;
    mutable std::variant<void*, panda::CopyableGlobal<panda::JSValueRef>> retVal_;
};

class JsiGCMarkCallbackInfo {
public:
    template<typename T>
    void Mark(const JsiRef<T>& val) const
    {}
};

class JsiDate : public JsiType<panda::DateRef> {
public:
    JsiDate() = default;
    explicit JsiDate(panda::Local<panda::DateRef> val);
    explicit JsiDate(const panda::CopyableGlobal<panda::DateRef>& val);
    ~JsiDate() override = default;

    static JsiRef<JsiValue> New(double value);
    template<typename S>
    static auto Cast(const JsiType<S>& that)
    {
        if (ACE_UNLIKELY(SystemProperties::DetectJsObjTypeConvertion() &&
            !JsiValue::Cast(that)->IsDate())) {
            LOGF_ABORT("bad cast to JsiDate.");
        }
        return JsiDate(that.GetHandle());
    }
    FAKE_PTR_FOR_FUNCTION_ACCESS(JsiDate)
};

class JsiException {
public:
    template<typename... Args>
    static void Throw(const char* format, Args... args);
    template<typename... Args>
    static void Throw(int32_t code, const char* format, Args... args);
    template<typename... Args>
    static void ThrowRangeError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowReferenceError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowSyntaxError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowTypeError(const char* format, Args... args);
    template<typename... Args>
    static void ThrowEvalError(const char* format, Args... args);
};

} // namespace OHOS::Ace::Framework

#include "jsi_types.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_TYPES_H
