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
#include "core/interfaces/native/utility/validators.h"
#include "core/components_ng/pattern/image_animator/image_animator_model_ng.h"
#include "core/components_ng/pattern/image_animator/image_animator_model_static.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::Converter {
template<>
void AssignCast(std::optional<int32_t>& dst, const Ark_AnimationStatus& src)
{
    switch (src) {
        case Ark_AnimationStatus::ARK_ANIMATION_STATUS_INITIAL: dst = static_cast<int32_t>(src); break;
        case Ark_AnimationStatus::ARK_ANIMATION_STATUS_RUNNING: dst = static_cast<int32_t>(src); break;
        case Ark_AnimationStatus::ARK_ANIMATION_STATUS_PAUSED: dst = static_cast<int32_t>(src); break;
        case Ark_AnimationStatus::ARK_ANIMATION_STATUS_STOPPED: dst = static_cast<int32_t>(src); break;
        default: LOGE("Unexpected enum value in Ark_AnimationStatus: %{public}d", src);
    }
}
template<>
void AssignCast(std::optional<int32_t>& dst, const Ark_FillMode& src)
{
    switch (src) {
        case Ark_FillMode::ARK_FILL_MODE_NONE: dst = static_cast<int32_t>(src); break;
        case Ark_FillMode::ARK_FILL_MODE_FORWARDS: dst = static_cast<int32_t>(src); break;
        case Ark_FillMode::ARK_FILL_MODE_BACKWARDS: dst = static_cast<int32_t>(src); break;
        case Ark_FillMode::ARK_FILL_MODE_BOTH: dst = static_cast<int32_t>(src); break;
        default: LOGE("Unexpected enum value in Ark_FillMode: %{public}d", src);
    }
}

template<>
ImageProperties Convert(const Ark_ImageFrameInfo& src)
{
    ImageProperties options;
    Converter::VisitUnion(src.src,
        [&options](const Ark_String& srcArkStr) {
            auto srcStr = Converter::Convert<std::string>(srcArkStr);
            if (!srcStr.empty()) {
                options.src = srcStr;
            }
        },
        [&options](const Ark_Resource& srcArkRes) {
            auto srcStr = Converter::OptConvert<std::string>(srcArkRes);
            auto moduleName = Converter::Convert<std::string>(srcArkRes.moduleName);
            auto bundleName = Converter::Convert<std::string>(srcArkRes.bundleName);
            if (!srcStr->empty()) {
                options.src = *srcStr;
                options.moduleName = moduleName;
                options.bundleName = bundleName;
            }
        },
        [&options](const Ark_PixelMap& srcArkPixelMap) {
            options.pixelMap = Converter::Convert<RefPtr<PixelMap>>(srcArkPixelMap);
        },
        []() {}
    );

    options.width =  OptConvert<CalcDimension>(src.width).value_or(CalcDimension(0));
    options.height = OptConvert<Dimension>(src.height).value_or(CalcDimension(0));
    options.top = OptConvert<CalcDimension>(src.top).value_or(CalcDimension(0));
    options.left = OptConvert<CalcDimension>(src.left).value_or(CalcDimension(0));
    options.duration = Converter::OptConvert<int32_t>(src.duration).value_or(0);

    return options;
}
} // OHOS::Ace::NG::Converter

namespace OHOS::Ace::NG::GeneratedModifier {
namespace ImageAnimatorModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = ImageAnimatorModelNG::CreateFrameNode(id);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // ImageAnimatorModifier
namespace ImageAnimatorInterfaceModifier {
void SetImageAnimatorOptionsImpl(Ark_NativePointer node)
{
    // No implementation is required
}
} // ImageAnimatorInterfaceModifier
namespace ImageAnimatorAttributeModifier {
void ImagesImpl(Ark_NativePointer node,
                const Opt_Array_ImageFrameInfo* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto images = Converter::OptConvert<std::vector<ImageProperties>>(*value);
    if (!images) {
        // TODO: Reset value
        return;
    }
    ImageAnimatorModelNG::SetImages(frameNode, *images);
}
void StateImpl(Ark_NativePointer node,
               const Opt_AnimationStatus* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto stateOpt = Converter::OptConvert<int32_t>(*value);
    ImageAnimatorModelStatic::SetState(frameNode, stateOpt);
}
void DurationImpl(Ark_NativePointer node,
                  const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto durationOpt = Converter::OptConvert<int32_t>(*value);
    Validator::ValidateNonNegative(durationOpt);
    ImageAnimatorModelStatic::SetDuration(frameNode, durationOpt);
}
void ReverseImpl(Ark_NativePointer node,
                 const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    ImageAnimatorModelNG::SetIsReverse(frameNode, *convValue);
}
void FixedSizeImpl(Ark_NativePointer node,
                   const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    ImageAnimatorModelNG::SetFixedSize(frameNode, *convValue);
}
void PreDecodeImpl(Ark_NativePointer node,
                   const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
}
void FillModeImpl(Ark_NativePointer node,
                  const Opt_FillMode* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto fillModeOpt = Converter::OptConvert<int32_t>(*value);
    ImageAnimatorModelStatic::SetFillMode(frameNode, fillModeOpt);
}
void IterationsImpl(Ark_NativePointer node,
                    const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto iterationOpt = Converter::OptConvert<int32_t>(*value);
    Validator::ValidateGreatOrEqual(iterationOpt, -1);
    ImageAnimatorModelStatic::SetIteration(frameNode, iterationOpt);
}
void MonitorInvisibleAreaImpl(Ark_NativePointer node,
                              const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<bool>(*value) : std::nullopt;
    if (!convValue) {
        return;
    }
    ImageAnimatorModelNG::SetAutoMonitorInvisibleArea(frameNode, *convValue);
}
void OnStartImpl(Ark_NativePointer node,
                 const Opt_Callback_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onStart = [arkCallback = CallbackHelper(*optValue)]() -> void {
        arkCallback.Invoke();
    };
    ImageAnimatorModelNG::SetOnStart(frameNode, std::move(onStart));
}
void OnPauseImpl(Ark_NativePointer node,
                 const Opt_Callback_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onPause = [arkCallback = CallbackHelper(*optValue)]() -> void {
        arkCallback.Invoke();
    };
    ImageAnimatorModelNG::SetOnPause(frameNode, std::move(onPause));
}
void OnRepeatImpl(Ark_NativePointer node,
                  const Opt_Callback_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onRepeat = [arkCallback = CallbackHelper(*optValue)]() -> void {
        arkCallback.Invoke();
    };
    ImageAnimatorModelNG::SetOnRepeat(frameNode, onRepeat);
}
void OnCancelImpl(Ark_NativePointer node,
                  const Opt_Callback_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onCancel = [arkCallback = CallbackHelper(*optValue)]() -> void {
        arkCallback.Invoke();
    };
    ImageAnimatorModelNG::SetOnCancel(frameNode, onCancel);
}
void OnFinishImpl(Ark_NativePointer node,
                  const Opt_Callback_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onFinish = [arkCallback = CallbackHelper(*optValue)]() -> void {
        arkCallback.Invoke();
    };
    ImageAnimatorModelNG::SetOnFinish(frameNode, onFinish);
}
} // ImageAnimatorAttributeModifier
const GENERATED_ArkUIImageAnimatorModifier* GetImageAnimatorModifier()
{
    static const GENERATED_ArkUIImageAnimatorModifier ArkUIImageAnimatorModifierImpl {
        ImageAnimatorModifier::ConstructImpl,
        ImageAnimatorInterfaceModifier::SetImageAnimatorOptionsImpl,
        ImageAnimatorAttributeModifier::ImagesImpl,
        ImageAnimatorAttributeModifier::StateImpl,
        ImageAnimatorAttributeModifier::DurationImpl,
        ImageAnimatorAttributeModifier::ReverseImpl,
        ImageAnimatorAttributeModifier::FixedSizeImpl,
        ImageAnimatorAttributeModifier::PreDecodeImpl,
        ImageAnimatorAttributeModifier::FillModeImpl,
        ImageAnimatorAttributeModifier::IterationsImpl,
        ImageAnimatorAttributeModifier::MonitorInvisibleAreaImpl,
        ImageAnimatorAttributeModifier::OnStartImpl,
        ImageAnimatorAttributeModifier::OnPauseImpl,
        ImageAnimatorAttributeModifier::OnRepeatImpl,
        ImageAnimatorAttributeModifier::OnCancelImpl,
        ImageAnimatorAttributeModifier::OnFinishImpl,
    };
    return &ArkUIImageAnimatorModifierImpl;
}

}
