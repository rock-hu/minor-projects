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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_WINDOW_SCENE_LAYOUT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_WINDOW_SCENE_LAYOUT_MANAGER_H

#include <event_handler.h>

#include "core/components_ng/base/frame_node.h"

namespace OHOS::Rosen {
struct SessionUIParam;
class RSObjAbsGeometry;
enum class RSUINodeType : uint32_t;
}

namespace OHOS::Ace::NG {
struct TraverseResult {
    uint32_t zOrderCnt_ = 0;
    std::unordered_map<int32_t, OHOS::Rosen::SessionUIParam> uiParams_;
    uint64_t screenId_ = -1;
};

class WindowSceneLayoutManager {
public:
    static WindowSceneLayoutManager* GetInstance();
    void FlushWindowPatternInfo(const RefPtr<FrameNode>& screenNode);
    void RegisterScreenNode(uint64_t screenId, const RefPtr<FrameNode>& node);
    void UnregisterScreenNode(uint64_t screenId);

private:
    WindowSceneLayoutManager() = default;
    ~WindowSceneLayoutManager() = default;
    void Init();
    void TraverseTree(const RefPtr<FrameNode>& rootNode, TraverseResult& res,
        bool isAncestorRecent, bool isAncestorDirty, bool notSyncPosition);
    void FillWindowSceneInfo(const RefPtr<FrameNode>& node, TraverseResult& res, bool isAncestorRecent,
        bool notSyncPosition);
    bool IsNodeVisible(const RefPtr<FrameNode>& node);
    bool IsNodeDirty(const RefPtr<FrameNode>& node);
    bool IsRecentContainerState(const RefPtr<FrameNode>& node);
    bool NoNeedSyncScenePanelGlobalPosition(const RefPtr<FrameNode>& node);
    std::shared_ptr<Rosen::RSNode> GetRSNode(const RefPtr<FrameNode>& node);
    std::shared_ptr<Rosen::RSObjAbsGeometry> GetGlobalGeometry(const RefPtr<FrameNode>& node);
    std::shared_ptr<Rosen::RSObjAbsGeometry> GetLocalGeometry(const RefPtr<FrameNode>& node);
    void UpdateGeometry(const RefPtr<FrameNode>& node, const RefPtr<FrameNode>& parentNode,
        bool isParentTransformScene);
    int32_t GetNodeZIndex(const RefPtr<FrameNode>& node);
    uint64_t GetScreenId(const RefPtr<FrameNode>& screenNode);
    uint64_t GetRSNodeId(const RefPtr<FrameNode>& node);
    std::string GetWindowName(const RefPtr<FrameNode>& node);
    template<typename T>
    std::string GetWindowNameInner(const RefPtr<T>& windowPattern);
    uint32_t GetWindowId(const RefPtr<FrameNode>& node);
    template<typename T>
    uint32_t GetWindowIdInner(const RefPtr<T>& windowPattern);
    void DumpFlushInfo(uint64_t screenId, TraverseResult& res);
    void DumpNodeInfo(const RefPtr<FrameNode>& node, const RefPtr<FrameNode>& parentNode, const std::string& reason);
    void GetUINodeInfo(const RefPtr<FrameNode>& node, int32_t parentId, std::ostringstream& oss);
    void GetUITreeInfo(const RefPtr<FrameNode>& node, int32_t depth, int32_t parentId, std::ostringstream& oss);
    void GetTotalUITreeInfo(uint64_t screenId, std::string& info);
    void DumpRSNodeType(Rosen::RSUINodeType rsNode, std::ostringstream& oss);
    void GetRSNodeTreeInfo(const std::shared_ptr<RSNode>& rsNode, int32_t depth,
        std::ostringstream& oss);
    void GetRSNodeInfo(const std::shared_ptr<RSNode>& rsNode,
        std::ostringstream& oss);
    std::unordered_map<uint64_t, RefPtr<FrameNode>> screenNodeMap_;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    bool isCoreDebugEnable_ = false;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_WINDOW_SCENE_LAYOUT_MANAGER_H
