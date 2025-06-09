/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <numeric>

#include "core/common/container.h"
#include "core/common/resource/resource_manager.h"
#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"

static thread_local std::vector<int32_t> restoreInstanceIds_;

namespace OHOS::Ace::NG::GeneratedModifier {
namespace SystemOpsAccessor {
Ark_NativePointer StartFrameImpl()
{
    return {};
}
void EndFrameImpl(Ark_NativePointer root)
{
    Converter::FC->Clear();
}
void SyncInstanceIdImpl(Ark_Int32 instanceId)
{
    restoreInstanceIds_.emplace_back(Container::CurrentId());
    ContainerScope::UpdateCurrent(instanceId);
}
void RestoreInstanceIdImpl()
{
    if (restoreInstanceIds_.empty()) {
        ContainerScope::UpdateCurrent(INSTANCE_ID_UNDEFINED);
        return;
    }
    ContainerScope::UpdateCurrent(restoreInstanceIds_.back());
    restoreInstanceIds_.pop_back();
}
Ark_Int32 GetResourceIdImpl(const Ark_String* bundleName,
                            const Ark_String* moduleName,
                            const Array_String* params)
{
    auto valueBundleName = Converter::Convert<std::string>(*bundleName);
    auto valueModuleName = Converter::Convert<std::string>(*moduleName);
    auto paramsStr = Converter::Convert<std::vector<std::string>>(*params);
    std::string resourceStr = "";
    if (paramsStr.size() > 0) {
        resourceStr = paramsStr[0];
    }
    auto resourceObject =
        AceType::MakeRefPtr<Ace::ResourceObject>(valueBundleName, valueModuleName, Container::CurrentIdSafely());
    auto resourceAdapter = ResourceManager::GetInstance().GetOrCreateResourceAdapter(resourceObject);
    CHECK_NULL_RETURN(resourceAdapter, -1);
    uint32_t resId = resourceAdapter->GetResId(resourceStr);
    return resId;
}
} // SystemOpsAccessor
const GENERATED_ArkUISystemOpsAccessor* GetSystemOpsAccessor()
{
    static const GENERATED_ArkUISystemOpsAccessor SystemOpsAccessorImpl {
        SystemOpsAccessor::StartFrameImpl,
        SystemOpsAccessor::EndFrameImpl,
        SystemOpsAccessor::SyncInstanceIdImpl,
        SystemOpsAccessor::RestoreInstanceIdImpl,
        SystemOpsAccessor::GetResourceIdImpl,
    };
    return &SystemOpsAccessorImpl;
}

}
