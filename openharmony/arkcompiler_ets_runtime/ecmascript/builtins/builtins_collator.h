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

#ifndef ECMASCRIPT_BUILTINS_BUILTINS_COLLATOR_H
#define ECMASCRIPT_BUILTINS_BUILTINS_COLLATOR_H

#include "ecmascript/base/builtins_base.h"

namespace panda::ecmascript::builtins {
class BuiltinsCollator : public base::BuiltinsBase {
public:
    // 11.1.2 Intl.Collator ( [ locales [ , options ] ] )
    static JSTaggedValue CollatorConstructor(EcmaRuntimeCallInfo *argv);

    // 11.2.2 Intl.Collator.supportedLocalesOf ( locales [ , options ] )
    static JSTaggedValue SupportedLocalesOf(EcmaRuntimeCallInfo *argv);

    // 11.3.3  get Intl.Collator.prototype.compare
    static JSTaggedValue Compare(EcmaRuntimeCallInfo *argv);

    // 11.3.4 Intl.Collator.prototype.resolvedOptions ()
    static JSTaggedValue ResolvedOptions(EcmaRuntimeCallInfo *argv);

private:
    friend class panda::ecmascript::EcmaVM;
    static JSTaggedValue AnonymousCollator(EcmaRuntimeCallInfo *argv);
};
}  // namespace panda::ecmascript::builtins
#endif  // ECMASCRIPT_BUILTINS_BUILTINS_COLLATOR_H