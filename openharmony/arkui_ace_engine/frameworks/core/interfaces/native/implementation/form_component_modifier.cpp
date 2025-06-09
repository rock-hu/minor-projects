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
#ifdef FORM_SUPPORTED
#include "core/components_ng/pattern/form/form_model_ng.h"
#endif
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "arkoala_api_generated.h"
#include "core/components/common/layout/constants.h"
#include "core/components/form/resource/form_request_data.h"
namespace OHOS::Ace::NG {
struct OptRequestFormInfo {
    std::optional<int64_t> id = std::nullopt;
    std::string name;
    std::string bundle;
    std::string ability;
    std::string module;
    std::optional<int32_t> dimension = std::nullopt;
    std::optional<bool> temporary = std::nullopt;
    RefPtr<WantWrap> want = nullptr;
    std::optional<int32_t> renderingMode = std::nullopt;
    std::optional<int32_t> shape = std::nullopt;
};
struct LiteralDimension {
    Dimension width;
    Dimension height;
};
namespace {
const auto FORM_SHAPE_RECT = 1;
const auto FORM_SHAPE_CIRCLE = 2;
const auto FORM_RENDERING_MODE_FULL_COLOR = 0;
const auto FORM_RENDERING_MODE_SINGLE_COLOR = 1;
const auto FORM_DIMENSION_DIMENSION_1_2 = 1;
const auto FORM_DIMENSION_DIMENSION_2_2 = 2;
const auto FORM_DIMENSION_DIMENSION_2_4 = 3;
const auto FORM_DIMENSION_DIMENSION_4_4 = 4;
const auto FORM_DIMENSION_DIMENSION_2_1 = 5;
const auto FORM_DIMENSION_DIMENSION_1_1 = 6;
const auto FORM_DIMENSION_DIMENSION_6_4 = 7;
const auto FORM_COMPONENT_ID_KEY = "id";
const auto FORM_COMPONENT_ID_STRING_KEY = "idString";
const uint32_t FORM_ON_ACQUIRED_ID_INVALID = -1;
const std::string FORM_ON_ACQUIRED_ID_STRING_INVALID = "-1";
const uint32_t MIN_UNSIGNED_NUMBER_OF_ARK = 0;
const uint32_t MAX_UNSIGNED_NUMBER_OF_ARK = UINT_MAX;
const auto FORM_ON_ERROR_CODE_KEY = "errcode";
const auto FORM_ON_ERROR_MSG_KEY = "msg";
const int32_t FORM_ON_ERROR_CODE_INVALID = -1;
const std::string FORM_EMPTY_STRING = "";
const int32_t MIN_SIGNED_NUMBER_OF_ARK = INT_MIN;
const int32_t MAX_SIGNED_NUMBER_OF_ARK = INT_MAX;
} // namespace
namespace Converter {
template<>
OptRequestFormInfo Convert(const Ark_FormInfo& src)
{
    OptRequestFormInfo dst;
    auto id = Converter::OptConvert<Dimension>(src.id);
    if (id) {
        dst.id = (int64_t)(*id).Value();
    }
    dst.name = Converter::Convert<std::string>(src.name);
    dst.bundle = Converter::Convert<std::string>(src.bundle);
    dst.ability = Converter::Convert<std::string>(src.ability);
    dst.module = Converter::Convert<std::string>(src.module);
    dst.dimension = Converter::OptConvert<int32_t>(src.dimension);
    dst.temporary = Converter::OptConvert<bool>(src.temporary);
    dst.want = nullptr;
    dst.renderingMode = Converter::OptConvert<int32_t>(src.renderingMode);
    dst.shape = Converter::OptConvert<int32_t>(src.shape);
    return dst;
}
template<>
void AssignCast(std::optional<int32_t>& dst, const Ark_FormShape& src)
{
    switch (src) {
        case ARK_FORM_SHAPE_RECT: dst = FORM_SHAPE_RECT; break;
        case ARK_FORM_SHAPE_CIRCLE: dst = FORM_SHAPE_CIRCLE; break;
        default: LOGE("Unexpected enum value in Ark_FormRenderingMode: %{public}d", src);
    }
}
template<>
void AssignCast(std::optional<int32_t>& dst, const Ark_FormRenderingMode& src)
{
    switch (src) {
        case ARK_FORM_RENDERING_MODE_FULL_COLOR: dst = FORM_RENDERING_MODE_FULL_COLOR; break;
        case ARK_FORM_RENDERING_MODE_SINGLE_COLOR: dst = FORM_RENDERING_MODE_SINGLE_COLOR; break;
        default: LOGE("Unexpected enum value in Ark_FormRenderingMode: %{public}d", src);
    }
}
template<>
LiteralDimension Convert(const Ark_SizeOptions& src)
{
    return LiteralDimension {
        .width = Converter::OptConvert<Dimension>(src.width).value_or(Dimension()),
        .height = Converter::OptConvert<Dimension>(src.height).value_or(Dimension()),
    };
}
template<>
void AssignCast(std::optional<int32_t>& dst, const Ark_FormDimension& src)
{
    switch (src) {
        case ARK_FORM_DIMENSION_DIMENSION_1_2: dst = FORM_DIMENSION_DIMENSION_1_2; break;
        case ARK_FORM_DIMENSION_DIMENSION_2_2: dst = FORM_DIMENSION_DIMENSION_2_2; break;
        case ARK_FORM_DIMENSION_DIMENSION_2_4: dst = FORM_DIMENSION_DIMENSION_2_4; break;
        case ARK_FORM_DIMENSION_DIMENSION_4_4: dst = FORM_DIMENSION_DIMENSION_4_4; break;
        case ARK_FORM_DIMENSION_DIMENSION_2_1: dst = FORM_DIMENSION_DIMENSION_2_1; break;
        case ARK_FORM_DIMENSION_DIMENSION_1_1: dst = FORM_DIMENSION_DIMENSION_1_1; break;
        case ARK_FORM_DIMENSION_DIMENSION_6_4: dst = FORM_DIMENSION_DIMENSION_6_4; break;
        default: LOGE("Unexpected enum value in Ark_FormDimension: %{public}d", src);
    }
}
template<>
void AssignCast(std::optional<VisibleType>& dst, const Ark_Visibility& src)
{
    switch (src) {
        case ARK_VISIBILITY_VISIBLE: dst = VisibleType::VISIBLE; break;
        case ARK_VISIBILITY_HIDDEN: dst = VisibleType::INVISIBLE; break;
        case ARK_VISIBILITY_NONE: dst = VisibleType::GONE; break;
        default: LOGE("Unexpected enum value in Ark_Visibility: %{public}d", src);
    }
}
} // namespace OHOS::Ace::NG::Converter
} // namespace OHOS::Ace::NG
namespace OHOS::Ace::NG::GeneratedModifier {
namespace FormComponentModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
#ifdef FORM_SUPPORTED
    // auto frameNode = FormModelNG::CreateFrameNode(id);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return {};
#else
    return {};
#endif
}
} // FormComponentModifier
namespace FormComponentInterfaceModifier {
void SetFormComponentOptionsImpl(Ark_NativePointer node,
                                 const Ark_FormInfo* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto formInfo = Converter::Convert<OptRequestFormInfo>(*value);
    FormModelNG::SetModuleName(frameNode, formInfo.module);
    if (formInfo.dimension) {
        FormModelNG::SetDimension(frameNode, *formInfo.dimension);
    }
    LOGE("ARKOALA FormComponentInterfaceModifier::OnAcquiredImpl - Ark_Number width of int32_t is not enough "
         "for implementation of int64_t functionality.");
    LOGE("ARKOALA FormComponentInterfaceModifier::SetFormComponentOptionsImpl - CustomObject is not supported "
         "the type Ark_FormInfo::Opt_Want::Opt_Map_String_CustomObject::Ark_CustomObject.");
#endif // FORM_SUPPORTED
}
} // FormComponentInterfaceModifier
namespace FormComponentAttributeModifier {
void SizeImpl(Ark_NativePointer node,
              const Ark_SizeOptions* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto dimension = Converter::Convert<LiteralDimension>(*value);
    FormModelNG::SetSize(frameNode, dimension.width, dimension.height);
#endif // FORM_SUPPORTED
}
void ModuleNameImpl(Ark_NativePointer node,
                    const Ark_String* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto name = Converter::Convert<std::string>(*value);
    FormModelNG::SetModuleName(frameNode, name);
#endif // FORM_SUPPORTED
}
void DimensionImpl(Ark_NativePointer node,
                   Ark_FormDimension value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto opt = Converter::OptConvert<int32_t>(value);
    CHECK_NULL_VOID(opt);
    FormModelNG::SetDimension(frameNode, *opt);
#endif // FORM_SUPPORTED
}
void AllowUpdateImpl(Ark_NativePointer node,
                     Ark_Boolean value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::Convert<bool>(value);
    FormModelNG::AllowUpdate(frameNode, convValue);
#endif // FORM_SUPPORTED
}
void VisibilityImpl(Ark_NativePointer node,
                    Ark_Visibility value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto opt = Converter::OptConvert<VisibleType>(value);
    CHECK_NULL_VOID(opt);
    FormModelNG::SetVisibility(frameNode, *opt);
#endif // FORM_SUPPORTED
}
void OnAcquiredImpl(Ark_NativePointer node,
                    const Callback_FormCallbackInfo_Void* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);

    auto onAcquired = [arkCallback = CallbackHelper(*value)](const std::string& param) {
        uint32_t id = FORM_ON_ACQUIRED_ID_INVALID;
        std::string idString = FORM_ON_ACQUIRED_ID_STRING_INVALID;
        auto sourceJson = JsonUtil::ParseJsonString(param);
        if (sourceJson && !sourceJson->IsNull()) {
            char* endptr;
            auto jsonId = sourceJson->GetString(FORM_COMPONENT_ID_KEY, FORM_ON_ACQUIRED_ID_STRING_INVALID);
            idString = sourceJson->GetString(FORM_COMPONENT_ID_STRING_KEY, FORM_ON_ACQUIRED_ID_STRING_INVALID);
            int64_t result = std::strtoul(jsonId.c_str(), &endptr, 10);
            if (*endptr == '\0' && result >= MIN_UNSIGNED_NUMBER_OF_ARK && result < MAX_UNSIGNED_NUMBER_OF_ARK) {
                id = static_cast<uint32_t>(result);
            }
        }
        Ark_FormCallbackInfo parameter = {
            .id = Converter::ArkValue<Ark_Number>(id),
            .idString = Converter::ArkValue<Ark_String>(idString) };
        arkCallback.Invoke(parameter);
    };
    LOGE("ARKOALA FormComponentInterfaceModifier::OnAcquiredImpl - Ark_Number width of int32_t is not enough "
        "for implementation of int64_t functionality.");
    // FormModelNG::SetOnAcquired(frameNode, onAcquired);
#endif // FORM_SUPPORTED
}
void OnErrorImpl(Ark_NativePointer node,
                 const Callback_Literal_Number_errcode_String_msg_Void* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);

    auto onError = [arkCallback = CallbackHelper(*value)](const std::string& param) {
        int32_t code = FORM_ON_ERROR_CODE_INVALID;
        std::string msg = FORM_EMPTY_STRING;
        auto sourceJson = JsonUtil::ParseJsonString(param);
        if (sourceJson && !sourceJson->IsNull()) {
            char* endptr;
            auto jsonCode = sourceJson->GetString(FORM_ON_ERROR_CODE_KEY, FORM_EMPTY_STRING);
            msg = sourceJson->GetString(FORM_ON_ERROR_MSG_KEY, FORM_EMPTY_STRING);
            int64_t result = std::strtol(jsonCode.c_str(), &endptr, 10);
            if (*endptr == '\0' && result >= MIN_SIGNED_NUMBER_OF_ARK && result <= MAX_SIGNED_NUMBER_OF_ARK) {
                code = static_cast<int32_t>(result);
            }
        }
        Ark_Literal_Number_errcode_String_msg parameter = {
            .errcode = Converter::ArkValue<Ark_Number>(code),
            .msg = Converter::ArkValue<Ark_String>(msg)
        };
        arkCallback.Invoke(parameter);
    };
    LOGE("ARKOALA FormComponentInterfaceModifier::OnErrorImpl - Ark_Number width of int32_t is not enough "
        "for implementation of int64_t functionality.");
    // FormModelNG::SetOnError(frameNode, onError);
#endif // FORM_SUPPORTED
}
void OnRouterImpl(Ark_NativePointer node,
                  const Callback_Any_Void* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    LOGE("ARKOALA FormComponentInterfaceModifier::OnRouterImpl - CustomObject is not supported "
        "the type Callback_Any_Void includes Ark_CustomObject.");
#endif // FORM_SUPPORTED
}
void OnUninstallImpl(Ark_NativePointer node,
                     const Callback_FormCallbackInfo_Void* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);

    auto onUninstall = [arkCallback = CallbackHelper(*value)](const std::string& param) {
        uint32_t id = FORM_ON_ACQUIRED_ID_INVALID;
        std::string idString = FORM_ON_ACQUIRED_ID_STRING_INVALID;
        auto sourceJson = JsonUtil::ParseJsonString(param);
        if (sourceJson && !sourceJson->IsNull()) {
            char* endptr;
            auto jsonId = sourceJson->GetString(FORM_COMPONENT_ID_KEY, FORM_ON_ACQUIRED_ID_STRING_INVALID);
            idString = sourceJson->GetString(FORM_COMPONENT_ID_STRING_KEY, FORM_ON_ACQUIRED_ID_STRING_INVALID);
            int64_t result = std::strtoul(jsonId.c_str(), &endptr, 10);
            if (*endptr == '\0' && result >= MIN_UNSIGNED_NUMBER_OF_ARK && result < MAX_UNSIGNED_NUMBER_OF_ARK) {
                id = static_cast<uint32_t>(result);
            }
        }
        Ark_FormCallbackInfo parameter = {
            .id = Converter::ArkValue<Ark_Number>(id),
            .idString = Converter::ArkValue<Ark_String>(idString)
        };
        arkCallback.Invoke(parameter);
    };
    LOGE("ARKOALA FormComponentInterfaceModifier::OnAcquiredImpl - Ark_Number width of int32_t is not enough "
        "for implementation of int64_t functionality.");
    // FormModelNG::SetOnUninstall(frameNode, onUninstall);
#endif // FORM_SUPPORTED
}
void OnLoadImpl(Ark_NativePointer node,
                const Callback_Void* value)
{
#ifdef FORM_SUPPORTED
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto onLoad = [arkCallback = CallbackHelper(*value)](const std::string& param) {
        arkCallback.Invoke();
    };
    // FormModelNG::SetOnLoad(frameNode, std::move(onLoad));
#endif // FORM_SUPPORTED
}
} // FormComponentAttributeModifier
const GENERATED_ArkUIFormComponentModifier* GetFormComponentModifier()
{
    static const GENERATED_ArkUIFormComponentModifier ArkUIFormComponentModifierImpl {
        FormComponentModifier::ConstructImpl,
        FormComponentInterfaceModifier::SetFormComponentOptionsImpl,
        FormComponentAttributeModifier::SizeImpl,
        FormComponentAttributeModifier::ModuleNameImpl,
        FormComponentAttributeModifier::DimensionImpl,
        FormComponentAttributeModifier::AllowUpdateImpl,
        FormComponentAttributeModifier::VisibilityImpl,
        FormComponentAttributeModifier::OnAcquiredImpl,
        FormComponentAttributeModifier::OnErrorImpl,
        FormComponentAttributeModifier::OnRouterImpl,
        FormComponentAttributeModifier::OnUninstallImpl,
        FormComponentAttributeModifier::OnLoadImpl,
    };
    return &ArkUIFormComponentModifierImpl;
}

}
