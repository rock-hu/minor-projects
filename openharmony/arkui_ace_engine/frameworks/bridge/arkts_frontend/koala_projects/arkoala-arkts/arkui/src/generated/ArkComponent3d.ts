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
import { ArkComponent3DPeer } from "./peers/ArkComponent3dPeer"
import { CommonMethod, Rectangle, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, CustomBuilder, DragItemInfo, UniformDataType, PreDragStatus, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions } from "./ArkUnitsInterfaces"
import { DrawModifier } from "./ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey } from "./ArkEnumsInterfaces"
import { Callback_Array_TouchTestInfo_TouchResult, Callback_ClickEvent_Void, Callback_Boolean_HoverEvent_Void, Callback_MouseEvent_Void, Callback_TouchEvent_Void, Callback_KeyEvent_Void, Callback_KeyEvent_Boolean, Callback_Void, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, Callback_DragEvent_String_Void, Callback_PreDragStatus_Void, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, Callback_TouchEvent_HitTestMode, Literal_Alignment_align } from "./SyntheticDeclarations"
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
import { Component3DAttribute, SceneOptions } from "./ArkComponent3dInterfaces"
/** @memo:stable */
export class ArkComponent3DComponent extends ArkCommonMethodComponent {
    getPeer(): ArkComponent3DPeer {
        return (this.peer as ArkComponent3DPeer)
    }
    /** @memo */
    public setComponent3DOptions(sceneOptions?: SceneOptions): this {
        if (this.checkPriority("setComponent3DOptions")) {
            const sceneOptions_casted = sceneOptions as (SceneOptions | undefined)
            this.getPeer()?.setComponent3DOptionsAttribute(sceneOptions_casted)
            return this
        }
        return this
    }
    /** @memo */
    public environment(value: ResourceStr): this {
        if (this.checkPriority("environment")) {
            const value_casted = value as (ResourceStr)
            this.getPeer()?.environmentAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public shader(value: ResourceStr): this {
        if (this.checkPriority("shader")) {
            const value_casted = value as (ResourceStr)
            this.getPeer()?.shaderAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public shaderImageTexture(value: ResourceStr): this {
        if (this.checkPriority("shaderImageTexture")) {
            const value_casted = value as (ResourceStr)
            this.getPeer()?.shaderImageTextureAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public shaderInputBuffer(value: Array<number>): this {
        if (this.checkPriority("shaderInputBuffer")) {
            const value_casted = value as (Array<number>)
            this.getPeer()?.shaderInputBufferAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public renderWidth(value: Dimension): this {
        if (this.checkPriority("renderWidth")) {
            const value_casted = value as (Dimension)
            this.getPeer()?.renderWidthAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public renderHeight(value: Dimension): this {
        if (this.checkPriority("renderHeight")) {
            const value_casted = value as (Dimension)
            this.getPeer()?.renderHeightAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public customRender(uri: ResourceStr, selfRenderUpdate: boolean): this {
        if (this.checkPriority("customRender")) {
            const uri_casted = uri as (ResourceStr)
            const selfRenderUpdate_casted = selfRenderUpdate as (boolean)
            this.getPeer()?.customRenderAttribute(uri_casted, selfRenderUpdate_casted)
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
export function ArkComponent3D(
  /** @memo */
  style: ((attributes: ArkComponent3DComponent) => void) | undefined,
  /** @memo */
  content_: (() => void) | undefined,
  sceneOptions?: SceneOptions | undefined
) {
    const receiver = remember(() => {
        return new ArkComponent3DComponent()
    })
    NodeAttach<ArkComponent3DPeer>((): ArkComponent3DPeer => ArkComponent3DPeer.create(receiver), (_: ArkComponent3DPeer) => {
        receiver.setComponent3DOptions(sceneOptions)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
