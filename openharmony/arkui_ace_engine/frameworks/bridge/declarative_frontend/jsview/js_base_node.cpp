/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "bridge/declarative_frontend/jsview/js_base_node.h"

#include <memory>
#include <string>

#include "canvas_napi/js_canvas.h"
#include "jsnapi_expo.h"

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "bridge/common/utils/engine_helper.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/ui_context_helper.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/engine/js_converter.h"
#include "bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "bridge/declarative_frontend/engine/js_types.h"
#include "bridge/declarative_frontend/jsview/js_utils.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/js_frontend/engine/jsi/js_value.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/custom_frame_node/custom_frame_node.h"
#include "core/components_ng/pattern/render_node/render_node_pattern.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/render/drawing_forward.h"
#include "core/event/touch_event.h"
#include "core/pipeline/pipeline_base.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::Framework {
namespace {
const std::unordered_set<std::string> EXPORT_TEXTURE_SUPPORT_TYPES = { V2::JS_VIEW_ETS_TAG, V2::COMMON_VIEW_ETS_TAG };
constexpr int32_t INFO_LENGTH_LIMIT = 2;
constexpr int32_t BUILD_PARAM_INDEX_TWO = 2;
constexpr int32_t BUILD_PARAM_INDEX_THREE = 3;
constexpr int32_t BUILD_PARAM_INDEX_FOUR = 4;
} // namespace

void JSBaseNode::BuildNode(const JSCallbackInfo& info)
{
    auto builder = info[0];
    CHECK_NULL_VOID(builder->IsFunction());
    auto buildFunc = AceType::MakeRefPtr<JsFunction>(info.This(), JSRef<JSFunc>::Cast(builder));

    auto infoLen = info.Length();
    JSRef<JSVal> param;
    if (infoLen >= INFO_LENGTH_LIMIT && (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)
        || (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE) && info[1]->IsObject()))) {
        param = info[1];
    }

    auto lazyBuilderFunc = [buildFunc, param, renderType = renderType_]() mutable {
        NG::ViewStackProcessor::GetInstance()->SetIsBuilderNode(true);
        NG::ViewStackProcessor::GetInstance()->SetIsExportTexture(renderType == NodeRenderType::RENDER_TYPE_TEXTURE);
        if (!param->IsEmpty()) {
            buildFunc->ExecuteJS(1, &param);
        } else {
            buildFunc->ExecuteJS();
        }
    };

    NG::ScopedViewStackProcessor builderViewStackProcessor;
    lazyBuilderFunc();
    auto parent = viewNode_ ? viewNode_->GetParent() : nullptr;
    auto newNode = NG::ViewStackProcessor::GetInstance()->Finish();
    if (newNode) {
        newNode->SetBuilderFunc(std::move(lazyBuilderFunc));
    }

    if (newNode && (infoLen >= BUILD_PARAM_INDEX_TWO + 1)) {
        auto updateTsNodeBuilder = info[BUILD_PARAM_INDEX_TWO];
        EcmaVM* vm = info.GetVm();
        auto updateTsFunc = AceType::MakeRefPtr<JsFunction>(info.This(), JSRef<JSFunc>::Cast(updateTsNodeBuilder));
        auto updateNodeFunc = [updateTsFunc, vm](int32_t instanceId, RefPtr<NG::UINode>& node) mutable {
            JSRef<JSVal> param[2];
            param[0] = JSRef<JSVal>::Make(ToJSValue(instanceId));
            param[1] = JSRef<JSVal>::Make(panda::NativePointerRef::New(vm, AceType::RawPtr(node)));
            updateTsFunc->ExecuteJS(2, param);
        };
        newNode->SetUpdateNodeFunc(std::move(updateNodeFunc));
    }

    if (newNode && (infoLen >= BUILD_PARAM_INDEX_THREE + 1)) {
        auto updateTsNodeConfig = info[BUILD_PARAM_INDEX_THREE];
        EcmaVM* vm = info.GetVm();
        auto updateTsConfig = AceType::MakeRefPtr<JsFunction>(info.This(), JSRef<JSFunc>::Cast(updateTsNodeConfig));
        auto updateNodeConfig = [updateTsConfig, vm]() mutable {
            updateTsConfig->ExecuteJS();
        };
        newNode->SetUpdateNodeConfig(std::move(updateNodeConfig));
    }

    bool isSupportLazyBuild = false;
    if (infoLen >= BUILD_PARAM_INDEX_FOUR + 1) {
        auto jsLazyBuildSupported = info[BUILD_PARAM_INDEX_FOUR];
        if (jsLazyBuildSupported->IsBoolean()) {
            isSupportLazyBuild = jsLazyBuildSupported->ToBoolean();
        }
    }

    // If the node is a UINode, amount it to a BuilderProxyNode if needProxy.
    auto flag = AceType::InstanceOf<NG::FrameNode>(newNode);
    auto isSupportExportTexture = newNode ? EXPORT_TEXTURE_SUPPORT_TYPES.count(newNode->GetTag()) > 0 : false;
    if (!flag && newNode) {
        auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto proxyNode = NG::FrameNode::GetOrCreateFrameNode(
            "BuilderProxyNode", nodeId, []() { return AceType::MakeRefPtr<NG::StackPattern>(); });
        auto stackLayoutAlgorithm = proxyNode->GetLayoutProperty<NG::LayoutProperty>();
        stackLayoutAlgorithm->UpdateAlignment(Alignment::TOP_LEFT);
        proxyNode->AddChild(newNode);
        newNode = proxyNode;
    }
    if (parent) {
        if (newNode) {
            parent->ReplaceChild(viewNode_, newNode);
            newNode->MarkNeedFrameFlushDirty(NG::PROPERTY_UPDATE_MEASURE);
        } else {
            parent->RemoveChild(viewNode_);
            parent->MarkNeedFrameFlushDirty(NG::PROPERTY_UPDATE_MEASURE);
        }
    }
    viewNode_ = newNode ? AceType::DynamicCast<NG::FrameNode>(newNode) : nullptr;
    CHECK_NULL_VOID(viewNode_);
    ProccessNode(isSupportExportTexture, isSupportLazyBuild);
    UpdateEnd(info);
    CHECK_NULL_VOID(viewNode_);
    JSRef<JSObject> thisObj = info.This();
    JSWeak<JSObject> jsObject(thisObj);
    viewNode_->RegisterUpdateJSInstanceCallback([jsObject, vm = info.GetVm()](int32_t id) {
        JSRef<JSObject> jsThis = jsObject.Lock();
        JSRef<JSVal> jsUpdateFunc = jsThis->GetProperty("updateInstance");
        if (jsUpdateFunc->IsFunction()) {
            auto jsFunc = JSRef<JSFunc>::Cast(jsUpdateFunc);
            auto uiContext = NG::UIContextHelper::GetUIContext(vm, id);
            auto jsVal = JSRef<JSVal>::Make(uiContext);
            jsFunc->Call(jsThis, 1, &jsVal);
        }
    });
}

void JSBaseNode::ProccessNode(bool isSupportExportTexture, bool isSupportLazyBuild)
{
    CHECK_NULL_VOID(viewNode_);
    viewNode_->SetIsRootBuilderNode(true);
    if (isSupportExportTexture) {
        viewNode_->CreateExportTextureInfoIfNeeded();
        auto exportTextureInfo = viewNode_->GetExportTextureInfo();
        CHECK_NULL_VOID(exportTextureInfo);
        exportTextureInfo->SetSurfaceId(surfaceId_);
        exportTextureInfo->SetCurrentRenderType(renderType_);
    }
    if (!isSupportLazyBuild) {
        viewNode_->Build(nullptr);
    }
}

void JSBaseNode::Create(const JSCallbackInfo& info)
{
    if (info.Length() >= 1 && !info[0]->IsFunction()) {
        return;
    }
    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE) && info.Length() >= INFO_LENGTH_LIMIT
        && !(info[1]->IsObject() || info[1]->IsUndefined() || info[1]->IsNull())) {
        return;
    }
    BuildNode(info);
    EcmaVM* vm = info.GetVm();
    info.SetReturnValue(JSRef<JSVal>::Make(panda::NativePointerRef::New(vm, AceType::RawPtr(viewNode_))));
}

void JSBaseNode::ConstructorCallback(const JSCallbackInfo& info)
{
    std::string surfaceId;
    NodeRenderType renderType = NodeRenderType::RENDER_TYPE_DISPLAY;
    NG::OptionalSizeF selfIdealSize;
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto renderOption = JSRef<JSObject>::Cast(info[0]);
        auto size = renderOption->GetProperty("selfIdealSize");
        if (size->IsObject()) {
            auto sizeObj = JSRef<JSObject>::Cast(size);
            auto width = sizeObj->GetProperty("width");
            auto widthValue = width->IsNumber() ? width->ToNumber<float>() : 0.0f;
            widthValue = LessNotEqual(widthValue, 0.0f) ? 0.0f : widthValue;
            auto height = sizeObj->GetProperty("height");
            auto heightValue = height->IsNumber() ? height->ToNumber<float>() : 0.0f;
            heightValue = LessNotEqual(heightValue, 0.0f) ? 0.0f : heightValue;
            selfIdealSize.SetWidth(PipelineBase::Vp2PxWithCurrentDensity(widthValue));
            selfIdealSize.SetHeight(PipelineBase::Vp2PxWithCurrentDensity(heightValue));
        }
        auto type = renderOption->GetProperty("type");
        if (type->IsNumber()) {
            renderType = static_cast<NodeRenderType>(type->ToNumber<uint32_t>());
        }
        auto id = renderOption->GetProperty("surfaceId");
        if (id->IsString()) {
            surfaceId = id->ToString();
        }
    }
    auto instance = AceType::MakeRefPtr<JSBaseNode>(selfIdealSize, renderType, surfaceId);
    instance->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(instance));
}

void JSBaseNode::DestructorCallback(JSBaseNode* node)
{
    if (node != nullptr) {
        node->DecRefCount();
    }
}

void JSBaseNode::FinishUpdateFunc(const JSCallbackInfo& info)
{
    NG::ViewStackProcessor::GetInstance()->FlushRerenderTask();
}

void JSBaseNode::PostTouchEvent(const JSCallbackInfo& info)
{
    if (!viewNode_ || info.Length() < 1 || !info[0]->IsObject()) {
        TAG_LOGW(AceLogTag::ACE_INPUTKEYFLOW, "PostTouchEvent params invalid");
        info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
        return;
    }
    TouchEvent touchEvent = InitTouchEvent(info);
    auto pipelineContext = NG::PipelineContext::GetCurrentContext();
    if (!pipelineContext) {
        TAG_LOGW(AceLogTag::ACE_INPUTKEYFLOW, "PostTouchEvent pipelineContext is invalid");
        info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
        return;
    }
    auto postEventManager = pipelineContext->GetPostEventManager();
    if (!postEventManager) {
        TAG_LOGW(AceLogTag::ACE_INPUTKEYFLOW, "PostTouchEvent postEventManager is invalid");
        info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
        return;
    }
    auto result = postEventManager->PostEvent(viewNode_, touchEvent);
    info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(result)));
}

TouchEvent JSBaseNode::InitTouchEvent(const JSCallbackInfo& info)
{   
    TouchEvent touchEvent;
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto typeJsVal = obj->GetProperty("type");
    if (typeJsVal->IsNumber()) {
        touchEvent.type = static_cast<TouchType>(typeJsVal->ToNumber<int32_t>());
    }
    auto sourceJsVal = obj->GetProperty("source");
    if (sourceJsVal->IsNumber()) {
        touchEvent.sourceType = static_cast<SourceType>((sourceJsVal->ToNumber<int32_t>()));
    }
    auto sourceToolJsVal = obj->GetProperty("sourceTool");
    if (sourceToolJsVal->IsNumber()) {
        touchEvent.sourceTool = static_cast<SourceTool>((sourceToolJsVal->ToNumber<int32_t>()));
    }
    auto pressureJsVal = obj->GetProperty("pressure");
    if (pressureJsVal->IsNumber()) {
        touchEvent.force = sourceToolJsVal->ToNumber<float>();
    }
    auto timestampJsVal = obj->GetProperty("timestamp");
    if (timestampJsVal->IsNumber()) {
        std::chrono::nanoseconds nanoseconds(static_cast<int64_t>(timestampJsVal->ToNumber<double>()));
        TimeStamp time(nanoseconds);
        touchEvent.time = time;
    }
    auto deviceIdJsVal = obj->GetProperty("deviceId");
    if (deviceIdJsVal->IsNumber()) {
        touchEvent.deviceId = deviceIdJsVal->ToNumber<int32_t>();
    }
    auto targetDisplayIdJsVal = obj->GetProperty("targetDisplayId");
    if (targetDisplayIdJsVal->IsNumber()) {
        touchEvent.targetDisplayId = targetDisplayIdJsVal->ToNumber<int32_t>();
    }
    auto touchesJsVal = obj->GetProperty("touches");
    if (touchesJsVal->IsArray()) {
        JSRef<JSArray> touchesArray = JSRef<JSArray>::Cast(touchesJsVal);
        for (auto index = 0; index < static_cast<int32_t>(touchesArray->Length()); index++) {
            JSRef<JSVal> item = touchesArray->GetValueAt(index);
            if (!item->IsObject()) {
                continue;
            }
            JSRef<JSObject> itemObj = JSRef<JSObject>::Cast(item);
            TouchPoint point;
            point.id = itemObj->GetPropertyValue<int32_t>("id", 0);
            point.x = itemObj->GetPropertyValue<float>("x", 0.0f);
            point.y = itemObj->GetPropertyValue<float>("y", 0.0f);
            point.screenX = itemObj->GetPropertyValue<float>("screenX", 0.0f);
            point.screenY = itemObj->GetPropertyValue<float>("screenY", 0.0f);
            point.originalId = itemObj->GetPropertyValue<int32_t>("id", 0);
            touchEvent.pointers.emplace_back(point);
        }
    }
    auto titleXJsVal = obj->GetProperty("tiltX");
    if (titleXJsVal->IsNumber()) {
        touchEvent.tiltX = titleXJsVal->ToNumber<float>();
    }
    auto titleYJsVal = obj->GetProperty("tiltY");
    if (titleYJsVal->IsNumber()) {
        touchEvent.tiltY = titleYJsVal->ToNumber<float>();
    }
    auto changedTouchesJsVal = obj->GetProperty("changedTouches");
    if (changedTouchesJsVal->IsArray()) {
        JSRef<JSArray> changedTouchesArray = JSRef<JSArray>::Cast(changedTouchesJsVal);
        if (static_cast<int32_t>(changedTouchesArray->Length()) <= 0) {
            TAG_LOGW(AceLogTag::ACE_INPUTKEYFLOW, "PostTouchEvent event changedTouchesArray is invalid");
            info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
            return touchEvent;
        }
        JSRef<JSVal> item = changedTouchesArray->GetValueAt(0);
        if (!item->IsObject()) {
            TAG_LOGW(AceLogTag::ACE_INPUTKEYFLOW, "PostTouchEvent event changedTouchesArray item is not an object");
            info.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
            return touchEvent;
        }
        JSRef<JSObject> itemObj = JSRef<JSObject>::Cast(item);
        touchEvent.id = itemObj->GetPropertyValue<int32_t>("id", 0);
        touchEvent.x = itemObj->GetPropertyValue<float>("x", 0.0f);
        touchEvent.y = itemObj->GetPropertyValue<float>("y", 0.0f);
        touchEvent.screenX = itemObj->GetPropertyValue<float>("screenX", 0.0f);
        touchEvent.screenY = itemObj->GetPropertyValue<float>("screenY", 0.0f);
        touchEvent.originalId = itemObj->GetPropertyValue<int32_t>("id", 0);
    }
    return touchEvent;
}

void JSBaseNode::UpdateStart(const JSCallbackInfo& info)
{
    scopedViewStackProcessor_ = std::make_unique<NG::ScopedViewStackProcessor>();
}

void JSBaseNode::UpdateEnd(const JSCallbackInfo& info)
{
    scopedViewStackProcessor_ = nullptr;
    CHECK_NULL_VOID(viewNode_);
    if (size_.IsValid()) {
        viewNode_->SetParentLayoutConstraint(size_.ConvertToSizeT());
    }
}

void JSBaseNode::JSBind(BindingTarget globalObj)
{
    JSClass<JSBaseNode>::Declare("__JSBaseNode__");

    JSClass<JSBaseNode>::CustomMethod("create", &JSBaseNode::Create);
    JSClass<JSBaseNode>::CustomMethod("finishUpdateFunc", &JSBaseNode::FinishUpdateFunc);
    JSClass<JSBaseNode>::CustomMethod("postTouchEvent", &JSBaseNode::PostTouchEvent);
    JSClass<JSBaseNode>::CustomMethod("disposeNode", &JSBaseNode::Dispose);
    JSClass<JSBaseNode>::CustomMethod("updateStart", &JSBaseNode::UpdateStart);
    JSClass<JSBaseNode>::CustomMethod("updateEnd", &JSBaseNode::UpdateEnd);

    JSClass<JSBaseNode>::Bind(globalObj, JSBaseNode::ConstructorCallback, JSBaseNode::DestructorCallback);
}
} // namespace OHOS::Ace::Framework
