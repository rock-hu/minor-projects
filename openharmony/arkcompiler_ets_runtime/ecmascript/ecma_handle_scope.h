/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_HANDLE_SCOPE_H
#define ECMASCRIPT_HANDLE_SCOPE_H

#include "ecmascript/js_tagged_value.h"

namespace panda::ecmascript {
class JSThread;
/*
 * Handles are only valid within a HandleScope. When a handle is created for an object a cell is allocated in the
 * current HandleScope.
 */

class EcmaHandleScope {
public:
    explicit PUBLIC_API EcmaHandleScope(JSThread *thread);

    PUBLIC_API ~EcmaHandleScope();
    static uintptr_t PUBLIC_API NewHandle(JSThread *thread, JSTaggedType value);
    static uintptr_t PUBLIC_API NewPrimitiveHandle(JSThread *thread, JSTaggedType value);
    void OpenHandleScope(EcmaVM *vm);
    void OpenPrimitiveScope(EcmaVM *vm);
    void CloseHandleScope(EcmaVM *vm);
    void ClosePrimitiveScope(EcmaVM *vm);

    JSThread *GetThread() const
    {
        return thread_;
    }

private:
    JSThread *thread_;
    JSTaggedType *prevNext_;
    JSTaggedType *prevEnd_;
    int prevHandleStorageIndex_ {-1};
    JSTaggedType *prevPrimitiveNext_;
    JSTaggedType *prevPrimitiveEnd_;
    int prevPrimitiveStorageIndex_ {-1};

    NO_COPY_SEMANTIC(EcmaHandleScope);
    NO_MOVE_SEMANTIC(EcmaHandleScope);
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_HANDLE_SCOPE_H
