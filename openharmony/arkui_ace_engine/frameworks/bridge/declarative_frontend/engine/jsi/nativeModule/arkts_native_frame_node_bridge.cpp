/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_frame_node_bridge.h"

#include "jsnapi_expo.h"

#include "base/log/log_wrapper.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "bridge/declarative_frontend/engine/jsi/jsi_types.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_api_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_render_node_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_toggle_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_utils_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_xcomponent_bridge.h"
#include "bridge/declarative_frontend/jsview/js_view_context.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/custom_frame_node/custom_frame_node.h"
#include "core/components_ng/pattern/custom_frame_node/custom_frame_node_pattern.h"
#include "core/components_ng/pattern/toggle/toggle_model_ng.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_model_ng.h"
#include "core/components_ng/syntax/node_content.h"
#include "core/interfaces/arkoala/arkoala_api.h"
#include "core/interfaces/native/node/extension_custom_node.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"

namespace OHOS::Ace::NG {
namespace {

constexpr double VISIBLE_RATIO_MIN = 0.0;
constexpr double VISIBLE_RATIO_MAX = 1.0;
constexpr int32_t INDEX_OF_INTERVAL = 4;
constexpr int32_t INDEX_OF_OPTION_OF_VISIBLE = 3;
constexpr int DEFAULT_EXPECTED_UPDATE_INTERVAL = 1000;
constexpr int32_t SIZE_OF_ARRAY = 2;

bool ParseFloatArray(const EcmaVM* vm, const panda::Local<panda::ArrayRef>& array, std::vector<float>& outArray)
{
    uint32_t length = array->Length(vm);
    for (uint32_t i = 0; i < length; ++i) {
        auto jsValue = panda::ArrayRef::GetValueAt(vm, array, i);
        bool isNumber = false;
        double value = jsValue->GetValueDouble(isNumber);
        if (!isNumber) {
            return false;
        }
        outArray.emplace_back(value);
    }
    return true;
}

bool CheckAnimationPropertyLength(AnimationPropertyType type, size_t size, bool allowEmpty)
{
    if (allowEmpty && size == 0) {
        return true;
    }
    const static std::unordered_map<AnimationPropertyType, std::pair<size_t, const char*>> requiredLength = {
        { AnimationPropertyType::ROTATION, { ROTATION_PARAM_SIZE, "rotation" } },
        { AnimationPropertyType::TRANSLATION, { TRANSLATION_PARAM_SIZE, "translation" } },
        { AnimationPropertyType::SCALE, { SCALE_PARAM_SIZE, "scale" } },
        { AnimationPropertyType::OPACITY, { OPACITY_PARAM_SIZE, "opacity" } },
    };
    auto iter = requiredLength.find(type);
    if (iter == requiredLength.end()) {
        return false;
    }
    if (iter->second.first == size) {
        return true;
    }
    TAG_LOGW(AceLogTag::ACE_ANIMATION,
        "animationProperty of %{public}s needs %{public}zu params while input size is %{public}zu", iter->second.second,
        iter->second.first, size);
    return false;
}

void AdjustPropertyValue(AnimationPropertyType type, std::vector<float>& startValue, std::vector<float>& endValue)
{
    if (type == AnimationPropertyType::OPACITY) {
        for (auto& opacityItem : startValue) {
            opacityItem = std::clamp(opacityItem, 0.0f, 1.0f);
        }
        for (auto& opacityItem : endValue) {
            opacityItem = std::clamp(opacityItem, 0.0f, 1.0f);
        }
    }
}

std::function<void()> ParseFinishCallback(const panda::Local<panda::ObjectRef>& obj, FrameNode* frameNode,
    const EcmaVM* vm, std::optional<int32_t>& finishCount)
{
    panda::Local<panda::JSValueRef> onFinishValue = obj->Get(vm, "onFinish");
    if (onFinishValue->IsFunction(vm)) {
        panda::Local<panda::FunctionRef> onFinish = onFinishValue->ToObject(vm);
        finishCount = GetAnimationFinishCount();
        return [func = panda::CopyableGlobal(vm, onFinish), id = Container::CurrentIdSafely(),
                   node = AceType::WeakClaim(frameNode), count = finishCount.value(), vm]() mutable {
            if (func.IsEmpty()) {
                return;
            }
            ContainerScope scope(id);
            ACE_SCOPED_TRACE("frameNode onFinish[cnt:%d]", count);
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_VOID(pipelineContext);
            pipelineContext->UpdateCurrentActiveNode(node);
            panda::LocalScope pandaScope(vm);
            TAG_LOGI(AceLogTag::ACE_ANIMATION, "FrameNode animation finish, cnt:%{public}d", count);
            func->Call(vm, func.ToLocal(), nullptr, 0);
            func.Reset();
        };
    }
    return nullptr;
}

bool ParseAnimationProperty(
    const EcmaVM* vm, const panda::Local<panda::JSValueRef>& propertyArg, AnimationPropertyType& result)
{
    CHECK_NULL_RETURN(propertyArg->IsNumber(), false);
    int32_t propertyInt = propertyArg->Int32Value(vm);
    if (propertyInt < static_cast<int32_t>(AnimationPropertyType::ROTATION) ||
        propertyInt > static_cast<int32_t>(AnimationPropertyType::OPACITY)) {
        return false;
    }
    result = static_cast<AnimationPropertyType>(propertyInt);
    return true;
}

ArkUI_Bool GetIsExpanded(ArkUIRuntimeCallInfo* runtimeCallInfo, ArkUI_Int32 index)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> isExpandedArg = runtimeCallInfo->GetCallArgRef(index);
    CHECK_NULL_RETURN(!isExpandedArg.IsNull(), true);
    return isExpandedArg->IsBoolean() ? isExpandedArg->ToBoolean(vm)->Value() : true;
}
ArkUI_Int32 GetExpandMode(ArkUIRuntimeCallInfo* runtimeCallInfo, ArkUI_Int32 index)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> expandModeArg = runtimeCallInfo->GetCallArgRef(index);
    CHECK_NULL_RETURN(!expandModeArg.IsNull(), 1);
    return expandModeArg->IsNumber() || expandModeArg->IsBoolean() ? expandModeArg->ToNumber(vm)->Value() : 1;
}
ArkUI_Bool GetIsExcludeInner(ArkUIRuntimeCallInfo* runtimeCallInfo, ArkUI_Int32 index)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> isExcludeInnerArg = runtimeCallInfo->GetCallArgRef(index);
    CHECK_NULL_RETURN(!isExcludeInnerArg.IsNull(), false);
    return isExcludeInnerArg->IsBoolean() ? isExcludeInnerArg->ToBoolean(vm)->Value() : false;
}
} // namespace
ArkUI_Bool FrameNodeBridge::IsCustomFrameNode(FrameNode* node)
{
    return node && (node->IsArkTsFrameNode() || node->GetIsRootBuilderNode());
}

ArkUINodeHandle FrameNodeBridge::GetFrameNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, nullptr);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull() && firstArg->IsNativePointer(vm), nullptr);
    auto* nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    return nativeNode;
}

int FrameNodeBridge::GetInstanceId(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, -1);
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(2); // index of instanceId
    CHECK_NULL_RETURN(!thirdArg.IsNull(), -1);
    return thirdArg->ToNumber(vm)->Value();
}

Local<panda::ObjectRef> FrameNodeBridge::CreateEventTargetObject(EcmaVM* vm, const BaseEventInfo& info)
{
    const auto& localOffset = info.GetTarget().area.GetOffset();
    const auto& origin = info.GetTarget().origin;
    const char* keysOfOffset[] = { "x", "y" };
    Local<JSValueRef> valuesOfOffset[] = { panda::NumberRef::New(vm, localOffset.GetX().ConvertToVp()),
        panda::NumberRef::New(vm, localOffset.GetY().ConvertToVp()) };
    auto offset = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfOffset), keysOfOffset, valuesOfOffset);

    const char* keysOfGlobalOffset[] = { "x", "y" };
    Local<JSValueRef> valuesOfGlobalOffset[] = { panda::NumberRef::New(
                                                     vm, (origin.GetX() + localOffset.GetX()).ConvertToVp()),
        panda::NumberRef::New(vm, (origin.GetY() + localOffset.GetY()).ConvertToVp()) };
    auto globalOffset = panda::ObjectRef::NewWithNamedProperties(
        vm, ArraySize(keysOfGlobalOffset), keysOfGlobalOffset, valuesOfGlobalOffset);
    const char* keysOfArea[] = { "position", "globalPosition", "width", "height" };
    Local<JSValueRef> valuesOfArea[] = { offset, globalOffset,
        panda::NumberRef::New(vm, info.GetTarget().area.GetWidth().ConvertToVp()),
        panda::NumberRef::New(vm, info.GetTarget().area.GetHeight().ConvertToVp()) };
    auto area = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfArea), keysOfArea, valuesOfArea);
    auto target = panda::ObjectRef::New(vm);
    target->Set(vm, panda::StringRef::NewFromUtf8(vm, "area"), area);
    if (info.GetTarget().id.empty()) {
        target->Set(vm, panda::StringRef::NewFromUtf8(vm, "id"), panda::JSValueRef().Undefined(vm));
    } else {
        target->Set(vm, panda::StringRef::NewFromUtf8(vm, "id"),
            panda::StringRef::NewFromUtf8(vm, info.GetTarget().id.c_str()));
    }
    return target;
}

Local<panda::ObjectRef> FrameNodeBridge::CreateTouchInfo(
    EcmaVM* vm, const TouchLocationInfo& touchInfo, TouchEventInfo& info)
{
    double density = PipelineBase::GetCurrentDensity();
    const Offset& globalOffset = touchInfo.GetGlobalLocation();
    const Offset& localOffset = touchInfo.GetLocalLocation();
    const Offset& screenOffset = touchInfo.GetScreenLocation();
    const Offset& globalDisplayOffset = touchInfo.GetGlobalDisplayLocation();
    const char* keys[] = { "type", "id", "displayX", "displayY", "windowX", "windowY", "screenX", "screenY", "x", "y",
        "pressedTime", "pressure", "width", "height", "hand", "globalDisplayX", "globalDisplayY" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, static_cast<int32_t>(touchInfo.GetTouchType())),
        panda::NumberRef::New(vm, touchInfo.GetFingerId()), panda::NumberRef::New(vm, screenOffset.GetX() / density),
        panda::NumberRef::New(vm, screenOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, localOffset.GetX() / density),
        panda::NumberRef::New(vm, localOffset.GetY() / density),
        panda::NumberRef::New(vm, static_cast<double>(touchInfo.GetPressedTime().time_since_epoch().count())),
        panda::NumberRef::New(vm, touchInfo.GetForce()),
        panda::NumberRef::New(vm, touchInfo.GetWidth() / density),
        panda::NumberRef::New(vm, touchInfo.GetHeight() / density),
        panda::NumberRef::New(vm, touchInfo.GetOperatingHand()),
        panda::NumberRef::New(vm, globalDisplayOffset.GetX() / density),
        panda::NumberRef::New(vm, globalDisplayOffset.GetY() / density) };
    auto touchInfoObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    touchInfoObj->SetNativePointerFieldCount(vm, 1);
    touchInfoObj->SetNativePointerField(vm, 0, static_cast<void*>(&info));
    return touchInfoObj;
}

ArkUINativeModuleValue FrameNodeBridge::IsModifiable(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto result = GetArkUINodeModifiers()->getFrameNodeModifier()->isModifiable(nativeNode);
    return panda::BooleanRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::MakeFrameNodeInfo(EcmaVM* vm, ArkUINodeHandle frameNode)
{
    const char* keys[] = { "nodeId", "nodePtr" };
    auto nodeId = GetArkUINodeModifiers()->getFrameNodeModifier()->getIdByNodePtr(frameNode);
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, nodeId), panda::NativePointerRef::New(vm, frameNode) };
    auto obj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    return obj;
}

ArkUINativeModuleValue FrameNodeBridge::CreateFrameNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto node = NG::CustomFrameNode::GetOrCreateCustomFrameNode(nodeId);
    node->SetExclusiveEventForChild(true);
    node->SetIsArkTsFrameNode(true);
    auto renderContext = node->GetRenderContext();
    if (renderContext) {
        renderContext->SetNeedDebugBoundary(true);
    }
    FrameNodeBridge::SetDrawFunc(node, runtimeCallInfo);
    FrameNodeBridge::SetCustomFunc(node, runtimeCallInfo);
    const char* keys[] = { "nodeId", "nativeStrongRef" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, nodeId), NativeUtilsBridge::CreateStrongRef(vm, node) };
    auto reslut = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    return reslut;
}

void FrameNodeBridge::SetCustomFunc(const RefPtr<FrameNode>& frameNode, ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    CHECK_NULL_VOID(frameNode);
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_VOID(firstArg->IsObject(vm));
    auto obj = Local<panda::ObjectRef>(firstArg);

    auto customFuncExisted = false;
    auto measureFuncName = panda::StringRef::NewFromUtf8(vm, "onMeasure");
    auto measureFuncObj = obj->Get(vm, measureFuncName);
    auto customNode = AceType::MakeRefPtr<ExtensionCustomNode>();
    if (measureFuncObj->IsFunction(vm)) {
        customFuncExisted = true;
        customNode->SetMeasureCallback(FrameNodeBridge::GetMeasureFunc(vm, obj));
    }
    auto layoutFuncName = panda::StringRef::NewFromUtf8(vm, "onLayout");
    auto layoutFuncObj = obj->Get(vm, layoutFuncName);
    if (layoutFuncObj->IsFunction(vm)) {
        customFuncExisted = true;
        customNode->SetLayoutCallback(FrameNodeBridge::GetLayoutFunc(vm, obj));
    }

    if (!customFuncExisted) {
        return;
    }

    frameNode->SetExtensionHandler(customNode);
}

std::function<void(LayoutConstraintF& layoutConstraint)> FrameNodeBridge::GetMeasureFunc(
    EcmaVM* vm, Local<panda::ObjectRef> obj)
{
    return [vm, object = JsWeak(panda::CopyableGlobal(vm, obj))](LayoutConstraintF& layoutConstraint) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        auto funcName = panda::StringRef::NewFromUtf8(vm, "onMeasure");
        FrameNodeBridge::FireMeasureCallback(vm, object, layoutConstraint, funcName);
    };
}

std::function<void(OffsetF& position)> FrameNodeBridge::GetLayoutFunc(EcmaVM* vm, Local<panda::ObjectRef> obj)
{
    return [vm, object = JsWeak(panda::CopyableGlobal(vm, obj))](OffsetF& position) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        auto funcName = panda::StringRef::NewFromUtf8(vm, "onLayout");
        FrameNodeBridge::FireLayoutCallback(vm, object, position, funcName);
    };
}

void FrameNodeBridge::FireMeasureCallback(EcmaVM* vm, JsWeak<panda::CopyableGlobal<panda::ObjectRef>> object,
    LayoutConstraintF& layoutConstraint, Local<panda::StringRef> funcName)
{
    auto obj = object.Lock();
    CHECK_NULL_VOID(!obj.IsEmpty());
    CHECK_NULL_VOID(obj->IsObject(vm));
    auto funcObj = obj->Get(vm, funcName);
    CHECK_NULL_VOID(funcObj->IsFunction(vm));
    panda::Local<panda::FunctionRef> func = funcObj;
    auto replaceInfinityFunc = [](float value) -> double {
        double res = static_cast<double>(value);
        if (GreatOrEqual(res, Infinity<double>())) {
            return std::numeric_limits<double>::infinity();
        }

        if (LessOrEqual(res, -Infinity<double>())) {
            return -std::numeric_limits<double>::infinity();
        }

        return res;
    };

    const char* keysOfSize[] = { "height", "width" };
    Local<JSValueRef> valuesOfMaxSize[] = { panda::NumberRef::New(
                                                vm, replaceInfinityFunc(layoutConstraint.maxSize.Height())),
        panda::NumberRef::New(vm, replaceInfinityFunc(layoutConstraint.maxSize.Width())) };
    Local<JSValueRef> valuesOfMinSize[] = { panda::NumberRef::New(
                                                vm, replaceInfinityFunc(layoutConstraint.minSize.Height())),
        panda::NumberRef::New(vm, replaceInfinityFunc(layoutConstraint.minSize.Width())) };
    Local<JSValueRef> valuesOfPercentReference[] = {
        panda::NumberRef::New(vm, replaceInfinityFunc(layoutConstraint.percentReference.Height())),
        panda::NumberRef::New(vm, replaceInfinityFunc(layoutConstraint.percentReference.Width()))
    };
    auto maxSizeObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfSize), keysOfSize, valuesOfMaxSize);
    auto minSizeObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfSize), keysOfSize, valuesOfMinSize);
    auto percentReferenceObj =
        panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfSize), keysOfSize, valuesOfPercentReference);

    Local<JSValueRef> values[] = { maxSizeObj, minSizeObj, percentReferenceObj };
    const char* keys[] = { "maxSize", "minSize", "percentReference" };
    auto constraintObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    panda::Local<panda::JSValueRef> params[1] = { constraintObj };
    func->Call(vm, obj.ToLocal(), params, 1);
}

void FrameNodeBridge::FireLayoutCallback(EcmaVM* vm, JsWeak<panda::CopyableGlobal<panda::ObjectRef>> object,
    OffsetF& position, Local<panda::StringRef> funcName)
{
    auto obj = object.Lock();
    CHECK_NULL_VOID(!obj.IsEmpty());
    CHECK_NULL_VOID(obj->IsObject(vm));
    auto funcObj = obj->Get(vm, funcName);
    CHECK_NULL_VOID(funcObj->IsFunction(vm));
    panda::Local<panda::FunctionRef> func = funcObj;

    const char* keys[] = { "x", "y" };
    Local<JSValueRef> valuesOfPosition[] = { panda::NumberRef::New(vm, static_cast<double>(position.GetX())),
        panda::NumberRef::New(vm, static_cast<double>(position.GetY())) };

    auto positionObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, valuesOfPosition);
    panda::Local<panda::JSValueRef> params[1] = { positionObj };
    func->Call(vm, obj.ToLocal(), params, 1);
}

static ArkUINodeType ParseNodeType(
    ArkUIRuntimeCallInfo* runtimeCallInfo, EcmaVM* vm, int32_t nodeId, RefPtr<FrameNode>& node)
{
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(1);
    std::string type = firstArg->IsString(vm) ? firstArg->ToString(vm)->ToString(vm) : "";
    static const std::unordered_map<std::string, ArkUINodeType> typeMap = { { "Text", ARKUI_TEXT },
        { "Column", ARKUI_COLUMN }, { "Row", ARKUI_ROW }, { "Stack", ARKUI_STACK }, { "Blank", ARKUI_BLANK },
        { "Image", ARKUI_IMAGE }, { "GridRow", ARKUI_GRID_ROW }, { "GridCol", ARKUI_GRID_COL }, { "Flex", ARKUI_FLEX },
        { "Swiper", ARKUI_SWIPER }, { "Progress", ARKUI_PROGRESS }, { "List", ARKUI_LIST },
        { "ListItem", ARKUI_LIST_ITEM }, { "Scroll", ARKUI_SCROLL }, { "RelativeContainer", ARKUI_RELATIVE_CONTAINER },
        { "Divider", ARKUI_DIVIDER }, { "LoadingProgress", ARKUI_LOADING_PROGRESS }, { "TextInput", ARKUI_TEXT_INPUT },
        { "Search", ARKUI_SEARCH }, { "Button", ARKUI_BUTTON }, { "XComponent", ARKUI_XCOMPONENT },
        { "ListItemGroup", ARKUI_LIST_ITEM_GROUP }, { "WaterFlow", ARKUI_WATER_FLOW },
        { "FlowItem", ARKUI_FLOW_ITEM }, { "QRCode", ARKUI_QRCODE }, { "Badge", ARKUI_BADGE }, { "Grid", ARKUI_GRID },
        { "GridItem", ARKUI_GRID_ITEM }, { "SymbolGlyph", ARKUI_SYMBOL_GLYPH}, { "TextClock", ARKUI_TEXT_CLOCK },
        { "TextTimer", ARKUI_TEXT_TIMER }, { "Marquee", ARKUI_MARQUEE }, { "TextArea", ARKUI_TEXTAREA },
        { "Checkbox", ARKUI_CHECKBOX }, {"CheckboxGroup", ARKUI_CHECK_BOX_GROUP }, { "Rating", ARKUI_RATING},
        { "Radio", ARKUI_RADIO }, { "Slider", ARKUI_SLIDER }, { "Select", ARKUI_SELECT }, { "Toggle", ARKUI_TOGGLE },
        { "EmbeddedComponent", ARKUI_EMBEDDED_COMPONENT } };
    ArkUINodeType nodeType = ARKUI_CUSTOM;
    auto iter = typeMap.find(type);
    if (iter != typeMap.end()) {
        nodeType = iter->second;
    }
    return nodeType;
}

static void HandleNodeParams(
    ArkUIRuntimeCallInfo* runtimeCallInfo, ArkUINodeType nodeType, int32_t nodeId, RefPtr<FrameNode>& node)
{
    ArkUINodeHandle nodePtr = nullptr;
    if (nodeType == ARKUI_XCOMPONENT) {
#ifdef XCOMPONENT_SUPPORTED
        ArkUI_XComponent_Params params;
        XComponentBridge::ParseParams(runtimeCallInfo, params);
        params.nodeType = ARKUI_XCOMPONENT;
        nodePtr = GetArkUIFullNodeAPI()->getBasicAPI()->createNodeWithParams(nodeType, nodeId, 0, params);
        XComponentBridge::SetControllerCallback(runtimeCallInfo, reinterpret_cast<FrameNode*>(nodePtr));
#else
        nodePtr = GetArkUIFullNodeAPI()->getBasicAPI()->createNode(nodeType, nodeId, 0);
#endif
    } else if (nodeType == ARKUI_TOGGLE) {
        ArkUI_Toggle_Params params;
        params.nodeType = ARKUI_TOGGLE;
        ToggleBridge::ParseParams(runtimeCallInfo, params);
        nodePtr = GetArkUIFullNodeAPI()->getBasicAPI()->createNodeWithParams(nodeType, nodeId, 0, params);
    } else {
        nodePtr = GetArkUIFullNodeAPI()->getBasicAPI()->createNode(nodeType, nodeId, 0);
    }
    if (nodePtr) {
        node = AceType::Claim(reinterpret_cast<FrameNode*>(nodePtr));
        node->SetIsArkTsFrameNode(true);
        auto renderContext = node->GetRenderContext();
        if (renderContext) {
            renderContext->SetNeedDebugBoundary(true);
        }
        // let 'node' take the reference, so decrease ref of C node
        node->DecRefCount();
    }
}

ArkUINativeModuleValue FrameNodeBridge::CreateTypedFrameNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    RefPtr<FrameNode> node;
    ArkUINodeType nodeType = ParseNodeType(runtimeCallInfo, vm, nodeId, node);
    if (nodeType != ARKUI_CUSTOM) {
        HandleNodeParams(runtimeCallInfo, nodeType, nodeId, node);
    }

    const char* keys[] = { "nodeId", "nativeStrongRef" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, nodeId), NativeUtilsBridge::CreateStrongRef(vm, node) };
    auto reslut = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    return reslut;
} // namespace OHOS::Ace::NG

ArkUINativeModuleValue FrameNodeBridge::Invalidate(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->invalidate(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

void FrameNodeBridge::SetDrawFunc(const RefPtr<FrameNode>& frameNode, ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    CHECK_NULL_VOID(frameNode);
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_VOID(firstArg->IsObject(vm));
    auto obj = Local<panda::ObjectRef>(firstArg);
    auto funcName = panda::StringRef::NewFromUtf8(vm, "onDraw");
    auto funcObj = obj->Get(vm, funcName);
    CHECK_NULL_VOID(funcObj->IsFunction(vm));
    auto drawCallback = [vm, object = JsWeak(panda::CopyableGlobal(vm, obj))](NG::DrawingContext& context) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        auto funcName = panda::StringRef::NewFromUtf8(vm, "onDraw");
        RenderNodeBridge::FireDrawCallback(vm, object, context, funcName);
    };
    auto pattern = frameNode->GetPattern<NG::CustomFrameNodePattern>();
    pattern->SetDrawCallback(std::move(drawCallback));
}

ArkUINativeModuleValue FrameNodeBridge::AddBuilderNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto childNativeNode = nodePtr(secondArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->addBuilderNode(nativeNode, childNativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::AppendChild(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, true));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto childNativeNode = nodePtr(secondArg->ToNativePointer(vm)->Value());
    auto result = GetArkUINodeModifiers()->getFrameNodeModifier()->appendChild(nativeNode, childNativeNode);
    return panda::BooleanRef::New(vm, result);
}
ArkUINativeModuleValue FrameNodeBridge::InsertChildAfter(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, true));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto child = nodePtr(secondArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(2); // 2 : index to get the insert node
    if (thirdArg.IsNull()) {
        GetArkUINodeModifiers()->getFrameNodeModifier()->insertChildAfter(nativeNode, child, nullptr);
        return panda::JSValueRef::Undefined(vm);
    }
    auto sibling = nodePtr(thirdArg->ToNativePointer(vm)->Value());
    auto result = GetArkUINodeModifiers()->getFrameNodeModifier()->insertChildAfter(nativeNode, child, sibling);
    return panda::BooleanRef::New(vm, result);
}
ArkUINativeModuleValue FrameNodeBridge::RemoveBuilderNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto childNativeNode = nodePtr(secondArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->removeBuilderNode(nativeNode, childNativeNode);
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue FrameNodeBridge::RemoveChild(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto childNativeNode = nodePtr(secondArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->removeChild(nativeNode, childNativeNode);
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue FrameNodeBridge::ClearBuilderNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    GetArkUINodeModifiers()->getFrameNodeModifier()->clearBuilderNode(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue FrameNodeBridge::ClearChildren(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->clearChildren(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::GetChildrenCount(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, 0));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    int isExpanded = GetIsExpanded(runtimeCallInfo, 1);
    auto number = GetArkUINodeModifiers()->getFrameNodeModifier()->getChildrenCount(nativeNode, isExpanded);
    return panda::NumberRef::New(vm, number);
}

ArkUINativeModuleValue FrameNodeBridge::GetChild(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    int index = secondArg->ToNumber(vm)->Value();
    int expandMode = GetExpandMode(runtimeCallInfo, 2);
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getChild(nativeNode, index, expandMode);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return FrameNodeBridge::MakeFrameNodeInfo(vm, nodePtr);
}

ArkUINativeModuleValue FrameNodeBridge::GetFirstChildIndexWithoutExpand(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, -1));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    uint32_t index = -1;
    GetArkUINodeModifiers()->getFrameNodeModifier()->getFirstChildIndexWithoutExpand(nativeNode, &index);
    return panda::NumberRef::New(vm, index);
}

ArkUINativeModuleValue FrameNodeBridge::GetLastChildIndexWithoutExpand(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, -1));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    uint32_t index = -1;
    GetArkUINodeModifiers()->getFrameNodeModifier()->getLastChildIndexWithoutExpand(nativeNode, &index);
    return panda::NumberRef::New(vm, index);
}

ArkUINativeModuleValue FrameNodeBridge::GetFirst(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    int isExpanded = GetIsExpanded(runtimeCallInfo, 1);
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getFirst(nativeNode, isExpanded);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return FrameNodeBridge::MakeFrameNodeInfo(vm, nodePtr);
}

ArkUINativeModuleValue FrameNodeBridge::GetNextSibling(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    int isExpanded = GetIsExpanded(runtimeCallInfo, 1);
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getNextSibling(nativeNode, isExpanded);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return FrameNodeBridge::MakeFrameNodeInfo(vm, nodePtr);
}

ArkUINativeModuleValue FrameNodeBridge::GetPreviousSibling(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    int isExpanded = GetIsExpanded(runtimeCallInfo, 1);
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getPreviousSibling(nativeNode, isExpanded);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return FrameNodeBridge::MakeFrameNodeInfo(vm, nodePtr);
}

ArkUINativeModuleValue FrameNodeBridge::GetParent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getParent(nativeNode);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return FrameNodeBridge::MakeFrameNodeInfo(vm, nodePtr);
}
ArkUINativeModuleValue FrameNodeBridge::MoveTo(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, true));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto targetNativeNode = nodePtr(secondArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(2); // 2 : index of child node position
    int index = thirdArg->ToNumber(vm)->Value();
    GetArkUINodeModifiers()->getFrameNodeModifier()->moveNodeTo(nativeNode, targetNativeNode, index);
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue FrameNodeBridge::GetIdByNodePtr(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, -1));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto nodeId = GetArkUINodeModifiers()->getFrameNodeModifier()->getIdByNodePtr(nativeNode);
    return panda::NumberRef::New(vm, nodeId);
}
static int32_t GetOperatingHand(GestureEvent& info)
{
    int32_t left = 0;
    int32_t right = 0;
    for (const FingerInfo& fingerInfo : info.GetFingerList()) {
        if (fingerInfo.operatingHand_ == HAND_LEFT) {
            ++left;
        } else if (fingerInfo.operatingHand_ == HAND_RIGHT) {
            ++right;
        }
    }
    if (left > right) {
        return HAND_LEFT;
    } else if (right > left) {
        return HAND_RIGHT;
    }
    return HAND_NONE;
}
Local<panda::ObjectRef> FrameNodeBridge::CreateGestureEventInfo(EcmaVM* vm, GestureEvent& info)
{
    const Offset& globalOffset = info.GetGlobalLocation();
    const Offset& localOffset = info.GetLocalLocation();
    const Offset& screenOffset = info.GetScreenLocation();
    double density = PipelineBase::GetCurrentDensity();
    const Offset& globalDisplayOffset = info.GetGlobalDisplayLocation();
    const char* keys[] = { "displayX", "displayY", "windowX", "windowY", "screenX", "screenY", "x", "y", "timestamp",
        "source", "pressure", "deviceId", "hand", "globalDisplayX", "globalDisplayY" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, screenOffset.GetX() / density),
        panda::NumberRef::New(vm, screenOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, localOffset.GetX() / density),
        panda::NumberRef::New(vm, localOffset.GetY() / density),
        panda::NumberRef::New(vm, static_cast<double>(info.GetTimeStamp().time_since_epoch().count())),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetSourceDevice())),
        panda::NumberRef::New(vm, info.GetForce()),
        panda::NumberRef::New(vm, info.GetDeviceId()),
        panda::NumberRef::New(vm, GetOperatingHand(info)),
        panda::NumberRef::New(vm, globalDisplayOffset.GetX() / density),
        panda::NumberRef::New(vm, globalDisplayOffset.GetY() / density) };
    auto obj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    obj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "targetDisplayId"), panda::NumberRef::New(vm, info.GetTargetDisplayId()));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltX"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltX().value_or(0.0f))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltY"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltY().value_or(0.0f))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "rollAngle"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetRollAngle().value_or(0.0f))));
    obj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisVertical"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    obj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisHorizontal"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "sourceTool"),
        panda::NumberRef::New(vm, static_cast<int32_t>(static_cast<int32_t>(info.GetSourceTool()))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "target"), CreateEventTargetObject(vm, info));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "getModifierKeyState"),
        panda::FunctionRef::New(vm, ArkTSUtils::JsGetModifierKeyState));
    obj->SetNativePointerFieldCount(vm, 1);
    obj->SetNativePointerField(vm, 0, static_cast<void*>(&info));
    return obj;
}

ArkUINativeModuleValue FrameNodeBridge::SetOnClick(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnClick(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onClick = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId](GestureEvent& info) {
        panda::JsiFastNativeScope fastNativeScope(vm);
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto obj = CreateGestureEventInfo(vm, info);
        panda::Local<panda::JSValueRef> params[1] = { obj };
        function->Call(vm, function.ToLocal(), params, 1);
    };
    NG::ViewAbstract::SetJSFrameNodeOnClick(frameNode, std::move(onClick));
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateTouchEventInfoObj(EcmaVM* vm, TouchEventInfo& info)
{
    const char* keys[] = { "source", "timestamp", "target", "pressure", "deviceId" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, static_cast<int32_t>(info.GetSourceDevice())),
        panda::NumberRef::New(vm, static_cast<double>(info.GetTimeStamp().time_since_epoch().count())),
        CreateEventTargetObject(vm, info), panda::NumberRef::New(vm, info.GetForce()),
        panda::NumberRef::New(vm, info.GetDeviceId()) };
    return panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateTouchEventInfo(EcmaVM* vm, TouchEventInfo& info)
{
    panda::JsiFastNativeScope fastNativeScope(vm);
    auto eventObj = CreateTouchEventInfoObj(vm, info);
    eventObj->SetNativePointerFieldCount(vm, 1);
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltX"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltX().value_or(0.0f))));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltY"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltY().value_or(0.0f))));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "rollAngle"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetRollAngle().value_or(0.0f))));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "sourceTool"),
        panda::NumberRef::New(vm, static_cast<int32_t>(static_cast<int32_t>(info.GetSourceTool()))));
    auto touchArr = panda::ArrayRef::New(vm);
    const std::list<TouchLocationInfo>& touchList = info.GetTouches();
    uint32_t idx = 0;
    for (const TouchLocationInfo& location : touchList) {
        panda::ArrayRef::SetValueAt(vm, touchArr, idx++, CreateTouchInfo(vm, location, info));
    }
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "touches"), touchArr);
    auto changeTouchArr = panda::ArrayRef::New(vm);
    idx = 0; // reset index counter
    const std::list<TouchLocationInfo>& changeTouch = info.GetChangedTouches();
    for (const TouchLocationInfo& change : changeTouch) {
        panda::ArrayRef::SetValueAt(vm, changeTouchArr, idx++, CreateTouchInfo(vm, change, info));
    }
    if (changeTouch.size() > 0) {
        eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "type"),
            panda::NumberRef::New(vm, static_cast<int32_t>(changeTouch.front().GetTouchType())));
    }
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "changedTouches"), changeTouchArr);
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "stopPropagation"),
        panda::FunctionRef::New(vm, Framework::JsStopPropagation));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "getHistoricalPoints"),
        panda::FunctionRef::New(vm, Framework::JsGetHistoricalPoints));
    eventObj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisVertical"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    eventObj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisHorizontal"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "preventDefault"),
        panda::FunctionRef::New(vm, Framework::JsTouchPreventDefault));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "getModifierKeyState"),
        panda::FunctionRef::New(vm, ArkTSUtils::JsGetModifierKeyState));
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "targetDisplayId"),
        panda::NumberRef::New(vm, info.GetTargetDisplayId()));
    eventObj->SetNativePointerField(vm, 0, static_cast<void*>(&info));
    return eventObj;
}

ArkUINativeModuleValue FrameNodeBridge::SetOnTouch(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnTouch(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = Container::CurrentId();
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onTouch = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId](TouchEventInfo& info) {
        panda::JsiFastNativeScope fastNativeScope(vm);
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto eventObj = CreateTouchEventInfo(vm, info);
        panda::Local<panda::JSValueRef> params[1] = { eventObj };
        function->Call(vm, function.ToLocal(), params, 1);
    };
    NG::ViewAbstract::SetJSFrameNodeOnTouch(frameNode, std::move(onTouch));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnAppear(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnAppear(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onAppear = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                        node = AceType::WeakClaim(frameNode), containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnAppear(frameNode, std::move(onAppear));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnDisappear(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnDisappear(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onDisappear = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                           node = AceType::WeakClaim(frameNode), containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnDisappear(frameNode, std::move(onDisappear));
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateKeyEventInfoObj(EcmaVM* vm, KeyEventInfo& info)
{
    const char* keys[] = { "type", "keyCode", "keyText", "keySource", "deviceId", "metaKey", "unicode",
        "timestamp", "stopPropagation", "getModifierKeyState", "intentionCode", "isNumLockOn", "isCapsLockOn",
        "isScrollLockOn" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, static_cast<int32_t>(info.GetKeyType())),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetKeyCode())),
        panda::StringRef::NewFromUtf8(vm, info.GetKeyText().c_str()),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetKeySource())),
        panda::NumberRef::New(vm, info.GetDeviceId()), panda::NumberRef::New(vm, info.GetMetaKey()),
        panda::NumberRef::New(vm, info.GetUnicode()),
        panda::NumberRef::New(vm, static_cast<double>(info.GetTimeStamp().time_since_epoch().count())),
        panda::FunctionRef::New(vm, Framework::JsStopPropagation),
        panda::FunctionRef::New(vm, NG::ArkTSUtils::JsGetModifierKeyState),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetKeyIntention())),
        panda::BooleanRef::New(vm, info.GetNumLock()),
        panda::BooleanRef::New(vm, info.GetCapsLock()),
        panda::BooleanRef::New(vm, info.GetScrollLock()) };
    return panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnKeyEvent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnKeyCallback(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onKeyEvent = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                          node = AceType::WeakClaim(frameNode), containerId](KeyEventInfo& info) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto obj = CreateKeyEventInfoObj(vm, info);
        obj->SetNativePointerFieldCount(vm, 1);
        obj->SetNativePointerField(vm, 0, static_cast<void*>(&info));
        panda::Local<panda::JSValueRef> params[] = { obj };
        function->Call(vm, function.ToLocal(), params, 1);
    };

    NG::ViewAbstract::SetJSFrameNodeOnKeyCallback(frameNode, std::move(onKeyEvent));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnFocus(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnFocusCallback(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onFocus = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnFocusCallback(frameNode, std::move(onFocus));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnBlur(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnBlurCallback(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onBlur = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag), node = AceType::WeakClaim(frameNode),
                      containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnBlurCallback(frameNode, std::move(onBlur));
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateHoverInfo(EcmaVM* vm, HoverInfo& hoverInfo)
{
    const char* keys[] = { "stopPropagation", "getModifierKeyState", "timestamp", "source", "target", "deviceId",
        "displayX", "displayY", "windowX", "windowY", "x", "y", "globalDisplayX", "globalDisplayY", };
    double density = PipelineBase::GetCurrentDensity();
    const Offset& globalOffset = hoverInfo.GetGlobalLocation();
    const Offset& localOffset = hoverInfo.GetLocalLocation();
    const Offset& screenOffset = hoverInfo.GetScreenLocation();
    const Offset& globalDisplayOffset = hoverInfo.GetGlobalDisplayLocation();
    Local<JSValueRef> values[] = { panda::FunctionRef::New(vm, Framework::JsStopPropagation),
        panda::FunctionRef::New(vm, ArkTSUtils::JsGetModifierKeyState),
        panda::NumberRef::New(vm, static_cast<double>(hoverInfo.GetTimeStamp().time_since_epoch().count())),
        panda::NumberRef::New(vm, static_cast<int32_t>(hoverInfo.GetSourceDevice())),
        CreateEventTargetObject(vm, hoverInfo),
        panda::NumberRef::New(vm, static_cast<int32_t>(hoverInfo.GetDeviceId())),
        panda::NumberRef::New(vm, density != 0 ? screenOffset.GetX() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? screenOffset.GetY() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? globalOffset.GetX() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? globalOffset.GetY() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? localOffset.GetX() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? localOffset.GetY() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? globalDisplayOffset.GetX() / density : 0),
        panda::NumberRef::New(vm, density != 0 ? globalDisplayOffset.GetY() / density : 0) };
    auto eventObj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    eventObj->Set(vm, panda::StringRef::NewFromUtf8(vm, "rollAngle"),
        panda::NumberRef::New(vm, static_cast<int32_t>(hoverInfo.GetRollAngle().value_or(0.0f))));
    return eventObj;
}

ArkUINativeModuleValue FrameNodeBridge::SetOnHover(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnHover(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onHover = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId](bool isHover, HoverInfo& hoverInfo) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto isHoverParam = panda::BooleanRef::New(vm, isHover);
        auto obj = CreateHoverInfo(vm, hoverInfo);
        obj->SetNativePointerFieldCount(vm, 1);
        obj->SetNativePointerField(vm, 0, static_cast<void*>(&hoverInfo));
        panda::Local<panda::JSValueRef> params[] = { isHoverParam, obj };
        function->Call(vm, function.ToLocal(), params, ArraySize(params));
    };
    NG::ViewAbstract::SetJSFrameNodeOnHover(frameNode, std::move(onHover));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnHoverMove(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnHoverMove(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onHoverMove = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId](HoverInfo& hoverInfo) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto obj = CreateHoverInfo(vm, hoverInfo);
        obj->SetNativePointerFieldCount(vm, 1);
        obj->SetNativePointerField(vm, 0, static_cast<void*>(&hoverInfo));
        panda::Local<panda::JSValueRef> params[] = { obj };
        function->Call(vm, function.ToLocal(), params, ArraySize(params));
    };
    NG::ViewAbstract::SetJSFrameNodeOnHoverMove(frameNode, std::move(onHoverMove));
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateMouseInfoObj(EcmaVM* vm, MouseInfo& info)
{
    const Offset& globalOffset = info.GetGlobalLocation();
    const Offset& localOffset = info.GetLocalLocation();
    const Offset& screenOffset = info.GetScreenLocation();
    const Offset& globalDisplayOffset = info.GetGlobalDisplayLocation();
    double density = PipelineBase::GetCurrentDensity();
    const char* keys[] = { "button", "action", "displayX", "displayY", "windowX", "windowY", "screenX", "screenY", "x",
        "y", "timestamp", "stopPropagation", "getModifierKeyState", "source", "pressure", "deviceId", "rawDeltaX",
        "rawDeltaY", "targetDisplayId", "globalDisplayX", "globalDisplayY" };
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, static_cast<int32_t>(info.GetButton())),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetAction())),
        panda::NumberRef::New(vm, screenOffset.GetX() / density),
        panda::NumberRef::New(vm, screenOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, globalOffset.GetX() / density),
        panda::NumberRef::New(vm, globalOffset.GetY() / density),
        panda::NumberRef::New(vm, localOffset.GetX() / density),
        panda::NumberRef::New(vm, localOffset.GetY() / density),
        panda::NumberRef::New(vm, static_cast<double>(info.GetTimeStamp().time_since_epoch().count())),
        panda::FunctionRef::New(vm, Framework::JsStopPropagation),
        panda::FunctionRef::New(vm, ArkTSUtils::JsGetModifierKeyState),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetSourceDevice())),
        panda::NumberRef::New(vm, info.GetForce()), panda::NumberRef::New(vm, info.GetDeviceId()),
        panda::NumberRef::New(vm, info.GetRawDeltaX() / density),
        panda::NumberRef::New(vm, info.GetRawDeltaY() / density),
        panda::NumberRef::New(vm, info.GetTargetDisplayId()),
        panda::NumberRef::New(vm, globalDisplayOffset.GetX() / density),
        panda::NumberRef::New(vm, globalDisplayOffset.GetY() / density) };
    return panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
}

Local<panda::ObjectRef> FrameNodeBridge::CreateMouseInfo(EcmaVM* vm, MouseInfo& info)
{
    auto obj = CreateMouseInfoObj(vm, info);
    obj->SetNativePointerFieldCount(vm, 1);
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltX"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltX().value_or(0.0f))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "tiltY"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetTiltY().value_or(0.0f))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "rollAngle"),
        panda::NumberRef::New(vm, static_cast<int32_t>(info.GetRollAngle().value_or(0.0f))));
    obj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisVertical"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    obj->Set(
        vm, panda::StringRef::NewFromUtf8(vm, "axisHorizontal"), panda::NumberRef::New(vm, static_cast<int32_t>(0.0f)));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "sourceTool"),
        panda::NumberRef::New(vm, static_cast<int32_t>(static_cast<int32_t>(info.GetSourceTool()))));
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "target"), CreateEventTargetObject(vm, info));
    obj->SetNativePointerField(vm, 0, static_cast<void*>(&info));
    auto pressedButtonArr = panda::ArrayRef::New(vm);
    uint32_t idx = 0;
    for (const auto& button : info.GetPressedButtons()) {
        panda::ArrayRef::SetValueAt(
            vm, pressedButtonArr, idx++, panda::NumberRef::New(vm, static_cast<int32_t>(button)));
    }
    obj->Set(vm, panda::StringRef::NewFromUtf8(vm, "pressedButtons"), pressedButtonArr);
    return obj;
}

ArkUINativeModuleValue FrameNodeBridge::SetOnMouse(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnMouse(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onMouse = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                       node = AceType::WeakClaim(frameNode), containerId](MouseInfo& info) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        auto obj = CreateMouseInfo(vm, info);
        panda::Local<panda::JSValueRef> params[1] = { obj };
        function->Call(vm, function.ToLocal(), params, 1);
    };
    NG::ViewAbstract::SetJSFrameNodeOnMouse(frameNode, std::move(onMouse));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToParent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 parentOffset[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToParent(nativeNode, &parentOffset, true);
    CHECK_NULL_RETURN(parentOffset, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, parentOffset[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, parentOffset[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToWindow(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 windowOffset[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToWindow(nativeNode, &windowOffset, true);
    CHECK_NULL_RETURN(windowOffset, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, windowOffset[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, windowOffset[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToScreen(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 screenPosition[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToScreen(nativeNode, &screenPosition, true);
    CHECK_NULL_RETURN(screenPosition, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, screenPosition[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, screenPosition[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetGlobalPositionOnDisplay(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, SIZE_OF_ARRAY);
    ArkUI_Float32 globalDisplayPosition[SIZE_OF_ARRAY];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getGlobalPositionOnDisplay(
        nativeNode, &globalDisplayPosition, true);
    CHECK_NULL_RETURN(globalDisplayPosition, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, globalDisplayPosition[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, globalDisplayPosition[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToParentWithTransform(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 parentPosition[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToParentWithTransform(
        nativeNode, &parentPosition, true);
    CHECK_NULL_RETURN(parentPosition, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, parentPosition[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, parentPosition[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToScreenWithTransform(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 screenPosition[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToScreenWithTransform(
        nativeNode, &screenPosition, true);
    CHECK_NULL_RETURN(screenPosition, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, screenPosition[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, screenPosition[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetPositionToWindowWithTransform(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    ArkUI_Float32 windowPosition[2];
    GetArkUINodeModifiers()->getFrameNodeModifier()->getPositionToWindowWithTransform(
        nativeNode, &windowPosition, true);
    CHECK_NULL_RETURN(windowPosition, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, windowPosition[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, windowPosition[1]));
    return valueArray;
}

ArkUINativeModuleValue FrameNodeBridge::GetMeasuredSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    auto size = GetArkUINodeModifiers()->getFrameNodeModifier()->getMeasuredSize(nativeNode);
    CHECK_NULL_RETURN(size, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, size[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, size[1]));
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::SetOnAttach(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::DisableOnAttach(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto onAttach = [vm, func = JsWeak(panda::CopyableGlobal(vm, func)), node = AceType::WeakClaim(frameNode),
                        containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetOnAttach(frameNode, std::move(onAttach));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnDetach(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::DisableOnDetach(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto onDetach = [vm, func = JsWeak(panda::CopyableGlobal(vm, func)), node = AceType::WeakClaim(frameNode),
                        containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetOnDetach(frameNode, std::move(onDetach));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::GetLayoutPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 2);
    auto position = GetArkUINodeModifiers()->getFrameNodeModifier()->getLayoutPosition(nativeNode);
    CHECK_NULL_RETURN(position, panda::JSValueRef::Undefined(vm));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 0, panda::NumberRef::New(vm, position[0]));
    Framework::ArrayRef::SetValueAt(vm, valueArray, 1, panda::NumberRef::New(vm, position[1]));
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::GetConfigBorderWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 8);
    ArkUI_Float32 borderWidthValue[4];
    ArkUI_Int32 borderWidthUnit[4];
    GetArkUINodeModifiers()->getCommonModifier()->getBorderWidthDimension(
        nativeNode, &borderWidthValue, &borderWidthUnit);
    for (int i = 0; i < 4; i++) {
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2, panda::NumberRef::New(vm, borderWidthValue[i]));
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2 + 1, panda::NumberRef::New(vm, borderWidthUnit[i]));
    }
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::GetConfigPadding(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 8);
    ArkUI_Float32 paddingValue[4];
    ArkUI_Int32 paddingUnit[4];
    GetArkUINodeModifiers()->getCommonModifier()->getPaddingDimension(nativeNode, &paddingValue, &paddingUnit);
    for (int i = 0; i < 4; i++) {
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2, panda::NumberRef::New(vm, paddingValue[i]));
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2 + 1, panda::NumberRef::New(vm, paddingUnit[i]));
    }
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::GetConfigMargin(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 8);
    ArkUI_Float32 marginValue[4];
    ArkUI_Int32 marginUnit[4];
    GetArkUINodeModifiers()->getCommonModifier()->getMarginDimension(nativeNode, &marginValue, &marginUnit);
    for (int i = 0; i < 4; i++) {
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2, panda::NumberRef::New(vm, marginValue[i]));
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2 + 1, panda::NumberRef::New(vm, marginUnit[i]));
    }
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::GetConfigSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<Framework::ArrayRef> valueArray = Framework::ArrayRef::New(vm, 4);
    ArkUI_Float32 sizeValue[2];
    ArkUI_Int32 sizeUnit[2];
    GetArkUINodeModifiers()->getCommonModifier()->getConfigSize(nativeNode, &sizeValue, &sizeUnit);
    for (int i = 0; i < 2; i++) {
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2, panda::NumberRef::New(vm, sizeValue[i]));
        Framework::ArrayRef::SetValueAt(vm, valueArray, i * 2 + 1, panda::NumberRef::New(vm, sizeUnit[i]));
    }
    return valueArray;
}
ArkUINativeModuleValue FrameNodeBridge::GetId(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::StringRef::NewFromUtf8(vm, ""));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::StringRef::NewFromUtf8(vm, ""));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto inspectorId = GetArkUINodeModifiers()->getFrameNodeModifier()->getInspectorId(nativeNode);
    return panda::StringRef::NewFromUtf8(vm, inspectorId);
}
ArkUINativeModuleValue FrameNodeBridge::GetNodeType(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::StringRef::NewFromUtf8(vm, ""));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::StringRef::NewFromUtf8(vm, ""));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto nodeType = GetArkUINodeModifiers()->getFrameNodeModifier()->getNodeType(nativeNode);
    return panda::StringRef::NewFromUtf8(vm, nodeType);
}
ArkUINativeModuleValue FrameNodeBridge::GetOpacity(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NumberRef::New(vm, 0));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, 0));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto opacity = GetArkUINodeModifiers()->getCommonModifier()->getOpacity(nativeNode);
    return panda::NumberRef::New(vm, opacity);
}
ArkUINativeModuleValue FrameNodeBridge::IsVisible(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::BooleanRef::New(vm, false));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto visible = GetArkUINodeModifiers()->getFrameNodeModifier()->isVisible(nativeNode);
    return panda::BooleanRef::New(vm, visible);
}
ArkUINativeModuleValue FrameNodeBridge::IsClipToFrame(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::BooleanRef::New(vm, false));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto clip = GetArkUINodeModifiers()->getCommonModifier()->getClip(nativeNode);
    return panda::BooleanRef::New(vm, clip != 0);
}
ArkUINativeModuleValue FrameNodeBridge::IsAttached(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::BooleanRef::New(vm, false));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    bool isAttached = GetArkUINodeModifiers()->getFrameNodeModifier()->isVisible(nativeNode);
    return panda::BooleanRef::New(vm, isAttached);
}
ArkUINativeModuleValue FrameNodeBridge::GetInspectorInfo(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::StringRef::NewFromUtf8(vm, "{}"));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::StringRef::NewFromUtf8(vm, "{}"));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto inspectorInfo = GetArkUINodeModifiers()->getFrameNodeModifier()->getInspectorInfo(nativeNode);
    return panda::StringRef::NewFromUtf8(vm, inspectorInfo);
}
ArkUINativeModuleValue FrameNodeBridge::GetCustomPropertyCapiByKey(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(!secondArg.IsNull(), defaultReturnValue);
    if (!secondArg->IsString(vm)) {
        return defaultReturnValue;
    }
    auto key = secondArg->ToString(vm)->ToString(vm);
    uint32_t size = 0;
    char* valuePtr = nullptr;
    if (GetArkUINodeModifiers()->getFrameNodeModifier()->getCustomPropertyCapiByKey(
        nativeNode, key.c_str(), &valuePtr, &size)) {
        CHECK_NULL_RETURN(valuePtr, defaultReturnValue);
        auto returnValue = panda::StringRef::NewFromUtf8(vm, valuePtr);
        GetArkUINodeModifiers()->getFrameNodeModifier()->freeCustomPropertyCharPtr(valuePtr, size);
        return returnValue;
    }
    GetArkUINodeModifiers()->getFrameNodeModifier()->freeCustomPropertyCharPtr(valuePtr, size);
    return defaultReturnValue;
}

std::function<std::string(const std::string&)> ParseGetFunc(ArkUIRuntimeCallInfo* runtimeCallInfo, int32_t nodeId)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    return [vm, nodeId](const std::string& key) -> std::string {
        std::string resultString = std::string();
        CHECK_NULL_RETURN(vm, resultString);
        panda::LocalScope scope(vm);
        auto global = JSNApi::GetGlobalObject(vm);
        if (global.IsNull()) {
            return resultString;
        }
        auto getCustomProperty = global->Get(vm, panda::StringRef::NewFromUtf8(vm, "__getCustomPropertyString__"));
        if (getCustomProperty->IsUndefined() || !getCustomProperty->IsFunction(vm)) {
            return resultString;
        }
        auto obj = getCustomProperty->ToObject(vm);
        panda::Local<panda::FunctionRef> func = obj;
        panda::Local<panda::JSValueRef> params2[2] = { panda::NumberRef::New(vm, nodeId), // 2 number of parameters
            panda::StringRef::NewFromUtf8(vm, key.c_str()) };
        auto function = panda::CopyableGlobal(vm, func);
        auto callValue = function->Call(vm, function.ToLocal(), params2, 2);
        if (callValue.IsNull() || callValue->IsUndefined() || !callValue->IsString(vm)) {
            return resultString;
        }
        auto value = callValue->ToString(vm)->ToString(vm);
        return value;
    };
}

std::function<std::string()> JsGetCustomMapFunc(ArkUIRuntimeCallInfo* runtimeCallInfo, int32_t nodeId)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    return [vm, nodeId]() -> std::string {
        std::string resultString = std::string();
        CHECK_NULL_RETURN(vm, resultString);
        panda::LocalScope scope(vm);
        auto global = JSNApi::GetGlobalObject(vm);
        if (global.IsNull()) {
            return resultString;
        }
        auto getCustomProperty = global->Get(vm, panda::StringRef::NewFromUtf8(vm, "__getCustomPropertyMapString__"));
        if (getCustomProperty->IsUndefined() || !getCustomProperty->IsFunction(vm)) {
            return resultString;
        }
        auto obj = getCustomProperty->ToObject(vm);
        panda::Local<panda::FunctionRef> func = obj;
        panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, nodeId) };
        auto function = panda::CopyableGlobal(vm, func);
        auto callValue = function->Call(vm, function.ToLocal(), params, 1);
        if (callValue.IsNull() || callValue->IsUndefined() || !callValue->IsString(vm)) {
            return resultString;
        }
        auto value = callValue->ToString(vm)->ToString(vm);
        return value;
    };
}

std::function<bool()> ParseFunc(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> keyArg = runtimeCallInfo->GetCallArgRef(1); // 1 key
    Local<JSValueRef> valueArg = runtimeCallInfo->GetCallArgRef(2); // 2 value
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), nullptr);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, nullptr);
    auto nodeId = frameNode->GetId();
    panda::Local<panda::JSValueRef> params3[3] = { panda::NumberRef::New(vm, nodeId), keyArg, // 3 number of parameters
        valueArg };
    return [vm, frameNode, params3]() -> bool {
        CHECK_NULL_RETURN(vm, false);
        panda::LocalScope scope(vm);
        auto global = JSNApi::GetGlobalObject(vm);
        auto setCustomProperty = global->Get(vm, panda::StringRef::NewFromUtf8(vm, "__setCustomProperty__"));
        if (setCustomProperty->IsUndefined() || !setCustomProperty->IsFunction(vm)) {
            return false;
        }
        auto obj = setCustomProperty->ToObject(vm);
        panda::Local<panda::FunctionRef> func = obj;
        auto nodeId = frameNode->GetId();
        auto function = panda::CopyableGlobal(vm, func);
        auto customPropertyExisted = function->Call(vm, function.ToLocal(), params3, 3)->ToBoolean(vm)->Value();
        if (customPropertyExisted) {
            frameNode->SetCustomPropertyMapFlagByKey(params3[1]->ToString(vm)->ToString(vm));
            frameNode->SetRemoveCustomProperties([vm, nodeId]() -> void {
                CHECK_NULL_VOID(vm);
                panda::LocalScope scope(vm);
                auto global = JSNApi::GetGlobalObject(vm);
                auto removeCustomProperty =
                    global->Get(vm, panda::StringRef::NewFromUtf8(vm, "__removeCustomProperties__"));
                if (removeCustomProperty->IsUndefined() || !removeCustomProperty->IsFunction(vm)) {
                    return;
                }
                auto obj = removeCustomProperty->ToObject(vm);
                panda::Local<panda::FunctionRef> func = obj;
                panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, nodeId) };
                auto function = panda::CopyableGlobal(vm, func);
                function->Call(vm, function.ToLocal(), params, 1);
            });
        }
        return true;
    };
}

ArkUINativeModuleValue FrameNodeBridge::SetCustomPropertyModiferByKey(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::BooleanRef::New(vm, false));
    if (!AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_THIRTEEN)) {
        return panda::BooleanRef::New(vm, false);
    }
    auto defaultReturnValue = panda::BooleanRef::New(vm, true);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> keyArg = runtimeCallInfo->GetCallArgRef(1); // 1 key
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, defaultReturnValue);
    if (keyArg->IsUndefined() || keyArg->IsNull() || !keyArg->IsString(vm)) {
        return defaultReturnValue;
    }
    auto nodeId = frameNode->GetId();
    std::function<bool()> funcCallback = ParseFunc(runtimeCallInfo);
    CHECK_NULL_RETURN(funcCallback, panda::BooleanRef::New(vm, false));
    std::function<std::string(const std::string&)> getFuncCallback = ParseGetFunc(runtimeCallInfo, nodeId);
    std::function<std::string()> getCustomPropertyMapCallback = JsGetCustomMapFunc(runtimeCallInfo, nodeId);
    GetArkUINodeModifiers()->getFrameNodeModifier()->setCustomPropertyModiferByKey(
        nativeNode, reinterpret_cast<void*>(&funcCallback), reinterpret_cast<void*>(&getFuncCallback),
            reinterpret_cast<void*>(&getCustomPropertyMapCallback));
    return defaultReturnValue;
}

ArkUINativeModuleValue FrameNodeBridge::SetRemoveCustomProperties(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    auto nodeId = frameNode->GetId();
    frameNode->SetRemoveCustomProperties([vm, nodeId]() -> void {
        CHECK_NULL_VOID(vm);
        panda::LocalScope scope(vm);
        auto global = JSNApi::GetGlobalObject(vm);
        auto removeCustomProperty =
            global->Get(vm, panda::StringRef::NewFromUtf8(vm, "__removeCustomProperties__"));
        if (removeCustomProperty->IsUndefined() || !removeCustomProperty->IsFunction(vm)) {
            return;
        }
        auto obj = removeCustomProperty->ToObject(vm);
        panda::Local<panda::FunctionRef> func = obj;
        panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, nodeId) };
        auto function = panda::CopyableGlobal(vm, func);
        function->Call(vm, function.ToLocal(), params, 1);
    });
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetMeasuredSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> width = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(width->IsNumber(), defaultReturnValue);
    Local<JSValueRef> height = runtimeCallInfo->GetCallArgRef(2);
    CHECK_NULL_RETURN(height->IsNumber(), defaultReturnValue);
    GetArkUIFullNodeAPI()->getExtendedAPI()->setMeasureWidth(nativeNode, width->ToNumber(vm)->Value());
    GetArkUIFullNodeAPI()->getExtendedAPI()->setMeasureHeight(nativeNode, height->ToNumber(vm)->Value());
    return defaultReturnValue;
}
ArkUINativeModuleValue FrameNodeBridge::SetLayoutPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> x = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(x->IsNumber(), defaultReturnValue);
    Local<JSValueRef> y = runtimeCallInfo->GetCallArgRef(2);
    CHECK_NULL_RETURN(y->IsNumber(), defaultReturnValue);
    GetArkUIFullNodeAPI()->getExtendedAPI()->setX(nativeNode, x->ToNumber(vm)->Value());
    GetArkUIFullNodeAPI()->getExtendedAPI()->setY(nativeNode, y->ToNumber(vm)->Value());
    return defaultReturnValue;
}
Local<JSValueRef> FrameNodeBridge::GetObjectValueByKey(EcmaVM* vm, Local<JSValueRef> object, const char* key)
{
    CHECK_NULL_RETURN(object->IsObject(vm), panda::JSValueRef::Undefined(vm));
    return object->ToObject(vm)->Get(vm, panda::StringRef::NewFromUtf8(vm, key));
}
ArkUINativeModuleValue FrameNodeBridge::MeasureNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    ArkUI_Float32 constraintValues[6];
    for (int i = 0; i < 6; i++) {
        Local<JSValueRef> constraintValue = runtimeCallInfo->GetCallArgRef(i + 1);
        CHECK_NULL_RETURN(constraintValue->IsNumber(), defaultReturnValue);
        constraintValues[i] = constraintValue->ToNumber(vm)->Value();
    }

    ArkUIVMContext vmContext = nullptr;
    GetArkUIFullNodeAPI()->getExtendedAPI()->measureNode(vmContext, nativeNode, constraintValues);
    return defaultReturnValue;
}
ArkUINativeModuleValue FrameNodeBridge::LayoutNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> x = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(x->IsNumber(), defaultReturnValue);
    Local<JSValueRef> y = runtimeCallInfo->GetCallArgRef(2);
    CHECK_NULL_RETURN(y->IsNumber(), defaultReturnValue);
    ArkUI_Float32 positionValue[2] = { static_cast<ArkUI_Float32>(x->ToNumber(vm)->Value()),
        static_cast<ArkUI_Float32>(y->ToNumber(vm)->Value()) };

    ArkUIVMContext vmContext = nullptr;
    GetArkUIFullNodeAPI()->getExtendedAPI()->layoutNode(vmContext, nativeNode, &positionValue);
    return defaultReturnValue;
}
ArkUINativeModuleValue FrameNodeBridge::SetNeedsLayout(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUIFullNodeAPI()->getBasicAPI()->markDirty(nativeNode, ARKUI_DIRTY_FLAG_MEASURE_SELF_AND_PARENT);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnSizeChange(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnSizeChange(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onSizeChange = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                            node = AceType::WeakClaim(frameNode),
                            containerId](const NG::RectF& oldRect, const NG::RectF& rect) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        double density = PipelineBase::GetCurrentDensity();
        const char* keys[] = { "width", "height" };
        Local<JSValueRef> oldValues[] = { panda::NumberRef::New(vm, oldRect.Width() / density),
            panda::NumberRef::New(vm, oldRect.Height() / density) };
        auto oldSize = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, oldValues);
        Local<JSValueRef> newValues[] = { panda::NumberRef::New(vm, rect.Width() / density),
            panda::NumberRef::New(vm, rect.Height() / density) };
        auto newSize = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, newValues);
        panda::Local<panda::JSValueRef> params[2] = { oldSize, newSize };
        function->Call(vm, function.ToLocal(), params, 2);
    };
    NG::ViewAbstract::SetJSFrameNodeOnSizeChange(frameNode, std::move(onSizeChange));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnVisibleAreaApproximateChange(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnVisibleAreaApproximateChange(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = secondArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = obj;
    auto flag = IsCustomFrameNode(frameNode);
    auto onVisibleAreaApproximateChange = [vm, func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                              node = AceType::WeakClaim(frameNode),
                                              containerId](bool visible, double ratio) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty() && function->IsFunction(vm));

        Local<JSValueRef> visibleValues = panda::BooleanRef::New(vm, visible);
        Local<JSValueRef> ratioValues = panda::NumberRef::New(vm, ratio);
        panda::Local<panda::JSValueRef> params[2] = { visibleValues, ratioValues };
        function->Call(vm, function.ToLocal(), params, 2);
    };
    Local<JSValueRef> ratiosArg = runtimeCallInfo->GetCallArgRef(INDEX_OF_OPTION_OF_VISIBLE);
    if (ratiosArg->IsUndefined() || !ratiosArg->IsArray(vm)) {
        return panda::JSValueRef::Undefined(vm);
    }
    panda::Local<panda::ArrayRef> ratioList = ratiosArg;
    uint32_t size = ratioList->Length(vm);
    std::vector<double> ratioVec(size);
    for (uint32_t i = 0; i < size; i++) {
        double radioNumber = 0.0;
        auto const radio = panda::ArrayRef::GetValueAt(vm, ratioList, i);
        radioNumber = radio->IsNumber() ? radio->ToNumber(vm)->Value() : 0.0;
        radioNumber = std::clamp(radioNumber, VISIBLE_RATIO_MIN, VISIBLE_RATIO_MAX);
        ratioVec.push_back(radioNumber);
    }
    Local<JSValueRef> intervalArg = runtimeCallInfo->GetCallArgRef(INDEX_OF_INTERVAL);
    if (intervalArg->IsUndefined() || !intervalArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    int32_t intervalMs = static_cast<int32_t>(intervalArg->ToNumber(vm)->Value());
    if (intervalMs < 0) {
        intervalMs = DEFAULT_EXPECTED_UPDATE_INTERVAL;
    }
    NG::ViewAbstract::SetJSFrameNodeOnVisibleAreaApproximateChange(
        frameNode, std::move(onVisibleAreaApproximateChange), ratioVec, intervalMs);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::PropertyUpdate(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->propertyUpdate(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::RegisterFrameCallback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    auto obj = firstArg->ToObject(vm);
    auto containerId = Container::CurrentIdSafely();
    panda::Local<panda::FunctionRef> func = obj;
    auto getVsyncFunc = [vm, func = panda::CopyableGlobal(vm, func), containerId]() {
        panda::LocalScope pandaScope(vm);
        ContainerScope scope(containerId);
        auto container = Container::Current();
        CHECK_NULL_VOID(container);
        auto frontend = container->GetFrontend();
        CHECK_NULL_VOID(frontend);
        func->Call(vm, func.ToLocal(), nullptr, 0);
    };
    auto pipelineContext = PipelineContext::GetCurrentContextSafely();
    CHECK_NULL_RETURN(pipelineContext, panda::JSValueRef::Undefined(vm));
    pipelineContext->SetOnceVsyncListener(std::move(getVsyncFunc));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::MarkDirty(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(secondeArg->IsNumber(), panda::JSValueRef::Undefined(vm));
    ArkUIDirtyFlag dirtyFlag = static_cast<ArkUIDirtyFlag>(secondeArg->ToNumber(vm)->Value());
    GetArkUIFullNodeAPI()->getBasicAPI()->markDirty(nativeNode, dirtyFlag);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::CreateNodeContent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto content = Referenced::MakeRefPtr<NodeContent>();
    return NativeUtilsBridge::CreateStrongRef(vm, content);
}

ArkUINativeModuleValue FrameNodeBridge::AddFrameNodeToNodeContent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, true));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeContent = reinterpret_cast<ArkUINodeContentHandle>(secondArg->ToNativePointer(vm)->Value());
    auto result = GetArkUINodeModifiers()->getNodeContentModifier()->addChild(nativeContent, nativeNode);
    GetArkUIFullNodeAPI()->getBasicAPI()->markDirty(nativeNode, ARKUI_DIRTY_FLAG_MEASURE_SELF_AND_PARENT);
    if (result != ERROR_CODE_NO_ERROR) {
        LOGW("AddFrameNodeToNodeContent failed error:%{public}d", result);
    }
    return panda::BooleanRef::New(vm, !result);
}

ArkUINativeModuleValue FrameNodeBridge::RemoveFrameNodeFromNodeContent(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, true));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeContent = reinterpret_cast<ArkUINodeContentHandle>(secondArg->ToNativePointer(vm)->Value());
    GetArkUIFullNodeAPI()->getBasicAPI()->markDirty(nativeNode, ARKUI_DIRTY_FLAG_MEASURE_SELF_AND_PARENT);
    auto result = GetArkUINodeModifiers()->getNodeContentModifier()->removeChild(nativeContent, nativeNode);
    if (result != ERROR_CODE_NO_ERROR) {
        LOGW("RemoveFrameNodeFromNodeContent failed error:%{public}d", result);
    }
    return panda::BooleanRef::New(vm, !result);
}

ArkUINativeModuleValue FrameNodeBridge::GetFirstUINode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto nodePtr = GetArkUINodeModifiers()->getFrameNodeModifier()->getFirstUINode(nativeNode);
    CHECK_NULL_RETURN(nodePtr, panda::JSValueRef::Undefined(vm));
    return panda::NativePointerRef::New(vm, nodePtr);
}

ArkUINativeModuleValue FrameNodeBridge::GetStackTopNode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto* node = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_RETURN(node, panda::JSValueRef::Undefined(vm));
    return panda::NativePointerRef::New(vm, node);
}

ArkUINativeModuleValue FrameNodeBridge::TriggerOnReuse(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    frameNode->OnReuse();
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::TriggerOnRecycle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    frameNode->OnRecycle();
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetCrossLanguageOptions(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::NumberRef::New(vm, ERROR_CODE_PARAM_INVALID));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    bool attributeSetting = secondArg->ToBoolean(vm)->Value();
    int result = GetArkUINodeModifiers()->getFrameNodeModifier()->setCrossLanguageOptions(nativeNode, attributeSetting);
    return panda::NumberRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::GetCrossLanguageOptions(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    bool result = GetArkUINodeModifiers()->getFrameNodeModifier()->getCrossLanguageOptions(nativeNode);
    return panda::BooleanRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::CheckIfCanCrossLanguageAttributeSetting(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::BooleanRef::New(vm, false));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    bool result = GetArkUINodeModifiers()->getFrameNodeModifier()->checkIfCanCrossLanguageAttributeSetting(nativeNode);
    return panda::BooleanRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::GetInteractionEventBindingInfo(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(!secondArg.IsNull(), panda::JSValueRef::Undefined(vm));
    int eventQueryType = secondArg->ToNumber(vm)->Value();
    auto result =
        GetArkUINodeModifiers()->getFrameNodeModifier()->getInteractionEventBindingInfo(nativeNode, eventQueryType);
    const char* keys[] = { "baseEventRegistered", "nodeEventRegistered", "nativeEventRegistered",
        "builtInEventRegistered" };
    Local<JSValueRef> values[] = { panda::BooleanRef::New(vm, result.baseEventRegistered),
        panda::BooleanRef::New(vm, result.nodeEventRegistered),
        panda::BooleanRef::New(vm, result.nativeEventRegistered),
        panda::BooleanRef::New(vm, result.builtInEventRegistered) };
    auto obj = panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
    return obj;
}

ArkUINativeModuleValue FrameNodeBridge::AddSupportedStates(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(secondArg->IsNumber(), defaultReturnValue);
    auto state = secondArg->ToNumber(vm)->Value();
    const int32_t thirdArgIndex = 2;
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(thirdArgIndex);
    CHECK_NULL_RETURN(thirdArg->IsFunction(vm), defaultReturnValue);
    auto obj = thirdArg->ToObject(vm);
    auto containerId = Container::CurrentIdSafely();
    panda::Local<panda::FunctionRef> func = obj;
    std::function<void(int64_t)> callback = [vm, func = panda::CopyableGlobal(vm, func), containerId](
                                                int64_t currentState) {
        panda::LocalScope pandaScope(vm);
        ContainerScope scope(containerId);
        auto container = Container::Current();
        CHECK_NULL_VOID(container);
        auto frontend = container->GetFrontend();
        CHECK_NULL_VOID(frontend);
        Local<JSValueRef> stateValues = panda::NumberRef::New(vm, currentState);
        panda::Local<panda::JSValueRef> params[1] = { stateValues };
        func->Call(vm, func.ToLocal(), params, 1);
    };
    int isExcludeInner = GetIsExcludeInner(runtimeCallInfo, 3);
    GetArkUINodeModifiers()->getUIStateModifier()->addSupportedUIState(
        nativeNode, state, reinterpret_cast<void*>(&callback), isExcludeInner);
    return defaultReturnValue;
}

ArkUINativeModuleValue FrameNodeBridge::RemoveSupportedStates(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(!firstArg.IsNull(), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(secondArg->IsNumber(), defaultReturnValue);
    auto state = secondArg->ToNumber(vm)->Value();
    GetArkUINodeModifiers()->getUIStateModifier()->removeSupportedUIState(nativeNode, state);
    return defaultReturnValue;
}

ArkUINativeModuleValue FrameNodeBridge::CreateAnimation(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    panda::Local<panda::JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    if (nodeArg.IsNull()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation, node is null");
        return panda::BooleanRef::New(vm, false);
    }
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::BooleanRef::New(vm, false));
    auto containerId = Container::CurrentIdSafelyWithCheck();
    ContainerScope scope(containerId);
    panda::Local<panda::JSValueRef> propertyArg = runtimeCallInfo->GetCallArgRef(1);
    AnimationPropertyType propertyType = AnimationPropertyType::ROTATION;
    if (!ParseAnimationProperty(vm, propertyArg, propertyType)) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation, property is invalid");
        return panda::BooleanRef::New(vm, false);
    }
    panda::Local<panda::JSValueRef> startValueArg = runtimeCallInfo->GetCallArgRef(2);
    std::vector<float> startValue;
    if (startValueArg->IsArray(vm)) {
        if (!ParseFloatArray(vm, panda::Local<panda::ArrayRef>(startValueArg), startValue)) {
            TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation, start value parse failed");
            return panda::BooleanRef::New(vm, false);
        }
    }
    std::vector<float> endValue;
    panda::Local<panda::JSValueRef> endValueArg = runtimeCallInfo->GetCallArgRef(3);
    if (!(endValueArg->IsArray(vm) && ParseFloatArray(vm, panda::Local<panda::ArrayRef>(endValueArg), endValue))) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation, end value parse failed");
        return panda::BooleanRef::New(vm, false);
    }
    if (!(CheckAnimationPropertyLength(propertyType, startValue.size(), true) &&
            CheckAnimationPropertyLength(propertyType, endValue.size(), false))) {
        return panda::BooleanRef::New(vm, false);
    }
    AdjustPropertyValue(propertyType, startValue, endValue);
    panda::Local<panda::JSValueRef> localParamArg = runtimeCallInfo->GetCallArgRef(4);
    if (!localParamArg->IsObject(vm)) {
        TAG_LOGI(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation, animate param is not object");
        return panda::BooleanRef::New(vm, false);
    }
    panda::Local<panda::ObjectRef> localParamObj = localParamArg->ToObject(vm);
    Framework::JSRef<Framework::JSObject> paramObj { Framework::JSObject(vm, localParamObj) };
    // not support form now, the second param is false
    AnimationOption option = Framework::JSViewContext::CreateAnimation(paramObj, false);
    std::optional<int32_t> finishCount;
    option.SetOnFinishEvent(ParseFinishCallback(localParamObj, frameNode, vm, finishCount));
    auto result = ViewAbstract::CreatePropertyAnimation(frameNode, propertyType, startValue, endValue, option);
    if (result && finishCount.has_value()) {
        TAG_LOGI(AceLogTag::ACE_ANIMATION, "FrameNode::createAnimation starts, property:%{public}d, cnt:%{public}d",
            static_cast<int32_t>(propertyType), finishCount.value());
    }
    return panda::BooleanRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::CancelAnimations(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    panda::Local<panda::JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    if (nodeArg.IsNull()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::cancelAnimations, node is null");
        return panda::BooleanRef::New(vm, false);
    }
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::BooleanRef::New(vm, false));
    auto containerId = Container::CurrentIdSafelyWithCheck();
    ContainerScope scope(containerId);
    panda::Local<panda::JSValueRef> propertiesArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(propertiesArg->IsArray(vm), panda::BooleanRef::New(vm, false));
    panda::Local<panda::ArrayRef> propertiesArrArg = panda::Local<panda::ArrayRef>(propertiesArg);
    std::vector<AnimationPropertyType> properties;
    auto length = propertiesArrArg->Length(vm);
    for (uint32_t i = 0; i != length; ++i) {
        panda::Local<panda::JSValueRef> propertyArg = panda::ArrayRef::GetValueAt(vm, propertiesArrArg, i);
        AnimationPropertyType propertyType = AnimationPropertyType::ROTATION;
        if (!ParseAnimationProperty(vm, propertyArg, propertyType)) {
            TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::cancelAnimations, property is invalid");
            return panda::BooleanRef::New(vm, false);
        }
        if (std::find(properties.begin(), properties.end(), propertyType) == properties.end()) {
            properties.emplace_back(propertyType);
        }
    }
    auto result = ViewAbstract::CancelPropertyAnimations(frameNode, properties);
    return panda::BooleanRef::New(vm, result);
}

ArkUINativeModuleValue FrameNodeBridge::GetNodePropertyValue(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    panda::Local<panda::JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    if (nodeArg.IsNull()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::getNodePropertyValue, node is null");
        return panda::ArrayRef::New(vm, 0);
    }
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::ArrayRef::New(vm, 0));
    panda::Local<panda::JSValueRef> propertyArg = runtimeCallInfo->GetCallArgRef(1);
    AnimationPropertyType propertyType = AnimationPropertyType::ROTATION;
    if (!ParseAnimationProperty(vm, propertyArg, propertyType)) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "FrameNode::getNodePropertyValue, property is invalid");
        return panda::ArrayRef::New(vm, 0);
    }
    auto resultVector = ViewAbstract::GetRenderNodePropertyValue(frameNode, propertyType);
    uint32_t resultLength = resultVector.size();
    panda::Local<panda::ArrayRef> result = panda::ArrayRef::New(vm, static_cast<uint32_t>(resultVector.size()));
    for (uint32_t index = 0; index != resultLength; ++index) {
        result->SetValueAt(vm, result, index, panda::NumberRef::New(vm, resultVector[index]));
    }
    return result;
}

ArkUINativeModuleValue FrameNodeBridge::SetOnReachStart(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnReachStart(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(void)> callback = [vm, node = AceType::WeakClaim(frameNode),
                                             func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                             containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnReachStart(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnReachEnd(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnReachEnd(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(void)> callback = [vm, node = AceType::WeakClaim(frameNode),
                                             func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                             containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnReachEnd(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollStart(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollStart(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void()> callback = [vm, node = AceType::WeakClaim(frameNode),
                                         func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag), containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnScrollStart(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollStop(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollStop(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(void)> callback = [vm, node = AceType::WeakClaim(frameNode),
                                             func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                             containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);
        function->Call(vm, function.ToLocal(), nullptr, 0);
    };
    NG::ViewAbstract::SetJSFrameNodeOnScrollStop(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollFrameBegin(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollFrameBegin(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<ScrollFrameResult(Dimension, ScrollState)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](Dimension offset, ScrollState state) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            OHOS::Ace::ScrollFrameResult scrollRes { .offset = offset };
            CHECK_NULL_RETURN(!function.IsEmpty(), scrollRes);
            CHECK_NULL_RETURN(function->IsFunction(vm), scrollRes);
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> offsetParam =
                panda::NumberRef::New(vm, static_cast<double>(offset.ConvertToVp()));
            panda::Local<panda::NumberRef> stateParam = panda::NumberRef::New(vm, static_cast<double>(state));
            // 2: Array length
            panda::Local<panda::JSValueRef> params[2] = { offsetParam, stateParam };
            auto value = function->Call(vm, function.ToLocal(), params, 2); // 2: Array length
            if (value->IsObject(vm)) {
                auto resultObj = value->ToObject(vm);
                panda::Local<panda::JSValueRef> remain =
                    resultObj->Get(vm, panda::StringRef::NewFromUtf8(vm, "offsetRemain"));
                if (remain->IsNumber()) {
                    scrollRes.offset = Dimension(remain->ToNumber(vm)->Value(), DimensionUnit::VP);
                }
            }
            return scrollRes;
        };
    NG::ViewAbstract::SetJSFrameNodeOnScrollFrameBegin(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnWillScroll(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnWillScroll(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<ScrollFrameResult(CalcDimension, ScrollState, ScrollSource)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](const CalcDimension& scrollOffset, const ScrollState& scrollState, ScrollSource scrollSource) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            ScrollFrameResult scrollRes { .offset = scrollOffset };
            CHECK_NULL_RETURN(!function.IsEmpty(), scrollRes);
            CHECK_NULL_RETURN(function->IsFunction(vm), scrollRes);
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> offsetParam =
                panda::NumberRef::New(vm, static_cast<double>(scrollOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> stateParam = panda::NumberRef::New(vm, static_cast<int32_t>(scrollState));
            panda::Local<panda::NumberRef> sourceParam = panda::NumberRef::New(vm, static_cast<int32_t>(scrollSource));
            // 3: Array length
            panda::Local<panda::JSValueRef> params[3] = { offsetParam, stateParam, sourceParam };
            auto result = function->Call(vm, function.ToLocal(), params, 3); // 3: Array length
            if (result->IsObject(vm)) {
                auto resultObj = result->ToObject(vm);
                panda::Local<panda::JSValueRef> dxRemainValue =
                    resultObj->Get(vm, panda::StringRef::NewFromUtf8(vm, "offsetRemain"));
                if (dxRemainValue->IsNumber()) {
                    scrollRes.offset = Dimension(dxRemainValue->ToNumber(vm)->Value(), DimensionUnit::VP);
                }
            }
            return scrollRes;
        };
    NG::ViewAbstract::SetJSFrameNodeOnWillScroll(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnDidScroll(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnDidScroll(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(Dimension, ScrollState)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](const CalcDimension& scrollOffset, const ScrollState& scrollState) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            CHECK_NULL_VOID(!function.IsEmpty());
            CHECK_NULL_VOID(function->IsFunction(vm));
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> offsetParam =
                panda::NumberRef::New(vm, static_cast<double>(scrollOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> stateParam = panda::NumberRef::New(vm, static_cast<int32_t>(scrollState));
            // 2: Array length
            panda::Local<panda::JSValueRef> params[2] = { offsetParam, stateParam };
            function->Call(vm, function.ToLocal(), params, 2); // 2: Array length
        };
    NG::ViewAbstract::SetJSFrameNodeOnDidScroll(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnListScrollIndex(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnListScrollIndex(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(int32_t, int32_t, int32_t)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](const int32_t start, const int32_t end, const int32_t center) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            CHECK_NULL_VOID(!function.IsEmpty());
            CHECK_NULL_VOID(function->IsFunction(vm));
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> startParam = panda::NumberRef::New(vm, start);
            panda::Local<panda::NumberRef> endParam = panda::NumberRef::New(vm, end);
            panda::Local<panda::NumberRef> centerParam = panda::NumberRef::New(vm, center);
            // 3: Array length
            panda::Local<panda::JSValueRef> params[3] = { startParam, endParam, centerParam };
            function->Call(vm, function.ToLocal(), params, 3); // 3: Array length
        };
    NG::ViewAbstract::SetJSFrameNodeOnListScrollIndex(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> FrameNodeBridge::SetListItemIndex(const EcmaVM* vm, const ListItemIndex indexInfo)
{
    const char* keys[] = { "index", "itemIndexInGroup", "itemGroupArea" };
    auto indexInGroup = panda::NumberRef::Undefined(vm);
    if (indexInfo.indexInGroup != -1) {
        indexInGroup = panda::NumberRef::New(vm, static_cast<int32_t>(indexInfo.indexInGroup));
    }
    auto area = panda::NumberRef::Undefined(vm);
    if (indexInfo.area != -1) {
        area = panda::NumberRef::New(vm, static_cast<int32_t>(indexInfo.area));
    }
    Local<JSValueRef> values[] = { panda::NumberRef::New(vm, static_cast<int32_t>(indexInfo.index)), indexInGroup,
        area };
    return panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keys), keys, values);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollVisibleContentChange(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollVisibleContentChange(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(ListItemIndex, ListItemIndex)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](const ListItemIndex start, const ListItemIndex end) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            CHECK_NULL_VOID(!function.IsEmpty());
            CHECK_NULL_VOID(function->IsFunction(vm));
            PipelineContext::SetCallBackNode(node);

            auto startParam = SetListItemIndex(vm, start);
            auto endParam = SetListItemIndex(vm, end);
            startParam->SetNativePointerFieldCount(vm, 1);
            startParam->SetNativePointerField(vm, 0, static_cast<void*>(&startParam));
            endParam->SetNativePointerFieldCount(vm, 1);
            endParam->SetNativePointerField(vm, 0, static_cast<void*>(&endParam));
            // 2: Array length
            panda::Local<panda::JSValueRef> params[2] = { startParam, endParam };
            function->Call(vm, function.ToLocal(), params, 2); // 2: Array length
        };
    NG::ViewAbstract::SetJSFrameNodeOnScrollVisibleContentChange(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollWillScroll(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollWillScroll(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<TwoDimensionScrollResult(Dimension, Dimension, ScrollState, ScrollSource)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](Dimension xOffset, Dimension yOffset, ScrollState state, ScrollSource scrollState) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            NG::TwoDimensionScrollResult scrollRes { .xOffset = xOffset, .yOffset = yOffset };
            CHECK_NULL_RETURN(!function.IsEmpty(), scrollRes);
            CHECK_NULL_RETURN(function->IsFunction(vm), scrollRes);
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> xOffsetParam =
                panda::NumberRef::New(vm, static_cast<double>(xOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> yOffsetParam =
                panda::NumberRef::New(vm, static_cast<double>(yOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> stateParam = panda::NumberRef::New(vm, static_cast<double>(state));
            panda::Local<panda::NumberRef> sourceParam = panda::NumberRef::New(vm, static_cast<double>(scrollState));
            // 4: Array length
            panda::Local<panda::JSValueRef> params[4] = { xOffsetParam, yOffsetParam, stateParam, sourceParam };
            auto result = function->Call(vm, function.ToLocal(), params, 4); // 4: Array length
            if (result->IsObject(vm)) {
                auto resultObj = result->ToObject(vm);
                panda::Local<panda::JSValueRef> x = resultObj->Get(vm, panda::StringRef::NewFromUtf8(vm, "xOffset"));
                panda::Local<panda::JSValueRef> y = resultObj->Get(vm, panda::StringRef::NewFromUtf8(vm, "yOffset"));
                scrollRes.xOffset =
                    x->IsNumber() ? Dimension(x->ToNumber(vm)->Value(), DimensionUnit::VP) : scrollRes.xOffset;
                scrollRes.yOffset =
                    y->IsNumber() ? Dimension(y->ToNumber(vm)->Value(), DimensionUnit::VP) : scrollRes.yOffset;
            }
            return scrollRes;
        };
    NG::ViewAbstract::SetJSFrameNodeOnScrollWillScroll(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnScrollDidScroll(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnScrollDidScroll(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(Dimension, Dimension, ScrollState)> callback =
        [vm, node = AceType::WeakClaim(frameNode), func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
            containerId](Dimension xOffset, Dimension yOffset, ScrollState state) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ContainerScope scope(containerId);
            auto function = func.Lock();
            CHECK_NULL_VOID(!function.IsEmpty());
            CHECK_NULL_VOID(function->IsFunction(vm));
            PipelineContext::SetCallBackNode(node);

            panda::Local<panda::NumberRef> xOffsetParam =
                panda::NumberRef::New(vm, static_cast<int32_t>(xOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> yOffsetParam =
                panda::NumberRef::New(vm, static_cast<int32_t>(yOffset.ConvertToVp()));
            panda::Local<panda::NumberRef> stateParam = panda::NumberRef::New(vm, static_cast<int32_t>(state));
            // 3: Array length
            panda::Local<panda::JSValueRef> params[3] = { xOffsetParam, yOffsetParam, stateParam };
            function->Call(vm, function.ToLocal(), params, 3); // 3: Array length
        };
    NG::ViewAbstract::SetJSFrameNodeOnScrollDidScroll(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnGridScrollIndex(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnGridScrollIndex(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(int32_t, int32_t)> callback = [vm, node = AceType::WeakClaim(frameNode),
                                                         func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                                         containerId](const int32_t first, const int32_t last) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);

        panda::Local<panda::NumberRef> firstParam = panda::NumberRef::New(vm, first);
        panda::Local<panda::NumberRef> lastParam = panda::NumberRef::New(vm, last);
        // 2: Array length
        panda::Local<panda::JSValueRef> params[2] = { firstParam, lastParam };
        function->Call(vm, function.ToLocal(), params, 2); // 2: Array length
    };
    NG::ViewAbstract::SetJSFrameNodeOnGridScrollIndex(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::SetOnWaterFlowScrollIndex(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    auto* nativeNode = GetFrameNode(runtimeCallInfo);
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> secondeArg = runtimeCallInfo->GetCallArgRef(1);
    if (secondeArg->IsUndefined()) {
        NG::ViewAbstract::ClearJSFrameNodeOnWaterFlowScrollIndex(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }

    CHECK_NULL_RETURN(secondeArg->IsFunction(vm), panda::JSValueRef::Undefined(vm));
    panda::Local<panda::FunctionRef> func = secondeArg->ToObject(vm);
    auto containerId = GetInstanceId(runtimeCallInfo);
    CHECK_NULL_RETURN(containerId != -1, panda::JSValueRef::Undefined(vm));
    auto flag = IsCustomFrameNode(frameNode);
    std::function<void(int32_t, int32_t)> callback = [vm, node = AceType::WeakClaim(frameNode),
                                                         func = JSFuncObjRef(panda::CopyableGlobal(vm, func), flag),
                                                         containerId](const int32_t first, const int32_t last) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        auto function = func.Lock();
        CHECK_NULL_VOID(!function.IsEmpty());
        CHECK_NULL_VOID(function->IsFunction(vm));
        PipelineContext::SetCallBackNode(node);

        panda::Local<panda::NumberRef> firstParam = panda::NumberRef::New(vm, first);
        panda::Local<panda::NumberRef> lastParam = panda::NumberRef::New(vm, last);
        // 2: Array length
        panda::Local<panda::JSValueRef> params[2] = { firstParam, lastParam };
        function->Call(vm, function.ToLocal(), params, 2); // 2: Array length
    };
    NG::ViewAbstract::SetJSFrameNodeOnWaterFlowScrollIndex(frameNode, std::move(callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue FrameNodeBridge::UpdateConfiguration(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), defaultReturnValue);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getFrameNodeModifier()->updateConfiguration(nativeNode);
    return defaultReturnValue;
}

ArkUINativeModuleValue FrameNodeBridge::FireArkUIObjectLifecycleCallback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    auto defaultReturnValue = panda::JSValueRef::Undefined(vm);
    CHECK_NULL_RETURN(vm, defaultReturnValue);
    Local<JSValueRef> arg = runtimeCallInfo->GetCallArgRef(3);
    CHECK_NULL_RETURN(!arg.IsNull() && arg->IsNativePointer(vm), defaultReturnValue);
    auto* nativeNode = nodePtr(arg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(nativeNode, defaultReturnValue);
    auto* frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, defaultReturnValue);
    auto context = frameNode->GetContext();
    CHECK_NULL_RETURN(context, defaultReturnValue);
    void* data = static_cast<void*>(runtimeCallInfo);
    context->FireArkUIObjectLifecycleCallback(data);
    return defaultReturnValue;
}
} // namespace OHOS::Ace::NG
