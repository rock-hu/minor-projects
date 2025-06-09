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
import { nullptr, KPointer, KInt, KBoolean, KStringPtr } from "@koalaui/interop"
import { runtimeType, RuntimeType } from "@koalaui/interop"
import { Serializer } from "./Serializer"
import { ComponentBase } from "../../ComponentBase"
import { PeerNode } from "../../PeerNode"
import { ArkUIGeneratedNativeModule } from "#components"
import { ArkCommonMethodPeer, ArkCommonMethodAttributes } from "./ArkCommonPeer"
import { CommonMethod, DrawModifier, Rectangle, Callback_Array_TouchTestInfo_TouchResult, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, Callback_ClickEvent_Void, ClickEvent, Callback_Boolean_HoverEvent_Void, HoverEvent, AccessibilityCallback, Callback_MouseEvent_Void, MouseEvent, Callback_TouchEvent_Void, TouchEvent, Callback_KeyEvent_Void, KeyEvent, Callback_KeyEvent_Boolean, AnimateParam, TransitionOptions, TransitionEffect, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, DragEvent, CustomBuilder, DragItemInfo, Callback_DragEvent_String_Void, UniformDataType, Callback_PreDragStatus_Void, PreDragStatus, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, MotionPathOptions, ShadowOptions, ShadowStyle, ProgressMask, StateStyles, PixelStretchEffectOptions, GestureModifier, BackgroundBrightnessOptions, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, Callback_TouchEvent_HitTestMode, SizeChangeCallback, SafeAreaType, SafeAreaEdge, Literal_Alignment_align, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./../../component/common"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, PX, VP, FP, LPX, Percentage } from "./../../component/units"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, FontWeight } from "./../../component/enums"
import { LengthMetrics } from "../../Graphics"
import { ResizableOptions } from "./../../component/image"
import { Resource } from "global/resource";
import { Callback_Void } from "./../../component/abilityComponent"
import { FocusBoxStyle, FocusPriority } from "./../../component/focus"
import { CircleShape } from "./../ArkCircleShapeMaterialized"
import { EllipseShape } from "./../ArkEllipseShapeMaterialized"
import { PathShape } from "./../ArkPathShapeMaterialized"
import { RectShape } from "./../ArkRectShapeMaterialized"
import { AttributeModifier } from "./../../component/common" 
import { GestureInfo, BaseGestureEvent, GestureJudgeResult, GestureType, GestureMask, TapGestureInterface, LongPressGestureInterface, PanGestureInterface, PinchGestureInterface, SwipeGestureInterface, RotationGestureInterface, GestureGroupInterface } from "./../../component/gesture"
import { PixelMap } from "./../ArkPixelMapMaterialized"
import { SymbolEffectStrategy, SymbolRenderingStrategy, SymbolGlyphAttribute } from "./../../component/symbolglyph"
import { SymbolEffect } from "./../ArkArkuiExternalInterfaces"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { TypeChecker } from "#components"
import { MaterializedBase, toPeerPtr, wrapCallback } from "@koalaui/interop"
export class ArkSymbolGlyphPeer extends ArkCommonMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkSymbolGlyphPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._SymbolGlyph_construct(peerId, flags)
        const _peer  = new ArkSymbolGlyphPeer(_peerPtr, peerId, "SymbolGlyph", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setSymbolGlyphOptionsAttribute(value?: Resource): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        thisSerializer.writeInt8(value_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_type)) {
            const value_value  = value!
            thisSerializer.writeResource(value_value)
        }
        ArkUIGeneratedNativeModule._SymbolGlyphInterface_setSymbolGlyphOptions(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
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
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_fontSize(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
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
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_fontColor(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
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
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_fontWeight(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    effectStrategyAttribute(value: SymbolEffectStrategy): void {
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_effectStrategy(this.peer.ptr, value.valueOf())
    }
    renderingStrategyAttribute(value: SymbolRenderingStrategy): void {
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_renderingStrategy(this.peer.ptr, value.valueOf())
    }
    symbolEffect0Attribute(symbolEffect: SymbolEffect, isActive?: boolean): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeSymbolEffect(symbolEffect)
        let isActive_type : int32 = RuntimeType.UNDEFINED
        isActive_type = runtimeType(isActive)
        thisSerializer.writeInt8(isActive_type as int32)
        if ((RuntimeType.UNDEFINED) != (isActive_type)) {
            const isActive_value  = isActive!
            thisSerializer.writeBoolean(isActive_value)
        }
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_symbolEffect0(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    symbolEffect1Attribute(symbolEffect: SymbolEffect, triggerValue?: number): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeSymbolEffect(symbolEffect)
        let triggerValue_type : int32 = RuntimeType.UNDEFINED
        triggerValue_type = runtimeType(triggerValue)
        thisSerializer.writeInt8(triggerValue_type as int32)
        if ((RuntimeType.UNDEFINED) != (triggerValue_type)) {
            const triggerValue_value  = triggerValue!
            thisSerializer.writeNumber(triggerValue_value)
        }
        ArkUIGeneratedNativeModule._SymbolGlyphAttribute_symbolEffect1(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkSymbolGlyphAttributes extends ArkCommonMethodAttributes {
    fontSize?: number | string | Resource
    fontColor?: Array<ResourceColor>
    fontWeight?: number | FontWeight | string
    effectStrategy?: SymbolEffectStrategy
    renderingStrategy?: SymbolRenderingStrategy
}
