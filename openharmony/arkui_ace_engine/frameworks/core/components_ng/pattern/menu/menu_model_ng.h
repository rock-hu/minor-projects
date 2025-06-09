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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_MENU_MENU_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_MENU_MENU_MODEL_NG_H

#include "core/components_ng/pattern/menu/menu_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT MenuModelNG : public OHOS::Ace::MenuModel {
public:
    void Create() override;
    void SetFontSize(const Dimension& fontSize) override;
    void SetFontWeight(FontWeight weight) override;
    void SetFontStyle(Ace::FontStyle style) override;
    void SetFontColor(const std::optional<Color>& color) override;
    void SetFontFamily(const std::vector<std::string> &families) override;
    void ResetFontFamily() override;
    void SetWidth(const Dimension& width) override;
    void SetBorderRadius(const Dimension& radius) override;
    void ResetBorderRadius() override;
    void SetBorderRadius(const std::optional<Dimension>& radiusTopLeft, const std::optional<Dimension>& radiusTopRight,
        const std::optional<Dimension>& radiusBottomLeft, const std::optional<Dimension>& radiusBottomRight) override;
    void SetExpandingMode(const SubMenuExpandingMode& expandingMode) override;
    void SetExpandSymbol(const std::function<void(WeakPtr<NG::FrameNode>)>& expandSymbol) override;
    void SetItemDivider(const V2::ItemDivider& divider, const DividerMode& mode) override;
    void SetItemGroupDivider(const V2::ItemDivider& divider, const DividerMode& mode) override;

    static RefPtr<FrameNode>CreateFrameNode(int32_t nodeId);
    static void SetFontColor(FrameNode* frameNode, const std::optional<Color>& color);
    static void SetFontSize(FrameNode* frameNode, const std::optional<Dimension>& fontSize);
    static void SetFontWeight(FrameNode* frameNode, const std::optional<FontWeight>& weight);
    static void SetFontStyle(FrameNode* frameNode, const std::optional<Ace::FontStyle>& style);
    static void SetFontFamily(FrameNode* frameNode, const std::optional<std::vector<std::string>>& families);
    static void SetBorderRadius(FrameNode* frameNode, const std::optional<Dimension>& radius);
    static void ResetBorderRadius(FrameNode* frameNode);
    static void SetBorderRadius(FrameNode* frameNode, const std::optional<Dimension>& radiusTopLeft,
        const std::optional<Dimension>& radiusTopRight, const std::optional<Dimension>& radiusBottomLeft,
        const std::optional<Dimension>& radiusBottomRight);
    static void SetWidth(FrameNode* frameNode, const Dimension& width);
    // static void SetItemDivider(FrameNode* frameNode, const V2::ItemDivider& divider, const DividerMode& mode);
    // static void SetItemGroupDivider(FrameNode* frameNode, const V2::ItemDivider& divider, const DividerMode& mode);
    // static void SetExpandingMode(FrameNode* frameNode, const SubMenuExpandingMode& expandingMode);
    static void SetExpandSymbol(FrameNode* frameNode, const std::function<void(WeakPtr<NG::FrameNode>)>& expandSymbol);
    static RefPtr<FrameNode> CreateMenu();
    static void SetItemDivider(
        FrameNode* frameNode, const std::optional<V2::ItemDivider>& divider, const DividerMode& mode);
    static void SetItemGroupDivider(
        FrameNode* frameNode, const std::optional<V2::ItemDivider>& divider, const DividerMode& mode);
    static void SetExpandingMode(FrameNode* frameNode, const std::optional<SubMenuExpandingMode>& expandingMode);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_MENU_MENU_MODEL_NG_H
