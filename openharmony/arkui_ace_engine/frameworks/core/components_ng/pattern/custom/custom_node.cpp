/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/custom/custom_node.h"

#include "base/log/ace_performance_monitor.h"
#include "base/log/dump_log.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
RefPtr<CustomNode> CustomNode::CreateCustomNode(int32_t nodeId, const std::string& viewKey)
{
    auto node = MakeRefPtr<CustomNode>(nodeId, viewKey);
    ElementRegister::GetInstance()->AddUINode(node);
    return node;
}

void CustomNode::Build(std::shared_ptr<std::list<ExtraInfo>> extraInfos)
{
    Render();
    if (extraInfos) {
        extraInfos_ = *extraInfos;
    }
    UINode::Build(extraInfos);
}

void CustomNode::Render()
{
    // NOTE: this function will be re-enter, we need backup needMarkParent_ first and restore it later.
    bool needMarkParentBak = needMarkParent_;
    needMarkParent_ = false;
    if (renderFunction_) {
        RenderFunction renderFunction = nullptr;
        std::swap(renderFunction, renderFunction_);
        {
            ACE_SCOPED_TRACE("CustomNode:OnAppear");
            FireOnAppear();
        }
        {
            COMPONENT_CREATION_DURATION();
            ACE_SCOPED_TRACE("CustomNode:BuildItem [%s][self:%d][parent:%d]", GetJSViewName().c_str(), GetId(),
                GetParent() ? GetParent()->GetId() : 0);
            // first create child node and wrapper.
            ScopedViewStackProcessor scopedViewStackProcessor;
            auto parent = GetParent();
            bool parentNeedExportTexture = parent ? parent->IsNeedExportTexture() : false;
            ViewStackProcessor::GetInstance()->SetIsExportTexture(parentNeedExportTexture || IsNeedExportTexture());
            auto child = renderFunction();
            if (child) {
                child->MountToParent(Claim(this));
            }
        }
        {
            ACE_SCOPED_TRACE("CustomNode::DidBuild");
            FireDidBuild();
        }
    }
    {
        FireRecycleRenderFunc();
    }
    needMarkParent_ = needMarkParentBak;
}

void CustomNode::FireCustomDisappear()
{
    if (!CheckFireOnAppear()) {
        FireOnAppear();
        FireDidBuild();
    }
    FireOnDisappear();
    Reset();
    UINode::FireCustomDisappear();
}

// used in HotReload to update root view @Component
void CustomNode::FlushReload()
{
    CHECK_NULL_VOID(completeReloadFunc_);
    Clean();
    renderFunction_ = completeReloadFunc_;
    Build(nullptr);
}

bool CustomNode::RenderCustomChild(int64_t deadline)
{
    if (GetSysTimestamp() > deadline) {
        return false;
    }
    Render();
    return UINode::RenderCustomChild(deadline);
}

void CustomNode::SetJSViewActive(bool active, bool isLazyForEachNode, bool isReuse)
{
    if (GetJsActive() != active) {
        SetJsActive(active);
        FireSetActiveFunc(active, isReuse);
    }
}

void CustomNode::AdjustLayoutWrapperTree(const RefPtr<LayoutWrapperNode>& parent, bool forceMeasure, bool forceLayout)
{
    if (parent->GetHostTag() != V2::TAB_CONTENT_ITEM_ETS_TAG) {
        Render();
        UINode::AdjustLayoutWrapperTree(parent, forceMeasure, forceLayout);
        return;
    }

    if (!renderFunction_ && !HasRecycleRenderFunc()) {
        UINode::AdjustLayoutWrapperTree(parent, forceMeasure, forceLayout);
        return;
    }

    parent->AppendChild(MakeRefPtr<LayoutWrapperNode>(
        [weak = AceType::WeakClaim(this), forceMeasure, forceLayout](RefPtr<LayoutWrapperNode> layoutWrapper) {
            auto customNode = weak.Upgrade();
            CHECK_NULL_VOID(customNode);

            customNode->Render();
            if (customNode->GetChildren().empty()) {
                return;
            }
            auto child = customNode->GetChildren().front();
            while (!InstanceOf<FrameNode>(child)) {
                auto custom = DynamicCast<CustomNode>(child);
                if (custom) {
                    custom->Render();
                }
                auto children = child->GetChildren();
                if (children.empty()) {
                    return;
                }
                child = children.front();
            }
            auto frameChild = DynamicCast<FrameNode>(child);
            CHECK_NULL_VOID(frameChild);
            frameChild->UpdateLayoutWrapper(layoutWrapper, forceMeasure, forceLayout);
        }));
}

RefPtr<LayoutWrapperNode> CustomNode::CreateLayoutWrapper(bool forceMeasure, bool forceLayout)
{
    Build(nullptr);
    return UINode::CreateLayoutWrapper(forceMeasure, forceLayout);
}

void CustomNode::MarkNeedSyncRenderTree(bool needRebuild)
{
    if (needMarkParent_) {
        UINode::MarkNeedSyncRenderTree(needRebuild);
    }
}

RefPtr<UINode> CustomNode::GetFrameChildByIndex(uint32_t index, bool needBuild, bool isCache, bool addToRenderTree)
{
    if (!isCache) {
        SetJSViewActive(true);
    }
    Render();
    return UINode::GetFrameChildByIndex(index, needBuild, isCache, addToRenderTree);
}

void CustomNode::DoSetActiveChildRange(
    int32_t start, int32_t end, int32_t cacheStart, int32_t cacheEnd, bool showCache)
{
    if (showCache) {
        start -= cacheStart;
        end += cacheEnd;
    }
    if (start <= end) {
        if (start > 0 || end < 0) {
            SetActive(false);
            SetJSViewActive(false);
        } else {
            SetJSViewActive(true);
        }
    } else {
        if (end < 0 && start > 0) {
            SetActive(false);
            SetJSViewActive(false);
        } else {
            SetJSViewActive(true);
        }
    }
}

std::unique_ptr<JsonValue> CustomNode::GetStateInspectorInfo()
{
    std::string res = FireOnDumpInspectorFunc();
    TAG_LOGD(AceLogTag::ACE_STATE_MGMT, "ArkUI State Inspector dump info %{public}s", res.c_str());
    auto json = JsonUtil::ParseJsonString(res);
    return json;
}

void CustomNode::DumpComponentInfo(std::unique_ptr<JsonValue>& componentInfo)
{
    DumpLog::GetInstance().AddDesc("ComponentName: " + componentInfo->GetValue("ComponentName")->ToString());
    DumpLog::GetInstance().AddDesc("isV2: " + componentInfo->GetValue("isV2")->ToString());
    DumpLog::GetInstance().AddDesc("isFreezeAllowed: " + \
                                    componentInfo->GetValue("isCompFreezeAllowed_")->ToString());
    DumpLog::GetInstance().AddDesc("isViewActive: " + componentInfo->GetValue("isViewActive_")->ToString());
}

void CustomNode::DumpDecoratorInfo(std::unique_ptr<JsonValue>& decoratorInfo)
{
    int size = decoratorInfo->GetArraySize();

    DumpLog::GetInstance().AddDesc("-----start print decoratorInfo");
    for (int i = 0; i < size; i++) {
        auto decoratorItem = decoratorInfo->GetArrayItem(i);
        DumpLog::GetInstance().AddDesc("decorator:" + decoratorItem->GetValue("decorator")->ToString() + \
                                       " propertyName:" + decoratorItem->GetValue("propertyName")->ToString() + \
                                       " value:" + decoratorItem->GetValue("value")->ToString());
        DumpLog::GetInstance().AddDesc("stateVariable id: " + decoratorItem->GetValue("id")->ToString());
        DumpLog::GetInstance().AddDesc("inRenderingElementId: " + \
                                        decoratorItem->GetValue("inRenderingElementId")->ToString());
        DumpLog::GetInstance().AddDesc("dependentElementIds: " + \
                                        decoratorItem->GetValue("dependentElementIds")->ToString());
        if (i < size - 1) {
            DumpLog::GetInstance().AddDesc("------------------------------");
        }
    }
    DumpLog::GetInstance().AddDesc("-----end print decoratorInfo");
}

void CustomNode::DumpInfo()
{
    std::string ret = FireOnDumpInspectorFunc();
    TAG_LOGD(AceLogTag::ACE_STATE_MGMT, "ArkUI DumpInfo %{public}s", ret.c_str());
    if (ret != "") {
        auto json = JsonUtil::ParseJsonString(ret);
        if (json == nullptr || !json->IsValid()) {
            TAG_LOGE(AceLogTag::ACE_STATE_MGMT, "ParseJsonString failed");
            return;
        }
        auto componentInfo = json->GetValue("viewInfo");
        if (componentInfo != nullptr) {
            DumpComponentInfo(componentInfo);
        }
        auto decoratorInfo = json->GetValue("observedPropertiesInfo");
        if (decoratorInfo != nullptr) {
            DumpDecoratorInfo(decoratorInfo);
        }
    }
}
} // namespace OHOS::Ace::NG
