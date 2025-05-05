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

#ifndef FOUNDATION_ACE_NAPI_REFERENCE_MANAGER_NATIVE_REFERENCE_MANAGER_H
#define FOUNDATION_ACE_NAPI_REFERENCE_MANAGER_NATIVE_REFERENCE_MANAGER_H

#include "native_engine/native_reference.h"
#include "utils/macros.h"

class NAPI_EXPORT NativeReferenceManager {
public:
    NativeReferenceManager() = default;
    virtual ~NativeReferenceManager();

    void CreateHandler(NativeReference* reference);
    void ReleaseHandler(NativeReference* reference);

private:
    NativeReference *references_ {nullptr};
};
#endif /* FOUNDATION_ACE_NAPI_REFERENCE_MANAGER_NATIVE_REFERENCE_MANAGER_H */
