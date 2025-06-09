/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// WARNING! THIS FILE IS AUTO-GENERATED, DO NOT MAKE CHANGES, THEY WILL BE LOST ON NEXT GENERATION!

import { int32, float32, unsafeCast } from "@koalaui/common"
import { KStringPtr, KBoolean, RuntimeType, runtimeType, isInstanceOf } from "@koalaui/interop"
import { NodeAttach, remember } from "@koalaui/runtime"
import { ComponentBase } from "./../ComponentBase"
import { isResource, isPadding } from "./../utils"
import { ArkCommonMethodComponent } from "./ArkCommon"
import { ArkSwiperPeer } from "./peers/ArkSwiperPeer"
import { CommonMethod, Rectangle, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, UniformDataType, CustomBuilder, DragItemInfo, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback, DrawContext, TouchTestInfo, TouchResult, AdaptiveColor, BlurStyleActivePolicy, RepeatMode, LinearGradient_common, FinishCallbackType, ExpectedFrameRateRange, TransitionEffects, TransitionEdge, MotionBlurAnchor, LocalizedHorizontalAlignParam, LocalizedVerticalAlignParam, PreDragStatus, ShadowType, UIGestureEvent, BlurStyleOptions, ThemeColorMode, FractionStop, DragPreviewMode, ImageModifier, OverlayOffset, TransitionHierarchyStrategy, PopupMessageOptions, SymbolGlyphModifier, MenuPreviewMode, ContextMenuAnimationOptions, BindOptions, SheetSize, SheetType, SheetTitleOptions, SheetMode, ScrollSizeMode, UIContext, SheetKeyboardAvoidMode, SourceTool, RectResult, TouchTestStrategy, EventTarget, SourceType, TouchObject, HistoricalPoint, IntentionCode, DragBehavior, Summary, DragResult, DismissPopupAction, DismissContentCoverAction, SheetDismiss, DismissSheetAction, SpringBackAction, DismissReason } from "./ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, PX, VP, FP, LPX, Percentage, Area, Bias, Font, VoidCallback } from "./ArkUnitsInterfaces"
import { DrawModifier } from "./ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, PixelRoundCalcPolicy, Curve, PlayMode, TransitionType, HorizontalAlign, VerticalAlign, ClickEffectLevel, SharedTransitionEffectType, Placement, ArrowPointPosition, MouseButton, MouseAction, TouchType, KeyType, KeySource, FontWeight, FontStyle, EdgeEffect } from "./ArkEnumsInterfaces"
import { LengthMetrics } from "./ArkLengthMetricsMaterialized"
import { ResizableOptions, DrawingLattice } from "./ArkImageInterfaces"
import { Resource } from "./ArkResourceInterfaces"
import { FocusBoxStyle, FocusPriority } from "./ArkFocusInterfaces"
import { TransitionEffect } from "./ArkTransitionEffectMaterialized"
import { CircleShape } from "./ArkCircleShapeMaterialized"
import { EllipseShape } from "./ArkEllipseShapeMaterialized"
import { PathShape } from "./ArkPathShapeMaterialized"
import { RectShape } from "./ArkRectShapeMaterialized"
import { ProgressMask } from "./ArkProgressMaskMaterialized"
import { AttributeModifier } from "./../handwritten"
import { GestureModifier } from "./ArkGestureModifierMaterialized"
import { PixelMap } from "./ArkPixelMapMaterialized"
import { GestureType, GestureMask, GestureInfo, GestureJudgeResult, GestureInterface, TapGestureParameters, PanDirection, SwipeDirection, GestureMode, GestureHandler, GesturePriority, FingerInfo } from "./ArkGestureInterfaces"
import { TapGestureInterface } from "./ArkTapGestureInterfaceMaterialized"
import { LongPressGestureInterface } from "./ArkLongPressGestureInterfaceMaterialized"
import { PanGestureInterface } from "./ArkPanGestureInterfaceMaterialized"
import { PinchGestureInterface } from "./ArkPinchGestureInterfaceMaterialized"
import { SwipeGestureInterface } from "./ArkSwipeGestureInterfaceMaterialized"
import { RotationGestureInterface } from "./ArkRotationGestureInterfaceMaterialized"
import { GestureGroupInterface } from "./ArkGestureGroupInterfaceMaterialized"
import { LengthUnit, ShapeSize, PathShapeOptions, RectShapeOptions, RoundRectShapeOptions } from "./ArkArkuiExternalInterfaces"
import { ClickEvent } from "./ArkClickEventMaterialized"
import { HoverEvent } from "./ArkHoverEventMaterialized"
import { MouseEvent } from "./ArkMouseEventMaterialized"
import { TouchEvent } from "./ArkTouchEventMaterialized"
import { KeyEvent } from "./ArkKeyEventMaterialized"
import { ColorMetrics } from "./ArkColorMetricsMaterialized"
import { ICurve } from "./ArkICurveMaterialized"
import { DragEvent } from "./ArkDragEventMaterialized"
import { BaseGestureEvent } from "./ArkBaseGestureEventMaterialized"
import { PanGestureOptions } from "./ArkPanGestureOptionsMaterialized"
import { BaseEvent } from "./ArkBaseEventMaterialized"
import { UnifiedData } from "./ArkUnifiedDataMaterialized"
import { GestureControl } from "./ArkGestureControlNamespace"
import { GestureEvent } from "./ArkGestureEventMaterialized"
import { DotIndicator } from "./ArkDotIndicatorBuilder"
import { DigitIndicator } from "./ArkDigitIndicatorBuilder"
import { IndicatorComponentController } from "./ArkIndicatorComponentControllerMaterialized"
import { SwiperDisplayMode, IndicatorStyle, OnSwiperAnimationStartCallback, OnSwiperAnimationEndCallback, OnSwiperGestureSwipeCallback, SwiperNestedScrollMode, SwiperContentAnimatedTransition, ContentDidScrollCallback, ArrowStyle, SwiperAttribute, SwiperAutoFill } from "./ArkSwiperInterfaces"
import { Indicator } from "./ArkIndicatorBuilder"
import { SwiperContentTransitionProxy } from "./ArkSwiperContentTransitionProxyMaterialized"
import { SwiperController } from "./ArkSwiperControllerMaterialized"
/** @memo:stable */
export class ArkSwiperComponent extends ArkCommonMethodComponent {
    protected getPeer(): ArkSwiperPeer {
        return (this.peer as ArkSwiperPeer)
    }
    /** @memo */
    public setSwiperOptions(controller?: SwiperController): this {
        if (this.checkPriority("setSwiperOptions")) {
            const controller_casted = controller as (SwiperController | undefined)
            this.getPeer()?.setSwiperOptionsAttribute(controller_casted)
            return this
        }
        return this
    }
    /** @memo */
    public index(value: number): this {
        if (this.checkPriority("index")) {
            const value_casted = value as (number)
            this.getPeer()?.indexAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public autoPlay(value: boolean): this {
        if (this.checkPriority("autoPlay")) {
            const value_casted = value as (boolean)
            this.getPeer()?.autoPlayAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public interval(value: number): this {
        if (this.checkPriority("interval")) {
            const value_casted = value as (number)
            this.getPeer()?.intervalAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public indicator(value: IndicatorComponentController | DotIndicator | DigitIndicator | boolean): this {
        if (this.checkPriority("indicator")) {
            const value_type = runtimeType(value)
            if (((RuntimeType.OBJECT) == (value_type)) && (value instanceof IndicatorComponentController)) {
                const value_casted = value as (IndicatorComponentController)
                this.getPeer()?.indicator1Attribute(value_casted)
                return this
            }
            if ((((RuntimeType.OBJECT) == (value_type)) && (value instanceof DotIndicator)) || (((RuntimeType.OBJECT) == (value_type)) && (value instanceof DigitIndicator)) || (RuntimeType.BOOLEAN == value_type)) {
                const value_casted = value as (DotIndicator | DigitIndicator | boolean)
                this.getPeer()?.indicator0Attribute(value_casted)
                return this
            }
            throw new Error("Can not select appropriate overload")
        }
        return this
    }
    /** @memo */
    public loop(value: boolean): this {
        if (this.checkPriority("loop")) {
            const value_casted = value as (boolean)
            this.getPeer()?.loopAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public duration(value: number): this {
        if (this.checkPriority("duration")) {
            const value_casted = value as (number)
            this.getPeer()?.durationAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public vertical(value: boolean): this {
        if (this.checkPriority("vertical")) {
            const value_casted = value as (boolean)
            this.getPeer()?.verticalAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public itemSpace(value: number | string): this {
        if (this.checkPriority("itemSpace")) {
            const value_casted = value as (number | string)
            this.getPeer()?.itemSpaceAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public displayMode(value: SwiperDisplayMode): this {
        if (this.checkPriority("displayMode")) {
            const value_casted = value as (SwiperDisplayMode)
            this.getPeer()?.displayModeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public cachedCount(value: number): this {
        if (this.checkPriority("cachedCount")) {
            const value_casted = value as (number)
            this.getPeer()?.cachedCountAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public effectMode(value: EdgeEffect): this {
        if (this.checkPriority("effectMode")) {
            const value_casted = value as (EdgeEffect)
            this.getPeer()?.effectModeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public disableSwipe(value: boolean): this {
        if (this.checkPriority("disableSwipe")) {
            const value_casted = value as (boolean)
            this.getPeer()?.disableSwipeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public curve(value: Curve | string | ICurve): this {
        if (this.checkPriority("curve")) {
            const value_casted = value as (Curve | string | ICurve)
            this.getPeer()?.curveAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onChange(value: ((index: number) => void)): this {
        if (this.checkPriority("onChange")) {
            const value_casted = value as (((index: number) => void))
            this.getPeer()?.onChangeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public indicatorStyle(value?: IndicatorStyle): this {
        if (this.checkPriority("indicatorStyle")) {
            const value_casted = value as (IndicatorStyle | undefined)
            this.getPeer()?.indicatorStyleAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onAnimationStart(value: OnSwiperAnimationStartCallback): this {
        if (this.checkPriority("onAnimationStart")) {
            const value_casted = value as (OnSwiperAnimationStartCallback)
            this.getPeer()?.onAnimationStartAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onAnimationEnd(value: OnSwiperAnimationEndCallback): this {
        if (this.checkPriority("onAnimationEnd")) {
            const value_casted = value as (OnSwiperAnimationEndCallback)
            this.getPeer()?.onAnimationEndAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onGestureSwipe(value: OnSwiperGestureSwipeCallback): this {
        if (this.checkPriority("onGestureSwipe")) {
            const value_casted = value as (OnSwiperGestureSwipeCallback)
            this.getPeer()?.onGestureSwipeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public nestedScroll(value: SwiperNestedScrollMode): this {
        if (this.checkPriority("nestedScroll")) {
            const value_casted = value as (SwiperNestedScrollMode)
            this.getPeer()?.nestedScrollAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public customContentTransition(value: SwiperContentAnimatedTransition): this {
        if (this.checkPriority("customContentTransition")) {
            const value_casted = value as (SwiperContentAnimatedTransition)
            this.getPeer()?.customContentTransitionAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onContentDidScroll(value: ContentDidScrollCallback): this {
        if (this.checkPriority("onContentDidScroll")) {
            const value_casted = value as (ContentDidScrollCallback)
            this.getPeer()?.onContentDidScrollAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public indicatorInteractive(value: boolean): this {
        if (this.checkPriority("indicatorInteractive")) {
            const value_casted = value as (boolean)
            this.getPeer()?.indicatorInteractiveAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public displayArrow(value: ArrowStyle | boolean, isHoverShow?: boolean): this {
        if (this.checkPriority("displayArrow")) {
            const value_casted = value as (ArrowStyle | boolean)
            const isHoverShow_casted = isHoverShow as (boolean | undefined)
            this.getPeer()?.displayArrowAttribute(value_casted, isHoverShow_casted)
            return this
        }
        return this
    }
    /** @memo */
    public displayCount(value: number | string | SwiperAutoFill, swipeByGroup?: boolean): this {
        if (this.checkPriority("displayCount")) {
            const value_casted = value as (number | string | SwiperAutoFill)
            const swipeByGroup_casted = swipeByGroup as (boolean | undefined)
            this.getPeer()?.displayCountAttribute(value_casted, swipeByGroup_casted)
            return this
        }
        return this
    }
    /** @memo */
    public prevMargin(value: Length, ignoreBlank?: boolean): this {
        if (this.checkPriority("prevMargin")) {
            const value_casted = value as (Length)
            const ignoreBlank_casted = ignoreBlank as (boolean | undefined)
            this.getPeer()?.prevMarginAttribute(value_casted, ignoreBlank_casted)
            return this
        }
        return this
    }
    /** @memo */
    public nextMargin(value: Length, ignoreBlank?: boolean): this {
        if (this.checkPriority("nextMargin")) {
            const value_casted = value as (Length)
            const ignoreBlank_casted = ignoreBlank as (boolean | undefined)
            this.getPeer()?.nextMarginAttribute(value_casted, ignoreBlank_casted)
            return this
        }
        return this
    }
    /** @memo */
    public _onChangeEvent_index(callback: ((index: number) => void)): void {
        if (this.checkPriority("_onChangeEvent_index")) {
            const callback_casted = callback as (((index: number) => void))
            this.getPeer()?._onChangeEvent_indexAttribute(callback_casted)
            return
        }
        return
    }
    public applyAttributesFinish(): void {
        // we calls this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}

/** @memo */
export function ArkSwiper(
  /** @memo */
  style: ((attributes: ArkSwiperComponent) => void) | undefined,
  /** @memo */
  content_: (() => void) | undefined,
  controller?: SwiperController | undefined
) {
    const receiver = remember(() => {
        return new ArkSwiperComponent()
    })
    NodeAttach<ArkSwiperPeer>((): ArkSwiperPeer => ArkSwiperPeer.create(receiver), (_: ArkSwiperPeer) => {
        receiver.setSwiperOptions(controller)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
