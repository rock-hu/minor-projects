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
import { isInstanceOf } from "@koalaui/interop"
import { isResource, isPadding } from "../../utils"
import { ArkUIGeneratedNativeModule } from "../ArkUIGeneratedNativeModule"
import { ArkCommonMethodPeer, ArkCommonMethodAttributes } from "./ArkCommonPeer"
import { CommonMethod, Rectangle, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, UniformDataType, CustomBuilder, DragItemInfo, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback, DrawContext, TouchTestInfo, TouchResult, AdaptiveColor, BlurStyleActivePolicy, RepeatMode, LinearGradient_common, FinishCallbackType, ExpectedFrameRateRange, TransitionEffects, TransitionEdge, MotionBlurAnchor, LocalizedHorizontalAlignParam, LocalizedVerticalAlignParam, PreDragStatus, ShadowType, UIGestureEvent, BlurStyleOptions, ThemeColorMode, FractionStop, DragPreviewMode, ImageModifier, OverlayOffset, TransitionHierarchyStrategy, PopupMessageOptions, SymbolGlyphModifier, MenuPreviewMode, ContextMenuAnimationOptions, BindOptions, SheetSize, SheetType, SheetTitleOptions, SheetMode, ScrollSizeMode, UIContext, SheetKeyboardAvoidMode, SourceTool, RectResult, TouchTestStrategy, EventTarget, SourceType, TouchObject, HistoricalPoint, IntentionCode, DragBehavior, Summary, DragResult, DismissPopupAction, DismissContentCoverAction, SheetDismiss, DismissSheetAction, SpringBackAction, DismissReason } from "./../ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, PX, VP, FP, LPX, Percentage, Area, Bias, Font } from "./../ArkUnitsInterfaces"
import { DrawModifier } from "./../ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, PixelRoundCalcPolicy, Curve, PlayMode, TransitionType, HorizontalAlign, VerticalAlign, ClickEffectLevel, SharedTransitionEffectType, Placement, ArrowPointPosition, MouseButton, MouseAction, TouchType, KeyType, KeySource, FontWeight, FontStyle, AnimationStatus, FillMode } from "./../ArkEnumsInterfaces"
import { LengthMetrics } from "./../ArkLengthMetricsMaterialized"
import { ResizableOptions, DrawingLattice } from "./../ArkImageInterfaces"
import { Resource } from "./../ArkResourceInterfaces"
import { FocusBoxStyle, FocusPriority } from "./../ArkFocusInterfaces"
import { TransitionEffect } from "./../ArkTransitionEffectMaterialized"
import { CircleShape } from "./../ArkCircleShapeMaterialized"
import { EllipseShape } from "./../ArkEllipseShapeMaterialized"
import { PathShape } from "./../ArkPathShapeMaterialized"
import { RectShape } from "./../ArkRectShapeMaterialized"
import { ProgressMask } from "./../ArkProgressMaskMaterialized"
import { AttributeModifier } from "./../../handwritten"
import { GestureModifier } from "./../ArkGestureModifierMaterialized"
import { PixelMap } from "./../ArkPixelMapMaterialized"
import { GestureType, GestureMask, GestureInfo, GestureJudgeResult, GestureInterface, TapGestureParameters, PanDirection, SwipeDirection, GestureMode, GestureHandler, GesturePriority, FingerInfo } from "./../ArkGestureInterfaces"
import { TapGestureInterface } from "./../ArkTapGestureInterfaceMaterialized"
import { LongPressGestureInterface } from "./../ArkLongPressGestureInterfaceMaterialized"
import { PanGestureInterface } from "./../ArkPanGestureInterfaceMaterialized"
import { PinchGestureInterface } from "./../ArkPinchGestureInterfaceMaterialized"
import { SwipeGestureInterface } from "./../ArkSwipeGestureInterfaceMaterialized"
import { RotationGestureInterface } from "./../ArkRotationGestureInterfaceMaterialized"
import { GestureGroupInterface } from "./../ArkGestureGroupInterfaceMaterialized"
import { LengthUnit, ShapeSize, PathShapeOptions, RectShapeOptions, RoundRectShapeOptions } from "./../ArkArkuiExternalInterfaces"
import { ClickEvent } from "./../ArkClickEventMaterialized"
import { HoverEvent } from "./../ArkHoverEventMaterialized"
import { MouseEvent } from "./../ArkMouseEventMaterialized"
import { TouchEvent } from "./../ArkTouchEventMaterialized"
import { KeyEvent } from "./../ArkKeyEventMaterialized"
import { ColorMetrics } from "./../ArkColorMetricsMaterialized"
import { ICurve } from "./../ArkICurveMaterialized"
import { DragEvent } from "./../ArkDragEventMaterialized"
import { BaseGestureEvent } from "./../ArkBaseGestureEventMaterialized"
import { PanGestureOptions } from "./../ArkPanGestureOptionsMaterialized"
import { BaseEvent } from "./../ArkBaseEventMaterialized"
import { UnifiedData } from "./../ArkUnifiedDataMaterialized"
import { GestureControl } from "./../ArkGestureControlNamespace"
import { GestureEvent } from "./../ArkGestureEventMaterialized"
import { ImageFrameInfo, ImageAnimatorAttribute } from "./../ArkImageAnimatorInterfaces"
import { GestureName, GestureComponent } from "./../shared/generated-utils"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { unsafeCast } from "@koalaui/common"
import { Deserializer, createDeserializer } from "./Deserializer"
import { MaterializedBase, toPeerPtr, wrapCallback } from "@koalaui/interop"
import { DotIndicator } from "./../ArkDotIndicatorBuilder"
import { DigitIndicator } from "./../ArkDigitIndicatorBuilder"
import { SubTabBarStyle } from "./../ArkSubTabBarStyleBuilder"
import { BottomTabBarStyle } from "./../ArkBottomTabBarStyleBuilder"
export class ArkImageAnimatorPeer extends ArkCommonMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkImageAnimatorPeer {
        const peerId = PeerNode.nextId()
        const _peerPtr = ArkUIGeneratedNativeModule._ImageAnimator_construct(peerId, flags)
        const _peer = new ArkImageAnimatorPeer(_peerPtr, peerId, "ImageAnimator", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setImageAnimatorOptionsAttribute(): void {
        ArkUIGeneratedNativeModule._ImageAnimatorInterface_setImageAnimatorOptions(this.peer.ptr)
    }
    imagesAttribute(value: Array<ImageFrameInfo>): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.writeInt32(value.length)
        for (let i = 0; i < value.length; i++) {
            const value_element: ImageFrameInfo = value[i]
            thisSerializer.writeImageFrameInfo(value_element)
        }
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_images(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    stateAttribute(value: AnimationStatus): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_state(this.peer.ptr, value)
    }
    durationAttribute(value: number): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_duration(this.peer.ptr, value)
    }
    reverseAttribute(value: boolean): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_reverse(this.peer.ptr, +value)
    }
    fixedSizeAttribute(value: boolean): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_fixedSize(this.peer.ptr, +value)
    }
    preDecodeAttribute(value: number): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_preDecode(this.peer.ptr, value)
    }
    fillModeAttribute(value: FillMode): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_fillMode(this.peer.ptr, value)
    }
    iterationsAttribute(value: number): void {
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_iterations(this.peer.ptr, value)
    }
    onStartAttribute(value: (() => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_onStart(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    onPauseAttribute(value: (() => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_onPause(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    onRepeatAttribute(value: (() => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_onRepeat(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    onCancelAttribute(value: (() => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_onCancel(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    onFinishAttribute(value: (() => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._ImageAnimatorAttribute_onFinish(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkImageAnimatorAttributes extends ArkCommonMethodAttributes {
    images?: Array<ImageFrameInfo>
    state?: AnimationStatus
    duration?: number
    reverse?: boolean
    fixedSize?: boolean
    preDecode?: number
    fillMode?: FillMode
    iterations?: number
    onStart?: (() => void)
    onPause?: (() => void)
    onRepeat?: (() => void)
    onCancel?: (() => void)
    onFinish?: (() => void)
}
