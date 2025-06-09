/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "file.h"
#include "handle_scope.h"
#include "include/coretypes/class.h"
#include "include/mem/panda_string.h"
#include "include/mtmanaged_thread.h"
#include "intrinsics.h"
#include "macros.h"
#include "mem/mem.h"
#include "mem/vm_handle.h"
#include "modifiers.h"
#include "plugins/ets/runtime/ets_panda_file_items.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/types/ets_string.h"
#include "types/ets_array.h"
#include "types/ets_box_primitive-inl.h"
#include "types/ets_class.h"
#include "types/ets_field.h"
#include "types/ets_method.h"
#include "types/ets_primitives.h"
#include "types/ets_runtime_linker.h"
#include "types/ets_type.h"
#include "types/ets_type_comptime_traits.h"
#include "types/ets_typeapi.h"
#include "types/ets_typeapi_field.h"
#include "types/ets_typeapi_method.h"
#include "types/ets_typeapi_parameter.h"

namespace ark::ets::intrinsics {

static EtsByte GetRefTypeKind(const EtsClass *refType)
{
    EtsByte result;

    if (refType->IsBoxed()) {
        const auto *runtimeClass = refType;
        auto ptypes = PlatformTypes(EtsCoroutine::GetCurrent());
        if (runtimeClass == ptypes->coreBoolean) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::BOOLEAN);
        } else if (runtimeClass == ptypes->coreByte) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::BYTE);
        } else if (runtimeClass == ptypes->coreChar) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::CHAR);
        } else if (runtimeClass == ptypes->coreShort) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::SHORT);
        } else if (runtimeClass == ptypes->coreInt) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::INT);
        } else if (runtimeClass == ptypes->coreLong) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::LONG);
        } else if (runtimeClass == ptypes->coreFloat) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::FLOAT);
        } else if (runtimeClass == ptypes->coreDouble) {
            result = static_cast<EtsByte>(EtsTypeAPIKind::DOUBLE);
        } else {
            UNREACHABLE();
        }
    } else if (refType->IsFunction()) {
        result = static_cast<EtsByte>(EtsTypeAPIKind::FUNCTIONAL);
    } else if (refType->IsInterface()) {
        result = static_cast<EtsByte>(EtsTypeAPIKind::INTERFACE);
    } else if (refType->IsArrayClass()) {
        result = static_cast<EtsByte>(EtsTypeAPIKind::ARRAY);
    } else if (refType->IsStringClass()) {
        result = static_cast<EtsByte>(EtsTypeAPIKind::STRING);
    } else if (refType->IsNullValue()) {
        result = static_cast<EtsByte>(EtsTypeAPIKind::NUL);
    } else {
        // NOTE(vpukhov): EtsTypeAPIKind:: UNION, TUPLE are not implemented
        ASSERT(refType->IsClass());
        result = static_cast<EtsByte>(EtsTypeAPIKind::CLASS);
    }
    return result;
}

static EtsByte GetValTypeKind(EtsValueTypeDesc td)
{
    EtsByte kind;
    switch (td) {
        case EtsValueTypeDesc::BOOLEAN:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::BOOLEAN) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::BYTE:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::BYTE) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::CHAR:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::CHAR) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::SHORT:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::SHORT) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::INT:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::INT) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::LONG:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::LONG) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::FLOAT:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::FLOAT) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        case EtsValueTypeDesc::DOUBLE:
            kind = static_cast<uint8_t>(EtsTypeAPIKind::DOUBLE) | static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK);
            break;
        default:
            kind = static_cast<EtsByte>(EtsTypeAPIKind::NONE);
    }
    return kind;
}

EtsClass *TypeAPIGetClass(EtsString *td, EtsRuntimeLinker *contextLinker)
{
    auto typeDesc = td->GetMutf8();
    auto *klass = PandaEtsVM::GetCurrent()->GetClassLinker()->GetClass(typeDesc.c_str(), true,
                                                                       contextLinker->GetClassLinkerContext());
    auto *coro = EtsCoroutine::GetCurrent();
    if (coro->HasPendingException()) {
        ASSERT(klass == nullptr);
        coro->ClearException();
    }
    // Note that Type API does not initialize the class
    return klass;
}

EtsByte TypeAPIGetTypeKind(EtsString *td, EtsRuntimeLinker *contextLinker)
{
    auto typeDesc = td->GetMutf8();
    if (typeDesc.empty()) {
        return static_cast<EtsByte>(EtsTypeAPIKind::NONE);
    }
    // Is Undefined?
    if (typeDesc == TYPE_API_UNDEFINED_TYPE_DESC) {
        return static_cast<EtsByte>(EtsTypeAPIKind::UNDEFINED);
    }
    // Is Function for methods, because currently there is no representation of them in runtime
    if (typeDesc[0] == METHOD_PREFIX) {
        return static_cast<EtsByte>(EtsTypeAPIKind::METHOD);
    }
    // Is machine-level void
    if (typeDesc[0] == VOID_PRIMITIVE_TYPE_DESC) {
        return static_cast<EtsByte>(EtsTypeAPIKind::VOID);
    }
    // Is RefType?
    if (typeDesc[0] == CLASS_TYPE_PREFIX || typeDesc[0] == ARRAY_TYPE_PREFIX) {
        auto *refType = TypeAPIGetClass(td, contextLinker);
        if (refType == nullptr) {
            return static_cast<EtsByte>(EtsTypeAPIKind::NONE);
        }
        return GetRefTypeKind(refType);
    }
    // Is ValueType?
    return GetValTypeKind(static_cast<EtsValueTypeDesc>(typeDesc[0]));
}

EtsBoolean TypeAPIIsValueType(EtsString *td, EtsRuntimeLinker *contextLinker)
{
    // NOTE(shumilov-petr): Add td is valid check
    return static_cast<EtsBoolean>(!((static_cast<uint8_t>(TypeAPIGetTypeKind(td, contextLinker)) &
                                      static_cast<uint8_t>(ETS_TYPE_KIND_VALUE_MASK)) == 0));
}

EtsString *TypeAPIGetNullTypeDescriptor()
{
    const auto *nullObject = EtsObject::FromCoreType(EtsCoroutine::GetCurrent()->GetNullValue());
    return EtsString::CreateFromMUtf8(nullObject->GetClass()->GetDescriptor());
}

EtsString *TypeAPIGetUndefinedTypeDescriptor()
{
    return EtsString::CreateFromMUtf8(TYPE_API_UNDEFINED_TYPE_DESC);
}

EtsInt TypeAPIGetClassAttributes(EtsClass *cls)
{
    uint32_t attrs = 0;
    attrs |= (cls->IsFinal()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::FINAL) : 0U;

    return static_cast<EtsInt>(attrs);
}

// Features
EtsLong TypeAPIGetFieldsNum(EtsClass *cls)
{
    return cls->GetFieldsNumber();
}

EtsLong TypeAPIGetOwnFieldsNum(EtsClass *cls)
{
    return cls->GetOwnFieldsNumber();
}

static EtsTypeAPIField *CreateField(const EtsClass *sourceClass, EtsField *field, EtsTypeAPIType *fieldType,
                                    EtsTypeAPIType *ownerType)
{
    ASSERT(sourceClass != nullptr);
    ASSERT(field != nullptr);
    ASSERT(fieldType != nullptr);
    ASSERT(ownerType != nullptr);
    ASSERT(ownerType->GetRuntimeTypeDescriptor()->GetMutf8() == field->GetDeclaringClass()->GetDescriptor());

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    EtsHandle fieldTypeHandle(coroutine, fieldType);
    EtsHandle ownerTypeHandle(coroutine, ownerType);

    // Make the instance of Type API Field
    EtsHandle<EtsTypeAPIField> typeapiField(coroutine, EtsTypeAPIField::Create(coroutine));

    // Set field's type, field's owner class type and name
    typeapiField->SetFieldType(fieldTypeHandle.GetPtr());
    typeapiField->SetOwnerType(ownerTypeHandle.GetPtr());
    auto name = field->GetNameString();
    typeapiField->SetName(name);

    // Set Access Modifier
    EtsTypeAPIAccessModifier accessMod;
    if (field->IsPublic()) {
        accessMod = EtsTypeAPIAccessModifier::PUBLIC;
    } else if (field->IsPrivate()) {
        accessMod = EtsTypeAPIAccessModifier::PRIVATE;
    } else {
        accessMod = EtsTypeAPIAccessModifier::PROTECTED;
    }
    typeapiField->SetAccessMod(accessMod);

    // Set specific attributes
    uint32_t attr = 0;
    attr |= (field->IsStatic()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::STATIC) : 0U;
    attr |= (!field->IsDeclaredIn(sourceClass)) ? static_cast<uint32_t>(EtsTypeAPIAttributes::INHERITED) : 0U;
    attr |= (field->IsReadonly()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::READONLY) : 0U;

    typeapiField->SetAttributes(attr);

    return typeapiField.GetPtr();
}

static ObjectHeader *GetFieldPtr(EtsField *field)
{
    return (field != nullptr)
               ? EtsBoxPrimitive<EtsLong>::Create(EtsCoroutine::GetCurrent(), reinterpret_cast<EtsLong>(field))
                     ->GetCoreType()
               : nullptr;
}

ObjectHeader *TypeAPIGetFieldPtr(EtsClass *cls, EtsLong idx)
{
    EtsField *field = cls->GetFieldByIndex(idx);
    return GetFieldPtr(field);
}

ObjectHeader *TypeAPIGetOwnFieldPtr(EtsClass *cls, EtsLong idx)
{
    if (idx < 0 || idx >= cls->GetOwnFieldsNumber()) {
        return nullptr;
    }
    EtsField *field = cls->GetOwnFieldByIndex(idx);
    return GetFieldPtr(field);
}

static EtsField *GetField(EtsClass *cls, EtsString *name)
{
    auto fieldName = name->GetMutf8();
    auto instanceField = cls->GetFieldIDByName(fieldName.c_str());
    if (instanceField != nullptr) {
        return instanceField;
    }
    auto staticField = cls->GetStaticFieldIDByName(fieldName.c_str());
    if (staticField != nullptr) {
        return staticField;
    }
    return nullptr;
}

ObjectHeader *TypeAPIGetFieldPtrByName(EtsClass *cls, EtsString *name)
{
    auto *field = GetField(cls, name);
    return GetFieldPtr(field);
}

EtsString *TypeAPIGetFieldDescriptor(EtsLong fieldPtr)
{
    auto *field = reinterpret_cast<EtsField *>(fieldPtr);
    ASSERT(field != nullptr);
    return EtsString::CreateFromMUtf8(field->GetTypeDescriptor());
}

EtsClass *TypeAPIGetFieldOwner(EtsLong fieldPtr)
{
    auto *field = reinterpret_cast<EtsField *>(fieldPtr);
    ASSERT(field != nullptr);
    return field->GetDeclaringClass();
}

EtsTypeAPIField *TypeAPIGetField(EtsClass *cls, EtsLong fieldPtr, EtsTypeAPIType *fieldType, EtsTypeAPIType *ownerType)
{
    auto *field = reinterpret_cast<EtsField *>(fieldPtr);
    ASSERT(field != nullptr);
    return CreateField(cls, field, fieldType, ownerType);
}

EtsObject *TypeAPIGetStaticFieldValue(EtsTypeAPIType *ownerType, EtsString *name)
{
    auto fieldName = name->GetMutf8();
    auto *ownerCls = TypeAPIGetClass(ownerType->GetRuntimeTypeDescriptor(), ownerType->GetContextLinker());
    ASSERT(ownerCls != nullptr);
    auto *field = ownerCls->GetStaticFieldIDByName(fieldName.c_str());
    ASSERT(field != nullptr && field->IsStatic());

    if (field->GetType()->IsPrimitive()) {
        return EtsPrimitiveTypeEnumToComptimeConstant(
            // CC-OFFNXT(G.FMT.14-CPP) project code style
            ConvertPandaTypeToEtsType(field->GetCoreType()->GetType()), [&](auto type) -> EtsObject * {
                using T = EtsTypeEnumToCppType<decltype(type)::value>;
                auto val = ownerCls->GetRuntimeClass()->GetFieldPrimitive<T>(*field->GetCoreType());
                // SUPPRESS_CSA_NEXTLINE(alpha.core.WasteObjHeader)
                return EtsBoxPrimitive<T>::Create(EtsCoroutine::GetCurrent(), val);
            });
    }

    return EtsObject::FromCoreType(ownerCls->GetRuntimeClass()->GetFieldObject(*field->GetCoreType()));
}

void TypeAPISetStaticFieldValue(EtsTypeAPIType *ownerType, EtsString *name, EtsObject *v)
{
    auto coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    EtsHandle valuePtr(coroutine, v);
    auto fieldName = name->GetMutf8();

    auto *ownerCls = TypeAPIGetClass(ownerType->GetRuntimeTypeDescriptor(), ownerType->GetContextLinker());
    ASSERT(ownerCls != nullptr);
    auto *field = ownerCls->GetStaticFieldIDByName(fieldName.c_str());
    ASSERT(field != nullptr && field->IsStatic());

    if (field->GetType()->IsPrimitive()) {
        EtsPrimitiveTypeEnumToComptimeConstant(
            ConvertPandaTypeToEtsType(field->GetCoreType()->GetType()), [&](auto type) {
                using T = EtsTypeEnumToCppType<decltype(type)::value>;
                // SUPPRESS_CSA_NEXTLINE(alpha.core.WasteObjHeader)
                ownerCls->GetRuntimeClass()->SetFieldPrimitive<T>(
                    *field->GetCoreType(), EtsBoxPrimitive<T>::FromCoreType(valuePtr.GetPtr())->GetValue());
            });
    } else {
        ownerCls->GetRuntimeClass()->SetFieldObject(*field->GetCoreType(), valuePtr->GetCoreType());
    }
}

EtsLong TypeAPIGetMethodsNum(EtsClass *cls)
{
    return cls->GetMethodsNum();
}

EtsLong TypeAPIGetConstructorsNum(EtsClass *cls)
{
    auto methods = cls->GetConstructors();
    return methods.size();
}

EtsString *TypeAPIGetMethodDescriptor(EtsClass *cls, EtsLong i)
{
    if (0 <= i && i < static_cast<EtsLong>(cls->GetMethodsNum())) {
        auto desc = cls->GetMethodByIndex(i)->GetDescriptor();
        return EtsString::CreateFromMUtf8(desc.c_str());
    }
    return nullptr;
}

EtsString *TypeAPIGetConstructorDescriptor(EtsClass *cls, EtsLong i)
{
    auto constructors = cls->GetConstructors();
    if (0 <= i && i < static_cast<EtsLong>(constructors.size())) {
        auto desc = constructors[i]->GetDescriptor();
        return EtsString::CreateFromMUtf8(desc.c_str());
    }
    return nullptr;
}

static EtsTypeAPIMethod *CreateMethodUnderHandleScope(EtsHandle<EtsTypeAPIType> &methodTypeHandle, EtsMethod *method,
                                                      const EtsClass *sourceClass)
{
    ASSERT(methodTypeHandle.GetPtr() != nullptr);
    ASSERT(method != nullptr);
    ASSERT(sourceClass != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    EtsHandle<EtsTypeAPIMethod> typeapiMethod(coroutine, EtsTypeAPIMethod::Create(coroutine));

    // Set Type
    typeapiMethod->SetMethodType(methodTypeHandle.GetPtr());
    EtsString *name;
    if (method->IsInstanceConstructor()) {
        name = EtsString::CreateFromMUtf8(CONSTRUCTOR_NAME);
    } else {
        name = method->GetNameString();
    }
    typeapiMethod.GetPtr()->SetName(name);

    // Set Access Modifier
    EtsTypeAPIAccessModifier accessMod;
    if (method->IsPublic()) {
        accessMod = EtsTypeAPIAccessModifier::PUBLIC;
    } else if (method->IsPrivate()) {
        accessMod = EtsTypeAPIAccessModifier::PRIVATE;
    } else {
        accessMod = EtsTypeAPIAccessModifier::PROTECTED;
    }
    typeapiMethod.GetPtr()->SetAccessMod(accessMod);

    // Set specific attributes
    uint32_t attr = 0;
    attr |= (method->IsStatic()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::STATIC) : 0U;
    attr |= (method->IsConstructor()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::CONSTRUCTOR) : 0U;
    attr |= (method->IsAbstract()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::ABSTRACT) : 0U;
    attr |= (!method->IsDeclaredIn(sourceClass)) ? static_cast<uint32_t>(EtsTypeAPIAttributes::INHERITED) : 0U;
    attr |= (method->IsGetter()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::GETTER) : 0U;
    attr |= (method->IsSetter()) ? static_cast<uint32_t>(EtsTypeAPIAttributes::SETTER) : 0U;

    typeapiMethod.GetPtr()->SetAttributes(attr);
    return typeapiMethod.GetPtr();
}

static EtsMethod *GetEtsMethod(EtsTypeAPIType *methodType)
{
    ASSERT(methodType != nullptr);
    auto desc = methodType->GetRuntimeTypeDescriptor()->GetMutf8();
    return EtsMethod::FromTypeDescriptor(desc, methodType->GetContextLinker());
}

EtsTypeAPIMethod *TypeAPIGetMethod(EtsClass *cls, ObjectHeader *methodTypeObj)
{
    auto *coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] EtsHandleScope scope(coro);
    EtsHandle<EtsTypeAPIType> methodTypeHandle(coro, EtsTypeAPIType::FromCoreType(methodTypeObj));

    auto *method = GetEtsMethod(methodTypeHandle.GetPtr());
    ASSERT(method != nullptr);
    return CreateMethodUnderHandleScope(methodTypeHandle, method, cls);
}

EtsLong TypeAPIGetInterfacesNum(EtsClass *cls)
{
    return cls->GetRuntimeClass()->GetInterfaces().size();
}

EtsClass *TypeAPIGetInterface(EtsClass *cls, EtsLong i)
{
    auto interfaces = cls->GetRuntimeClass()->GetInterfaces();
    if (0 <= i && i <= static_cast<EtsLong>(interfaces.size())) {
        return EtsClass::FromRuntimeClass(interfaces[i]);
    }
    return nullptr;
}

EtsInt TypeAPIGetFunctionAttributes([[maybe_unused]] EtsString *td)
{
    // NOTE(shumilov-petr): Not implemented
    return 0;
}

EtsString *TypeAPIGetArrayElementType(EtsClass *cls)
{
    // These checks must be done before intrinsic is called
    ASSERT(cls->IsArrayClass());
    auto *componentClass = cls->GetComponentType();
    ASSERT(componentClass != nullptr);
    return EtsString::CreateFromMUtf8(componentClass->GetDescriptor());
}

static EtsObject *MakeObjectsArray(EtsString *elemTd, EtsRuntimeLinker *contextLinker, EtsLong len)
{
    auto *coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] EtsHandleScope scope(coro);

    auto *elementClass = TypeAPIGetClass(elemTd, contextLinker);
    ASSERT(elementClass != nullptr);
    EtsHandle arrayHandle(coro, EtsObjectArray::Create(elementClass, len));
    for (EtsLong i = 0; i < len; ++i) {
        auto *element = elementClass->CreateInstance();
        if (element == nullptr) {
            ASSERT(coro->HasPendingException());
            return nullptr;
        }
        arrayHandle->Set(i, element);
    }
    return arrayHandle->AsObject();
}

EtsObject *TypeAPIMakeArrayInstance(EtsString *td, EtsRuntimeLinker *contextLinker, EtsLong len)
{
    auto typeDesc = td->GetMutf8();
    // This check is always done before intrinsic is called
    ASSERT(!typeDesc.empty());
    auto valType = ark::panda_file::Type::GetTypeIdBySignature(typeDesc[0]);
    switch (valType.GetId()) {
        case panda_file::Type::TypeId::U1:
            return EtsBooleanArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::I8:
            return EtsByteArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::U16:
            return EtsCharArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::I16:
            return EtsShortArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::I32:
            return EtsIntArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::I64:
            return EtsLongArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::F32:
            return EtsFloatArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::F64:
            return EtsDoubleArray::Create(len)->AsObject();
        case panda_file::Type::TypeId::REFERENCE:
            return MakeObjectsArray(td, contextLinker, len);
        default:
            UNREACHABLE();
    }
}

EtsBoolean TypeAPIIsInheritedFrom(EtsClass *lhs, EtsClass *rhs)
{
    return static_cast<EtsBoolean>(lhs->GetRuntimeClass()->Implements(rhs->GetRuntimeClass()));
}

EtsLong TypeAPIGetParametersNum(ObjectHeader *functionType)
{
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    ASSERT(function != nullptr);
    return function->GetParametersNum();
}

EtsString *TypeAPIGetParameterDescriptor(ObjectHeader *functionType, EtsLong i)
{
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    ASSERT(function != nullptr);

    if (i < 0 || i >= function->GetParametersNum()) {
        return nullptr;
    }
    // 0 is recevier type
    i = function->IsStatic() ? i : i + 1;
    const auto *desc = function->ResolveArgType(i)->GetDescriptor();
    return EtsString::CreateFromMUtf8(desc);
}

static EtsTypeAPIParameter *CreateParameterUnderHandleScope(EtsHandle<EtsTypeAPIType> &paramTypeHandle,
                                                            std::string_view name)
{
    ASSERT(paramTypeHandle.GetPtr() != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    EtsHandle<EtsTypeAPIParameter> typeapiParameter(coroutine, EtsTypeAPIParameter::Create(coroutine));

    // Set parameter's Type
    typeapiParameter->SetParameterType(paramTypeHandle.GetPtr());

    // NOTE(shumilov-petr): It's a temporary solution, extra type info dumping required
    EtsHandle<EtsString> pnameHandle(coroutine, EtsString::CreateFromUtf8(name.data(), name.size()));
    typeapiParameter.GetPtr()->SetName(pnameHandle.GetPtr());

    // Set specific attributes
    uint32_t attr = 0U;
    // NOTE(kirill-mitkin): Need to dump attributes of parameters from frontend to runtime
    typeapiParameter.GetPtr()->SetAttributes(attr);
    return typeapiParameter.GetPtr();
}

EtsTypeAPIParameter *TypeAPIGetParameter(ObjectHeader *functionType, EtsLong i, EtsTypeAPIType *paramType)
{
    auto *coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] EtsHandleScope scope(coro);
    EtsHandle<EtsTypeAPIType> paramTypeHandle(coro, paramType);

    // The resolved `function` might be used for attributes resolution in future implementation
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    ASSERT(function != nullptr);
    ASSERT(i >= 0 && i < function->GetNumArgs());
    // 0 is recevier type
    i = function->IsStatic() ? i : i + 1;
    return CreateParameterUnderHandleScope(paramTypeHandle, std::to_string(i));
}

EtsString *TypeAPIGetResultTypeDescriptor(ObjectHeader *functionType)
{
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    ASSERT(function != nullptr);
    const auto *type = function->GetReturnTypeDescriptor().data();
    return EtsString::CreateFromMUtf8(type);
}

EtsString *TypeAPIGetReceiverTypeDescriptor(ObjectHeader *functionType)
{
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    ASSERT(function != nullptr);
    if (function->IsStatic()) {
        return nullptr;
    }
    auto type = function->ResolveArgType(0);
    return EtsString::CreateFromMUtf8(type->GetDescriptor());
}

EtsClass *TypeAPIGetDeclaringClass(ObjectHeader *functionType)
{
    auto *function = GetEtsMethod(EtsTypeAPIType::FromCoreType(functionType));
    return (function != nullptr) ? function->GetClass() : nullptr;
}

}  // namespace ark::ets::intrinsics
