/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACE_FRAMEWORK_CJ_DATA_CHANGE_LISTENER_H
#define OHOS_ACE_FRAMEWORK_CJ_DATA_CHANGE_LISTENER_H

#include "ffi_remote_data.h"
#include "bridge/cj_frontend/interfaces/cj_ffi/cj_macro.h"

extern "C" {
struct CArrStr {
    char** head;
    int64_t size;
};

struct CJNativeOperation {
    char* type;
    int32_t count;
    int32_t index;
    int32_t coupleFromIndex;
    int32_t coupleToIndex;
    char* key;
    char* coupleFromKey;
    char* coupleToKey;
    CArrStr keyList;
};

struct NativeOperation {
    int32_t size;
    CJNativeOperation* cjNativeOperation;
};

CJ_EXPORT void FfiOHOSAceFrameworkDataChangeListenerOnDataReloaded(int64_t listenerId);
CJ_EXPORT void FfiOHOSAceFrameworkDataChangeListenerOnDataAdded(int64_t listenerId, int64_t index);
CJ_EXPORT void FfiOHOSAceFrameworkDataChangeListenerOnDataDeleted(int64_t listenerId, int64_t index);
CJ_EXPORT void FfiOHOSAceFrameworkDataChangeListenerOnDataChanged(int64_t listenerId, int64_t index);
CJ_EXPORT void FfiOHOSAceFrameworkDataChangeListenerOnDataMoved(int64_t listenerId, int64_t from, int64_t to);
CJ_EXPORT void FfiFrameworkDataChangeListenerOnDataSetChanged(int64_t listenerId, NativeOperation operation);
}

#endif // OHOS_ACE_FRAMEWORK_CJ_DATA_CHANGE_LISTENER_H
