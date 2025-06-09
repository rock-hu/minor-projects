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
import { ComponentBase } from "../ComponentBase"
import { TypeChecker } from "#components"
import { ArkCommonMethodComponent } from "./ArkCommon"
import { ArkShapePeer } from "./peers/ArkShapePeer"
import { CommonMethod, DrawModifier, Rectangle, Callback_Array_TouchTestInfo_TouchResult, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, Callback_ClickEvent_Void, ClickEvent, Callback_Boolean_HoverEvent_Void, HoverEvent, AccessibilityCallback, Callback_MouseEvent_Void, MouseEvent, Callback_TouchEvent_Void, TouchEvent, Callback_KeyEvent_Void, KeyEvent, Callback_KeyEvent_Boolean, AnimateParam, TransitionOptions, TransitionEffect, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, DragEvent, CustomBuilder, DragItemInfo, Callback_DragEvent_String_Void, UniformDataType, Callback_PreDragStatus_Void, PreDragStatus, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, MotionPathOptions, ShadowOptions, ShadowStyle, ProgressMask, StateStyles, PixelStretchEffectOptions, GestureModifier, BackgroundBrightnessOptions, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, Callback_TouchEvent_HitTestMode, SizeChangeCallback, SafeAreaType, SafeAreaEdge, Literal_Alignment_align, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./../component/common"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions } from "./../component/units"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, LineCapStyle, LineJoinStyle } from "./../component/enums"
import { LengthMetrics } from "../Graphics"
import { ResizableOptions } from "./../component/image"
import { Resource } from "global/resource";
import { Callback_Void } from "./../component/abilityComponent"
import { FocusBoxStyle, FocusPriority } from "./../component/focus"
import { CircleShape } from "./ArkCircleShapeMaterialized"
import { EllipseShape } from "./ArkEllipseShapeMaterialized"
import { PathShape } from "./ArkPathShapeMaterialized"
import { RectShape } from "./ArkRectShapeMaterialized"
import { AttributeModifier } from "./../component/common" 
import { GestureInfo, BaseGestureEvent, GestureJudgeResult, GestureType, GestureMask } from "./../component/gesture"
import { PixelMap } from "./ArkPixelMapMaterialized"
import { ViewportRect, ShapeAttribute } from "./../component/shape"
/** @memo:stable */
export class ArkShapeComponent extends ArkCommonMethodComponent implements ShapeAttribute {
    getPeer(): ArkShapePeer {
        return (this.peer as ArkShapePeer)
    }
    /** @memo */
    public setShapeOptions(value?: PixelMap): this {
        if (this.checkPriority("setShapeOptions")) {
            const value_type = runtimeType(value)
            if (RuntimeType.UNDEFINED == value_type) {
                this.getPeer()?.setShapeOptions1Attribute()
                return this
            }
            if (RuntimeType.OBJECT == value_type) {
                const value_casted = value as (PixelMap)
                this.getPeer()?.setShapeOptions0Attribute(value_casted)
                return this
            }
            throw new Error("Can not select appropriate overload")
        }
        return this
    }
    /** @memo */
    public viewPort(value: ViewportRect): this {
        if (this.checkPriority("viewPort")) {
            const value_casted = value as (ViewportRect)
            this.getPeer()?.viewPortAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public stroke(value: ResourceColor): this {
        if (this.checkPriority("stroke")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.strokeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fill(value: ResourceColor): this {
        if (this.checkPriority("fill")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.fillAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeDashOffset(value: number | string): this {
        if (this.checkPriority("strokeDashOffset")) {
            const value_casted = value as (number | string)
            this.getPeer()?.strokeDashOffsetAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeDashArray(value: Array<Length>): this {
        if (this.checkPriority("strokeDashArray")) {
            const value_casted = value as (Array<Length>)
            this.getPeer()?.strokeDashArrayAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeLineCap(value: LineCapStyle): this {
        if (this.checkPriority("strokeLineCap")) {
            const value_casted = value as (LineCapStyle)
            this.getPeer()?.strokeLineCapAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeLineJoin(value: LineJoinStyle): this {
        if (this.checkPriority("strokeLineJoin")) {
            const value_casted = value as (LineJoinStyle)
            this.getPeer()?.strokeLineJoinAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeMiterLimit(value: number | string): this {
        if (this.checkPriority("strokeMiterLimit")) {
            const value_casted = value as (number | string)
            this.getPeer()?.strokeMiterLimitAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeOpacity(value: number | string | Resource): this {
        if (this.checkPriority("strokeOpacity")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.strokeOpacityAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fillOpacity(value: number | string | Resource): this {
        if (this.checkPriority("fillOpacity")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.fillOpacityAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public strokeWidth(value: number | string): this {
        if (this.checkPriority("strokeWidth")) {
            const value_casted = value as (number | string)
            this.getPeer()?.strokeWidthAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public antiAlias(value: boolean): this {
        if (this.checkPriority("antiAlias")) {
            const value_casted = value as (boolean)
            this.getPeer()?.antiAliasAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public mesh(value: Array<number>, column: number, row: number): this {
        if (this.checkPriority("mesh")) {
            const value_casted = value as (Array<number>)
            const column_casted = column as (number)
            const row_casted = row as (number)
            this.getPeer()?.meshAttribute(value_casted, column_casted, row_casted)
            return this
        }
        return this
    }
    public applyAttributesFinish(): void {
        // we calls this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}
