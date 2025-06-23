/*
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

#ifndef CPP_ABCKIT_ARKTS_EXPORT_DESCRIPTOR_IMPL_H
#define CPP_ABCKIT_ARKTS_EXPORT_DESCRIPTOR_IMPL_H

#include "export_descriptor.h"

namespace abckit::arkts {

inline AbckitArktsExportDescriptor *ExportDescriptor::TargetCast() const
{
    auto ret = GetApiConfig()->cArktsIapi_->coreExportDescriptorToArktsExportDescriptor(GetView());
    CheckError(GetApiConfig());
    return ret;
}

inline ExportDescriptor::ExportDescriptor(const core::ExportDescriptor &coreOther)
    : core::ExportDescriptor(coreOther), targetChecker_(this)
{
}

}  // namespace abckit::arkts

#endif  // CPP_ABCKIT_ARKTS_EXPORT_DESCRIPTOR_IMPL_H
