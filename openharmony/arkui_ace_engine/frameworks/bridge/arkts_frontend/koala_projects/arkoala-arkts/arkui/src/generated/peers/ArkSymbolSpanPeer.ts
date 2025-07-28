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
import { nullptr, KPointer, KInt, KBoolean, KStringPtr, runtimeType, RuntimeType, MaterializedBase, toPeerPtr, wrapCallback } from "@koalaui/interop"
import { Serializer } from "./Serializer"
import { ComponentBase } from "./../../ComponentBase"
import { PeerNode } from "./../../PeerNode"
import { ArkUIGeneratedNativeModule, TypeChecker } from "#components"
import { ArkCommonMethodPeer, ArkCommonMethodAttributes } from "./ArkCommonPeer"
import { CommonMethod, Rectangle, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, CustomBuilder, DragItemInfo, UniformDataType, PreDragStatus, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./../ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, PX, VP, FP, LPX, Percentage } from "./../ArkUnitsInterfaces"
import { DrawModifier } from "./../ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, FontWeight } from "./../ArkEnumsInterfaces"
import { Callback_Array_TouchTestInfo_TouchResult, Callback_ClickEvent_Void, Callback_Boolean_HoverEvent_Void, Callback_MouseEvent_Void, Callback_TouchEvent_Void, Callback_KeyEvent_Void, Callback_KeyEvent_Boolean, Callback_Void, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, Callback_DragEvent_String_Void, Callback_PreDragStatus_Void, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, Callback_TouchEvent_HitTestMode, Literal_Alignment_align } from "./../SyntheticDeclarations"
import { LengthMetrics } from "./../ArkLengthMetricsMaterialized"
import { ResizableOptions } from "./../ArkImageInterfaces"
import { Resource } from "./../ArkResourceInterfaces"
import { ClickEvent } from "./../ArkClickEventMaterialized"
import { HoverEvent } from "./../ArkHoverEventMaterialized"
import { MouseEvent } from "./../ArkMouseEventMaterialized"
import { TouchEvent } from "./../ArkTouchEventMaterialized"
import { KeyEvent } from "./../ArkKeyEventMaterialized"
import { FocusBoxStyle, FocusPriority } from "./../ArkFocusInterfaces"
import { TransitionEffect } from "./../ArkTransitionEffectMaterialized"
import { DragEvent } from "./../ArkDragEventMaterialized"
import { CircleShape } from "./../ArkCircleShapeMaterialized"
import { EllipseShape } from "./../ArkEllipseShapeMaterialized"
import { PathShape } from "./../ArkPathShapeMaterialized"
import { RectShape } from "./../ArkRectShapeMaterialized"
import { ProgressMask } from "./../ArkProgressMaskMaterialized"
import { AttributeModifier } from "./../../handwritten"
import { GestureModifier } from "./../ArkGestureModifierMaterialized"
import { GestureInfo, GestureJudgeResult, GestureType, GestureMask } from "./../ArkGestureInterfaces"
import { BaseGestureEvent } from "./../ArkBaseGestureEventMaterialized"
import { PixelMap } from "./../ArkPixelMapMaterialized"
import { SymbolEffectStrategy, SymbolRenderingStrategy } from "./../ArkSymbolglyphInterfaces"
import { SymbolSpanAttribute } from "./../ArkSymbolSpanInterfaces"
import { TapGestureInterface } from "./../ArkTapGestureInterfaceMaterialized"
import { LongPressGestureInterface } from "./../ArkLongPressGestureInterfaceMaterialized"
import { PanGestureInterface } from "./../ArkPanGestureInterfaceMaterialized"
import { PinchGestureInterface } from "./../ArkPinchGestureInterfaceMaterialized"
import { SwipeGestureInterface } from "./../ArkSwipeGestureInterfaceMaterialized"
import { RotationGestureInterface } from "./../ArkRotationGestureInterfaceMaterialized"
import { GestureGroupInterface } from "./../ArkGestureGroupInterfaceMaterialized"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { DotIndicator } from "./../ArkDotIndicatorBuilder"
import { DigitIndicator } from "./../ArkDigitIndicatorBuilder"
import { SubTabBarStyle } from "./../ArkSubTabBarStyleBuilder"
import { BottomTabBarStyle } from "./../ArkBottomTabBarStyleBuilder"
export class ArkSymbolSpanPeer extends ArkCommonMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkSymbolSpanPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._SymbolSpan_construct(peerId, flags)
        const _peer  = new ArkSymbolSpanPeer(_peerPtr, peerId, "SymbolSpan", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setSymbolSpanOptionsAttribute(value: Resource): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeResource(value)
        ArkUIGeneratedNativeModule._SymbolSpanInterface_setSymbolSpanOptions(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    fontSizeAttribute(value: number | string | Resource): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        if (RuntimeType.NUMBER == value_type) {
            thisSerializer.writeInt8(0 as int32)
            const value_0  = value as number
            thisSerializer.writeNumber(value_0)
        }
        else if (RuntimeType.STRING == value_type) {
            thisSerializer.writeInt8(1 as int32)
            const value_1  = value as string
            thisSerializer.writeString(value_1)
        }
        else if (RuntimeType.OBJECT == value_type) {
            thisSerializer.writeInt8(2 as int32)
            const value_2  = value as Resource
            thisSerializer.writeResource(value_2)
        }
        ArkUIGeneratedNativeModule._SymbolSpanAttribute_fontSize(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    fontColorAttribute(value: Array<ResourceColor>): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeInt32(value.length as int32)
        for (let i = 0; i < value.length; i++) {
            const value_element : ResourceColor = value[i]
            let value_element_type : int32 = RuntimeType.UNDEFINED
            value_element_type = runtimeType(value_element)
            if (TypeChecker.isColor(value_element)) {
                thisSerializer.writeInt8(0 as int32)
                const value_element_0  = value_element as Color
                thisSerializer.writeInt32(value_element_0.valueOf())
            }
            else if (RuntimeType.NUMBER == value_element_type) {
                thisSerializer.writeInt8(1 as int32)
                const value_element_1  = value_element as number
                thisSerializer.writeNumber(value_element_1)
            }
            else if (RuntimeType.STRING == value_element_type) {
                thisSerializer.writeInt8(2 as int32)
                const value_element_2  = value_element as string
                thisSerializer.writeString(value_element_2)
            }
            else if (RuntimeType.OBJECT == value_element_type) {
                thisSerializer.writeInt8(3 as int32)
                const value_element_3  = value_element as Resource
                thisSerializer.writeResource(value_element_3)
            }
        }
        ArkUIGeneratedNativeModule._SymbolSpanAttribute_fontColor(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    fontWeightAttribute(value: number | FontWeight | string): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        if (RuntimeType.NUMBER == value_type) {
            thisSerializer.writeInt8(0 as int32)
            const value_0  = value as number
            thisSerializer.writeNumber(value_0)
        }
        else if (TypeChecker.isFontWeight(value)) {
            thisSerializer.writeInt8(1 as int32)
            const value_1  = value as FontWeight
            thisSerializer.writeInt32(value_1.valueOf())
        }
        else if (RuntimeType.STRING == value_type) {
            thisSerializer.writeInt8(2 as int32)
            const value_2  = value as string
            thisSerializer.writeString(value_2)
        }
        ArkUIGeneratedNativeModule._SymbolSpanAttribute_fontWeight(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    effectStrategyAttribute(value: SymbolEffectStrategy): void {
        ArkUIGeneratedNativeModule._SymbolSpanAttribute_effectStrategy(this.peer.ptr, value.valueOf())
    }
    renderingStrategyAttribute(value: SymbolRenderingStrategy): void {
        ArkUIGeneratedNativeModule._SymbolSpanAttribute_renderingStrategy(this.peer.ptr, value.valueOf())
    }
}
export interface ArkSymbolSpanAttributes extends ArkCommonMethodAttributes {
    fontSize?: number | string | Resource
    fontColor?: Array<ResourceColor>
    fontWeight?: number | FontWeight | string
    effectStrategy?: SymbolEffectStrategy
    renderingStrategy?: SymbolRenderingStrategy
    attributeModifier?: AttributeModifier<object>
}
