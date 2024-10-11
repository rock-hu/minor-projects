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
#ifndef PANDA_RUNTIME_VTABLE_BUILDER_VARIANCE_INL_H
#define PANDA_RUNTIME_VTABLE_BUILDER_VARIANCE_INL_H

#include "runtime/include/vtable_builder_variance.h"
#include "runtime/include/vtable_builder_base-inl.h"

namespace ark {

template <class ProtoCompatibility, class OverridePred>
bool VarianceVTableBuilder<ProtoCompatibility, OverridePred>::IsOverriddenBy(Method::ProtoId const &base,
                                                                             Method::ProtoId const &derv)
{
    if (&base.GetPandaFile() == &derv.GetPandaFile() && base.GetEntityId() == derv.GetEntityId()) {
        return true;
    }
    return ProtoCompatibility()(base, derv);
}

template <class ProtoCompatibility, class OverridePred>
bool VarianceVTableBuilder<ProtoCompatibility, OverridePred>::IsOverriddenOrOverrides(Method::ProtoId const &p1,
                                                                                      Method::ProtoId const &p2)
{
    if (&p1.GetPandaFile() == &p2.GetPandaFile() && p1.GetEntityId() == p2.GetEntityId()) {
        return true;
    }
    return ProtoCompatibility()(p1, p2) || ProtoCompatibility()(p2, p1);
}

template <class ProtoCompatibility, class OverridePred>
bool VarianceVTableBuilder<ProtoCompatibility, OverridePred>::ProcessClassMethod(const MethodInfo *info)
{
    bool compatibleFound = false;

    for (auto it = SameNameMethodInfoIterator(vtable_.Methods(), info); !it.IsEmpty(); it.Next()) {
        auto &[itInfo, itEntry] = it.Value();

        if (!itInfo->IsBase()) {
            continue;
        }
        if (IsOverriddenBy(itInfo->GetProtoId(), info->GetProtoId()) && OverridePred()(itInfo, info)) {
            if (UNLIKELY(itInfo->IsFinal())) {
                OnVTableConflict(errorHandler_, ClassLinker::Error::OVERRIDES_FINAL, "Method overrides final method",
                                 info, itInfo);
                return false;
            }
            if (UNLIKELY(itEntry.GetCandidate() != nullptr)) {
                OnVTableConflict(errorHandler_, ClassLinker::Error::MULTIPLE_OVERRIDE, "Multiple override", info,
                                 itInfo);
                return false;
            }
            itEntry.SetCandidate(info);
            compatibleFound = true;
        }
    }

    if (!compatibleFound) {
        vtable_.AddEntry(info);
    }
    return true;
}

template <class ProtoCompatibility, class OverridePred>
std::optional<MethodInfo const *>
VarianceVTableBuilder<ProtoCompatibility, OverridePred>::ScanConflictingDefaultMethods(const MethodInfo *info)
{
    // NOTE(vpukhov): test public flag
    for (auto it = SameNameMethodInfoIterator(vtable_.Methods(), info); !it.IsEmpty(); it.Next()) {
        MethodInfo const *itinfo = it.Value().second.CandidateOr(it.Value().first);
        if (itinfo->IsInterfaceMethod()) {
            continue;
        }
        if (IsOverriddenBy(info->GetProtoId(), itinfo->GetProtoId())) {
            return std::nullopt;
        }
    }

    for (auto it = SameNameMethodInfoIterator(vtable_.Methods(), info); !it.IsEmpty(); it.Next()) {
        MethodInfo const *itinfo = it.Value().second.CandidateOr(it.Value().first);
        if (!itinfo->IsInterfaceMethod()) {
            continue;
        }
        ASSERT(itinfo->GetMethod()->GetClass() != info->GetMethod()->GetClass());
        if (IsOverriddenOrOverrides(info->GetProtoId(), itinfo->GetProtoId())) {
            return itinfo;
        }
    }

    for (auto it = SameNameMethodInfoIterator(vtable_.CopiedMethods(), info); !it.IsEmpty(); it.Next()) {
        MethodInfo const *itinfo = it.Value().first;
        if (!itinfo->IsInterfaceMethod()) {
            continue;
        }
        if (itinfo->GetMethod()->GetClass() == info->GetMethod()->GetClass()) {
            continue;
        }
        if (IsOverriddenOrOverrides(info->GetProtoId(), itinfo->GetProtoId())) {
            return itinfo;
        }
    }
    return nullptr;
}

template <class ProtoCompatibility, class OverridePred>
bool VarianceVTableBuilder<ProtoCompatibility, OverridePred>::ProcessDefaultMethod(ITable itable, size_t itableIdx,
                                                                                   MethodInfo *methodInfo)
{
    auto skipOrConflict = ScanConflictingDefaultMethods(methodInfo);
    if (!skipOrConflict.has_value()) {
        return true;
    }
    MethodInfo const *conflict = skipOrConflict.value();
    [[maybe_unused]] Class *iface = itable[itableIdx].GetInterface();

    // if the default method is added for the first time, just add it.
    if (LIKELY(conflict == nullptr)) {
        vtable_.AddCopiedEntry(methodInfo);
        return true;
    }

    OnVTableConflict(errorHandler_, ClassLinker::Error::MULTIPLE_IMPLEMENT, "Conflicting default implementations",
                     conflict, methodInfo);
    return false;
}

}  // namespace ark

#endif  // PANDA_RUNTIME_VTABLE_BUILDER_VARIANCE_INL_H
