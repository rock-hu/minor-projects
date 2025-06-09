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
import { ArkRowPeer } from "./peers/ArkRowPeer"
import { CommonMethod, Rectangle, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, CustomBuilder, DragItemInfo, UniformDataType, PreDragStatus, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback, PointLightStyle } from "./ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions } from "./ArkUnitsInterfaces"
import { DrawModifier } from "./ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, VerticalAlign, FlexAlign } from "./ArkEnumsInterfaces"
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
import { RowOptions, RowAttribute, RowInterface } from "./ArkRowInterfaces"
import { AttributeUpdater } from "../handwritten/modifiers/ArkAttributeUpdater"
import { ArkRowNode } from "../handwritten/modifiers/ArkRowNode"
import { ArkRowAttributeSet,RowModifier } from "../handwritten/modifiers/ArkRowModifier"
import { applyUIAttributes } from "../handwritten/modifiers/ArkCommonModifier"
/** @memo:stable */
export class ArkRowComponent extends ArkCommonMethodComponent {
    getPeer(): ArkRowPeer {
        return (this.peer as ArkRowPeer)
    }

    getModifierHost(): ArkRowNode {
        if (this._modifierHost === undefined || this._modifierHost === null) {
            this._modifierHost = new ArkRowNode()
            this._modifierHost!.setPeer(this.getPeer())
        }
        return this._modifierHost as ArkRowNode
    }

    /** @memo */
    public setRowOptions(options?: RowOptions): this {
        if (this.checkPriority("setRowOptions")) {
            const options_casted = options as (RowOptions | undefined)
            this.getPeer()?.setRowOptionsAttribute(options_casted)
            return this
        }
        return this
    }

    attributeModifier(modifier: AttributeModifier<RowAttribute>): this {
        let peerNode = this.getPeer();
        let attributeSet = peerNode._attributeSet;
        let isAttributeUpdater = modifier instanceof AttributeUpdater
        if (isAttributeUpdater) {
            let attributeUpdater = modifier as AttributeUpdater<RowAttribute>;
            attributeUpdater.attribute = this.getModifierHost();
            if (!attributeSet) {
                attributeSet = new ArkRowAttributeSet();
                peerNode._attributeSet = attributeSet;
            }
            attributeUpdater.initializeModifier((peerNode._attributeSet as Object) as RowAttribute)
        } else {
            let attributeModifier = ((modifier as Object) as AttributeModifier<RowAttribute>);
            let rowModifier = (attributeModifier as Object) as RowModifier;
            if (!attributeSet) {
                attributeSet = new ArkRowAttributeSet();
                attributeSet._modifiersWithKeys = rowModifier._modifiersWithKeys;
                peerNode._attributeSet = attributeSet;
            }
        }
        applyUIAttributes(modifier, peerNode);
        peerNode._attributeSet!.applyModifierPatch(peerNode);
        return this;
    }

    /** @memo */
    public alignItems(value: VerticalAlign): this {
        if (this.checkPriority("alignItems")) {
            const value_casted = value as (VerticalAlign)
            this.getPeer()?.alignItemsAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public justifyContent(value: FlexAlign): this {
        if (this.checkPriority("justifyContent")) {
            const value_casted = value as (FlexAlign)
            this.getPeer()?.justifyContentAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public pointLight(value: PointLightStyle): this {
        if (this.checkPriority("pointLight")) {
            const value_casted = value as (PointLightStyle)
            this.getPeer()?.pointLightAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public reverse(value?: boolean): this {
        if (this.checkPriority("reverse")) {
            const value_casted = value as (boolean | undefined)
            this.getPeer()?.reverseAttribute(value_casted)
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
export function ArkRow(
  /** @memo */
  style: ((attributes: ArkRowComponent) => void) | undefined,
  /** @memo */
  content_: (() => void) | undefined,
  options?: RowOptions | undefined
) {
    const receiver = remember(() => {
        return new ArkRowComponent()
    })
    NodeAttach<ArkRowPeer>((): ArkRowPeer => ArkRowPeer.create(receiver), (_: ArkRowPeer) => {
        receiver.setRowOptions(options)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
