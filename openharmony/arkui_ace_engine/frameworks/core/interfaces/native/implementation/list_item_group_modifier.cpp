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

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/list/list_item_group_model_ng.h"
#include "core/components_ng/pattern/list/list_item_group_model_static.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "children_main_size_peer.h"

namespace OHOS::Ace::NG::Converter {
template<>
V2::ItemDivider Convert(const Ark_ListDividerOptions& src);
}

namespace OHOS::Ace::NG::GeneratedModifier {
namespace ListItemGroupModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = ListItemGroupModelStatic::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // ListItemGroupModifier
namespace ListItemGroupInterfaceModifier {
void SetListItemGroupOptionsImpl(Ark_NativePointer node,
                                 const Opt_ListItemGroupOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    auto arkOptions = Converter::OptConvert<Ark_ListItemGroupOptions>(*options);
    CHECK_NULL_VOID(arkOptions);
    auto space = Converter::OptConvert<Dimension>(arkOptions.value().space);
    ListItemGroupModelStatic::SetSpace(frameNode, space);
    auto style = Converter::OptConvert<V2::ListItemGroupStyle>(arkOptions.value().style);
    ListItemGroupModelStatic::SetStyle(frameNode, style);
    auto header = Converter::OptConvert<CustomNodeBuilder>(arkOptions.value().header);
    if (header.has_value()) {
        CallbackHelper(header.value()).BuildAsync([frameNode](const RefPtr<UINode>& uiNode) {
            auto builder = [uiNode]() -> RefPtr<UINode> {
                return uiNode;
            };
            ListItemGroupModelStatic::SetHeader(frameNode, std::move(builder));
            }, node);
    }
    auto footer = Converter::OptConvert<CustomNodeBuilder>(arkOptions.value().footer);
    if (footer.has_value()) {
        CallbackHelper(footer.value()).BuildAsync([frameNode](const RefPtr<UINode>& uiNode) {
            auto builder = [uiNode]() -> RefPtr<UINode> {
                return uiNode;
            };
            ListItemGroupModelStatic::SetFooter(frameNode, std::move(builder));
            }, node);
    }
}
} // ListItemGroupInterfaceModifier
namespace ListItemGroupAttributeModifier {
void DividerImpl(Ark_NativePointer node,
                 const Opt_ListDividerOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto divider = Converter::OptConvert<V2::ItemDivider>(*value);
    ListItemGroupModelStatic::SetDivider(frameNode, divider);
}
void ChildrenMainSizeImpl(Ark_NativePointer node,
                          const Opt_ChildrenMainSize* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto peer = *optValue;
    CHECK_NULL_VOID(peer);
    RefPtr<ListChildrenMainSize> handler = ListItemGroupModelStatic::GetOrCreateListChildrenMainSize(frameNode);
    peer->SetHandler(handler);
}
} // ListItemGroupAttributeModifier
const GENERATED_ArkUIListItemGroupModifier* GetListItemGroupModifier()
{
    static const GENERATED_ArkUIListItemGroupModifier ArkUIListItemGroupModifierImpl {
        ListItemGroupModifier::ConstructImpl,
        ListItemGroupInterfaceModifier::SetListItemGroupOptionsImpl,
        ListItemGroupAttributeModifier::DividerImpl,
        ListItemGroupAttributeModifier::ChildrenMainSizeImpl,
    };
    return &ArkUIListItemGroupModifierImpl;
}

}
