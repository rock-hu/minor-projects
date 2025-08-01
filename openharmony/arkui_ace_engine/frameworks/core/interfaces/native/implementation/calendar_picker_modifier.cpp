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
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "arkoala_api_generated.h"
#include "core/components_ng/pattern/calendar_picker/calendar_picker_model_ng.h"
#include "core/interfaces/native/utility/callback_helper.h"

namespace OHOS::Ace::NG {
namespace Converter {
template<>
void AssignCast(std::optional<CalendarEdgeAlign>& dst, const Ark_CalendarAlign& src)
{
    switch (src) {
        case ARK_CALENDAR_ALIGN_START: dst = CalendarEdgeAlign::EDGE_ALIGN_START; break;
        case ARK_CALENDAR_ALIGN_CENTER: dst = CalendarEdgeAlign::EDGE_ALIGN_CENTER; break;
        case ARK_CALENDAR_ALIGN_END: dst = CalendarEdgeAlign::EDGE_ALIGN_END; break;
        default: LOGE("Unexpected enum value in Ark_CalendarAlign: %{public}d", src);
    }
}

template<>
void AssignCast(std::optional<CalendarSettingData>& dst, const Ark_CalendarOptions& src)
{
    CalendarSettingData options;
    options.dayRadius = Converter::OptConvert<Dimension>(src.hintRadius);
    auto selected = Converter::OptConvert<PickerDate>(src.selected);
    if (selected) {
        options.selectedDate = selected.value();
    }
    dst = options;
}

} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace CalendarPickerModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = CalendarPickerModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // CalendarPickerModifier
namespace CalendarPickerInterfaceModifier {
void SetCalendarPickerOptionsImpl(Ark_NativePointer node,
                                  const Opt_CalendarOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    auto data = Converter::OptConvert<CalendarSettingData>(*options);
    CalendarPickerModelNG::SetCalendarData(frameNode, data);
}
} // CalendarPickerInterfaceModifier
namespace CalendarPickerAttributeModifier {
void TextStyle1Impl(Ark_NativePointer node, const Opt_PickerTextStyle* value);
void OnChange1Impl(Ark_NativePointer node, const Opt_Callback_Date_Void* value);

void TextStyle0Impl(Ark_NativePointer node,
                    const Opt_PickerTextStyle* value)
{
    TextStyle1Impl(node, value);
}
void TextStyle1Impl(Ark_NativePointer node,
                    const Opt_PickerTextStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CalendarPickerModelNG::SetTextStyle(frameNode,
        value ? Converter::OptConvert<PickerTextStyle>(*value) : std::nullopt);
}
void OnChange0Impl(Ark_NativePointer node,
                   const Opt_Callback_Date_Void* value)
{
    OnChange1Impl(node, value);
}
void OnChange1Impl(Ark_NativePointer node,
                   const Opt_Callback_Date_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optCallback = Converter::OptConvert<Callback_Date_Void>(*value);
    // TODO: Reset value
    CHECK_NULL_VOID(optCallback);
    auto onChange = [arkCallback = CallbackHelper(*optCallback)](const std::string& selectedStr) {
        Ark_Date result = Converter::ArkValue<Ark_Date>(selectedStr);
        arkCallback.Invoke(result);
    };
    CalendarPickerModelNG::SetOnChangeWithNode(frameNode, std::move(onChange));
}
void MarkTodayImpl(Ark_NativePointer node,
                   const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
}
void EdgeAlign0Impl(Ark_NativePointer node,
                    const Opt_CalendarAlign* alignType,
                    const Opt_Offset* offset)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<DimensionOffset> convOffset;
    if (offset) {
        convOffset = Converter::OptConvert<DimensionOffset>(*offset);
    }
    auto convAlignType = Converter::OptConvert<CalendarEdgeAlign>(*alignType);
    CalendarPickerModelNG::SetEdgeAlign(frameNode, convAlignType, convOffset);
}
void EdgeAlign1Impl(Ark_NativePointer node,
                    const Opt_CalendarAlign* alignType,
                    const Opt_Offset* offset)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<DimensionOffset> convOffset;
    if (offset) {
        convOffset = Converter::OptConvert<DimensionOffset>(*offset);
    }
    auto align = alignType ? Converter::OptConvert<CalendarEdgeAlign>(*alignType) : std::nullopt;
    CalendarPickerModelNG::SetEdgeAlign(frameNode, align, convOffset);
}
} // CalendarPickerAttributeModifier
const GENERATED_ArkUICalendarPickerModifier* GetCalendarPickerModifier()
{
    static const GENERATED_ArkUICalendarPickerModifier ArkUICalendarPickerModifierImpl {
        CalendarPickerModifier::ConstructImpl,
        CalendarPickerInterfaceModifier::SetCalendarPickerOptionsImpl,
        CalendarPickerAttributeModifier::TextStyle0Impl,
        CalendarPickerAttributeModifier::TextStyle1Impl,
        CalendarPickerAttributeModifier::OnChange0Impl,
        CalendarPickerAttributeModifier::OnChange1Impl,
        CalendarPickerAttributeModifier::MarkTodayImpl,
        CalendarPickerAttributeModifier::EdgeAlign0Impl,
        CalendarPickerAttributeModifier::EdgeAlign1Impl,
    };
    return &ArkUICalendarPickerModifierImpl;
}

}
