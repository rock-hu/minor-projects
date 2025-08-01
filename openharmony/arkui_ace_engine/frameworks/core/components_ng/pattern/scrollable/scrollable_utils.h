/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLLABLE_SCROLLABLE_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLLABLE_SCROLLABLE_UTILS_H

#include "base/geometry/axis.h"
#include "core/components_ng/layout/layout_property.h"

namespace OHOS::Ace::NG {
class LayoutWrapper;

struct MoveOffsetParam {
    bool isVertical = false;
    float contentStartOffset = 0;
    float contentEndOffset = 0;
    bool noNeedMargin = false;
};

struct ScrollableUtils {
    /**
     * @brief Disable lazyForEach prebuild cache item.
     *
     * @param node The host node.
     */
    static void DisableLazyForEachBuildCache(const RefPtr<UINode>& node);

    /**
     * @brief Checks if contentHeight needs to expand based on safeArea property.
     *
     * @param layoutProps The layout properties to check.
     * @param axis The scrollable axis.
     * @return The height expansion amount.
     */
    static float CheckHeightExpansion(const RefPtr<LayoutProperty>& layoutProps, Axis axis);

    /**
     * @brief Recycle items which are out of boundary.
     *
     * @param axis scroll direction
     * @param offset scroll offset, positive means scroll down, negative means scroll up.
     * @param start first item index
     * @param end last item index
     * @param wrapper layout wrapper
     */
    static void RecycleItemsOutOfBoundary(Axis axis, float offset, int32_t start, int32_t end, LayoutWrapper* wrapper);

    /**
     * @brief get moveoffset from parentFrameNode when curFrameNode focused.
     *
     * @param parentFrameNode parent framenode with scroll function
     * @param curFrameNode current focused framenode
     * @param isVertical vertical or not
     * @param contentStartOffset content start offset from parentFrameNode
     * @param contentEndOffset content end offset from parentFrameNode
     */
    static float GetMoveOffset(
        const RefPtr<FrameNode>& parentFrameNode,
        const RefPtr<FrameNode>& curFrameNode,
        const MoveOffsetParam& param);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SCROLLABLE_SCROLLABLE_UTILS_H
