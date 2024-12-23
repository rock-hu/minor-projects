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

#ifndef CPP_ABCKIT_JS_EXPORT_DESCRIPTOR_H
#define CPP_ABCKIT_JS_EXPORT_DESCRIPTOR_H

#include "../core/export_descriptor.h"
#include "../base_concepts.h"

namespace abckit::js {

/**
 * @brief ExportDescriptor
 */
class ExportDescriptor final : public core::ExportDescriptor {
    // We restrict constructors in order to prevent C/C++ API mix-up by user.
    /// @brief to access private constructor
    friend class js::Module;
    /// @brief abckit::DefaultHash<ExportDescriptor>
    friend class abckit::DefaultHash<ExportDescriptor>;
    /// @brief to access private TargetCast
    friend class abckit::traits::TargetCheckCast<ExportDescriptor>;

public:
    /**
     * @brief Constructor Js API ExportDescriptor from the Core API with compatibility check
     * @param coreOther - Core API ExportDescriptor
     */
    explicit ExportDescriptor(const core::ExportDescriptor &coreOther);

    /**
     * @brief Construct a new Export Descriptor object
     * @param other
     */
    ExportDescriptor(const ExportDescriptor &other) = default;

    /**
     * @brief Constructor
     * @param other
     * @return ExportDescriptor&
     */
    ExportDescriptor &operator=(const ExportDescriptor &other) = default;

    /**
     * @brief Construct a new Export Descriptor object
     * @param other
     */
    ExportDescriptor(ExportDescriptor &&other) = default;

    /**
     * @brief Constructor
     * @param other
     * @return ExportDescriptor&
     */
    ExportDescriptor &operator=(ExportDescriptor &&other) = default;

    /**
     * @brief Destroy the Export Descriptor object
     */
    ~ExportDescriptor() override = default;
    // Other API.
    // ...

private:
    /**
     * @brief Converts export descriptor from Core to Js target
     * @return AbckitJsExportDescriptor* - converted export descriptor
     * @note Set `ABCKIT_STATUS_WRONG_TARGET` error if `this` is does not have `ABCKIT_TARGET_ARK_TS_V1` or
     * `ABCKIT_TARGET_ARK_TS_V2` target.
     */
    AbckitJsExportDescriptor *TargetCast() const;

    ABCKIT_NO_UNIQUE_ADDRESS traits::TargetCheckCast<ExportDescriptor> targetChecker_;
};

}  // namespace abckit::js

#endif  // CPP_ABCKIT_JS_EXPORT_DESCRIPTOR_H
