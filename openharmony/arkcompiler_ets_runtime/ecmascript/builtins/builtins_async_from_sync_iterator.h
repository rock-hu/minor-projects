/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_BUILTINS_BUILTINS_ASYNC_FROM_SYNC_ITERATOR_H
#define ECMASCRIPT_BUILTINS_BUILTINS_ASYNC_FROM_SYNC_ITERATOR_H

#include "ecmascript/base/builtins_base.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_object.h"
#include "ecmascript/js_tagged_value-inl.h"

namespace panda::ecmascript::builtins {
class BuiltinsAsyncFromSyncIterator : public base::BuiltinsBase {
public:
    static JSTaggedValue Next(EcmaRuntimeCallInfo *argv);
    static JSTaggedValue Throw(EcmaRuntimeCallInfo *argv);
    static JSTaggedValue Return(EcmaRuntimeCallInfo *argv);
};
}  // namespace panda::ecmascript::builtins
#endif  // ECMASCRIPT_BUILTINS_BUILTINS_ASYNC_FROM_SYNC_ITERATOR_H