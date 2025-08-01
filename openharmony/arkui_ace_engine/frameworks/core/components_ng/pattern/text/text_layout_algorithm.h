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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_ALGORITHM_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components_ng/layout/box_layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/text/multiple_paragraph_layout_algorithm.h"
#include "core/components_ng/pattern/text/span_node.h"
#include "core/components_ng/pattern/text/text_adapt_font_sizer.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/render/paragraph.h"

namespace OHOS::Ace::NG {
class PipelineContext;
class TextContentModifier;

struct DragSpanPosition {
    int32_t dragStart { 0 };
    int32_t dragEnd { 0 };
    int32_t spanStart { 0 };
    int32_t spanEnd { 0 };
};

// TextLayoutAlgorithm acts as the underlying text layout.
class ACE_EXPORT TextLayoutAlgorithm : public MultipleParagraphLayoutAlgorithm, public TextAdaptFontSizer {
    DECLARE_ACE_TYPE(TextLayoutAlgorithm, BoxLayoutAlgorithm, TextAdaptFontSizer);

public:
    TextLayoutAlgorithm();
    explicit TextLayoutAlgorithm(std::list<RefPtr<SpanItem>> spans, RefPtr<ParagraphManager> paragraphManager_,
        bool isSpanStringMode, const TextStyle& textStyle, const bool isMarquee = false);
    ~TextLayoutAlgorithm() override = default;

    void OnReset() override;

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper) override;

    void GetSuitableSize(SizeF& maxSize, LayoutWrapper* layoutWrapper) override {};
    bool CreateParagraphAndLayout(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper, bool needLayout = true) override;
    bool ReLayoutParagraphs(const TextStyle& textStyle, LayoutWrapper* layoutWrapper, const SizeF& maxSize);
    bool LayoutParagraphs(float maxWidth);

    float GetBaselineOffset() const override;

    size_t GetLineCount() const;

    const TextStyle& GetTextStyle() const;

    RefPtr<Paragraph> GetParagraph() const override
    {
        CHECK_NULL_RETURN(paragraphManager_, nullptr);
        CHECK_NULL_RETURN(!paragraphManager_->GetParagraphs().empty(), nullptr);
        return paragraphManager_->GetParagraphs().front().paragraph;
    }

protected:
    virtual int32_t GetPreviousLength() const
    {
        return 0;
    }

    virtual void UpdateParagraphForAISpan(
        const TextStyle& textStyle, LayoutWrapper* layoutWrapper, const RefPtr<Paragraph>& paragraph);

    void GrayDisplayAISpan(const DragSpanPosition& dragSpanPosition, const std::u16string textForAI,
        const TextStyle& textStyle, bool isDragging, const RefPtr<Paragraph>& paragraph);
    bool DidExceedMaxLines(const SizeF& maxSize) override;

    std::u16string StringOutBoundProtection(int32_t position, int32_t length, std::u16string wTextForAI);
    bool IsNeedParagraphReLayout() const override;
    double GetIndentMaxWidth(double width) const override;
    void MeasureWidthLayoutCalPolicy(LayoutWrapper* layoutWrapper) override;

private:
    OffsetF GetContentOffset(LayoutWrapper* layoutWrapper) override;
    bool UpdateSingleParagraph(LayoutWrapper* layoutWrapper, ParagraphStyle paraStyle, const TextStyle& textStyle,
        const std::u16string& content, double maxWidth);
    void UpdateRelayoutShaderStyle(LayoutWrapper* layoutWrapper);
    bool UpdateSymbolTextStyle(const TextStyle& textStyle, const ParagraphStyle& paraStyle,
        LayoutWrapper* layoutWrapper, RefPtr<FrameNode>& frameNode);
    void CreateParagraphDrag(
        const TextStyle& textStyle, const std::vector<std::u16string>& contents, const RefPtr<Paragraph>& paragraph);
    bool AdaptMinTextSize(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    bool AddPropertiesAndAnimations(TextStyle& textStyle, const RefPtr<TextLayoutProperty>& textLayoutProperty,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    bool CreateParagraph(const TextStyle& textStyle, std::u16string content, LayoutWrapper* layoutWrapper,
        double maxWidth = 0.0) override;
    bool BuildParagraph(TextStyle& textStyle, const RefPtr<TextLayoutProperty>& layoutProperty,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    bool BuildParagraphAdaptUseMinFontSize(TextStyle& textStyle, const RefPtr<TextLayoutProperty>& layoutProperty,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    bool BuildParagraphAdaptUseLayoutConstraint(TextStyle& textStyle, const RefPtr<TextLayoutProperty>& layoutProperty,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    std::optional<SizeF> BuildTextRaceParagraph(TextStyle& textStyle, const RefPtr<TextLayoutProperty>& layoutProperty,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    bool AdaptMaxTextSize(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    void UpdateSensitiveContent(std::u16string& content);
    void CheckNeedReCreateParagraph(LayoutWrapper* layoutWrapper, const TextStyle& textStyle);
    void ResetNeedReCreateParagraph(LayoutWrapper* layoutWrapper);
    void RelayoutShaderStyle(const RefPtr<TextLayoutProperty>& layoutProperty);
    bool AlwaysReCreateParagraph(LayoutWrapper* layoutWrapper);
    std::pair<bool, double> GetSuitableSize(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    std::pair<bool, double> GetSuitableSizeLD(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper, double stepSize);
    std::pair<bool, double> GetSuitableSizeBS(TextStyle& textStyle, const std::u16string& content,
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper, double stepSize);
    bool IsAdaptExceedLimit(const SizeF& maxSize) override;
    bool IsParentSizeNearZero(const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper);
    void CreateOrUpdateTextEffect(const RefPtr<Paragraph>& oldParagraph, const RefPtr<Paragraph>& newParagraph,
        const RefPtr<TextPattern>& textPattern, const std::u16string& content);
    bool IsFixIdealSizeAndNoMaxSize(LayoutWrapper* layoutWrapper, bool isWidth);
    LayoutConstraintF CalcContentConstraint(const LayoutConstraintF& constraint, LayoutWrapper* layoutWrapper);
    std::optional<float> GetCalcLayoutConstraintLength(LayoutWrapper* layoutWrapper, bool isMax, bool isWidth);
    void MeasureWithFixAtIdealSize(LayoutWrapper* layoutWrapper);
    void MeasureWithMatchParent(LayoutWrapper* layoutWrapper);
    RefPtr<PropertyBool> showSelect_;
    std::optional<LayoutConstraintF> cachedCalcContentConstraint_;
    bool isFixIdealSizeAndNoMaxWidth_ = false;
    bool alwaysReCreateParagraph_ = false;
    ACE_DISALLOW_COPY_AND_MOVE(TextLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_ALGORITHM_H
