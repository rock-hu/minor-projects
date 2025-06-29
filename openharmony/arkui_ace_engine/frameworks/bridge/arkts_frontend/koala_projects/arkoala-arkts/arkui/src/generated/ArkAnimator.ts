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

import { int32, float32 } from "@koalaui/common"
import { KStringPtr, KBoolean, RuntimeType, runtimeType } from "@koalaui/interop"
import { NodeAttach, remember } from "@koalaui/runtime"
import { ComponentBase } from "./../ComponentBase"
import { TypeChecker } from "#components"
import { ArkCommonMethodComponent } from "./ArkCommon"
import { ArkAnimatorPeer } from "./peers/ArkAnimatorPeer"
import { CommonMethod, Rectangle, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, CustomBuilder, DragItemInfo, UniformDataType, PreDragStatus, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions } from "./ArkUnitsInterfaces"
import { DrawModifier } from "./ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, AnimationStatus, Curve, FillMode, PlayMode } from "./ArkEnumsInterfaces"
import { Callback_Array_TouchTestInfo_TouchResult, Callback_ClickEvent_Void, Callback_Boolean_HoverEvent_Void, Callback_MouseEvent_Void, Callback_TouchEvent_Void, Callback_KeyEvent_Void, Callback_KeyEvent_Boolean, Callback_Void, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, Callback_DragEvent_String_Void, Callback_PreDragStatus_Void, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, Callback_TouchEvent_HitTestMode, Literal_Alignment_align, Callback_Number_Void } from "./SyntheticDeclarations"
import { LengthMetrics } from "./ArkLengthMetricsMaterialized"
import { ResizableOptions } from "./ArkImageInterfaces"
import { Resource } from "./ArkResourceInterfaces"
import { ClickEvent } from "./ArkClickEventMaterialized"
import { HoverEvent } from "./ArkHoverEventMaterialized"
import { MouseEvent } from "./ArkMouseEventMaterialized"
import { TouchEvent } from "./ArkTouchEventMaterialized"
import { KeyEvent } from "./ArkKeyEventMaterialized"
import { FocusBoxStyle, FocusPriority } from "./ArkFocusInterfaces"
import { TransitionEffect } from "./ArkTransitionEffectMaterialized"
import { DragEvent } from "./ArkDragEventMaterialized"
import { CircleShape } from "./ArkCircleShapeMaterialized"
import { EllipseShape } from "./ArkEllipseShapeMaterialized"
import { PathShape } from "./ArkPathShapeMaterialized"
import { RectShape } from "./ArkRectShapeMaterialized"
import { ProgressMask } from "./ArkProgressMaskMaterialized"
import { AttributeModifier } from "./../handwritten"
import { GestureModifier } from "./ArkGestureModifierMaterialized"
import { GestureInfo, GestureJudgeResult, GestureType, GestureMask } from "./ArkGestureInterfaces"
import { BaseGestureEvent } from "./ArkBaseGestureEventMaterialized"
import { PixelMap } from "./ArkPixelMapMaterialized"
import { SpringMotion } from "./ArkSpringMotionMaterialized"
import { AnimatorAttribute } from "./ArkAnimatorInterfaces"
/** @memo:stable */
export class ArkAnimatorComponent extends ArkCommonMethodComponent {
    getPeer(): ArkAnimatorPeer {
        return (this.peer as ArkAnimatorPeer)
    }
    /** @memo */
    public setAnimatorOptions(value: string): this {
        if (this.checkPriority("setAnimatorOptions")) {
            const value_casted = value as (string)
            this.getPeer()?.setAnimatorOptionsAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public state(value: AnimationStatus): this {
        if (this.checkPriority("state")) {
            const value_casted = value as (AnimationStatus)
            this.getPeer()?.stateAttribute(value_casted)
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
    public curve(value: Curve): this {
        if (this.checkPriority("curve")) {
            const value_casted = value as (Curve)
            this.getPeer()?.curveAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public delay(value: number): this {
        if (this.checkPriority("delay")) {
            const value_casted = value as (number)
            this.getPeer()?.delayAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fillMode(value: FillMode): this {
        if (this.checkPriority("fillMode")) {
            const value_casted = value as (FillMode)
            this.getPeer()?.fillModeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public iterations(value: number): this {
        if (this.checkPriority("iterations")) {
            const value_casted = value as (number)
            this.getPeer()?.iterationsAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public playMode(value: PlayMode): this {
        if (this.checkPriority("playMode")) {
            const value_casted = value as (PlayMode)
            this.getPeer()?.playModeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public motion(value: SpringMotion): this {
        if (this.checkPriority("motion")) {
            const value_casted = value as (SpringMotion)
            this.getPeer()?.motionAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onStart(value: (() => void)): this {
        if (this.checkPriority("onStart")) {
            const value_casted = value as ((() => void))
            this.getPeer()?.onStartAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onPause(value: (() => void)): this {
        if (this.checkPriority("onPause")) {
            const value_casted = value as ((() => void))
            this.getPeer()?.onPauseAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onRepeat(value: (() => void)): this {
        if (this.checkPriority("onRepeat")) {
            const value_casted = value as ((() => void))
            this.getPeer()?.onRepeatAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onCancel(value: (() => void)): this {
        if (this.checkPriority("onCancel")) {
            const value_casted = value as ((() => void))
            this.getPeer()?.onCancelAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onFinish(value: (() => void)): this {
        if (this.checkPriority("onFinish")) {
            const value_casted = value as ((() => void))
            this.getPeer()?.onFinishAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onFrame(value: ((index: number) => void)): this {
        if (this.checkPriority("onFrame")) {
            const value_casted = value as (((index: number) => void))
            this.getPeer()?.onFrameAttribute(value_casted)
            return this
        }
        return this
    }
    public applyAttributesFinish(): void {
        // we calls this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}

/** @memo */
export function ArkAnimator(
  /** @memo */
  style: ((attributes: ArkAnimatorComponent) => void) | undefined,
  /** @memo */
  content_: (() => void) | undefined,
  value: string
) {
    const receiver = remember(() => {
        return new ArkAnimatorComponent()
    })
    NodeAttach<ArkAnimatorPeer>((): ArkAnimatorPeer => ArkAnimatorPeer.create(receiver), (_: ArkAnimatorPeer) => {
        receiver.setAnimatorOptions(value)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
