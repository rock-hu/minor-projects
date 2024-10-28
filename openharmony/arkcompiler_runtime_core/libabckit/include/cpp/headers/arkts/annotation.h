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

#ifndef CPP_ABCKIT_ARKTS_ANNOTATION_H
#define CPP_ABCKIT_ARKTS_ANNOTATION_H

#include "libabckit/include/c/abckit.h"
#include "cpp/headers/declarations.h"
#include "cpp/headers/config.h"
#include "cpp/headers/base_classes.h"
#include "cpp/headers/value.h"
#include "cpp/headers/core/annotation.h"
#include "cpp/headers/core/annotation_element.h"
#include "cpp/headers/arkts/annotation_element.h"
#include "metadata_inspect_impl.h"

#include <string_view>

namespace abckit::arkts {

class Annotation : public core::Annotation {
    // To access private constructor.
    // We restrict constructors in order to prevent C/C++ API mix-up by user.
    friend class arkts::Class;
    friend class arkts::Function;

public:
    Annotation(const Annotation &other) = default;
    Annotation &operator=(const Annotation &other) = default;
    Annotation(Annotation &&other) = default;
    Annotation &operator=(Annotation &&other) = default;

    // CC-OFFNXT(G.FMT.02) project code style
    explicit Annotation(const core::Annotation &coreOther) : core::Annotation(coreOther) {};

    ~Annotation() override = default;

    arkts::Annotation &AddElement(const abckit::Value &val, const std::string &name);
    arkts::AnnotationElement AddAndGetElement(const abckit::Value &val, const std::string_view name);
    AbckitCoreAnnotationElement *AddAndGetElementImpl(AbckitArktsAnnotationElementCreateParams *params);

    // Other API.
    // ...
};

}  // namespace abckit::arkts

#endif  // CPP_ABCKIT_ARKTS_ANNOTATION_H
