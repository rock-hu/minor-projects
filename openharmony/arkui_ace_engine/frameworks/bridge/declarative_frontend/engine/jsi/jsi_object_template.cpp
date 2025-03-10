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

#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_object_template.h"

namespace OHOS::Ace::Framework {

JsiObjectTemplate::JsiObjectTemplate()
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    auto vm = runtime->GetEcmaVm();
    proto_ = panda::CopyableGlobal<panda::ObjectRef>(runtime->GetEcmaVm(), panda::ObjectRef::New(vm));
}

panda::Local<panda::ObjectRef> JsiObjectTemplate::operator*() const
{
    return proto_.ToLocal();
}

panda::Local<panda::ObjectRef> JsiObjectTemplate::NewInstance() const
{
    auto runtime = std::static_pointer_cast<ArkJSRuntime>(JsiDeclarativeEngineInstance::GetCurrentRuntime());
    return panda::ObjectRef::New(runtime->GetEcmaVm());
}

JSView* JsiObjectTemplate::GetNativeView(const panda::Local<panda::ObjectRef> obj, const EcmaVM* vm)
{
    auto jsObj = JSRef<JSObject>::Make(obj);
    if (jsObj->HasProperty("nativeViewPartialUpdate")) {
        JSRef<JSObject> nativeViewPartialUpdate = jsObj->GetProperty("nativeViewPartialUpdate");
        return nativeViewPartialUpdate->Unwrap<JSView>();
    }
    return static_cast<JSView*>(obj->GetNativePointerField(vm, 0));
}
} // namespace OHOS::Ace::Framework
