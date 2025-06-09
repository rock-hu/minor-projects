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

#include "core/interfaces/native/utility/converter.h"
#include "arkoala_api_generated.h"

#include "core/components_ng/pattern/shape/shape_abstract_model_ng.h"
#include "core/components_ng/pattern/shape/path_model_ng.h"
#include "core/interfaces/native/generated/interface/node_api.h"


namespace OHOS::Ace::NG {
namespace {
struct PathOptions {
    std::optional<Dimension> width;
    std::optional<Dimension> height;
    std::optional<std::string> commands;
};
}

namespace Converter {
template<>
PathOptions Convert(const Ark_PathOptions& src)
{
    return {
        .width = Converter::OptConvert<Dimension>(src.width),
        .height = Converter::OptConvert<Dimension>(src.height),
        .commands = OptConvert<std::string>(src.commands),
    };
}
} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace PathModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    // auto frameNode = PathModelNG::CreateFrameNode(id);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}
} // PathModifier
namespace PathInterfaceModifier {
void SetPathOptionsImpl(Ark_NativePointer node,
                        const Opt_PathOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    auto opt = Converter::OptConvert<PathOptions>(*options);
    if (opt && opt->width) {
        ShapeAbstractModelNG::SetWidth(frameNode, *(opt->width));
    }
    if (opt && opt->height) {
        ShapeAbstractModelNG::SetHeight(frameNode, *(opt->height));
    }
    if (opt && opt->commands) {
        PathModelNG::SetCommands(frameNode, *(opt->commands));
    }
}
} // PathInterfaceModifier
namespace PathAttributeModifier {
void CommandsImpl(Ark_NativePointer node,
                  const Ark_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    PathModelNG::SetCommands(frameNode, Converter::Convert<std::string>(*value));
}
} // PathAttributeModifier
const GENERATED_ArkUIPathModifier* GetPathModifier()
{
    static const GENERATED_ArkUIPathModifier ArkUIPathModifierImpl {
        PathModifier::ConstructImpl,
        PathInterfaceModifier::SetPathOptionsImpl,
        PathAttributeModifier::CommandsImpl,
    };
    return &ArkUIPathModifierImpl;
}

}
