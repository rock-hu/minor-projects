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

import { int32, int64, float32 } from "@koalaui/common"
import { KInt, KPointer, KBoolean, KStringPtr, wrapCallback, NativeBuffer } from "@koalaui/interop"
import { NodeAttach, remember } from "@koalaui/runtime"
import { CommonMethod, DrawModifier, Rectangle, Callback_Array_TouchTestInfo_TouchResult, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, Callback_ClickEvent_Void, ClickEvent, Callback_Boolean_HoverEvent_Void, HoverEvent, AccessibilityCallback, Callback_MouseEvent_Void, MouseEvent, Callback_TouchEvent_Void, TouchEvent, Callback_KeyEvent_Void, KeyEvent, Callback_KeyEvent_Boolean, AnimateParam, TransitionOptions, TransitionEffect, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, DragEvent, CustomBuilder, DragItemInfo, Callback_DragEvent_String_Void, UniformDataType, Callback_PreDragStatus_Void, PreDragStatus, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, MotionPathOptions, ShadowOptions, ShadowStyle, ProgressMask, StateStyles, PixelStretchEffectOptions, GestureModifier, BackgroundBrightnessOptions, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, Callback_TouchEvent_HitTestMode, SizeChangeCallback, SafeAreaType, SafeAreaEdge, Literal_Alignment_align, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./common"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions } from "./units"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey } from "./enums"
import { LengthMetrics } from "../Graphics"
import { ResizableOptions } from "./image"
import { Resource } from "global/resource";
import { Callback_Void } from "./abilityComponent"
import { FocusBoxStyle, FocusPriority } from "./focus"
import { CircleShape } from "./../generated/ArkCircleShapeMaterialized"
import { EllipseShape } from "./../generated/ArkEllipseShapeMaterialized"
import { PathShape } from "./../generated/ArkPathShapeMaterialized"
import { RectShape } from "./../generated/ArkRectShapeMaterialized"
import { AttributeModifier } from "./../component/common" 
import { GestureInfo, BaseGestureEvent, GestureJudgeResult, GestureType, GestureMask } from "./gesture"
import { PixelMap } from "./../generated/ArkPixelMapMaterialized"
import { Callback_Number_Void } from "./alphabetIndexer"
import { ArkPanelComponent } from "./../generated/ArkPanel"
import { ArkPanelPeer } from "./../generated/peers/ArkPanelPeer"
export enum PanelMode {
    MINI = 0,
    Mini = 0,
    HALF = 1,
    Half = 1,
    FULL = 2,
    Full = 2
}
export enum PanelType {
    MINIBAR = 0,
    Minibar = 0,
    FOLDABLE = 1,
    Foldable = 1,
    TEMPORARY = 2,
    Temporary = 2,
    CUSTOM = 3
}
export enum PanelHeight {
    WRAP_CONTENT = "wrapContent"
}
export type Callback_Number_Number_PanelMode_Void = (width: number, height: number, mode: PanelMode) => void;
export type Callback_PanelMode_Void = (mode: PanelMode) => void;
/** @memo:stable */
export interface PanelAttribute extends CommonMethod {
    /** @memo */
    setPanelOptions(show: boolean): this
    /** @memo */
    mode(value: PanelMode): this
    /** @memo */
    type(value: PanelType): this
    /** @memo */
    dragBar(value: boolean): this
    /** @memo */
    customHeight(value: Dimension | PanelHeight): this
    /** @memo */
    fullHeight(value: number | string): this
    /** @memo */
    halfHeight(value: number | string): this
    /** @memo */
    miniHeight(value: number | string): this
    /** @memo */
    show(value: boolean): this
    /** @memo */
    backgroundMask(value: ResourceColor): this
    /** @memo */
    showCloseIcon(value: boolean): this
    /** @memo */
    onChange(value: ((width: number,height: number,mode: PanelMode) => void)): this
    /** @memo */
    onHeightChange(value: ((index: number) => void)): this
    /** @memo */
    _onChangeEvent_mode(callback: ((mode: PanelMode) => void)): void
}
/** @memo */
export function Panel(
  /** @memo */
  style: ((attributes: PanelAttribute) => void) | undefined,
  show: boolean, 
  /** @memo */
  content_?: () => void,
) {
    const receiver = remember(() => {
        return new ArkPanelComponent()
    })
    NodeAttach<ArkPanelPeer>((): ArkPanelPeer => ArkPanelPeer.create(receiver), (_: ArkPanelPeer) => {
        receiver.setPanelOptions(show)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
