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
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_paint_method.h"

#include "core/components/select/select_theme.h"
#include "core/components_ng/pattern/menu/menu_paint_property.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/node_paint_method.h"
#include "core/components_ng/render/paint_wrapper.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t RADIUS_MIDPOINT = 2;
constexpr float BORDER_MULTIPLES = 2.0f;
constexpr Dimension ARROW_RADIUS = 2.0_vp;
constexpr Dimension ARROW_P1_OFFSET_X = 8.0_vp;
constexpr Dimension ARROW_P2_OFFSET_X = 1.5_vp;
constexpr Dimension ARROW_P1_OFFSET_Y = 8.0_vp;
constexpr Dimension ARROW_P2_OFFSET_Y = 0.68_vp;

RefPtr<MenuTheme> GetMenuThemeFromPaintWrapper(PaintWrapper* paintWrapper)
{
    CHECK_NULL_RETURN(paintWrapper, nullptr);
    auto renderContext = paintWrapper->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, nullptr);
    auto host = renderContext->GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto pipelineContext = host->GetContextRefPtr();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    return pipelineContext->GetTheme<MenuTheme>();
}

RefPtr<MenuWrapperPattern> GetMenuWrapperPatternFromPaintWrapper(PaintWrapper* paintWrapper)
{
    CHECK_NULL_RETURN(paintWrapper, nullptr);
    auto renderContext = paintWrapper->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, nullptr);
    auto host = renderContext->GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    return host->GetPattern<MenuWrapperPattern>();
}
}

CanvasDrawFunction MenuWrapperPaintMethod::GetOverlayDrawFunction(PaintWrapper* paintWrapper)
{
    return [weak = WeakClaim(this), paintWrapper](RSCanvas& canvas) {
        if (!Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
            return;
        }
        auto menuTheme = GetMenuThemeFromPaintWrapper(paintWrapper);
        CHECK_NULL_VOID(menuTheme);
        auto wrapperPattern = GetMenuWrapperPatternFromPaintWrapper(paintWrapper);
        CHECK_NULL_VOID(wrapperPattern);
        if (!menuTheme->GetDoubleBorderEnable() && !wrapperPattern->GetHasCustomOutlineWidth()) {
            return;
        }
        auto paintMethod = weak.Upgrade();
        if (paintMethod) {
            paintMethod->PaintDoubleBorder(canvas, paintWrapper);
        }
    };
}

void MenuWrapperPaintMethod::PaintDoubleBorder(RSCanvas& canvas, PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(paintWrapper);
    auto renderContext = paintWrapper->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto host = renderContext->GetHost();
    CHECK_NULL_VOID(host);
    auto wrapperPattern = host->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(wrapperPattern);
    if (wrapperPattern->GetMenuStatus() != MenuStatus::SHOW) {
        return;
    }
    auto children = host->GetChildren();
    if (children.empty()) {
        return;
    }
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        if (!child || child->GetTag() != V2::MENU_ETS_TAG) {
            continue;
        }
        auto frameNode = AceType::DynamicCast<FrameNode>(child);
        CHECK_NULL_VOID(frameNode);
        auto renderContext = frameNode->GetRenderContext();
        CHECK_NULL_VOID(renderContext);
        auto opacity = renderContext->GetOpacity().value_or(1.0);
        if (!NearEqual(opacity, 1.0)) {
            continue;
        }
        auto pattern = frameNode->GetPattern<MenuPattern>();
        CHECK_NULL_VOID(pattern);
        auto params = pattern->GetMenuPathParams();
        if (params.has_value()) {
            PaintInnerBorderAndClipSinglePath(canvas, paintWrapper, params.value());
            PaintOuterBorderAndClipSinglePath(canvas, paintWrapper, params.value());
        }
    }
}

void MenuWrapperPaintMethod::PaintInnerBorderAndClipSinglePath(
    RSCanvas& canvas, PaintWrapper* paintWrapper, const MenuPathParams& params)
{
    RSPath rsPath;
    BuildCompletePath(rsPath, params);
    canvas.Save();
    if (!params.didNeedArrow) {
        canvas.ClipPath(rsPath, RSClipOp::DIFFERENCE, true);
        return;
    }
    RSPen paint;
    paint.SetAntiAlias(true);
    auto menuTheme = GetMenuThemeFromPaintWrapper(paintWrapper);
    CHECK_NULL_VOID(menuTheme);
    // Because the canvas is cut by the ContentDrawFunction method, only half of the width can be drawn
    paint.SetWidth(menuTheme->GetInnerBorderWidth() * BORDER_MULTIPLES);
    paint.SetColor(menuTheme->GetInnerBorderColor().GetValue());
    canvas.AttachPen(paint);
    canvas.ClipPath(rsPath, RSClipOp::INTERSECT, true);
    canvas.DrawPath(rsPath);
    canvas.DetachPen();
    canvas.Restore();
}

void CanvasDrawOutline(RSCanvas& canvas, const RSPen& paint, const RSPath& rsPathEdge)
{
    canvas.Save();
    canvas.AttachPen(paint);
    canvas.DrawPath(rsPathEdge);
    canvas.DetachPen();
}

void MenuWrapperPaintMethod::PaintEdgeOuterBorder(
    const MenuPathParams& params, RSCanvas& canvas, const MenuParam& menuParam, const RSPath& rsPath)
{
    RSPath rsPathTop;
    RSPath rsPathRight;
    RSPath rsPathBottom;
    RSPath rsPathLeft;
    RSPen paint;
    paint.SetAntiAlias(true);

    rsPathTop.MoveTo(
        params.childOffset.GetX() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT));
    BuildTopLinePath(rsPathTop, params);
    paint.SetWidth(menuParam.outlineWidth->topDimen->ConvertToPx() * BORDER_MULTIPLES);
    paint.SetColor(menuParam.outlineColor->topColor->GetValue());
    CanvasDrawOutline(canvas, paint, rsPathTop);

    rsPathRight.MoveTo(params.childOffset.GetX() + params.frameSize.Width() - params.radiusTopRightPx +
                           params.radiusTopRightPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.radiusTopRightPx - params.radiusTopRightPx / sqrt(RADIUS_MIDPOINT));
    BuildRightLinePath(rsPathRight, params);
    paint.SetWidth(menuParam.outlineWidth->rightDimen->ConvertToPx() * BORDER_MULTIPLES);
    paint.SetColor(menuParam.outlineColor->rightColor->GetValue());
    CanvasDrawOutline(canvas, paint, rsPathRight);

    rsPathBottom.MoveTo(params.childOffset.GetX() + params.frameSize.Width() - params.radiusBottomRightPx +
                            params.radiusBottomRightPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomRightPx +
            params.radiusBottomRightPx / sqrt(RADIUS_MIDPOINT));
    BuildBottomLinePath(rsPathBottom, params);
    paint.SetWidth(menuParam.outlineWidth->bottomDimen->ConvertToPx() * BORDER_MULTIPLES);
    paint.SetColor(menuParam.outlineColor->bottomColor->GetValue());
    CanvasDrawOutline(canvas, paint, rsPathBottom);

    rsPathLeft.MoveTo(
        params.childOffset.GetX() + params.radiusBottomLeftPx - params.radiusBottomLeftPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomLeftPx +
            params.radiusBottomLeftPx / sqrt(RADIUS_MIDPOINT));
    BuildLeftLinePath(rsPathLeft, params);
    paint.SetWidth(menuParam.outlineWidth->leftDimen->ConvertToPx() * BORDER_MULTIPLES);
    paint.SetColor(menuParam.outlineColor->leftColor->GetValue());
    CanvasDrawOutline(canvas, paint, rsPathLeft);
}

void MenuWrapperPaintMethod::PaintOuterBorderAndClipSinglePath(
    RSCanvas& canvas, PaintWrapper* paintWrapper, const MenuPathParams& params)
{
    CHECK_NULL_VOID(paintWrapper);
    auto wrapperPattern = GetMenuWrapperPatternFromPaintWrapper(paintWrapper);
    CHECK_NULL_VOID(wrapperPattern);
    RSPath rsPath;
    MenuParam menuParam = wrapperPattern->GetMenuParam();
    BuildCompletePath(rsPath, params);
    canvas.Save();
    if (!params.didNeedArrow) {
        canvas.ClipPath(rsPath, RSClipOp::DIFFERENCE, true);
        return;
    }
    RSPen paint;
    paint.SetAntiAlias(true);
    auto menuTheme = GetMenuThemeFromPaintWrapper(paintWrapper);
    CHECK_NULL_VOID(menuTheme);
    if (!wrapperPattern->GetHasCustomOutlineWidth()) {
        paint.SetWidth(menuTheme->GetOuterBorderWidth() * BORDER_MULTIPLES);
        paint.SetColor(menuTheme->GetOuterBorderColor().GetValue());
        CanvasDrawOutline(canvas, paint, rsPath);
    } else {
        canvas.Save();
        canvas.ClipPath(rsPath, RSClipOp::DIFFERENCE, true);
        PaintEdgeOuterBorder(params, canvas, menuParam, rsPath);
    }
}

void MenuWrapperPaintMethod::BuildCompletePath(RSPath& rsPath, const MenuPathParams& params)
{
    rsPath.MoveTo(params.childOffset.GetX() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT));
    BuildTopLinePath(rsPath, params);
    BuildRightLinePath(rsPath, params);
    BuildBottomLinePath(rsPath, params);
    BuildLeftLinePath(rsPath, params);
}

void MenuWrapperPaintMethod::BuildTopLinePath(RSPath& rsPath, const MenuPathParams& params)
{
    rsPath.ArcTo(params.radiusTopLeftPx, params.radiusTopLeftPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.radiusTopLeftPx, params.childOffset.GetY());
    if (params.didNeedArrow) {
        switch (params.arrowPlacement) {
            case Placement::BOTTOM:
            case Placement::BOTTOM_LEFT:
            case Placement::BOTTOM_RIGHT:
                BuildBottomArrowPath(rsPath, params.arrowPosition.GetX(), params.arrowPosition.GetY());
                break;
            default:
                break;
        }
    }
    rsPath.LineTo(
        params.childOffset.GetX() + params.frameSize.Width() - params.radiusTopRightPx, params.childOffset.GetY());
    rsPath.ArcTo(params.radiusTopRightPx, params.radiusTopRightPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.frameSize.Width() - params.radiusTopRightPx +
            params.radiusTopRightPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.radiusTopRightPx - params.radiusTopRightPx / sqrt(RADIUS_MIDPOINT));
}

void MenuWrapperPaintMethod::BuildRightLinePath(RSPath& rsPath, const MenuPathParams& params)
{
    rsPath.ArcTo(params.radiusTopRightPx, params.radiusTopRightPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.frameSize.Width(), params.childOffset.GetY() + params.radiusTopRightPx);
    if (params.didNeedArrow) {
        switch (params.arrowPlacement) {
            case Placement::LEFT:
            case Placement::LEFT_TOP:
            case Placement::LEFT_BOTTOM:
                BuildLeftArrowPath(rsPath, params.arrowPosition.GetX(), params.arrowPosition.GetY());
                break;
            default:
                break;
        }
    }
    rsPath.LineTo(params.childOffset.GetX() + params.frameSize.Width(),
        params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomRightPx);
    rsPath.ArcTo(params.radiusBottomRightPx, params.radiusBottomRightPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.frameSize.Width() - params.radiusBottomRightPx +
            params.radiusBottomRightPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomRightPx +
            params.radiusBottomRightPx / sqrt(RADIUS_MIDPOINT));
}

void MenuWrapperPaintMethod::BuildBottomLinePath(RSPath& rsPath, const MenuPathParams& params)
{
    rsPath.ArcTo(params.radiusBottomRightPx, params.radiusBottomRightPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.frameSize.Width() - params.radiusBottomRightPx,
        params.childOffset.GetY() + params.frameSize.Height());
    if (params.didNeedArrow) {
        switch (params.arrowPlacement) {
            case Placement::TOP:
            case Placement::TOP_LEFT:
            case Placement::TOP_RIGHT:
                BuildTopArrowPath(rsPath, params.arrowPosition.GetX(), params.arrowPosition.GetY());
                break;
            default:
                break;
        }
    }
    rsPath.LineTo(
        params.childOffset.GetX() + params.radiusBottomLeftPx, params.childOffset.GetY() + params.frameSize.Height());
    rsPath.ArcTo(params.radiusBottomLeftPx, params.radiusBottomLeftPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.radiusBottomLeftPx - params.radiusBottomLeftPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomLeftPx +
            params.radiusBottomLeftPx / sqrt(RADIUS_MIDPOINT));
}

void MenuWrapperPaintMethod::BuildLeftLinePath(RSPath& rsPath, const MenuPathParams& params)
{
    rsPath.ArcTo(params.radiusBottomLeftPx, params.radiusBottomLeftPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX(), params.childOffset.GetY() + params.frameSize.Height() - params.radiusBottomLeftPx);
    if (params.didNeedArrow) {
        switch (params.arrowPlacement) {
            case Placement::RIGHT:
            case Placement::RIGHT_TOP:
            case Placement::RIGHT_BOTTOM:
                BuildRightArrowPath(rsPath, params.arrowPosition.GetX(), params.arrowPosition.GetY());
                break;
            default:
                break;
        }
    }
    rsPath.LineTo(params.childOffset.GetX(), params.childOffset.GetY() + params.radiusTopLeftPx);
    rsPath.ArcTo(params.radiusTopLeftPx, params.radiusTopLeftPx, 0.0f, RSPathDirection::CW_DIRECTION,
        params.childOffset.GetX() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT),
        params.childOffset.GetY() + params.radiusTopLeftPx - params.radiusTopLeftPx / sqrt(RADIUS_MIDPOINT));
}

void MenuWrapperPaintMethod::BuildBottomArrowPath(RSPath& rsPath, float arrowX, float arrowY)
{
    rsPath.LineTo(arrowX - ARROW_P1_OFFSET_X.ConvertToPx(), arrowY + ARROW_P1_OFFSET_Y.ConvertToPx()); //P1
    rsPath.LineTo(arrowX - ARROW_P2_OFFSET_X.ConvertToPx(), arrowY + ARROW_P2_OFFSET_Y.ConvertToPx()); //P2
    rsPath.ArcTo(ARROW_RADIUS.ConvertToPx(), ARROW_RADIUS.ConvertToPx(), 0.0f, RSPathDirection::CW_DIRECTION,
        arrowX + ARROW_P2_OFFSET_X.ConvertToPx(), arrowY + ARROW_P2_OFFSET_Y.ConvertToPx()); //P4
    rsPath.LineTo(arrowX + ARROW_P1_OFFSET_X.ConvertToPx(), arrowY + ARROW_P1_OFFSET_Y.ConvertToPx()); //P5
}

void MenuWrapperPaintMethod::BuildTopArrowPath(RSPath& rsPath, float arrowX, float arrowY)
{
    rsPath.LineTo(arrowX + ARROW_P1_OFFSET_X.ConvertToPx(), arrowY - ARROW_P1_OFFSET_Y.ConvertToPx()); //P1
    rsPath.LineTo(arrowX + ARROW_P2_OFFSET_X.ConvertToPx(), arrowY - ARROW_P2_OFFSET_Y.ConvertToPx()); //P2
    rsPath.ArcTo(ARROW_RADIUS.ConvertToPx(), ARROW_RADIUS.ConvertToPx(), 0.0f, RSPathDirection::CW_DIRECTION,
        arrowX - ARROW_P2_OFFSET_X.ConvertToPx(), arrowY - ARROW_P2_OFFSET_Y.ConvertToPx()); //P4
    rsPath.LineTo(arrowX - ARROW_P1_OFFSET_X.ConvertToPx(), arrowY - ARROW_P1_OFFSET_Y.ConvertToPx()); //P5
}

void MenuWrapperPaintMethod::BuildRightArrowPath(RSPath& rsPath, float arrowX, float arrowY)
{
    rsPath.LineTo(arrowX + ARROW_P1_OFFSET_Y.ConvertToPx(), arrowY + ARROW_P1_OFFSET_X.ConvertToPx()); //P1
    rsPath.LineTo(arrowX + ARROW_P2_OFFSET_Y.ConvertToPx(), arrowY + ARROW_P2_OFFSET_X.ConvertToPx()); //P2
    rsPath.ArcTo(ARROW_RADIUS.ConvertToPx(), ARROW_RADIUS.ConvertToPx(), 0.0f, RSPathDirection::CW_DIRECTION,
        arrowX + ARROW_P2_OFFSET_Y.ConvertToPx(), arrowY - ARROW_P2_OFFSET_X.ConvertToPx()); //P4
    rsPath.LineTo(arrowX + ARROW_P1_OFFSET_Y.ConvertToPx(), arrowY - ARROW_P1_OFFSET_X.ConvertToPx()); //P5
}

void MenuWrapperPaintMethod::BuildLeftArrowPath(RSPath& rsPath, float arrowX, float arrowY)
{
    rsPath.LineTo(arrowX - ARROW_P1_OFFSET_Y.ConvertToPx(), arrowY - ARROW_P1_OFFSET_X.ConvertToPx()); //P1
    rsPath.LineTo(arrowX - ARROW_P2_OFFSET_Y.ConvertToPx(), arrowY - ARROW_P2_OFFSET_X.ConvertToPx()); //P2
    rsPath.ArcTo(ARROW_RADIUS.ConvertToPx(), ARROW_RADIUS.ConvertToPx(), 0.0f, RSPathDirection::CW_DIRECTION,
        arrowX - ARROW_P2_OFFSET_Y.ConvertToPx(), arrowY + ARROW_P2_OFFSET_X.ConvertToPx()); //P4
    rsPath.LineTo(arrowX - ARROW_P1_OFFSET_Y.ConvertToPx(), arrowY + ARROW_P1_OFFSET_X.ConvertToPx()); //P5
}
} // namespace OHOS::Ace::NG
