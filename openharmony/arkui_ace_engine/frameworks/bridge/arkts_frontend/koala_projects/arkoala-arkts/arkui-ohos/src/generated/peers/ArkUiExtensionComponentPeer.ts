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
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey } from "./../../component/enums"
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
import { Callback_UIExtensionProxy_Void, UIExtensionProxy, ReceiveCallback, Callback_Literal_Number_code_Want_want_Void, Literal_Number_code_Want_want, UIExtensionOptions, UIExtensionComponentAttribute } from "./../../component/uiExtensionComponent"
import { Want, ErrorCallback } from "./../ArkArkuiExternalInterfaces"
import { Callback_Number_Void } from "./../../component/alphabetIndexer"
import { Callback_TerminationInfo_Void, TerminationInfo } from "./../../component/embeddedComponent"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { TypeChecker } from "#components"
import { MaterializedBase, toPeerPtr, wrapCallback } from "@koalaui/interop"
export class ArkUIExtensionComponentPeer extends ArkCommonMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkUIExtensionComponentPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._UIExtensionComponent_construct(peerId, flags)
        const _peer  = new ArkUIExtensionComponentPeer(_peerPtr, peerId, "UIExtensionComponent", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setUIExtensionComponentOptionsAttribute(want: Want, options?: UIExtensionOptions): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeWant(want)
        let options_type : int32 = RuntimeType.UNDEFINED
        options_type = runtimeType(options)
        thisSerializer.writeInt8(options_type as int32)
        if ((RuntimeType.UNDEFINED) != (options_type)) {
            const options_value  = options!
            thisSerializer.writeUIExtensionOptions(options_value)
        }
        ArkUIGeneratedNativeModule._UIExtensionComponentInterface_setUIExtensionComponentOptions(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onRemoteReadyAttribute(value: ((parameter: UIExtensionProxy) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onRemoteReady(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onReceiveAttribute(value: ReceiveCallback): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeReceiveCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onReceive(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onResultAttribute(value: ((parameter: Literal_Number_code_Want_want) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onResult(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onReleaseAttribute(value: ((index: number) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onRelease(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onErrorAttribute(value: ErrorCallback): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onError(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onTerminatedAttribute(value: ((parameter: TerminationInfo) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._UIExtensionComponentAttribute_onTerminated(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkUIExtensionComponentAttributes extends ArkCommonMethodAttributes {
    onRemoteReady?: ((parameter: UIExtensionProxy) => void)
    onReceive?: ReceiveCallback
    onResult?: ((parameter: Literal_Number_code_Want_want) => void)
    onRelease?: ((index: number) => void)
    onError?: ErrorCallback
    onTerminated?: ((parameter: TerminationInfo) => void)
}
