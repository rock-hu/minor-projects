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

#include "arkoala_api_generated.h"
#include "core/components_ng/pattern/form_link/form_link_model_ng.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "core/interfaces/native/utility/converter.h"

namespace OHOS::Ace::NG {
namespace {
struct FormLinkOptions {
    std::string action;
    std::optional<std::string> moduleName;
    std::optional<std::string> bundleName;
    std::optional<std::string> abilityName;
    std::optional<std::string> uri;
};
}


namespace Converter {
template<>
FormLinkOptions Convert(const Ark_FormLinkOptions& src)
{
    return {
        .action = Convert<std::string>(src.action),
        .moduleName = Converter::OptConvert<std::string>(src.moduleName),
        .bundleName = Converter::OptConvert<std::string>(src.bundleName),
        .abilityName = Converter::OptConvert<std::string>(src.abilityName),
        .uri = Converter::OptConvert<std::string>(src.uri)
    };
}
} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace FormLinkModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    // auto frameNode = FormLinkModelNG::CreateFrameNode(id);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}
} // FormLinkModifier
namespace FormLinkInterfaceModifier {
void SetFormLinkOptionsImpl(Ark_NativePointer node,
                            const Ark_FormLinkOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    FormLinkOptions formLinkOptions = Converter::Convert<FormLinkOptions>(*options);
    // FormLinkModelNG::SetAction(frameNode, formLinkOptions.action);
}
} // FormLinkInterfaceModifier
const GENERATED_ArkUIFormLinkModifier* GetFormLinkModifier()
{
    static const GENERATED_ArkUIFormLinkModifier ArkUIFormLinkModifierImpl {
        FormLinkModifier::ConstructImpl,
        FormLinkInterfaceModifier::SetFormLinkOptionsImpl,
    };
    return &ArkUIFormLinkModifierImpl;
}

}
