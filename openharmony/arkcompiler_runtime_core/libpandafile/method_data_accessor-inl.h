/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef LIBPANDAFILE_METHOD_DATA_ACCESSOR_INL_H
#define LIBPANDAFILE_METHOD_DATA_ACCESSOR_INL_H

#include "helpers.h"
#include "method_data_accessor.h"
#include "annotation_data_accessor.h"
#include "proto_data_accessor.h"

namespace panda::panda_file {

// static
inline File::EntityId MethodDataAccessor::GetNameId(const File &panda_file, File::EntityId method_id)
{
    constexpr size_t SKIP_NUM = 2;  // skip class_idx and proto_idx
    auto sp = panda_file.GetSpanFromId(method_id).SubSpan(IDX_SIZE * SKIP_NUM);
    return File::EntityId(helpers::Read<ID_SIZE>(&sp));
}

// static
inline panda_file::File::StringData MethodDataAccessor::GetName(const File &panda_file, File::EntityId method_id)
{
    return panda_file.GetStringData(GetNameId(panda_file, method_id));
}

// static
inline File::EntityId MethodDataAccessor::GetProtoId(const File &panda_file, File::EntityId method_id)
{
    constexpr size_t SKIP_NUM = 1;  // skip class_idx
    auto sp = panda_file.GetSpanFromId(method_id).SubSpan(IDX_SIZE * SKIP_NUM);
    auto proto_idx_ = helpers::Read<IDX_SIZE>(&sp);
    return File::EntityId(panda_file.ResolveProtoIndex(method_id, proto_idx_).GetOffset());
}

// static
inline File::EntityId MethodDataAccessor::GetClassId(const File &panda_file, File::EntityId method_id)
{
    auto sp = panda_file.GetSpanFromId(method_id);
    auto class_idx = helpers::Read<IDX_SIZE>(&sp);
    return File::EntityId(panda_file.ResolveClassIndex(method_id, class_idx).GetOffset());
}

inline panda_file::File::StringData MethodDataAccessor::GetName() const
{
    return panda_file_.GetStringData(GetNameId());
}

inline void MethodDataAccessor::SkipCode()
{
    GetCodeId();
}

inline void MethodDataAccessor::SkipSourceLang()
{
    GetSourceLang();
}

inline void MethodDataAccessor::SkipRuntimeAnnotations()
{
    EnumerateRuntimeAnnotations([](File::EntityId /* unused */) {});
}

inline void MethodDataAccessor::SkipRuntimeParamAnnotation()
{
    GetRuntimeParamAnnotationId();
}

inline void MethodDataAccessor::SkipDebugInfo()
{
    GetDebugInfoId();
}

inline void MethodDataAccessor::SkipAnnotations()
{
    EnumerateAnnotations([](File::EntityId /* unused */) {});
}

inline void MethodDataAccessor::SkipParamAnnotation()
{
    GetParamAnnotationId();
}

inline void MethodDataAccessor::SkipTypeAnnotation()
{
    EnumerateTypeAnnotations([](File::EntityId /* unused */) {});
}

inline void MethodDataAccessor::SkipRuntimeTypeAnnotation()
{
    EnumerateRuntimeTypeAnnotations([](File::EntityId /* unused */) {});
}

inline std::optional<File::EntityId> MethodDataAccessor::GetCodeId()
{
    if (is_external_) {
        // NB! This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80635
        // which fails Release builds for GCC 8 and 9.
        std::optional<File::EntityId> novalue;
        return novalue;
    }

    return helpers::GetOptionalTaggedValue<File::EntityId>(tagged_values_sp_, MethodTag::CODE, &source_lang_sp_);
}

inline std::optional<SourceLang> MethodDataAccessor::GetSourceLang()
{
    if (is_external_) {
        // NB! This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80635
        // which fails Release builds for GCC 8 and 9.
        std::optional<SourceLang> novalue;
        return novalue;
    }

    if (source_lang_sp_.data() == nullptr) {
        SkipCode();
    }

    return helpers::GetOptionalTaggedValue<SourceLang>(source_lang_sp_, MethodTag::SOURCE_LANG,
                                                       &runtime_annotations_sp_);
}

template <class Callback>
inline void MethodDataAccessor::EnumerateRuntimeAnnotations(Callback cb)
{
    if (is_external_) {
        return;
    }

    if (runtime_annotations_sp_.data() == nullptr) {
        SkipSourceLang();
    }

    helpers::EnumerateTaggedValues<File::EntityId, MethodTag, Callback>(
        runtime_annotations_sp_, MethodTag::RUNTIME_ANNOTATION, cb, &runtime_param_annotation_sp_);
}

inline std::optional<File::EntityId> MethodDataAccessor::GetRuntimeParamAnnotationId()
{
    if (is_external_) {
        return {};
    }

    if (runtime_param_annotation_sp_.data() == nullptr) {
        SkipRuntimeAnnotations();
    }

    return helpers::GetOptionalTaggedValue<File::EntityId>(runtime_param_annotation_sp_,
                                                           MethodTag::RUNTIME_PARAM_ANNOTATION, &debug_sp_);
}

inline std::optional<File::EntityId> MethodDataAccessor::GetDebugInfoId()
{
    if (is_external_) {
        return {};
    }

    if (debug_sp_.data() == nullptr) {
        SkipRuntimeParamAnnotation();
    }

    return helpers::GetOptionalTaggedValue<File::EntityId>(debug_sp_, MethodTag::DEBUG_INFO, &annotations_sp_);
}

template <class Callback>
inline void MethodDataAccessor::EnumerateAnnotations(Callback cb)
{
    if (is_external_) {
        return;
    }

    if (annotations_sp_.data() == nullptr) {
        SkipDebugInfo();
    }

    helpers::EnumerateTaggedValues<File::EntityId, MethodTag, Callback>(annotations_sp_, MethodTag::ANNOTATION, cb,
                                                                        &param_annotation_sp_);
}

template <class Callback>
inline bool MethodDataAccessor::EnumerateRuntimeAnnotationsWithEarlyStop(Callback cb)
{
    if (is_external_) {
        return false;
    }

    if (runtime_annotations_sp_.data() == nullptr) {
        SkipSourceLang();
    }

    return helpers::EnumerateTaggedValuesWithEarlyStop<File::EntityId, MethodTag, Callback>(
        runtime_annotations_sp_, MethodTag::RUNTIME_ANNOTATION, cb);
}

template <class Callback>
inline bool MethodDataAccessor::EnumerateAnnotationsWithEarlyStop(Callback cb)
{
    if (is_external_) {
        return false;
    }

    if (annotations_sp_.data() == nullptr) {
        SkipDebugInfo();
    }

    return helpers::EnumerateTaggedValuesWithEarlyStop<File::EntityId, MethodTag, Callback>(annotations_sp_,
                                                                                            MethodTag::ANNOTATION, cb);
}

template <class Callback>
inline void MethodDataAccessor::EnumerateTypeAnnotations(Callback cb)
{
    if (is_external_) {
        return;
    }

    if (type_annotation_sp_.data() == nullptr) {
        SkipParamAnnotation();
    }

    helpers::EnumerateTaggedValues<File::EntityId, MethodTag, Callback>(type_annotation_sp_, MethodTag::TYPE_ANNOTATION,
                                                                        cb, &runtime_type_annotation_sp_);
}

template <class Callback>
inline void MethodDataAccessor::EnumerateRuntimeTypeAnnotations(Callback cb)
{
    if (is_external_) {
        return;
    }

    if (runtime_type_annotation_sp_.data() == nullptr) {
        SkipTypeAnnotation();
    }

    Span<const uint8_t> sp {nullptr, nullptr};
    size_ = panda_file_.GetIdFromPointer(runtime_type_annotation_sp_.data()).GetOffset() - method_id_.GetOffset() +
            1;  // + 1 for NOTHING tag
    helpers::EnumerateTaggedValues<File::EntityId, MethodTag, Callback>(runtime_type_annotation_sp_,
                                                                        MethodTag::RUNTIME_TYPE_ANNOTATION, cb, &sp);
}

inline std::optional<File::EntityId> MethodDataAccessor::GetParamAnnotationId()
{
    if (is_external_) {
        // NB! This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80635
        // which fails Release builds for GCC 8 and 9.
        std::optional<File::EntityId> novalue;
        return novalue;
    }

    if (param_annotation_sp_.data() == nullptr) {
        SkipAnnotations();
    }

    auto v = helpers::GetOptionalTaggedValue<File::EntityId>(param_annotation_sp_, MethodTag::PARAM_ANNOTATION,
                                                             &type_annotation_sp_);

    return v;
}

inline uint32_t MethodDataAccessor::GetAnnotationsNumber()
{
    size_t n = 0;
    EnumerateRuntimeAnnotations([&n](File::EntityId /* unused */) { n++; });
    return n;
}

inline uint32_t MethodDataAccessor::GetRuntimeAnnotationsNumber()
{
    size_t n = 0;
    EnumerateRuntimeAnnotations([&n](File::EntityId /* unused */) { n++; });
    return n;
}

inline uint32_t MethodDataAccessor::GetTypeAnnotationsNumber()
{
    size_t n = 0;
    EnumerateTypeAnnotations([&n](File::EntityId /* unused */) { n++; });
    return n;
}

inline uint32_t MethodDataAccessor::GetRuntimeTypeAnnotationsNumber()
{
    size_t n = 0;
    EnumerateRuntimeTypeAnnotations([&n](File::EntityId /* unused */) { n++; });
    return n;
}

template <typename Callback>
void MethodDataAccessor::EnumerateTypesInProto(Callback cb, bool skip_this)
{
    size_t ref_idx = 0;
    panda_file::ProtoDataAccessor pda(GetPandaFile(), GetProtoId());

    auto type = pda.GetReturnType();
    panda_file::File::EntityId class_id;

    if (!type.IsPrimitive()) {
        class_id = pda.GetReferenceType(ref_idx++);
    }

    cb(type, class_id);

    if (!IsStatic() && !skip_this) {
        // first arg type is method class
        cb(panda_file::Type {panda_file::Type::TypeId::REFERENCE}, GetClassId());
    }

    for (uint32_t idx = 0; idx < pda.GetNumArgs(); ++idx) {
        auto arg_type = pda.GetArgType(idx);
        panda_file::File::EntityId klass_id;
        if (!arg_type.IsPrimitive()) {
            klass_id = pda.GetReferenceType(ref_idx++);
        }
        cb(arg_type, klass_id);
    }
}

inline uint32_t MethodDataAccessor::GetNumericalAnnotation(uint32_t field_id)
{
    static constexpr uint32_t NUM_ELEMENT = 3;
    static std::array<const char *, NUM_ELEMENT> elem_name_table = {"icSize", "parameterLength", "funcName"};
    uint32_t result = 0;
    EnumerateAnnotations([&](File::EntityId annotation_id) {
        AnnotationDataAccessor ada(panda_file_, annotation_id);
        auto *annotation_name = reinterpret_cast<const char *>(panda_file_.GetStringData(ada.GetClassId()).data);
        if (::strcmp("L_ESAnnotation;", annotation_name) == 0) {
            uint32_t elem_count = ada.GetCount();
            for (uint32_t i = 0; i < elem_count; i++) {
                AnnotationDataAccessor::Elem adae = ada.GetElement(i);
                auto *elem_name = reinterpret_cast<const char *>(panda_file_.GetStringData(adae.GetNameId()).data);
                if (::strcmp(elem_name_table[field_id], elem_name) == 0) {
                    result = adae.GetScalarValue().GetValue();
                }
            }
        }
    });
    return result;
}

}  // namespace panda::panda_file

#endif  // LIBPANDAFILE_METHOD_DATA_ACCESSOR_INL_H
