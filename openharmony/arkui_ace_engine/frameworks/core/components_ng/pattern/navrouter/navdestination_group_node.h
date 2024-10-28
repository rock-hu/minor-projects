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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVROUTER_NAVDESTINATION_GROUP_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVROUTER_NAVDESTINATION_GROUP_NODE_H

#include <cstdint>
#include <list>

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/group_node.h"
#include "core/components_ng/pattern/navigation/navdestination_node_base.h"
#include "core/components_ng/pattern/navigation/navigation_declaration.h"
#include "core/components_ng/property/property.h"
#include "core/pipeline/base/element_register.h"

namespace OHOS::Ace::NG {

class CustomNodeBase;
class NavigationTransitionProxy;

using NavDestinationBackButtonEvent = std::function<bool(GestureEvent&)>;

class ACE_EXPORT NavDestinationGroupNode : public NavDestinationNodeBase {
    DECLARE_ACE_TYPE(NavDestinationGroupNode, NavDestinationNodeBase)
public:
    NavDestinationGroupNode(const std::string& tag, int32_t nodeId, const RefPtr<Pattern>& pattern)
        : NavDestinationNodeBase(tag, nodeId, pattern)
    {
        isNewToolbar_ = true;
    }
    ~NavDestinationGroupNode() override;
    void AddChildToGroup(const RefPtr<UINode>& child, int32_t slot = DEFAULT_NODE_SLOT) override;
    void DeleteChildFromGroup(int32_t slot = DEFAULT_NODE_SLOT) override;
    static RefPtr<NavDestinationGroupNode> GetOrCreateGroupNode(
        const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator);
    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    void SetNavDestinationBackButtonEvent(const NavDestinationBackButtonEvent& backButtonEvent)
    {
        backButtonEvent_ = backButtonEvent;
    }

    NavDestinationBackButtonEvent GetNavDestinationBackButtonEvent() const
    {
        return backButtonEvent_;
    }

    void OnAttachToMainTree(bool recursive) override;

    void OnOffscreenProcess(bool recursive) override;

    void ProcessShallowBuilder();

    void SetIsOnAnimation(bool isOnAnimation)
    {
        isOnAnimation_ = isOnAnimation;
    }

    bool IsOnAnimation() const
    {
        return isOnAnimation_;
    }

    RefPtr<CustomNodeBase> GetNavDestinationCustomNode();
    
    void SetNavDestinationCustomNode(WeakPtr<CustomNodeBase> customNode)
    {
        customNode_ = customNode;
    }

    void SetNavDestinationMode(NavDestinationMode mode);

    NavDestinationMode GetNavDestinationMode() const
    {
        return mode_;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetIsCacheNode(bool cache)
    {
        isCacheNode_ = cache;
    }

    bool IsCacheNode() const
    {
        return isCacheNode_;
    }

    void SetIsAnimated(const bool isAnimated)
    {
        isAnimated_ = isAnimated;
    }

    bool GetIsAnimated() const
    {
        return isAnimated_;
    }

    void SetCanReused(bool canReused)
    {
        canReused_ = canReused;
    }

    bool GetCanReused() const
    {
        return canReused_;
    }

    void SetNavDestinationPathInfo(const std::string& moduleName, const std::string& pagePath)
    {
        navDestinationPathInfo_ = pagePath;
        navDestinationModuleName_ = moduleName;
    }

    const std::string& GetNavDestinationPathInfo() const
    {
        return navDestinationPathInfo_;
    }

    void SetNeedRemoveInPush(bool need)
    {
        needRemoveInPush_ = need;
    }

    bool NeedRemoveInPush() const
    {
        return needRemoveInPush_;
    }

    std::shared_ptr<AnimationUtils::Animation> BackButtonAnimation(bool isTransitionIn);
    std::shared_ptr<AnimationUtils::Animation> TitleOpacityAnimation(bool isTransitionOut);

    void InitSystemTransitionPush(bool transitionIn);
    void StartSystemTransitionPush(bool transitionIn);
    void SystemTransitionPushCallback(bool transitionIn, const int32_t animationId);
    void InitSystemTransitionPop(bool isTransitionIn);
    void StartSystemTransitionPop(bool transitionIn);
    bool SystemTransitionPopCallback(const int32_t animationId);
    void InitDialogTransition(bool isZeroY);

    void SetRecoverable(bool recoverable)
    {
        recoverable_ = recoverable;
    }

    void SetFromNavrouterAndNoRouteInfo(bool fromNavrouterAndNoRouteInfo)
    {
        fromNavrouterAndNoRouteInfo_ = fromNavrouterAndNoRouteInfo;
    }

    bool CanRecovery() const
    {
        return recoverable_ && !fromNavrouterAndNoRouteInfo_;
    }

    void SetNeedAppearFromRecovery(bool needAppear)
    {
        needAppearFromRecovery_ = needAppear;
    }

    bool NeedAppearFromRecovery() const
    {
        return needAppearFromRecovery_;
    }

    void UpdateTextNodeListAsRenderGroup(bool isPopPage, const RefPtr<NavigationTransitionProxy>& proxy);
    void ReleaseTextNodeList();
    void CollectTextNodeAsRenderGroup(bool isPopPage);

    void CleanContent();
private:
    WeakPtr<CustomNodeBase> customNode_; // nearest parent customNode
    NavDestinationBackButtonEvent backButtonEvent_;
    bool isOnAnimation_ = false;
    int32_t index_ = -1;
    NavDestinationMode mode_ = NavDestinationMode::STANDARD;
    bool isCacheNode_ = false;
    bool isAnimated_ = true;
    bool canReused_ = true;
    bool recoverable_ = true;
    bool fromNavrouterAndNoRouteInfo_ = false;
    bool needAppearFromRecovery_ = false;
    std::string navDestinationPathInfo_;
    std::string navDestinationModuleName_;
    bool needRemoveInPush_ = false;
    std::list<WeakPtr<UINode>> textNodeList_;
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVROUTER_NAVDESTINATION_GROUP_NODE_H
