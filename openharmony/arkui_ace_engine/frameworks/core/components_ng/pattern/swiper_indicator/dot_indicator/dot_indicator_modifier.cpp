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

#include "core/components_ng/pattern/swiper_indicator/dot_indicator/dot_indicator_modifier.h"

#include "base/utils/utils.h"
#include "core/animation/spring_curve.h"
#include "core/components_ng/render/animation_utils.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/paint_property.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t POINT_HOVER_ANIMATION_DURATION = 100;
constexpr int32_t COMPONENT_DILATE_ANIMATION_DURATION = 250;
constexpr int32_t COMPONENT_SHRINK_ANIMATION_DURATION = 300;
constexpr int32_t MOUSE_PRESS_ANIMATION_DURATION = 250;

constexpr float BLACK_POINT_CENTER_BEZIER_CURVE_VELOCITY = 0.4f;
constexpr float CENTER_BEZIER_CURVE_MASS = 0.0f;
constexpr float CENTER_BEZIER_CURVE_STIFFNESS = 1.0f;
constexpr float CENTER_BEZIER_CURVE_DAMPING = 1.0f;
constexpr uint32_t ITEM_HALF_WIDTH = 0;
constexpr uint32_t ITEM_HALF_HEIGHT = 1;
constexpr uint32_t SELECTED_ITEM_HALF_WIDTH = 2;
constexpr uint32_t SELECTED_ITEM_HALF_HEIGHT = 3;
constexpr float TOUCH_BOTTOM_CURVE_VELOCITY = 0.1f;
constexpr float TOUCH_BOTTOM_CURVE_MASS = 0.2f;
constexpr float TOUCH_BOTTOM_CURVE_STIFFNESS = 0.48f;
constexpr float TOUCH_BOTTOM_CURVE_DAMPING = 1.0f;
constexpr float TOUCH_BOTTOM_BACKGROUND_WIDTH_MULTIPLE = 1.225f;
constexpr float TOUCH_BOTTOM_BACKGROUND_HEIGHT_MULTIPLE = 0.8f;
constexpr float TOUCH_BOTTOM_DOT_WIDTH_MULTIPLE = 0.0125f;
constexpr int32_t DEFAULT_TOUCH_BOTTOM_ANIMATION_DURATION = 200;
constexpr int32_t DEFAULT_OPACITY_ANIMATION_DURATION = 100;
constexpr float LOOP_TRANSLATE_DURATION_PERCENT = 0.5f;
constexpr float LOOP_OPACITY_DURATION_PERCENT = 0.25f;
constexpr uint8_t TARGET_ALPHA = 255;
constexpr int32_t BLACK_POINT_DURATION = 400;
constexpr float DEFAULT_MINIMUM_AMPLITUDE_PX = 1.0f;
constexpr float HALF_FLOAT = 0.5f;
constexpr float TWO_FLOAT = 2.0f;
constexpr int32_t LONG_POINT_STEP_TWO = 2;
constexpr int32_t LONG_POINT_STEP_THREE = 3;
constexpr int32_t LONG_POINT_STEP_FOUR = 4;
// velocity:0, mass:1, stiffness:81, damping:11
const auto LONG_POINT_DEFAULT_CURVE = AceType::MakeRefPtr<InterpolatingSpring>(0, 1, 81, 11);
const auto LONG_POINT_STEP_TWO_CURVE = AceType::MakeRefPtr<InterpolatingSpring>(0, 1, 108, 16);
const auto LONG_POINT_STEP_THREE_CURVE = AceType::MakeRefPtr<InterpolatingSpring>(0, 1, 128, 18);
const auto LONG_POINT_STEP_FOUR_CURVE = AceType::MakeRefPtr<InterpolatingSpring>(0, 1, 128, 20);
const auto LONG_POINT_STEP_FIVE_CURVE = AceType::MakeRefPtr<InterpolatingSpring>(0, 1, 148, 28);
constexpr float SMALLEST_POINT_RATIO = 1.0f / 3.0f;
constexpr float SECOND_SMALLEST_POINT_RATIO = 2.0f / 3.0f;
constexpr int32_t OVERLONG_SMALL_COUNT = 2;
} // namespace

void DotIndicatorModifier::onDraw(DrawingContext& context)
{
    ContentProperty contentProperty;
    contentProperty.backgroundColor = backgroundColor_->Get().ToColor();
    contentProperty.vectorBlackPointCenterX = vectorBlackPointCenterX_->Get();
    contentProperty.longPointLeftCenterX = longPointLeftCenterX_->Get();
    contentProperty.longPointRightCenterX = longPointRightCenterX_->Get();
    contentProperty.normalToHoverPointDilateRatio = normalToHoverPointDilateRatio_->Get();
    contentProperty.hoverToNormalPointDilateRatio = hoverToNormalPointDilateRatio_->Get();
    contentProperty.longPointDilateRatio = longPointDilateRatio_->Get();
    contentProperty.indicatorPadding = indicatorPadding_->Get();
    contentProperty.indicatorMargin = indicatorMargin_->Get();
    contentProperty.itemHalfSizes = itemHalfSizes_->Get();
    SetFocusedAndSelectedColor(contentProperty);
    PaintBackground(context, contentProperty);
    PaintContent(context, contentProperty);
}

void DotIndicatorModifier::SetFocusedAndSelectedColor(ContentProperty& contentProperty)
{
    auto indicatorTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(indicatorTheme);
    paddingSide_ = indicatorTheme->GetIndicatorPaddingDot();
    scaleIndicator_ = indicatorTheme->GetIndicatorScale();
    Color currentSelectedColor = selectedColor_->Get().ToColor();
    Color currentUnselectedColor = unselectedColor_->Get();
    if (isFocused_->Get()) {
        originalUnselectColor_ = (indicatorTheme->GetColor() == currentUnselectedColor)
                                     ? indicatorTheme->GetFocusUnSelectedColor()
                                     : currentUnselectedColor;
        originalSelectColor_ = (indicatorTheme->GetSelectedColor() == currentSelectedColor)
                                   ? indicatorTheme->GetFocusedSelectedColor()
                                   : currentSelectedColor;
        contentProperty.backgroundColor = indicatorTheme->GetFocusedBgColor();
    } else {
        originalUnselectColor_ = currentUnselectedColor;
        originalSelectColor_ = currentSelectedColor;
        contentProperty.backgroundColor = backgroundColor_->Get().ToColor();
    }
}

void DotIndicatorModifier::PaintBackground(
    DrawingContext& context, const ContentProperty& contentProperty, int32_t maxDisplayCount, bool isBindIndicator)
{
    CHECK_NULL_VOID(contentProperty.backgroundColor.GetAlpha());
    auto itemWidth = contentProperty.itemHalfSizes[ITEM_HALF_WIDTH] * 2;
    auto itemHeight = contentProperty.itemHalfSizes[ITEM_HALF_HEIGHT] * 2;
    auto selectedItemWidth = contentProperty.itemHalfSizes[SELECTED_ITEM_HALF_WIDTH] * 2;
    auto selectedItemHeight = contentProperty.itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT] * 2;
    auto pointNumber = static_cast<float>(contentProperty.vectorBlackPointCenterX.size());
    float allPointDiameterSum = itemWidth * static_cast<float>(pointNumber + 1);
    if (isCustomSize_) {
        allPointDiameterSum = itemWidth * static_cast<float>(pointNumber - 1) + selectedItemWidth;
    }
    float allPointSpaceSum = static_cast<float>(GetIndicatorDotItemSpace().ConvertToPx()) * (pointNumber - 1);

    if (maxDisplayCount > 0) {
        allPointSpaceSum = static_cast<float>(GetIndicatorDotItemSpace().ConvertToPx()) * (maxDisplayCount - 1);
        allPointDiameterSum = itemWidth * (maxDisplayCount - OVERLONG_SMALL_COUNT - 1) + selectedItemWidth +
                              itemWidth * SECOND_SMALLEST_POINT_RATIO + itemWidth * SMALLEST_POINT_RATIO;
    }

    // Background necessary property
    float rectWidth =
        contentProperty.indicatorPadding + allPointDiameterSum + allPointSpaceSum + contentProperty.indicatorPadding;
    auto indicatorTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(indicatorTheme);
    auto indicatorHeightPadding = indicatorTheme->GetIndicatorBgHeight().ConvertToPx();
    float rectHeight = indicatorHeightPadding + itemHeight + indicatorHeightPadding;
    if (selectedItemHeight > itemHeight) {
        rectHeight = indicatorHeightPadding + selectedItemHeight + indicatorHeightPadding;
    }

    auto [rectLeft, rectRight, rectTop, rectBottom] =
        CalcAndAdjustIndicatorPaintRect(contentProperty, rectWidth, rectHeight);
    // Paint background
    RSCanvas& canvas = context.canvas;
    RSBrush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ToRSColor(contentProperty.backgroundColor));
    canvas.AttachBrush(brush);
    auto radius = axis_ == Axis::HORIZONTAL ? rectHeight : rectWidth;
    canvas.DrawRoundRect({ { rectLeft, rectTop, rectRight, rectBottom }, radius, radius });
    canvas.DetachBrush();
}

std::tuple<float, float, float, float> DotIndicatorModifier::CalcAndAdjustIndicatorPaintRect(
    const ContentProperty& contentProperty, float& rectWidth, float& rectHeight)
{
    auto widthChangeValue = (backgroundWidthDilateRatio_->Get() - 1.0f) * rectWidth;
    auto heightChangeValue = (1.0f - backgroundHeightDilateRatio_->Get()) * rectHeight;
    if (axis_ == Axis::VERTICAL) {
        std::swap(widthChangeValue, heightChangeValue);
    }
    // Property to get the rectangle offset
    float rectLeft =
        axis_ == Axis::HORIZONTAL ? contentProperty.indicatorMargin.GetX() : contentProperty.indicatorMargin.GetY();
    float rectTop =
        axis_ == Axis::HORIZONTAL ? contentProperty.indicatorMargin.GetY() : contentProperty.indicatorMargin.GetX();
    // Adapter circle and rect
    float rectRight = rectLeft + (axis_ == Axis::HORIZONTAL ? rectWidth : rectHeight);
    float rectBottom = rectTop + (axis_ == Axis::HORIZONTAL ? rectHeight : rectWidth);

    if (axis_ == Axis::HORIZONTAL) {
        boundsRectF_.SetWidth(rectRight - rectLeft + widthChangeValue * TWO_FLOAT);
        boundsRectF_.SetHeight(rectBottom - rectTop);
        if (touchBottomType_ == TouchBottomType::START) {
            rectLeft -= widthChangeValue;
        }
        if (touchBottomType_ == TouchBottomType::END) {
            rectRight += widthChangeValue;
        }
        rectTop = rectTop + heightChangeValue * 0.5f;
        rectBottom = rectBottom - heightChangeValue * 0.5f;
        rectHeight -= heightChangeValue;
    } else {
        boundsRectF_.SetWidth(rectRight - rectLeft);
        boundsRectF_.SetHeight(rectBottom - rectTop + heightChangeValue * TWO_FLOAT);
        if (touchBottomType_ == TouchBottomType::START) {
            rectTop -= heightChangeValue;
        }
        if (touchBottomType_ == TouchBottomType::END) {
            rectBottom += heightChangeValue;
        }
        rectLeft = rectLeft + widthChangeValue * 0.5f;
        rectRight = rectRight - widthChangeValue * 0.5f;
        rectWidth -= widthChangeValue;
    }
    boundsRectF_.SetLeft(rectLeft);
    boundsRectF_.SetTop(rectTop);
    return { rectLeft, rectRight, rectTop, rectBottom };
}

std::pair<float, float> DotIndicatorModifier::GetTouchBottomCenterX(ContentProperty& contentProperty)
{
    float leftCenterX = contentProperty.longPointLeftCenterX;
    float rightCenterX = contentProperty.longPointRightCenterX;

    if (contentProperty.vectorBlackPointCenterX.empty()) {
        return { leftCenterX, rightCenterX };
    }
    auto totalCount = contentProperty.vectorBlackPointCenterX.size();
    // 2.0 means get the long point radius
    float radius = (rightCenterX - leftCenterX) / 2.0f;
    bool isLeftTouchBottom = (currentIndex_ == static_cast<int32_t>(totalCount) - 1);
    bool isRightTouchBottom = (currentIndex_ == 0);

    if ((animationState_ == TouchBottomAnimationStage::STAGE_SHRINKT_TO_BLACK_POINT && isLeftTouchBottom) ||
        (animationState_ == TouchBottomAnimationStage::STAGE_EXPAND_TO_LONG_POINT && isRightTouchBottom)) {
        leftCenterX = contentProperty.vectorBlackPointCenterX[0] - radius;
        rightCenterX = contentProperty.vectorBlackPointCenterX[0] + radius;
    } else if ((animationState_ == TouchBottomAnimationStage::STAGE_EXPAND_TO_LONG_POINT && isLeftTouchBottom) ||
        (animationState_ == TouchBottomAnimationStage::STAGE_SHRINKT_TO_BLACK_POINT && isRightTouchBottom)) {
        leftCenterX = contentProperty.vectorBlackPointCenterX[totalCount - 1] - radius;
        rightCenterX = contentProperty.vectorBlackPointCenterX[totalCount - 1] + radius;
    }

    return { leftCenterX, rightCenterX };
}

void DotIndicatorModifier::PaintContent(DrawingContext& context, ContentProperty& contentProperty)
{
    RSCanvas& canvas = context.canvas;
    OffsetF selectedCenter = {};
    auto totalCount = contentProperty.vectorBlackPointCenterX.size();
    if (totalCount == 0 && NearZero(contentProperty.longPointLeftCenterX) &&
        NearZero(contentProperty.longPointRightCenterX)) {
        return;
    }

    for (size_t i = 0; i < totalCount; ++i) {
        LinearVector<float> itemHalfSizes = GetItemHalfSizes(i, contentProperty);
        OffsetF center = { contentProperty.vectorBlackPointCenterX[i], centerY_ };
        if (static_cast<int32_t>(i) != currentIndex_) {
            PaintUnselectedIndicator(canvas, center, itemHalfSizes, false, LinearColor(originalUnselectColor_));
        } else {
            selectedCenter = center;
            PaintUnselectedIndicator(canvas, center, itemHalfSizes, isCustomSize_,
                LinearColor(originalUnselectColor_));
        }
    }

    auto [leftCenterX, rightCenterX] = GetTouchBottomCenterX(contentProperty);

    OffsetF leftCenter = { leftCenterX, centerY_ };
    OffsetF rightCenter = { rightCenterX, centerY_ };
    OffsetF centerDistance = rightCenter - leftCenter;
    OffsetF centerDilateDistance = centerDistance * contentProperty.longPointDilateRatio;
    leftCenter -= (centerDilateDistance - centerDistance) * 0.5;
    rightCenter += (centerDilateDistance - centerDistance) * 0.5;
    PaintSelectedIndicator(canvas, leftCenter, rightCenter,
        contentProperty.itemHalfSizes * contentProperty.longPointDilateRatio);

    bool isLeftTouchBottom = (currentIndex_ == static_cast<int32_t>(totalCount) - 1);
    bool isRightTouchBottom = (currentIndex_ == 0);
    bool isTouchBottom = (isLeftTouchBottom || isRightTouchBottom);
    if (!isTouchBottom || totalCount == 0 || !isTouchBottomLoop_) {
        return;
    }

    size_t index = 0;
    if (isRightTouchBottom) {
        index = totalCount - 1;
    }
    LinearVector<float> itemHalfSizes = GetItemHalfSizes(index, contentProperty);
    OffsetF center = { contentProperty.vectorBlackPointCenterX[index], centerY_ };
    PaintUnselectedIndicator(canvas, center, itemHalfSizes, false, touchBottomPointColor_->Get());
}

LinearVector<float> DotIndicatorModifier::GetItemHalfSizes(size_t index, ContentProperty& contentProperty)
{
    if (normalToHoverIndex_.has_value() && normalToHoverIndex_ == index) {
        return contentProperty.itemHalfSizes * contentProperty.normalToHoverPointDilateRatio;
    }
    if (hoverToNormalIndex_.has_value() && hoverToNormalIndex_ == index) {
        return contentProperty.itemHalfSizes * contentProperty.hoverToNormalPointDilateRatio;
    }
    return contentProperty.itemHalfSizes;
}

void DotIndicatorModifier::PaintUnselectedIndicator(RSCanvas& canvas, const OffsetF& center,
    const LinearVector<float>& itemHalfSizes, bool currentIndexFlag, const LinearColor& indicatorColor)
{
    RSBrush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ToRSColor(indicatorColor));
    canvas.AttachBrush(brush);
    if (!NearEqual(itemHalfSizes[ITEM_HALF_WIDTH], itemHalfSizes[ITEM_HALF_HEIGHT]) || currentIndexFlag ||
        !isCustomSize_) {
        float rectItemWidth = itemHalfSizes[ITEM_HALF_WIDTH] * 2;
        float rectItemHeight = itemHalfSizes[ITEM_HALF_HEIGHT] * 2;
        float rectLeft =
            (axis_ == Axis::HORIZONTAL ? center.GetX() - rectItemWidth * 0.5 : center.GetY() - rectItemHeight * 0.5);
        float rectTop =
            (axis_ == Axis::HORIZONTAL ? center.GetY() - rectItemHeight * 0.5 : center.GetX() - rectItemWidth * 0.5);
        float rectRight =
            (axis_ == Axis::HORIZONTAL ? center.GetX() + rectItemWidth * 0.5 : center.GetY() + rectItemHeight * 0.5);
        float rectBottom =
            (axis_ == Axis::HORIZONTAL ? center.GetY() + rectItemHeight * 0.5 : center.GetX() + rectItemWidth * 0.5);

        if (rectItemHeight > rectItemWidth || !isCustomSize_) {
            canvas.DrawRoundRect({ { rectLeft, rectTop, rectRight, rectBottom }, rectItemWidth, rectItemWidth });
        } else if (rectItemHeight < rectItemWidth) {
            canvas.DrawRoundRect({ { rectLeft, rectTop, rectRight, rectBottom }, rectItemHeight, rectItemHeight });
        } else {
            float customPointX = axis_ == Axis::HORIZONTAL ? center.GetX() : center.GetY();
            float customPointY = axis_ == Axis::HORIZONTAL ? center.GetY() : center.GetX();
            canvas.DrawCircle({ customPointX, customPointY }, rectItemHeight * 0.5);
        }
    } else {
        float pointX = axis_ == Axis::HORIZONTAL ? center.GetX() : center.GetY();
        float pointY = axis_ == Axis::HORIZONTAL ? center.GetY() : center.GetX();
        canvas.DrawCircle({ pointX, pointY }, itemHalfSizes[ITEM_HALF_HEIGHT]);
    }
    canvas.DetachBrush();
}

void DotIndicatorModifier::PaintSelectedIndicator(RSCanvas& canvas, const OffsetF& leftCenter,
    const OffsetF& rightCenter, const LinearVector<float>& itemHalfSizes, bool isOverlong)
{
    RSBrush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ToRSColor(originalSelectColor_));
    canvas.AttachBrush(brush);

    auto selectedItemHalfWidth = itemHalfSizes[SELECTED_ITEM_HALF_WIDTH];
    if (isCustomSize_ && !isOverlong) {
        selectedItemHalfWidth = itemHalfSizes[SELECTED_ITEM_HALF_WIDTH] * HALF_FLOAT;
    }

    float rectLeft = (axis_ == Axis::HORIZONTAL ? leftCenter.GetX() - selectedItemHalfWidth
                                                : leftCenter.GetY() - itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT]);

    float rectTop = (axis_ == Axis::HORIZONTAL ? leftCenter.GetY() - itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT]
                                               : leftCenter.GetX() - selectedItemHalfWidth);
    float rectRight = (axis_ == Axis::HORIZONTAL ? rightCenter.GetX() + selectedItemHalfWidth
                                                 : rightCenter.GetY() + itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT]);

    float rectBottom = (axis_ == Axis::HORIZONTAL ? rightCenter.GetY() + itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT]
                                                  : rightCenter.GetX() + selectedItemHalfWidth);

    float rectSelectedItemWidth = selectedItemHalfWidth * TWO_FLOAT;
    float rectSelectedItemHeight = itemHalfSizes[SELECTED_ITEM_HALF_HEIGHT] * TWO_FLOAT;

    if (rectSelectedItemHeight > rectSelectedItemWidth && !isCustomSize_) {
        canvas.DrawRoundRect(
            { { rectLeft, rectTop, rectRight, rectBottom }, rectSelectedItemWidth, rectSelectedItemWidth });
    } else {
        canvas.DrawRoundRect(
            { { rectLeft, rectTop, rectRight, rectBottom }, rectSelectedItemHeight, rectSelectedItemHeight });
    }
    canvas.DetachBrush();
}

void DotIndicatorModifier::PaintMask(DrawingContext& context)
{
    RSCanvas& canvas = context.canvas;

    RSBrush brush;
    brush.SetAntiAlias(true);
    canvas.Save();

    std::vector<RSColorQuad> colors;
    colors.push_back(0x00000000);
    colors.push_back(0xff000000);
    colors.push_back(0xff000000);

    RSPoint startPt = { offset_.GetX(), offset_.GetY() };
    RSPoint endPt = { offset_.GetX(), offset_.GetY() };
    startPt -= axis_ == Axis::HORIZONTAL ? RSPoint(0, (9.0_vp).ConvertToPx()) : RSPoint((9.0_vp).ConvertToPx(), 0);
    endPt += axis_ == Axis::HORIZONTAL ? RSPoint(0, (15.0_vp).ConvertToPx()) : RSPoint((15.0_vp).ConvertToPx(), 0);

    std::vector<float> pos = { 0.0f, 0.75f, 1.0f };

    brush.SetShaderEffect(RSShaderEffect::CreateLinearGradient(startPt, endPt, colors, pos, RSTileMode::CLAMP));
    canvas.DrawRect({ startPt.GetX(), startPt.GetY(), endPt.GetX(), endPt.GetY() });
}

void DotIndicatorModifier::UpdateShrinkPaintProperty(
    const OffsetF& margin, const LinearVector<float>& normalItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    indicatorMargin_->Set(margin);
    indicatorPadding_->Set(static_cast<float>(paddingSide_.ConvertToPx()));

    if (longPointLeftAnimEnd_ && longPointRightAnimEnd_) {
        vectorBlackPointCenterX_->Set(vectorBlackPointCenterX);
        longPointLeftCenterX_->Set(longPointCenterX.first);
        longPointRightCenterX_->Set(longPointCenterX.second);
    }

    itemHalfSizes_->Set(normalItemHalfSizes);
    normalToHoverPointDilateRatio_->Set(1.0f);
    hoverToNormalPointDilateRatio_->Set(1.0f);
    longPointDilateRatio_->Set(1.0f);
    backgroundWidthDilateRatio_->Set(1.0f);
    backgroundHeightDilateRatio_->Set(1.0f);
}

void DotIndicatorModifier::UpdateDilatePaintProperty(
    const LinearVector<float>& hoverItemHalfSizes, const LinearVector<float>& vectorBlackPointCenterX,
    const std::pair<float, float>& longPointCenterX)
{
    indicatorMargin_->Set({ 0, 0 });
    indicatorPadding_->Set(static_cast<float>(paddingSide_.ConvertToPx()));

    vectorBlackPointCenterX_->Set(vectorBlackPointCenterX);
    if (longPointLeftAnimEnd_ && longPointRightAnimEnd_) {
        longPointLeftCenterX_->Set(longPointCenterX.first);
        longPointRightCenterX_->Set(longPointCenterX.second);
    }
    itemHalfSizes_->Set(hoverItemHalfSizes);
    backgroundWidthDilateRatio_->Set(1.0f);
    backgroundHeightDilateRatio_->Set(1.0f);
}

void DotIndicatorModifier::UpdateBackgroundColor(const Color& backgroundColor)
{
    backgroundColor_->Set(LinearColor(backgroundColor));
}

void DotIndicatorModifier::UpdateNormalPaintProperty(
    const OffsetF& margin, const LinearVector<float>& normalItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    auto swiperTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(swiperTheme);
    auto backgroundColor = indicatorMask_ ?
        swiperTheme->GetPressedColor() :
        swiperTheme->GetHoverColor().ChangeOpacity(0);
    UpdateShrinkPaintProperty(margin, normalItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    UpdateBackgroundColor(backgroundColor);
}

void DotIndicatorModifier::UpdateHoverPaintProperty(
    const LinearVector<float>& hoverItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    auto swiperTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(swiperTheme);
    auto backgroundColor = swiperTheme->GetHoverColor();
    UpdateDilatePaintProperty(hoverItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    UpdateBackgroundColor(backgroundColor);
}

void DotIndicatorModifier::UpdatePressPaintProperty(
    const LinearVector<float>& hoverItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    auto swiperTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(swiperTheme);
    auto backgroundColor = swiperTheme->GetPressedColor();
    UpdateDilatePaintProperty(hoverItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    UpdateBackgroundColor(backgroundColor);
}

void DotIndicatorModifier::UpdateNormalToHoverPaintProperty(
    const LinearVector<float>& hoverItemHalfSizes, const LinearVector<float>& vectorBlackPointCenterX,
    const std::pair<float, float>& longPointCenterX)
{
    AnimationOption option;
    option.SetDuration(COMPONENT_DILATE_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    longPointLeftAnimEnd_ = true;
    longPointRightAnimEnd_ = true;
    AnimationUtils::Animate(option, [weak = WeakClaim(this), hoverItemHalfSizes, vectorBlackPointCenterX,
        longPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->UpdateHoverPaintProperty(hoverItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    });
}

void DotIndicatorModifier::UpdateHoverToNormalPaintProperty(
    const OffsetF& margin, const LinearVector<float>& normalItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    AnimationOption option;
    option.SetDuration(COMPONENT_SHRINK_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    longPointLeftAnimEnd_ = true;
    longPointRightAnimEnd_ = true;
    AnimationUtils::Animate(option, [weak = WeakClaim(this), margin, normalItemHalfSizes, vectorBlackPointCenterX,
        longPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->UpdateNormalPaintProperty(margin, normalItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    });
}

void DotIndicatorModifier::UpdateNormalToPressPaintProperty(
    const LinearVector<float>& hoverItemHalfSizes, const LinearVector<float>& vectorBlackPointCenterX,
    const std::pair<float, float>& longPointCenterX)
{
    AnimationOption option;
    option.SetDuration(COMPONENT_DILATE_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    longPointLeftAnimEnd_ = true;
    longPointRightAnimEnd_ = true;
    AnimationUtils::Animate(option, [weak = WeakClaim(this), hoverItemHalfSizes, vectorBlackPointCenterX,
        longPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->UpdatePressPaintProperty(hoverItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    });
}

void DotIndicatorModifier::UpdatePressToNormalPaintProperty(
    const OffsetF& margin, const LinearVector<float>& normalItemHalfSizes,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    AnimationOption option;
    option.SetDuration(COMPONENT_SHRINK_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    AnimationUtils::Animate(option, [weak = WeakClaim(this), margin, normalItemHalfSizes, vectorBlackPointCenterX,
        longPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->UpdateNormalPaintProperty(margin, normalItemHalfSizes, vectorBlackPointCenterX, longPointCenterX);
    });
}

void DotIndicatorModifier::UpdateHoverAndPressConversionPaintProperty()
{
    auto swiperTheme = GetSwiperIndicatorTheme();
    CHECK_NULL_VOID(swiperTheme);
    Color backgroundColor = isPressed_ ? swiperTheme->GetPressedColor() : swiperTheme->GetHoverColor();
    AnimationOption option;
    option.SetDuration(MOUSE_PRESS_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    AnimationUtils::Animate(option, [weak = WeakClaim(this), backgroundColor]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->UpdateBackgroundColor(backgroundColor);
    });
}

void DotIndicatorModifier::UpdateNormalToHoverPointDilateRatio()
{
    normalToHoverPointDilateRatio_->Set(1.0f);
    AnimationOption option;
    option.SetDuration(POINT_HOVER_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    AnimationUtils::Animate(option, [weak = WeakClaim(this)]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->normalToHoverPointDilateRatio_->Set(modifier->scaleIndicator_);
    });
}

void DotIndicatorModifier::UpdateHoverToNormalPointDilateRatio()
{
    hoverToNormalPointDilateRatio_->Set(normalToHoverPointDilateRatio_->Get());
    AnimationOption option;
    option.SetDuration(POINT_HOVER_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    AnimationUtils::Animate(option, [weak = WeakClaim(this)]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->hoverToNormalPointDilateRatio_->Set(1.0f);
    });
}

void DotIndicatorModifier::UpdateLongPointDilateRatio()
{
    AnimationOption option;
    option.SetDuration(POINT_HOVER_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    if (longPointIsHover_) {
        AnimationUtils::Animate(option, [weak = WeakClaim(this)]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->longPointDilateRatio_->Set(modifier->scaleIndicator_);
        });
    } else {
        AnimationUtils::Animate(option, [weak = WeakClaim(this)]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->longPointDilateRatio_->Set(1.0f);
        });
    }
}

void DotIndicatorModifier::UpdateAllPointCenterXAnimation(GestureState gestureState,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX)
{
    AnimationOption blackPointOption;
    blackPointOption.SetDuration(BLACK_POINT_DURATION);
    blackPointOption.SetCurve(AceType::MakeRefPtr<CubicCurve>(BLACK_POINT_CENTER_BEZIER_CURVE_VELOCITY,
        CENTER_BEZIER_CURVE_MASS, CENTER_BEZIER_CURVE_STIFFNESS, CENTER_BEZIER_CURVE_DAMPING));
    AnimationUtils::Animate(blackPointOption, [weak = WeakClaim(this), vectorBlackPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->vectorBlackPointCenterX_->Set(vectorBlackPointCenterX);
    });

    // normal page turning
    AnimationOption optionHead;
    RefPtr<Curve> curve = headCurve_;
    optionHead.SetCurve(curve);
    optionHead.SetDuration(animationDuration_);

    AnimationOption optionTail;
    optionTail.SetCurve(GetTailCurve());
    optionTail.SetDuration(animationDuration_);
    AnimationOption optionLeft = optionTail;
    AnimationOption optionRight = optionHead;

    if (gestureState == GestureState::GESTURE_STATE_RELEASE_LEFT) {
        optionLeft = optionHead;
        optionRight = optionTail;
    }

    if (longPointLeftAnimEnd_ && longPointRightAnimEnd_) {
        longPointLeftAnimEnd_ = false;
        longPointRightAnimEnd_ = false;
        auto weak = WeakClaim(this);
        AnimationUtils::StartAnimation(optionLeft, [weak, longPointCenterX]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointLeftCenterX_->Set(longPointCenterX.first);
            }, [weak]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointLeftAnimEnd_ = true;
            });

        AnimationUtils::StartAnimation(optionRight, [weak, longPointCenterX]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointRightCenterX_->Set(longPointCenterX.second);
            }, [weak]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointRightAnimEnd_ = true;
            });
    }
}

void DotIndicatorModifier::UpdateTouchBottomAnimation(TouchBottomType touchBottomType,
    const LinearVector<float>& vectorBlackPointCenterX, const std::pair<float, float>& longPointCenterX,
    float touchBottomRate)
{
    AnimationOption option;
    option.SetDuration(POINT_HOVER_ANIMATION_DURATION);
    option.SetCurve(AceType::MakeRefPtr<CubicCurve>(TOUCH_BOTTOM_CURVE_VELOCITY, TOUCH_BOTTOM_CURVE_MASS,
        TOUCH_BOTTOM_CURVE_STIFFNESS, TOUCH_BOTTOM_CURVE_DAMPING));

    auto backgroundWidthDilateRatio = 1.0f;
    auto backgroundHeightDilateRatio = 1.0f;

    if (touchBottomType != TouchBottomType::NONE) {
        backgroundWidthDilateRatio = TOUCH_BOTTOM_BACKGROUND_WIDTH_MULTIPLE -
                                     TOUCH_BOTTOM_DOT_WIDTH_MULTIPLE * vectorBlackPointCenterX_->Get().size();
        backgroundHeightDilateRatio = TOUCH_BOTTOM_BACKGROUND_HEIGHT_MULTIPLE;
        backgroundWidthDilateRatio = (backgroundWidthDilateRatio - 1.0f) * touchBottomRate + 1.0f;
        backgroundHeightDilateRatio = (backgroundHeightDilateRatio - 1.0f) * touchBottomRate + 1.0f;
    }
    touchBottomType_ = touchBottomType;
    AnimationUtils::Animate(option, [weak = WeakClaim(this), backgroundWidthDilateRatio, backgroundHeightDilateRatio,
                                        vectorBlackPointCenterX, longPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->backgroundWidthDilateRatio_->Set(backgroundWidthDilateRatio);
        modifier->backgroundHeightDilateRatio_->Set(backgroundHeightDilateRatio);
        modifier->vectorBlackPointCenterX_->Set(vectorBlackPointCenterX);
        if (modifier->longPointLeftAnimEnd_) {
            modifier->longPointLeftCenterX_->Set(longPointCenterX.first);
        }
        if (modifier->longPointRightAnimEnd_) {
            modifier->longPointRightCenterX_->Set(longPointCenterX.second);
        }
    });
}

void DotIndicatorModifier::PlayBlackPointsAnimation(const LinearVector<float>& vectorBlackPointCenterX)
{
    auto curve = AceType::MakeRefPtr<CubicCurve>(BLACK_POINT_CENTER_BEZIER_CURVE_VELOCITY, CENTER_BEZIER_CURVE_MASS,
        CENTER_BEZIER_CURVE_STIFFNESS, CENTER_BEZIER_CURVE_DAMPING);
    AnimationOption option;
    option.SetCurve(curve);
    option.SetDuration(animationDuration_);
    AnimationUtils::StartAnimation(option, [weak = WeakClaim(this), vectorBlackPointCenterX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->vectorBlackPointCenterX_->Set(vectorBlackPointCenterX);
    });
}

void DotIndicatorModifier::PlayOpacityAnimation()
{
    AnimationOption optionOpacity;
    // x0:0.33, y0:0, x1:0.67, y1:1
    optionOpacity.SetCurve(AceType::MakeRefPtr<CubicCurve>(0.33, 0, 0.67, 1));
    optionOpacity.SetDuration(GetLoopOpacityDuration());
    isSelectedColorAnimEnd_ = false;
    isTouchBottomLoop_ = true;
    selectedColor_->Set(LinearColor(selectedColor_->Get().BlendOpacity(0.0f)));
    auto weak = WeakClaim(this);
    AnimationUtils::StartAnimation(optionOpacity, [weak]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            auto color = modifier->selectedColor_->Get();
            auto targetColor =
                LinearColor(Color::FromARGB(TARGET_ALPHA, color.GetRed(), color.GetGreen(), color.GetBlue()));
            modifier->selectedColor_->Set(targetColor);
            modifier->touchBottomPointColor_->Set(LinearColor(modifier->touchBottomPointColor_->Get().BlendOpacity(0)));
        }, [weak]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->touchBottomPointColor_->Set(LinearColor(modifier->unselectedColor_->Get()));
            modifier->isTouchBottomLoop_ = false;
            modifier->isSelectedColorAnimEnd_ = true;
        });
}

int32_t DotIndicatorModifier::GetLoopTranslateDuration() const
{
    if (InstanceOf<InterpolatingSpring>(headCurve_)) {
        return DEFAULT_TOUCH_BOTTOM_ANIMATION_DURATION;
    }

    return static_cast<int32_t>(static_cast<float>(animationDuration_) * LOOP_TRANSLATE_DURATION_PERCENT);
}

int32_t DotIndicatorModifier::GetLoopOpacityDuration() const
{
    if (InstanceOf<InterpolatingSpring>(headCurve_)) {
        return DEFAULT_OPACITY_ANIMATION_DURATION;
    }

    return static_cast<int32_t>(static_cast<float>(animationDuration_) * LOOP_OPACITY_DURATION_PERCENT);
}

void DotIndicatorModifier::PlayTouchBottomAnimation(const std::vector<std::pair<float, float>>& longPointCenterX,
    TouchBottomTypeLoop touchBottomTypeLoop, const LinearVector<float>& vectorBlackPointCenterX)
{
    if (vectorBlackPointCenterX.empty()) {
        return;
    }

    AnimationOption optionBottom;
    // x0:0.33, y0:0, x1:0.67, y1:1
    optionBottom.SetCurve(AceType::MakeRefPtr<CubicCurve>(0.33, 0, 0.67, 1));
    optionBottom.SetDuration(GetLoopTranslateDuration());

    auto weak = WeakClaim(this);
    FinishCallback bottomFinishCallback = [weak, optionBottom, longPointCenterX, vectorBlackPointCenterX,
                                              touchBottomTypeLoop]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->animationState_ = TouchBottomAnimationStage::STAGE_NONE;
        if (!(modifier->ifNeedFinishCallback_)) {
            return;
        }
        modifier->animationState_ = TouchBottomAnimationStage::STAGE_EXPAND_TO_LONG_POINT;
        modifier->PlayOpacityAnimation();
        if (touchBottomTypeLoop == TouchBottomTypeLoop::TOUCH_BOTTOM_TYPE_LOOP_LEFT) {
            modifier->longPointLeftCenterX_->Set(vectorBlackPointCenterX[vectorBlackPointCenterX.size() - 1]);
            modifier->longPointRightCenterX_->Set(vectorBlackPointCenterX[vectorBlackPointCenterX.size() - 1]);
        } else if (touchBottomTypeLoop == TouchBottomTypeLoop::TOUCH_BOTTOM_TYPE_LOOP_RIGHT) {
            modifier->longPointLeftCenterX_->Set(vectorBlackPointCenterX[0]);
            modifier->longPointRightCenterX_->Set(vectorBlackPointCenterX[0]);
        }

        AnimationUtils::StartAnimation(optionBottom, [weak, longPointCenterX]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->longPointRightCenterX_->Set(longPointCenterX[1].second);
            if (modifier->isCustomSize_) {
                modifier->longPointLeftCenterX_->Set(longPointCenterX[1].second);
            } else {
                modifier->longPointLeftCenterX_->Set(longPointCenterX[1].first);
            }
        }, [weak]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->longPointLeftAnimEnd_ = true;
            modifier->longPointRightAnimEnd_ = true;
            modifier->isBottomAnimationFinished_ = true;
            modifier->animationState_ = TouchBottomAnimationStage::STAGE_NONE;
        });
    };
    if (longPointLeftAnimEnd_ && longPointRightAnimEnd_) {
        longPointLeftAnimEnd_ = false;
        longPointRightAnimEnd_ = false;
        ifNeedFinishCallback_ = true;
        isBottomAnimationFinished_ = false;
        animationState_ = TouchBottomAnimationStage::STAGE_SHRINKT_TO_BLACK_POINT;
        touchBottomPointColor_->Set(LinearColor(selectedColor_->Get()));
        AnimationUtils::StartAnimation(optionBottom, [weak, longPointCenterX]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->longPointLeftCenterX_->Set(longPointCenterX[0].first);
            modifier->longPointRightCenterX_->Set(longPointCenterX[0].second);
            modifier->bottomCenterX_ = longPointCenterX[1];
        }, bottomFinishCallback);
    }
}

float DotIndicatorModifier::CalculateMinimumAmplitudeRatio(
    const std::vector<std::pair<float, float>>& longPointCenterX, GestureState gestureState) const
{
    auto minimumAmplitudeRatio =
        NearEqual(longPointCenterX[0].first, longPointLeftCenterX_->Get())
            ? InterpolatingSpring::DEFAULT_INTERPOLATING_SPRING_AMPLITUDE_RATIO
            : DEFAULT_MINIMUM_AMPLITUDE_PX / std::abs(longPointCenterX[0].first - longPointLeftCenterX_->Get());
    if (gestureState == GestureState::GESTURE_STATE_RELEASE_LEFT) {
        minimumAmplitudeRatio =
            NearEqual(longPointCenterX[0].second, longPointRightCenterX_->Get())
                ? InterpolatingSpring::DEFAULT_INTERPOLATING_SPRING_AMPLITUDE_RATIO
                : DEFAULT_MINIMUM_AMPLITUDE_PX / std::abs(longPointCenterX[0].second - longPointRightCenterX_->Get());
    }
    return std::max(minimumAmplitudeRatio, InterpolatingSpring::DEFAULT_INTERPOLATING_SPRING_AMPLITUDE_RATIO);
}

RefPtr<InterpolatingSpring> DotIndicatorModifier::GetTailCurve()
{
    auto step = std::abs(currentIndex_ - currentIndexActual_);
    if (step == LONG_POINT_STEP_TWO) {
        return LONG_POINT_STEP_TWO_CURVE;
    }

    if (step == LONG_POINT_STEP_THREE) {
        return LONG_POINT_STEP_THREE_CURVE;
    }

    if (step == LONG_POINT_STEP_FOUR) {
        return LONG_POINT_STEP_FOUR_CURVE;
    }

    if (step > LONG_POINT_STEP_FOUR) {
        return LONG_POINT_STEP_FIVE_CURVE;
    }

    return LONG_POINT_DEFAULT_CURVE;
}

AnimationOption DotIndicatorModifier::CreateTailOption(
    const std::vector<std::pair<float, float>>& longPointCenterX, GestureState gestureState, bool isNormal)
{
    AnimationOption optionTail;
    optionTail.SetDuration(animationDuration_);

    if (userSetSwiperCurve_) {
        optionTail.SetCurve(headCurve_);
        return optionTail;
    }

    auto interpolatingSpring = GetTailCurve();
    if (isNormal) {
        interpolatingSpring->UpdateMinimumAmplitudeRatio(
            CalculateMinimumAmplitudeRatio(longPointCenterX, gestureState));
    }
    optionTail.SetCurve(interpolatingSpring);
    return optionTail;
}

void DotIndicatorModifier::PlayLongPointAnimation(const std::vector<std::pair<float, float>>& longPointCenterX,
    GestureState gestureState, TouchBottomTypeLoop touchBottomTypeLoop,
    const LinearVector<float>& vectorBlackPointCenterX, bool isNormal)
{
    if (longPointCenterX.empty()) {
        return;
    }
    // touch bottom
    if (longPointCenterX.size() > 1) {
        PlayTouchBottomAnimation(longPointCenterX, touchBottomTypeLoop, vectorBlackPointCenterX);
        return;
    }
    // normal page turning
    AnimationOption optionHead;
    RefPtr<Curve> curve = headCurve_;
    optionHead.SetCurve(curve);
    optionHead.SetDuration(animationDuration_);

    AnimationOption optionTail = CreateTailOption(longPointCenterX, gestureState, isNormal);
    AnimationOption optionLeft = optionTail;
    AnimationOption optionRight = optionHead;

    if (gestureState == GestureState::GESTURE_STATE_RELEASE_LEFT) {
        optionLeft = optionHead;
        optionRight = optionTail;
    }

    if (longPointLeftAnimEnd_ && longPointRightAnimEnd_) {
        longPointLeftAnimEnd_ = false;
        longPointRightAnimEnd_ = false;
        auto weak = WeakClaim(this);
        AnimationUtils::StartAnimation(optionLeft, [weak, longPointCenterX]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointLeftCenterX_->Set(longPointCenterX[0].first);
            }, [weak]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointLeftAnimEnd_ = true;
            });

        AnimationUtils::StartAnimation(optionRight, [weak, longPointCenterX]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointRightCenterX_->Set(longPointCenterX[0].second);
            }, [weak]() {
                auto modifier = weak.Upgrade();
                CHECK_NULL_VOID(modifier);
                modifier->longPointRightAnimEnd_ = true;
            });
    }
}

void DotIndicatorModifier::PlayIndicatorAnimation(const LinearVector<float>& vectorBlackPointCenterX,
    const std::vector<std::pair<float, float>>& longPointCenterX, GestureState gestureState,
    TouchBottomTypeLoop touchBottomTypeLoop)
{
    StopAnimation();
    isTouchBottomLoop_ = false;
    animationState_ = TouchBottomAnimationStage::STAGE_NONE;
    PlayBlackPointsAnimation(vectorBlackPointCenterX);
    PlayLongPointAnimation(longPointCenterX, gestureState, touchBottomTypeLoop, vectorBlackPointCenterX);
}

void DotIndicatorModifier::FinishAnimationToTargetImmediately(std::pair<float, float> centerX)
{
    AnimationOption option;
    option.SetDuration(0);
    option.SetCurve(Curves::LINEAR);
    AnimationUtils::StartAnimation(option, [weak = WeakClaim(this), centerX]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->isBottomAnimationFinished_ = true;
        modifier->ifNeedFinishCallback_ = false;
        modifier->longPointRightCenterX_->Set(centerX.second);
        if (modifier->isCustomSize_) {
            modifier->longPointLeftCenterX_->Set(centerX.second);
        } else {
            modifier->longPointLeftCenterX_->Set(centerX.first);
        }
    });
}

void DotIndicatorModifier::StopAnimation(bool ifImmediately)
{
    if (ifImmediately) {
        AnimationOption option;
        option.SetDuration(0);
        option.SetCurve(Curves::LINEAR);
        AnimationUtils::StartAnimation(option, [weak = WeakClaim(this)]() {
            auto modifier = weak.Upgrade();
            CHECK_NULL_VOID(modifier);
            modifier->ifNeedFinishCallback_ = false;
            modifier->animationState_ = TouchBottomAnimationStage::STAGE_NONE;
            modifier->longPointLeftCenterX_->Set(modifier->longPointLeftCenterX_->Get());
            modifier->longPointRightCenterX_->Set(modifier->longPointRightCenterX_->Get());
        });
    }
    AnimationOption option;
    option.SetDuration(0);
    option.SetCurve(Curves::LINEAR);
    AnimationUtils::StartAnimation(option, [weak = WeakClaim(this)]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        modifier->vectorBlackPointCenterX_->Set(modifier->vectorBlackPointCenterX_->Get());
    });
    longPointLeftAnimEnd_ = true;
    longPointRightAnimEnd_ = true;
    ifNeedFinishCallback_ = false;
}
} // namespace OHOS::Ace::NG
