/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "plugins/ets/runtime/interop_js/call/call.h"
#include "plugins/ets/runtime/interop_js/call/arg_convertors.h"
#include "plugins/ets/runtime/interop_js/call/proto_reader.h"
#include "plugins/ets/runtime/interop_js/code_scopes.h"

namespace ark::ets::interop::js {

static ALWAYS_INLINE inline arch::ArgReader<RUNTIME_ARCH> CreateProxyBridgeArgReader(uint8_t *args,
                                                                                     uint8_t *inStackArgs)
{
    Span<uint8_t> inGprArgs(args, arch::ExtArchTraits<RUNTIME_ARCH>::GP_ARG_NUM_BYTES);
    Span<uint8_t> inFprArgs(inGprArgs.end(), arch::ExtArchTraits<RUNTIME_ARCH>::FP_ARG_NUM_BYTES);
    return arch::ArgReader<RUNTIME_ARCH>(inGprArgs, inFprArgs, inStackArgs);
}

class CallJSHandler {
public:
    ALWAYS_INLINE CallJSHandler(EtsCoroutine *coro, Method *method, uint8_t *args, uint8_t *inStackArgs)
        : coro_(coro),
          ctx_(InteropCtx::Current(coro_)),
          protoReader_(method, ctx_->GetClassLinker(), ctx_->LinkerCtx()),
          argReader_(CreateProxyBridgeArgReader(args, inStackArgs))
    {
    }

    ALWAYS_INLINE ObjectHeader *SetupArgreader(bool isInstance)
    {
        auto method = protoReader_.GetMethod();
        protoReader_.Advance();                // skip return type
        argReader_.template Read<Method *>();  // skip method
        ASSERT(isInstance == !method->IsStatic());
        numArgs_ = method->GetNumArgs() - static_cast<uint32_t>(isInstance);
        return isInstance ? argReader_.Read<ObjectHeader *>() : nullptr;
    }

    template <panda_file::Type::TypeId PF_TYPEID, typename T>
    ALWAYS_INLINE T ReadFixedArg()
    {
        ASSERT(protoReader_.GetType() == panda_file::Type(PF_TYPEID));
        protoReader_.Advance();
        numArgs_--;
        return argReader_.Read<T>();
    }

    template <typename T>
    ALWAYS_INLINE T *ReadFixedRefArg([[maybe_unused]] Class *expected)
    {
        ASSERT(expected == nullptr || protoReader_.GetClass() == expected);
        return ReadFixedArg<panda_file::Type::TypeId::REFERENCE, T *>();
    }

    ALWAYS_INLINE void SetupJSCallee(napi_value jsThis, napi_value jsFn)
    {
        jsThis_ = jsThis;
        jsFn_ = jsFn;
    }

    template <bool IS_NEWCALL, typename ArgSetup>
    static ALWAYS_INLINE uint64_t HandleImpl(Method *method, uint8_t *args, uint8_t *inStackArgs)
    {
        CallJSHandler st(EtsCoroutine::GetCurrent(), method, args, inStackArgs);
        return st.Handle<IS_NEWCALL, ArgSetup>();
    }

    ALWAYS_INLINE Method *GetMethod()
    {
        return protoReader_.GetMethod();
    }

    ~CallJSHandler() = default;

private:
    static uint64_t __attribute__((noinline)) ForwardException(InteropCtx *ctx, EtsCoroutine *coro)
    {
        if (NapiIsExceptionPending(ctx->GetJSEnv())) {
            ctx->ForwardJSException(coro);
        }
        ASSERT(ctx->SanityETSExceptionPending());
        return 0;
    }

    template <bool IS_NEWCALL, typename ArgSetup>
    ALWAYS_INLINE uint64_t Handle();

    template <bool IS_NEWCALL>
    ALWAYS_INLINE std::optional<napi_value> ConvertArgsAndCall();

    template <bool IS_NEWCALL>
    ALWAYS_INLINE std::optional<Value> ConvertRetval(napi_value jsRet);

    NO_COPY_SEMANTIC(CallJSHandler);
    NO_MOVE_SEMANTIC(CallJSHandler);

    EtsCoroutine *const coro_;
    InteropCtx *const ctx_;

    ProtoReader protoReader_;
    arch::ArgReader<RUNTIME_ARCH> argReader_;
    uint32_t numArgs_ {};
    napi_value jsThis_ {};
    napi_value jsFn_ {};
};

template <bool IS_NEWCALL, typename ArgSetup>
ALWAYS_INLINE inline uint64_t CallJSHandler::Handle()
{
    [[maybe_unused]] InteropCodeScopeETS codeScope(coro_, __PRETTY_FUNCTION__);
    napi_env env = ctx_->GetJSEnv();
    NapiScope jsHandleScope(env);

    if (UNLIKELY(!ArgSetup()(ctx_, this))) {
        return ForwardException(ctx_, coro_);
    }
    if (UNLIKELY(GetValueType(env, jsFn_) != napi_function)) {
        ctx_->ThrowJSTypeError(env, "call target is not a function");
        return ForwardException(ctx_, coro_);
    }

    std::optional<napi_value> jsRes = ConvertArgsAndCall<IS_NEWCALL>();
    if (UNLIKELY(!jsRes.has_value())) {
        return ForwardException(ctx_, coro_);
    }
    std::optional<Value> etsRes = ConvertRetval<IS_NEWCALL>(jsRes.value());
    if (UNLIKELY(!etsRes.has_value())) {
        return ForwardException(ctx_, coro_);
    }
    return static_cast<uint64_t>(etsRes.value().GetAsLong());
}

template <bool IS_NEWCALL>
ALWAYS_INLINE inline std::optional<napi_value> CallJSHandler::ConvertArgsAndCall()
{
    napi_env env = ctx_->GetJSEnv();
    auto jsargs = ctx_->GetTempArgs<napi_value>(numArgs_);

    for (uint32_t argIdx = 0; argIdx < numArgs_; ++argIdx, protoReader_.Advance()) {
        auto readVal = [this](auto typeTag) { return argReader_.template Read<typename decltype(typeTag)::type>(); };
        if (UNLIKELY(!ConvertArgToJS(ctx_, protoReader_, &jsargs[argIdx], readVal))) {
            return std::nullopt;
        }
    }

    napi_value jsRetval;
    napi_status jsStatus;
    {
        ScopedNativeCodeThread nativeScope(coro_);
        if constexpr (IS_NEWCALL) {
            jsStatus = napi_new_instance(env, jsFn_, jsargs->size(), jsargs->data(), &jsRetval);
        } else {
            jsStatus = napi_call_function(env, jsThis_, jsFn_, jsargs->size(), jsargs->data(), &jsRetval);
        }
    }
    if (UNLIKELY(jsStatus != napi_ok)) {
        INTEROP_FATAL_IF(jsStatus != napi_pending_exception);
        return std::nullopt;
    }
    return jsRetval;
}

template <bool IS_NEWCALL>
ALWAYS_INLINE inline std::optional<Value> CallJSHandler::ConvertRetval(napi_value jsRet)
{
    napi_env env = ctx_->GetJSEnv();
    Value etsRet;
    protoReader_.Reset();

    if constexpr (IS_NEWCALL) {
        ASSERT(protoReader_.GetClass() == ctx_->GetJSValueClass());
        auto res = JSConvertJSValue::Unwrap(ctx_, env, jsRet);
        if (UNLIKELY(!res.has_value())) {
            return std::nullopt;
        }
        etsRet = Value(res.value()->GetCoreType());
    } else {
        auto store = [&etsRet](auto val) { etsRet = Value(val); };
        if (UNLIKELY(!ConvertArgToEts(ctx_, protoReader_, store, jsRet))) {
            return std::nullopt;
        }
    }
    return etsRet;
}

static std::optional<std::pair<napi_value, napi_value>> ResolveQualifiedReceiverTarget(napi_env env, napi_value jsVal,
                                                                                       coretypes::String *qnameStr)
{
    napi_value jsThis {};
    ASSERT(qnameStr->IsMUtf8());
    auto qname = std::string(utf::Mutf8AsCString(qnameStr->GetDataMUtf8()), qnameStr->GetMUtf8Length());

    auto resolveName = [&jsThis, &jsVal, &env](const std::string &name) -> bool {
        jsThis = jsVal;
        INTEROP_LOG(DEBUG) << "JSRuntimeCallJS: resolve name: " << name;
        napi_status rc = napi_get_named_property(env, jsVal, name.c_str(), &jsVal);
        if (UNLIKELY(rc == napi_object_expected || NapiThrownGeneric(rc))) {
            ASSERT(NapiIsExceptionPending(env));
            return false;
        }
        return true;
    };
    jsThis = jsVal;
    if (UNLIKELY(!WalkQualifiedName(qname, resolveName))) {
        return std::nullopt;
    }
    return std::make_pair(jsThis, jsVal);
}

template <bool IS_NEWCALL>
static ALWAYS_INLINE inline uint64_t JSRuntimeCallJSQNameBase(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    struct ArgSetup {
        ALWAYS_INLINE bool operator()(InteropCtx *ctx, CallJSHandler *st)
        {
            st->SetupArgreader(false);
            napi_env env = ctx->GetJSEnv();

            napi_value jsVal = JSConvertJSValue::Wrap(env, st->ReadFixedRefArg<JSValue>(ctx->GetJSValueClass()));
            auto qnameStr = st->ReadFixedRefArg<coretypes::String>(ctx->GetStringClass());

            auto res = ResolveQualifiedReceiverTarget(env, jsVal, qnameStr);
            if (UNLIKELY(!res.has_value())) {
                ASSERT(NapiIsExceptionPending(env));
                return false;
            }

            st->SetupJSCallee(res->first, res->second);
            return true;
        }
    };
    return CallJSHandler::HandleImpl<IS_NEWCALL, ArgSetup>(method, args, inStackArgs);
}

extern "C" uint64_t JSRuntimeCallJSQName(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    return JSRuntimeCallJSQNameBase<false>(method, args, inStackArgs);  // IS_NEWCALL is false
}
extern "C" void JSRuntimeCallJSQNameBridge(Method *method, ...);

extern "C" uint64_t JSRuntimeNewCallJSQName(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    return JSRuntimeCallJSQNameBase<true>(method, args, inStackArgs);  // IS_NEWCALL is true
}
extern "C" void JSRuntimeNewCallJSQNameBridge(Method *method, ...);

static uint32_t GetClassQnameOffset(InteropCtx *ctx, Method *method)
{
    auto klass = method->GetClass();
    ctx->GetConstStringStorage()->LoadDynamicCallClass(klass);
    auto fields = klass->GetStaticFields();
    ASSERT(fields.size() == 1);
    return klass->GetFieldPrimitive<uint32_t>(fields[0]);
}

template <bool IS_NEWCALL>
static ALWAYS_INLINE inline uint64_t JSRuntimeCallJSBase(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    struct ArgSetup {
        ALWAYS_INLINE bool operator()(InteropCtx *ctx, CallJSHandler *st)
        {
            st->SetupArgreader(false);
            napi_env env = ctx->GetJSEnv();

            napi_value jsVal = JSConvertJSValue::Wrap(env, st->ReadFixedRefArg<JSValue>(ctx->GetJSValueClass()));

            auto classQnameOffset = GetClassQnameOffset(ctx, st->GetMethod());
            auto qnameStart = st->ReadFixedArg<panda_file::Type::TypeId::I32, int32_t>() + classQnameOffset;
            auto qnameLen = st->ReadFixedArg<panda_file::Type::TypeId::I32, int32_t>();
            napi_value jsThis {};

            auto success = ctx->GetConstStringStorage()->EnumerateStrings(
                qnameStart, qnameLen, [&jsThis, &jsVal, env](napi_value jsStr) {
                    jsThis = jsVal;
                    napi_status rc = napi_get_property(env, jsVal, jsStr, &jsVal);
                    if (UNLIKELY(rc == napi_object_expected || NapiThrownGeneric(rc))) {
                        ASSERT(NapiIsExceptionPending(env));
                        return false;
                    }
                    return true;
                });

            if (!success) {
                ASSERT(NapiIsExceptionPending(env));
                return false;
            }
            st->SetupJSCallee(jsThis, jsVal);
            return true;
        }
    };
    return CallJSHandler::HandleImpl<IS_NEWCALL, ArgSetup>(method, args, inStackArgs);
}

extern "C" uint64_t JSRuntimeCallJS(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    return JSRuntimeCallJSBase<false>(method, args, inStackArgs);  // IS_NEWCALL is false
}
extern "C" void JSRuntimeCallJSBridge(Method *method, ...);

extern "C" uint64_t JSRuntimeNewCallJS(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    return JSRuntimeCallJSBase<true>(method, args, inStackArgs);  // IS_NEWCALL is true
}
extern "C" void JSRuntimeNewCallJSBridge(Method *method, ...);

extern "C" uint64_t JSRuntimeCallJSByValue(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    struct ArgSetup {
        ALWAYS_INLINE bool operator()(InteropCtx *ctx, CallJSHandler *st)
        {
            st->SetupArgreader(false);
            napi_env env = ctx->GetJSEnv();

            napi_value jsFn = JSConvertJSValue::Wrap(env, st->ReadFixedRefArg<JSValue>(ctx->GetJSValueClass()));
            napi_value jsThis = JSConvertJSValue::Wrap(env, st->ReadFixedRefArg<JSValue>(ctx->GetJSValueClass()));

            st->SetupJSCallee(jsThis, jsFn);
            return true;
        }
    };
    return CallJSHandler::HandleImpl<false, ArgSetup>(method, args, inStackArgs);
}
extern "C" void JSRuntimeCallJSByValueBridge(Method *method, ...);

extern "C" uint64_t CallJSProxy(Method *method, uint8_t *args, uint8_t *inStackArgs)
{
    struct ArgSetup {
        ALWAYS_INLINE bool operator()(InteropCtx *ctx, CallJSHandler *st)
        {
            ObjectHeader *etsThis = st->SetupArgreader(true);
            napi_env env = ctx->GetJSEnv();

            auto refconv = JSRefConvertResolve(ctx, etsThis->ClassAddr<Class>());
            napi_value jsThis = refconv->Wrap(ctx, EtsObject::FromCoreType(etsThis));
            ASSERT(GetValueType(env, jsThis) == napi_object);
            const char *methodName = utf::Mutf8AsCString(st->GetMethod()->GetName().data);
            napi_value jsFn;
            napi_status rc = napi_get_named_property(env, jsThis, methodName, &jsFn);
            if (UNLIKELY(rc == napi_object_expected || NapiThrownGeneric(rc))) {
                ASSERT(NapiIsExceptionPending(env));
                return false;
            }

            st->SetupJSCallee(jsThis, jsFn);
            return true;
        }
    };
    return CallJSHandler::HandleImpl<false, ArgSetup>(method, args, inStackArgs);
}
extern "C" void CallJSProxyBridge(Method *method, ...);

static void *SelectCallJSEntrypoint(InteropCtx *ctx, Method *method)
{
    ASSERT(method->IsStatic());
    ProtoReader protoReader(method, ctx->GetClassLinker(), ctx->LinkerCtx());

    // skip return type
    protoReader.Advance();

    ASSERT(protoReader.GetClass() == ctx->GetJSValueClass());
    protoReader.Advance();

    if (protoReader.GetType().IsPrimitive()) {
        if (protoReader.GetType().GetId() != panda_file::Type::TypeId::I32) {
            return nullptr;
        }
        protoReader.Advance();

        if (protoReader.GetType().GetId() != panda_file::Type::TypeId::I32) {
            return nullptr;
        }
        return reinterpret_cast<void *>(JSRuntimeCallJSBridge);
    }
    if (protoReader.GetClass() == ctx->GetStringClass()) {
        return reinterpret_cast<void *>(JSRuntimeCallJSQNameBridge);
    }
    if (protoReader.GetClass() == ctx->GetJSValueClass()) {
        return reinterpret_cast<void *>(JSRuntimeCallJSByValueBridge);
    }
    InteropFatal("Bad jscall signature");
}

static void *SelectNewCallJSEntrypoint(InteropCtx *ctx, Method *method)
{
    ASSERT(method->IsStatic());
    ProtoReader protoReader(method, ctx->GetClassLinker(), ctx->LinkerCtx());

    if (protoReader.GetClass() != ctx->GetJSValueClass()) {
        return nullptr;
    }
    protoReader.Advance();

    if (protoReader.GetClass() != ctx->GetJSValueClass()) {
        return nullptr;
    }
    protoReader.Advance();

    if (protoReader.GetType().IsPrimitive()) {
        if (protoReader.GetType().GetId() != panda_file::Type::TypeId::I32) {
            return nullptr;
        }
        protoReader.Advance();

        if (protoReader.GetType().GetId() != panda_file::Type::TypeId::I32) {
            return nullptr;
        }
        return reinterpret_cast<void *>(JSRuntimeNewCallJSBridge);
    }
    if (protoReader.GetClass() != ctx->GetStringClass()) {
        return nullptr;
    }
    return reinterpret_cast<void *>(JSRuntimeNewCallJSQNameBridge);
}

static std::optional<uint32_t> GetQnameCount(Class *klass)
{
    auto pf = klass->GetPandaFile();
    panda_file::ClassDataAccessor cda(*pf, klass->GetFileId());
    auto qnameCount =
        cda.EnumerateAnnotation("Lets/annotation/DynamicCall;", [pf](panda_file::AnnotationDataAccessor &ada) {
            for (uint32_t i = 0; i < ada.GetCount(); i++) {
                auto adae = ada.GetElement(i);
                auto *elemName = pf->GetStringData(adae.GetNameId()).data;
                if (utf::IsEqual(utf::CStringAsMutf8("value"), elemName)) {
                    return adae.GetArrayValue().GetCount();
                }
            }
            UNREACHABLE();
        });
    return qnameCount;
}

static uint8_t InitCallJSClass(coretypes::String *descriptorStr, bool isNewCall)
{
    auto coro = EtsCoroutine::GetCurrent();
    auto ctx = InteropCtx::Current(coro);
    auto *klass = ctx->GetClassLinker()->GetClass(descriptorStr->GetDataMUtf8(), true, ctx->LinkerCtx());
    INTEROP_FATAL_IF(klass == nullptr);
    INTEROP_LOG(DEBUG) << "Bind bridge call methods for " << utf::Mutf8AsCString(klass->GetDescriptor());

    for (auto &method : klass->GetMethods()) {
        if (method.IsConstructor()) {
            continue;
        }
        void *ep = nullptr;
        if (method.IsStatic()) {
            ep = isNewCall ? SelectNewCallJSEntrypoint(ctx, &method) : SelectCallJSEntrypoint(ctx, &method);
        }
        if (ep == nullptr) {
            InteropFatal("Bad interop call bridge signature");
        }
        method.SetCompiledEntryPoint(ep);
        method.SetNativePointer(nullptr);
    }

    auto qnameCount = GetQnameCount(klass);
    // JSCallClass which was manually created in test will not have the required annotation and field
    if (qnameCount.has_value()) {
        auto fields = klass->GetStaticFields();
        INTEROP_FATAL_IF(fields.Size() != 1);
        INTEROP_FATAL_IF(klass->GetFieldPrimitive<uint32_t>(fields[0]) != 0);
        klass->SetFieldPrimitive<uint32_t>(fields[0], ctx->AllocateSlotsInStringBuffer(*qnameCount));
    }
    return 1;
}

uint8_t JSRuntimeInitJSCallClass(EtsString *clsStr)
{
    return InitCallJSClass(clsStr->GetCoreType(), false);
}

uint8_t JSRuntimeInitJSNewClass(EtsString *clsStr)
{
    return InitCallJSClass(clsStr->GetCoreType(), true);
}

}  // namespace ark::ets::interop::js
