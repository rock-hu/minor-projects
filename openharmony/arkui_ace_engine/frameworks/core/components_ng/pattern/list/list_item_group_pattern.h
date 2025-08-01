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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_GROUP_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_GROUP_PATTERN_H

#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "base/utils/utils.h"
#include "core/components_ng/pattern/list/list_item_group_accessibility_property.h"
#include "core/components_ng/pattern/list/list_children_main_size.h"
#include "core/components_ng/pattern/list/list_item_group_layout_algorithm.h"
#include "core/components_ng/pattern/list/list_item_group_layout_property.h"
#include "core/components_ng/pattern/list/list_layout_property.h"
#include "core/components_ng/pattern/list/list_position_map.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/syntax/shallow_builder.h"

namespace OHOS::Ace::NG {

struct ListItemGroupPaintInfo {
    TextDirection layoutDirection = TextDirection::LTR;
    float mainSize = 0.0f;
    bool vertical = false;
    int32_t lanes = 1;
    float spaceWidth = 0.0f;
    float laneGutter = 0.0f;
    int32_t totalItemCount = 0;
    float listContentSize = FLT_MAX;
};

enum ListItemGroupArea {
    NONE_AREA,
    IN_LIST_ITEM_AREA,
    IN_HEADER_AREA,
    IN_FOOTER_AREA
};

struct VisibleContentInfo {
    int32_t area = -1;
    int32_t indexInGroup = -1;
};

struct ListMainSizeValues {
    float startPos = 0.0f;
    float endPos = 0.0f;
    std::optional<int32_t> jumpIndexInGroup;
    float prevContentMainSize = 0.0f;
    ScrollAlign scrollAlign = ScrollAlign::START;
    std::optional<float> layoutStartMainPos;
    std::optional<float> layoutEndMainPos;
    float referencePos = 0.0f;
    float contentStartOffset = 0.0f;
    float contentEndOffset = 0.0f;
    bool forward = true;
    bool backward = false;
};

class ACE_FORCE_EXPORT ListItemGroupPattern : public Pattern {
    DECLARE_ACE_TYPE(ListItemGroupPattern, Pattern);

public:
    explicit ListItemGroupPattern(
        const RefPtr<ShallowBuilder>& shallowBuilder, V2::ListItemGroupStyle listItemGroupStyle)
        : shallowBuilder_(shallowBuilder), listItemGroupStyle_(listItemGroupStyle)
    {}
    ~ListItemGroupPattern() override = default;

    void DumpAdvanceInfo() override;
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json) override;
    bool IsAtomicNode() const override
    {
        return false;
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    ScopeFocusAlgorithm GetScopeFocusAlgorithm() override;

    bool FindHeadOrTailChild(const RefPtr<FocusHub>& groupFocus, FocusStep step, WeakPtr<FocusHub>& target);

    WeakPtr<FocusHub> GetChildFocusNodeByIndex(int32_t tarIndexInGroup);

    WeakPtr<FocusHub> GetNextFocusNode(FocusStep step, const WeakPtr<FocusHub>& currentFocusNode);

    void NotifyDataChange(int32_t index, int32_t count) override;

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<ListItemGroupLayoutProperty>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<ListItemGroupAccessibilityProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override;

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override;

    bool OnAttachAdapter(const RefPtr<FrameNode>& node, const RefPtr<UINode>& child) override
    {
        node->AddChild(child);
        return true;
    }

    void AddHeader(const RefPtr<NG::UINode>& header)
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto prevHeader = header_.Upgrade();
        if (!prevHeader) {
            host->AddChild(header, 0);
            // Initialize headerIndex_, itemStartIndex_
            if (headerIndex_ == -1) {
                auto count = header->FrameCount();
                if (count > 0) {
                    headerIndex_ = 0;
                    itemStartIndex_ += count;
                }
                if (footerIndex_ >= 0) {
                    footerIndex_ = footerIndex_ + count;
                }
            }
            host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
        } else {
            if (header != prevHeader) {
                host->ReplaceChild(prevHeader, header);
                host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
            }
        }
        header_ = header;
    }

    void AddFooter(const RefPtr<NG::UINode>& footer)
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto prevFooter = footer_.Upgrade();
        auto prevHeader = header_.Upgrade();
        if (!prevFooter) {
            if (prevHeader) {
                host->AddChildAfter(footer, prevHeader);
            } else {
                host->AddChild(footer, 0);
            }
            // Initialize itemStartIndex_, footerIndex_
            if (footerIndex_ == -1) {
                int32_t count = footer->FrameCount();
                if (count > 0) {
                    footerIndex_ = itemStartIndex_;
                    itemStartIndex_ += count;
                }
            }
            host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
        } else {
            if (footer != prevFooter) {
                host->ReplaceChild(prevFooter, footer);
                host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
            }
        }
        footer_ = footer;
    }

    void RemoveHeader()
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto prevHeader = header_.Upgrade();
        if (prevHeader && isHeaderComponentContentExist_) {
            host->RemoveChild(prevHeader);
            host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
            header_ = nullptr;
            headerIndex_ = -1;
            itemStartIndex_ = 0;
            isHeaderComponentContentExist_ = false;
        }
    }

    void RemoveFooter()
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto prevFooter = footer_.Upgrade();
        if (prevFooter && isFooterComponentContentExist_) {
            host->RemoveChild(prevFooter);
            host->MarkDirtyNode(PROPERTY_UPDATE_BY_CHILD_REQUEST);
            footer_ = nullptr;
            footerIndex_ = -1;
            footerCount_ = 0;
            isFooterComponentContentExist_ = false;
        }
    }

    bool IsHasHeader()
    {
        auto headerNode = DynamicCast<FrameNode>(header_.Upgrade());
        return headerNode ? true : false;
    }

    bool IsHasFooter()
    {
        auto footerGroup = DynamicCast<FrameNode>(footer_.Upgrade());
        return footerGroup ? true : false;
    }

    const ListItemGroupLayoutAlgorithm::PositionMap& GetItemPosition()
    {
        return itemPosition_;
    }

    void SetIndexInList(int32_t index)
    {
        indexInList_ = index;
    }

    void SetHeaderComponentContentExist(bool isHeaderComponentContentExist)
    {
        isHeaderComponentContentExist_ = isHeaderComponentContentExist;
    }

    void SetFooterComponentContentExist(bool isFooterComponentContentExist)
    {
        isFooterComponentContentExist_ = isFooterComponentContentExist;
    }

    int32_t GetIndexInList() const
    {
        return indexInList_;
    }

    int32_t GetDisplayEndIndexInGroup() const
    {
        return itemDisplayEndIndex_;
    }

    int32_t GetDisplayStartIndexInGroup() const
    {
        return itemDisplayStartIndex_;
    }

    int32_t GetItemStartIndex() const
    {
        return itemStartIndex_;
    }

    int32_t GetEndIndexInGroup() const
    {
        return (itemTotalCount_ - 1);
    }

    int32_t GetTotalItemCount() const
    {
        return itemTotalCount_;
    }

    bool IsDisplayStart() const
    {
        return itemDisplayStartIndex_ == 0;
    }

    int32_t IsDisplayEnd() const
    {
        return itemTotalCount_ == 0 || itemDisplayEndIndex_ == (itemTotalCount_ - 1);
    }

    int32_t GetLanesInGroup() const
    {
        return lanes_;
    }

    void SetLanes(int32_t num)
    {
        lanes_ = num;
    }

    V2::ListItemGroupStyle GetListItemGroupStyle() const
    {
        return listItemGroupStyle_;
    }

    float GetHeaderMainSize() const
    {
        return headerMainSize_;
    }

    float GetFooterMainSize() const
    {
        return footerMainSize_;
    }

    float GetEstimateOffset(float height, const std::pair<float, float>& targetPos,
        float headerMainSize, float footerMainSize) const;
    float GetEstimateHeight(float& averageHeight, float headerMainSize, float footerMainSize, float spaceWidth) const;
    bool HasLayoutedItem() const
    {
        return layouted_ && (layoutedItemInfo_.has_value() || itemTotalCount_ == 0);
    }

    void SetItemPressed(bool isPressed, int32_t id)
    {
        if (isPressed) {
            pressedItem_.emplace(id);
        } else {
            pressedItem_.erase(id);
        }
    }

    void ResetLayoutedInfo()
    {
        layouted_ = false;
        layoutedItemInfo_.reset();
        itemPosition_.clear();
    }

    void SetListItemGroupStyle(V2::ListItemGroupStyle style);
    RefPtr<ListChildrenMainSize> GetOrCreateListChildrenMainSize();
    void SetListChildrenMainSize(float defaultSize, const std::vector<float>& mainSize);
    void OnChildrenSizeChanged(std::tuple<int32_t, int32_t, int32_t> change, ListChangeFlag flag);
    bool ListChildrenSizeExist();
    RefPtr<FrameNode> GetListFrameNode() const;
    VisibleContentInfo GetStartListItemIndex();
    VisibleContentInfo GetEndListItemIndex();
    void ResetChildrenSize();
    bool IsInViewport(int32_t index) const;

    void ClearItemPosition();
    void ClearCachedItemPosition();
    void CalculateItemStartIndex();
    bool NeedCacheForward(const LayoutWrapper* listWrapper) const;
    CachedIndexInfo UpdateCachedIndex(bool outOfView, bool reCache, int32_t forwardCache, int32_t backwardCache);
    int32_t UpdateCachedIndexForward(bool outOfView, bool show, int32_t cacheCount);
    int32_t UpdateCachedIndexBackward(bool outOfView, bool show, int32_t cacheCount);
    std::pair<int32_t, int32_t> UpdateCachedIndexOmni(int32_t forwardCache, int32_t backwardCache);
    void UpdateActiveChildRange(bool forward, int32_t cacheCount, bool show);
    void UpdateActiveChildRange(bool show);
    void SyncItemsToCachedItemPosition();
    bool IsVisible() const;
    void SetRecache(bool value)
    {
        reCache_ = value;
    }
    void LayoutCache(const LayoutConstraintF& constraint, int64_t deadline, int32_t forwardCached,
        int32_t backwardCached, ListMainSizeValues listSizeValues);
    // void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    RefPtr<UINode> GetHeader() const
    {
        return header_.Upgrade();
    }

    RefPtr<UINode> GetFooter() const
    {
        return footer_.Upgrade();
    }

    void OnColorModeChange(uint32_t colorMode) override;
    void UpdateDefaultColor();

    bool ChildPreMeasureHelperEnabled() override
    {
        return true;
    }
    bool PostponedTaskForIgnoreEnabled() override
    {
        return true;
    }

    bool NeedCustomizeSafeAreaPadding() override
    {
        return true;
    }

    PaddingPropertyF CustomizeSafeAreaPadding(PaddingPropertyF safeAreaPadding, bool needRotate) override;

    bool ChildTentativelyLayouted() override
    {
        return true;
    }

    bool AccumulatingTerminateHelper(RectF& adjustingRect, ExpandEdges& totalExpand, bool fromSelf = false,
        LayoutSafeAreaType ignoreType = NG::LAYOUT_SAFE_AREA_TYPE_SYSTEM) override;

private:
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;
    void OnAttachToFrameNode() override;
    void SetListItemGroupDefaultAttributes(const RefPtr<FrameNode>& itemGroupNode);
    void OnColorConfigurationUpdate() override;
    void CheckListDirectionInCardStyle();
    float GetPaddingAndMargin() const;
    float GetListPaddingOffset(const RefPtr<FrameNode>& listNode) const;
    bool FirstItemFullVisible(const RefPtr<FrameNode>& listNode) const;
    bool CheckDataChangeOutOfStart(int32_t index, int32_t count, int32_t startIndex, int32_t endIndex);

    void HandleForwardStep(
        const RefPtr<FrameNode>& curFrame, int32_t curIndexInGroup, int32_t& moveStep, int32_t& nextIndex);
    void HandleBackwardStep(
        const RefPtr<FrameNode>& curFrame, int32_t curIndexInGroup, int32_t& moveStep, int32_t& nextIndex);
    bool HandleCrossAxisRightOrDownStep(
        bool isVertical, int32_t curIndexInGroup, int32_t& moveStep, int32_t& nextIndex);
    bool HandleCrossAxisLeftOrUpStep(bool isVertical, int32_t curIndexInGroup, int32_t& moveStep, int32_t& nextIndex);

    bool GetCurrentFocusIndices(
        const RefPtr<FrameNode>& curFrame, const RefPtr<Pattern>& curPattern, int32_t& curIndexInGroup);
    bool IsListVertical() const;
    void AdjustFocusStepForRtl(FocusStep& step, bool isVertical);
    bool DetermineSingleLaneStep(
        FocusStep step, bool isVertical, int32_t curIndex, int32_t& moveStep, int32_t& nextIndex);
    bool DetermineMultiLaneStep(FocusStep step, bool isVertical, const RefPtr<FrameNode>& curFrame,
        int32_t curIndexInGroup, int32_t& moveStep, int32_t& nextIndex);
    bool IsIndexInValidRange(int32_t index, int32_t maxIndex);
    bool IsFocusMovementBlock(int32_t nextIndex, int32_t curIndex, int32_t maxIndex) const;
    WeakPtr<FocusHub> FindNextValidFocus(int32_t moveStep, int32_t curIndexInGroup, int32_t curGroupIndexInList,
        int32_t nextIndexInGroup, const WeakPtr<FocusHub>& currentFocusNode, FocusStep step);
    void AdjustMountTreeSequence(int32_t footerCount);
    void MappingPropertiesFromLayoutAlgorithm(const RefPtr<ListItemGroupLayoutAlgorithm>& layoutAlgorithm);
    const ListItemGroupInfo* GetPosition(int32_t index) const;
    bool NextPositionBlocksMove(
        const ListItemGroupInfo* curPos, const ListItemGroupInfo* nextPos, bool isVertical) const;

    RefPtr<ShallowBuilder> shallowBuilder_;
    RefPtr<ListPositionMap> posMap_;
    RefPtr<ListChildrenMainSize> childrenSize_;
    V2::ListItemGroupStyle listItemGroupStyle_ = V2::ListItemGroupStyle::NONE;

    int32_t indexInList_ = 0;

    WeakPtr<UINode> header_;
    WeakPtr<UINode> footer_;
    bool isHeaderComponentContentExist_ = false;
    bool isFooterComponentContentExist_ = false;
    int32_t itemStartIndex_ = 0;
    int32_t headerIndex_ = -1;
    int32_t footerIndex_ = -1;
    int32_t footerCount_ = 0;
    int32_t itemTotalCount_ = -1;
    int32_t itemDisplayEndIndex_ = -1;
    int32_t itemDisplayStartIndex_ = -1;
    float_t headerMainSize_ = 0.0f;
    float_t footerMainSize_ = 0.0f;

    std::optional<LayoutedItemInfo> layoutedItemInfo_;
    std::set<int32_t> pressedItem_;
    bool layouted_ = false;

    bool reCache_ = false;
    int32_t backwardCachedIndex_ = INT_MAX;
    int32_t forwardCachedIndex_ = -1;
    ListItemGroupLayoutAlgorithm::PositionMap cachedItemPosition_;
    float adjustRefPos_ = 0.0f;
    float adjustTotalSize_ = 0.0f;

    ListItemGroupLayoutAlgorithm::PositionMap itemPosition_;
    float spaceWidth_ = 0.0f;
    Axis axis_ = Axis::VERTICAL;
    int32_t lanes_ = 1;
    float laneGutter_ = 0.0f;
    float startHeaderPos_ = 0.0f;
    float endFooterPos_ = 0.0f;
    TextDirection layoutDirection_ = TextDirection::LTR;
    float mainSize_ = 0.0f;
    float listContentSize_ = 0.0f;
    bool isStackFromEnd_ = false;
    bool prevMeasureBreak_ = false;
    ACE_DISALLOW_COPY_AND_MOVE(ListItemGroupPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_PATTERN_H
