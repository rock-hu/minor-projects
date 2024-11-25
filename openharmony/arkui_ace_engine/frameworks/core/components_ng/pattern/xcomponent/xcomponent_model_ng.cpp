/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/xcomponent/xcomponent_model_ng.h"

#include "core/components_ng/pattern/xcomponent/xcomponent_pattern.h"

namespace OHOS::Ace::NG {
const uint32_t DEFAULT_SURFACE_SIZE = 0;
void XComponentModelNG::Create(const std::optional<std::string>& id, XComponentType type,
    const std::optional<std::string>& libraryname,
    const std::shared_ptr<InnerXComponentController>& xcomponentController)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::XCOMPONENT_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::XCOMPONENT_ETS_TAG, nodeId, [id, type, libraryname, xcomponentController]() {
            return AceType::MakeRefPtr<XComponentPattern>(id, type, libraryname, xcomponentController);
        });
    stack->Push(frameNode);
    ACE_UPDATE_LAYOUT_PROPERTY(XComponentLayoutProperty, XComponentType, type);
}

RefPtr<AceType> XComponentModelNG::Create(int32_t nodeId, float width, float height, const std::string& id,
    XComponentType type, const std::string& libraryname,
    const std::shared_ptr<InnerXComponentController>& xcomponentController)
{
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::XCOMPONENT_ETS_TAG, nodeId);
    auto calcWidth = CalcLength(width, DimensionUnit::VP);
    auto calcHeight = CalcLength(height, DimensionUnit::VP);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::XCOMPONENT_ETS_TAG, nodeId, [id, type, libraryname, xcomponentController, calcWidth, calcHeight]() {
            return AceType::MakeRefPtr<XComponentPattern>(id, type, libraryname, xcomponentController,
                calcWidth.GetDimension().ConvertToPx(), calcHeight.GetDimension().ConvertToPx());
        });

    CHECK_NULL_RETURN(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    if (layoutProperty) {
        layoutProperty->UpdateXComponentType(type);
        layoutProperty->UpdateUserDefinedIdealSize(CalcSize(calcWidth, calcHeight));
    }
    return frameNode;
}

void XComponentModelNG::InitXComponent(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->InitXComponent();
}

std::optional<std::string> XComponentModelNG::GetLibraryName()
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(frameNode, std::nullopt);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT) {
        return std::nullopt;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, std::nullopt);
    return xcPattern->GetLibraryName();
}

XComponentType XComponentModelNG::GetTypeImpl(const RefPtr<FrameNode>& frameNode)
{
    CHECK_NULL_RETURN(frameNode, XComponentType::UNKNOWN);
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, XComponentType::UNKNOWN);
    return layoutProperty->GetXComponentTypeValue();
}

XComponentType XComponentModelNG::GetType()
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    return GetTypeImpl(frameNode);
}

void XComponentModelNG::SetSoPath(const std::string& soPath)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetSoPath(soPath);
}

void XComponentModelNG::SetOnLoad(LoadEvent&& onLoad)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnLoad(std::move(onLoad));
}

void XComponentModelNG::SetOnLoad(FrameNode* frameNode, LoadEvent&& onLoad)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    if (xcPattern->NeedTriggerLoadEventImmediately()) {
        if (onLoad) {
            onLoad(xcPattern->GetId());
        }
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnLoad(std::move(onLoad));
}

void XComponentModelNG::SetOnDestroy(DestroyEvent&& onDestroy)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnDestroy(std::move(onDestroy));
}

void XComponentModelNG::SetOnDestroy(FrameNode* frameNode, DestroyEvent&& onDestroy)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnDestroy(std::move(onDestroy));
}

void XComponentModelNG::RegisterOnCreate(const RefPtr<AceType>& node, LoadEvent&& onLoad)
{
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    CHECK_NULL_VOID(frameNode);
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    if (!layoutProperty || layoutProperty->GetXComponentTypeValue() == XComponentType::COMPONENT) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnLoad(std::move(onLoad));
}

void XComponentModelNG::RegisterOnDestroy(const RefPtr<AceType>& node, DestroyEvent&& onDestroy)
{
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    CHECK_NULL_VOID(frameNode);
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    if (!layoutProperty || layoutProperty->GetXComponentTypeValue() == XComponentType::COMPONENT) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnDestroy(std::move(onDestroy));
}

bool XComponentModelNG::IsTexture()
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(frameNode, false);
    return GetTypeImpl(frameNode) == XComponentType::TEXTURE;
}

void XComponentModelNG::SetDetachCallback(DetachCallback&& onDetach)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetDetachEvent(std::move(onDetach));
}

void XComponentModelNG::SetControllerOnCreated(SurfaceCreatedEvent&& onCreated)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerCreatedEvent(std::move(onCreated));
}

void XComponentModelNG::SetControllerOnChanged(SurfaceChangedEvent&& onChanged)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerChangedEvent(std::move(onChanged));
}

void XComponentModelNG::SetControllerOnDestroyed(SurfaceDestroyedEvent&& onDestroyed)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerDestroyedEvent(std::move(onDestroyed));
}

void XComponentModelNG::SetRenderFit(RenderFit renderFit)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type != XComponentType::SURFACE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetRenderFit(renderFit);
}

void XComponentModelNG::EnableSecure(bool isSecure)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->EnableSecure(isSecure);
}

bool XComponentModelNG::IsTexture(FrameNode *frameNode)
{
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, false);
    return layoutProperty->GetXComponentTypeValue() == XComponentType::TEXTURE;
}

XComponentType XComponentModelNG::GetType(FrameNode* frameNode)
{
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, XComponentType::SURFACE);
    return layoutProperty->GetXComponentTypeValue(XComponentType::SURFACE);
}

RefPtr<FrameNode> XComponentModelNG::CreateFrameNode(int32_t nodeId, const std::string& id, XComponentType type,
    const std::optional<std::string>& libraryname)
{
    std::shared_ptr<InnerXComponentController> controller = nullptr;
    auto frameNode = FrameNode::CreateFrameNode(
        V2::XCOMPONENT_ETS_TAG, nodeId, AceType::MakeRefPtr<XComponentPattern>(id, type, libraryname, controller));
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    if (layoutProperty) {
        layoutProperty->UpdateXComponentType(type);
    }
    return frameNode;
}

RefPtr<FrameNode> XComponentModelNG::CreateTypeNode(int32_t nodeId, ArkUI_XComponent_Params* params)
{
    auto id = params->id;
    auto type = params->type;
    auto libraryName = params->libraryName;
    auto controller = params->controller;

    auto frameNode = FrameNode::CreateFrameNode(V2::XCOMPONENT_ETS_TAG, nodeId,
        AceType::MakeRefPtr<XComponentPattern>(id, type, libraryName, controller, 0.0, 0.0, true));
    auto layoutProperty = frameNode->GetLayoutProperty<XComponentLayoutProperty>();
    if (layoutProperty) {
        layoutProperty->UpdateXComponentType(type);
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, nullptr);
    xcPattern->SetImageAIOptions(params->aiOptions);
    return frameNode;
}

void XComponentModelNG::SetXComponentId(FrameNode* frameNode, const std::string& id)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetId(id);
}

void XComponentModelNG::SetXComponentType(FrameNode* frameNode, XComponentType type)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetType(type);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(XComponentLayoutProperty, XComponentType, type, frameNode);
}

void XComponentModelNG::SetXComponentSurfaceSize(FrameNode* frameNode, uint32_t width, uint32_t height)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->ConfigSurface(width, height);
}

std::string XComponentModelNG::GetXComponentId(FrameNode* frameNode)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, "");
    return xcPattern->GetId();
}

XComponentType XComponentModelNG::GetXComponentType(FrameNode* frameNode)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, XComponentType::SURFACE);
    return xcPattern->GetType();
}

uint32_t XComponentModelNG::GetXComponentSurfaceWidth(FrameNode* frameNode)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, DEFAULT_SURFACE_SIZE);
    auto drawSize = xcPattern->GetDrawSize();
    return drawSize.Width();
}

uint32_t XComponentModelNG::GetXComponentSurfaceHeight(FrameNode* frameNode)
{
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_RETURN(xcPattern, DEFAULT_SURFACE_SIZE);
    auto drawSize = xcPattern->GetDrawSize();
    return drawSize.Height();
}

void XComponentModelNG::EnableAnalyzer(bool enable)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->EnableAnalyzer(enable);
}

void XComponentModelNG::EnableAnalyzer(FrameNode* frameNode, bool enable)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->EnableAnalyzer(enable);
}

void XComponentModelNG::SetImageAIOptions(void* options)
{
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(frameNode);
    auto type = GetTypeImpl(frameNode);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetImageAIOptions(options);
}

void XComponentModelNG::SetXComponentLibraryname(FrameNode* frameNode, const std::string& libraryname)
{
    CHECK_NULL_VOID(frameNode);
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetLibraryName(libraryname);
}

void XComponentModelNG::SetControllerOnCreated(FrameNode* frameNode, SurfaceCreatedEvent&& onCreated)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerCreatedEvent(std::move(onCreated));
}

void XComponentModelNG::SetControllerOnChanged(FrameNode* frameNode, SurfaceChangedEvent&& onChanged)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerChangedEvent(std::move(onChanged));
}

void XComponentModelNG::SetControllerOnDestroyed(FrameNode* frameNode, SurfaceDestroyedEvent&& onDestroyed)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = frameNode->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetControllerDestroyedEvent(std::move(onDestroyed));
}

void XComponentModelNG::SetDetachCallback(FrameNode* frameNode, DetachCallback&& onDetach)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto eventHub = node->GetEventHub<XComponentEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetDetachEvent(std::move(onDetach));
}

void XComponentModelNG::SetImageAIOptions(FrameNode* frameNode, void* options)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::Claim(frameNode);
    auto type = GetTypeImpl(node);
    if (type == XComponentType::COMPONENT || type == XComponentType::NODE) {
        return;
    }
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetImageAIOptions(options);
}

void XComponentModelNG::EnableSecure(FrameNode* frameNode, bool enable)
{
    CHECK_NULL_VOID(frameNode);
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->EnableSecure(enable);
}

void XComponentModelNG::SetRenderFit(FrameNode* frameNode, RenderFit renderFit)
{
    CHECK_NULL_VOID(frameNode);
    auto xcPattern = AceType::DynamicCast<XComponentPattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(xcPattern);
    xcPattern->SetRenderFit(renderFit);
}
} // namespace OHOS::Ace::NG
