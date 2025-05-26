/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and limitations under the
 * License.
 */

#include <cassert>
#include <cstdint>
#include "libabckit/include/c/abckit.h"
#include "libabckit/include/c/metadata_core.h"

#include "libabckit/include/c/statuses.h"
#include "libabckit/src/macros.h"
#include "scoped_timer.h"

#include "libabckit/src/metadata_inspect_impl.h"
#include "libabckit/src/ir_impl.h"
#include "libabckit/src/adapter_dynamic/metadata_modify_dynamic.h"
#include "libabckit/src/adapter_static/metadata_modify_static.h"
#include "libabckit/src/helpers_common.h"
#include "libabckit/src/statuses_impl.h"

namespace libabckit {

// ========================================
// File
// ========================================

// ========================================
// Module
// ========================================

// ========================================
// Class
// ========================================

// ========================================
// AnnotationInterface
// ========================================

// ========================================
// Function
// ========================================

extern "C" void FunctionSetGraph(AbckitCoreFunction *function, AbckitGraph *graph)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT_VOID(function)
    LIBABCKIT_BAD_ARGUMENT_VOID(graph)

    LIBABCKIT_WRONG_CTX_VOID(function->owningModule->file, graph->file);

    if (IsDynamic(function->owningModule->target)) {
        FunctionSetGraphDynamic(function, graph);
    } else {
        FunctionSetGraphStatic(function, graph);
    }
}

// ========================================
// Annotation
// ========================================

// ========================================
// Type
// ========================================

extern "C" AbckitType *CreateType(AbckitFile *file, AbckitTypeId id)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    if (id == ABCKIT_TYPE_ID_INVALID) {
        statuses::SetLastError(ABCKIT_STATUS_BAD_ARGUMENT);
        return nullptr;
    }
    return GetOrCreateType(file, id, 0, nullptr);
}

extern "C" AbckitType *CreateReferenceType(AbckitFile *file, AbckitCoreClass *klass)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(klass, nullptr);
    return GetOrCreateType(file, AbckitTypeId::ABCKIT_TYPE_ID_REFERENCE, 0, klass);
}

// ========================================
// Value
// ========================================

extern "C" AbckitValue *CreateValueU1(AbckitFile *file, bool value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateValueU1Dynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateValueU1Static(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitValue *CreateValueDouble(AbckitFile *file, double value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateValueDoubleDynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateValueDoubleStatic(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitValue *CreateValueString(AbckitFile *file, const char *value, size_t len)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(value, nullptr);
    LIBABCKIT_ZERO_ARGUMENT(len, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateValueStringDynamic(file, value, len);
        case Mode::STATIC:
            return FindOrCreateValueStringStatic(file, value, len);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitValue *CreateLiteralArrayValue(AbckitFile *file, AbckitValue **value, size_t size)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(value, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralArrayValueDynamic(file, value, size);
        case Mode::STATIC:
            statuses::SetLastError(ABCKIT_STATUS_UNSUPPORTED);
            return nullptr;
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

// ========================================
// String
// ========================================

extern "C" AbckitString *CreateString(AbckitFile *file, const char *value, size_t len)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(value, nullptr);
    LIBABCKIT_ZERO_ARGUMENT(len, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return CreateStringDynamic(file, value, len);
        case Mode::STATIC:
            return CreateStringStatic(file, value, len);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

// ========================================
// LiteralArray
// ========================================

extern "C" AbckitLiteralArray *CreateLiteralArray(AbckitFile *file, AbckitLiteral **value, size_t size)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(value, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return CreateLiteralArrayDynamic(file, value, size);
        case Mode::STATIC:
            return CreateLiteralArrayStatic(file, value, size);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralBool(AbckitFile *file, bool value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralBoolDynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralBoolStatic(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralU8(AbckitFile *file, uint8_t value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralU8Dynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralU8Static(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralU16(AbckitFile *file, uint16_t value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralU16Dynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralU16Static(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralMethodAffiliate(AbckitFile *file, uint16_t value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralMethodAffiliateDynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralMethodAffiliateStatic(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}
extern "C" AbckitLiteral *CreateLiteralU32(AbckitFile *file, uint32_t value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralU32Dynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralU32Static(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralU64(AbckitFile *file, uint64_t value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralU64Dynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralU64Static(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralFloat(AbckitFile *file, float value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralFloatDynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralFloatStatic(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralDouble(AbckitFile *file, double value)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralDoubleDynamic(file, value);
        case Mode::STATIC:
            return FindOrCreateLiteralDoubleStatic(file, value);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralLiteralArray(AbckitFile *file, AbckitLiteralArray *litarr)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(litarr, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralLiteralArrayDynamic(file, litarr);
        case Mode::STATIC:
            statuses::SetLastError(ABCKIT_STATUS_UNSUPPORTED);
            return nullptr;
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralString(AbckitFile *file, const char *value, size_t len)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(value, nullptr);
    LIBABCKIT_ZERO_ARGUMENT(len, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralStringDynamic(file, value, len);
        case Mode::STATIC:
            return FindOrCreateLiteralStringStatic(file, value, len);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

extern "C" AbckitLiteral *CreateLiteralMethod(AbckitFile *file, AbckitCoreFunction *function)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(file, nullptr);
    LIBABCKIT_BAD_ARGUMENT(function, nullptr);
    switch (file->frontend) {
        case Mode::DYNAMIC:
            return FindOrCreateLiteralMethodDynamic(file, function);
        case Mode::STATIC:
            return FindOrCreateLiteralMethodStatic(file, function);
        default:
            LIBABCKIT_UNREACHABLE;
    }
}

AbckitModifyApi g_modifyApiImpl = {

    // ========================================
    // File
    // ========================================

    // ========================================
    // Module
    // ========================================

    // ========================================
    // Class
    // ========================================

    // ========================================
    // AnnotationInterface
    // ========================================

    // ========================================
    // Function
    // ========================================

    FunctionSetGraph,

    // ========================================
    // Annotation
    // ========================================

    // ========================================
    // Type
    // ========================================

    CreateType,
    CreateReferenceType,

    // ========================================
    // Value
    // ========================================

    CreateValueU1,
    CreateValueDouble,
    CreateValueString,
    CreateLiteralArrayValue,

    // ========================================
    // String
    // ========================================

    CreateString,

    // ========================================
    // LiteralArray
    // ========================================

    CreateLiteralArray,

    // ========================================
    // LiteralArray
    // ========================================

    CreateLiteralBool,
    CreateLiteralU8,
    CreateLiteralU16,
    CreateLiteralMethodAffiliate,
    CreateLiteralU32,
    CreateLiteralU64,
    CreateLiteralFloat,
    CreateLiteralDouble,
    CreateLiteralLiteralArray,
    CreateLiteralString,
    CreateLiteralMethod,
};

}  // namespace libabckit

#ifdef ABCKIT_ENABLE_MOCK_IMPLEMENTATION
#include "./mock/abckit_mock.h"
#endif

extern "C" AbckitModifyApi const *AbckitGetModifyApiImpl(AbckitApiVersion version)
{
#ifdef ABCKIT_ENABLE_MOCK_IMPLEMENTATION
    return AbckitGetMockModifyApiImpl(version);
#endif
    switch (version) {
        case ABCKIT_VERSION_RELEASE_1_0_0:
            return &libabckit::g_modifyApiImpl;
        default:
            libabckit::statuses::SetLastError(ABCKIT_STATUS_UNKNOWN_API_VERSION);
            return nullptr;
    }
}
