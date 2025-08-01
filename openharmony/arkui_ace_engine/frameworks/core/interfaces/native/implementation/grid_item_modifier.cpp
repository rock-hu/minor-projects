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

#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"

#include "core/components_ng/pattern/grid/grid_item_model_ng.h"
#include "core/components_ng/pattern/grid/grid_item_model_static.h"
#include "core/interfaces/native/generated/interface/node_api.h"

namespace OHOS::Ace::NG::Converter {
template<>
inline void AssignCast(std::optional<GridItemStyle>& dst, const Ark_GridItemStyle& src)
{
    switch (src) {
        case static_cast<Ark_GridItemStyle>(GridItemStyle::NONE): dst = GridItemStyle::NONE; break;
        case static_cast<Ark_GridItemStyle>(GridItemStyle::PLAIN): dst = GridItemStyle::PLAIN; break;
        default: LOGE("Unexpected enum value in Ark_GridItemStyle: %{public}d", src);
    }
}

template<>
inline void AssignCast(std::optional<GridItemStyle>& dst, const Ark_GridItemOptions& src)
{
    dst = Converter::OptConvert<GridItemStyle>(src.style);
}
} // namespace OHOS::Ace::NG::Converter

namespace OHOS::Ace::NG::GeneratedModifier {
namespace GridItemModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = GridItemModelStatic::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // GridItemModifier
namespace GridItemInterfaceModifier {
void SetGridItemOptionsImpl(Ark_NativePointer node,
                            const Opt_GridItemOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    std::optional<GridItemStyle> style = Converter::OptConvert<GridItemStyle>(*value);
    if (style) {
        GridItemModelStatic::SetGridItemStyle(frameNode, style.value());
    }
}
} // GridItemInterfaceModifier
namespace GridItemAttributeModifier {
void RowStartImpl(Ark_NativePointer node,
                  const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int32_t>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetRowStart(frameNode, *convValue);
}
void RowEndImpl(Ark_NativePointer node,
                const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int32_t>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetRowEnd(frameNode, *convValue);
}
void ColumnStartImpl(Ark_NativePointer node,
                     const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int32_t>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetColumnStart(frameNode, *convValue);
}
void ColumnEndImpl(Ark_NativePointer node,
                   const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int32_t>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetColumnEnd(frameNode, *convValue);
}
void ForceRebuildImpl(Ark_NativePointer node,
                      const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetForceRebuild(frameNode, *convValue);
}
void SelectableImpl(Ark_NativePointer node,
                    const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    GridItemModelStatic::SetSelectable(frameNode, *convValue);
}
void SelectedImpl(Ark_NativePointer node,
                  const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        GridItemModelStatic::SetSelected(frameNode, false);
        return;
    }
    GridItemModelStatic::SetSelected(frameNode, *convValue);
}
void OnSelectImpl(Ark_NativePointer node,
                  const Opt_Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onSelect = [arkCallback = CallbackHelper(*optValue)](bool isSelected) {
        arkCallback.Invoke(isSelected);
    };
    GridItemModelStatic::SetOnSelect(frameNode, onSelect);
}
void _onChangeEvent_selectedImpl(Ark_NativePointer node,
                                 const Callback_Opt_Boolean_Void* callback)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(callback);
    WeakPtr<FrameNode> weakNode = AceType::WeakClaim(frameNode);
    auto onEvent = [arkCallback = CallbackHelper(*callback), weakNode](bool isSelected) {
        PipelineContext::SetCallBackNode(weakNode);
        arkCallback.Invoke(Converter::ArkValue<Opt_Boolean>(isSelected));
    };
    GridItemModelStatic::SetSelectChangeEvent(frameNode, std::move(onEvent));
}
} // GridItemAttributeModifier
const GENERATED_ArkUIGridItemModifier* GetGridItemModifier()
{
    static const GENERATED_ArkUIGridItemModifier ArkUIGridItemModifierImpl {
        GridItemModifier::ConstructImpl,
        GridItemInterfaceModifier::SetGridItemOptionsImpl,
        GridItemAttributeModifier::RowStartImpl,
        GridItemAttributeModifier::RowEndImpl,
        GridItemAttributeModifier::ColumnStartImpl,
        GridItemAttributeModifier::ColumnEndImpl,
        GridItemAttributeModifier::ForceRebuildImpl,
        GridItemAttributeModifier::SelectableImpl,
        GridItemAttributeModifier::SelectedImpl,
        GridItemAttributeModifier::OnSelectImpl,
        GridItemAttributeModifier::_onChangeEvent_selectedImpl,
    };
    return &ArkUIGridItemModifierImpl;
}

}
