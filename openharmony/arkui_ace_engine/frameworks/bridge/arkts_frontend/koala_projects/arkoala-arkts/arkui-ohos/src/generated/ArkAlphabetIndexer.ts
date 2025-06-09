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
import { ArkAlphabetIndexerPeer } from "./peers/ArkAlphabetIndexerPeer"
import { CommonMethod, DrawModifier, Rectangle, Callback_Array_TouchTestInfo_TouchResult, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, Callback_ClickEvent_Void, ClickEvent, Callback_Boolean_HoverEvent_Void, HoverEvent, AccessibilityCallback, Callback_MouseEvent_Void, MouseEvent, Callback_TouchEvent_Void, TouchEvent, Callback_KeyEvent_Void, KeyEvent, Callback_KeyEvent_Boolean, AnimateParam, TransitionOptions, TransitionEffect, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, DragEvent, CustomBuilder, DragItemInfo, Callback_DragEvent_String_Void, UniformDataType, Callback_PreDragStatus_Void, PreDragStatus, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, MotionPathOptions, ShadowOptions, ShadowStyle, ProgressMask, StateStyles, PixelStretchEffectOptions, GestureModifier, BackgroundBrightnessOptions, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, Callback_TouchEvent_HitTestMode, SizeChangeCallback, SafeAreaType, SafeAreaEdge, Literal_Alignment_align, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./../component/common"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, Font } from "./../component/units"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey } from "./../component/enums"
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
import { Callback_Number_Void, OnAlphabetIndexerSelectCallback, OnAlphabetIndexerRequestPopupDataCallback, OnAlphabetIndexerPopupSelectCallback, IndexerAlign, AlphabetIndexerAttribute, AlphabetIndexerOptions } from "./../component/alphabetIndexer"
/** @memo:stable */
export class ArkAlphabetIndexerComponent extends ArkCommonMethodComponent implements AlphabetIndexerAttribute {
    getPeer(): ArkAlphabetIndexerPeer {
        return (this.peer as ArkAlphabetIndexerPeer)
    }
    /** @memo */
    public setAlphabetIndexerOptions(options: AlphabetIndexerOptions): this {
        if (this.checkPriority("setAlphabetIndexerOptions")) {
            const options_casted = options as (AlphabetIndexerOptions)
            this.getPeer()?.setAlphabetIndexerOptionsAttribute(options_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onSelected(value: ((index: number) => void)): this {
        if (this.checkPriority("onSelected")) {
            const value_casted = value as (((index: number) => void))
            this.getPeer()?.onSelectedAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public color(value: ResourceColor): this {
        if (this.checkPriority("color")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.colorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public selectedColor(value: ResourceColor): this {
        if (this.checkPriority("selectedColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.selectedColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupColor(value: ResourceColor): this {
        if (this.checkPriority("popupColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public selectedBackgroundColor(value: ResourceColor): this {
        if (this.checkPriority("selectedBackgroundColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.selectedBackgroundColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupBackground(value: ResourceColor): this {
        if (this.checkPriority("popupBackground")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupBackgroundAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupSelectedColor(value: ResourceColor): this {
        if (this.checkPriority("popupSelectedColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupSelectedColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupUnselectedColor(value: ResourceColor): this {
        if (this.checkPriority("popupUnselectedColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupUnselectedColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupItemBackgroundColor(value: ResourceColor): this {
        if (this.checkPriority("popupItemBackgroundColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupItemBackgroundColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public usingPopup(value: boolean): this {
        if (this.checkPriority("usingPopup")) {
            const value_casted = value as (boolean)
            this.getPeer()?.usingPopupAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public selectedFont(value: Font): this {
        if (this.checkPriority("selectedFont")) {
            const value_casted = value as (Font)
            this.getPeer()?.selectedFontAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupFont(value: Font): this {
        if (this.checkPriority("popupFont")) {
            const value_casted = value as (Font)
            this.getPeer()?.popupFontAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupItemFont(value: Font): this {
        if (this.checkPriority("popupItemFont")) {
            const value_casted = value as (Font)
            this.getPeer()?.popupItemFontAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public itemSize(value: string | number): this {
        if (this.checkPriority("itemSize")) {
            const value_casted = value as (string | number)
            this.getPeer()?.itemSizeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public font(value: Font): this {
        if (this.checkPriority("font")) {
            const value_casted = value as (Font)
            this.getPeer()?.fontAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onSelect(value: OnAlphabetIndexerSelectCallback): this {
        if (this.checkPriority("onSelect")) {
            const value_casted = value as (OnAlphabetIndexerSelectCallback)
            this.getPeer()?.onSelectAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onRequestPopupData(value: OnAlphabetIndexerRequestPopupDataCallback): this {
        if (this.checkPriority("onRequestPopupData")) {
            const value_casted = value as (OnAlphabetIndexerRequestPopupDataCallback)
            this.getPeer()?.onRequestPopupDataAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onPopupSelect(value: OnAlphabetIndexerPopupSelectCallback): this {
        if (this.checkPriority("onPopupSelect")) {
            const value_casted = value as (OnAlphabetIndexerPopupSelectCallback)
            this.getPeer()?.onPopupSelectAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public selected(value: number): this {
        if (this.checkPriority("selected")) {
            const value_casted = value as (number)
            this.getPeer()?.selectedAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupPosition(value: Position): this {
        if (this.checkPriority("popupPosition")) {
            const value_casted = value as (Position)
            this.getPeer()?.popupPositionAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public autoCollapse(value: boolean): this {
        if (this.checkPriority("autoCollapse")) {
            const value_casted = value as (boolean)
            this.getPeer()?.autoCollapseAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupItemBorderRadius(value: number): this {
        if (this.checkPriority("popupItemBorderRadius")) {
            const value_casted = value as (number)
            this.getPeer()?.popupItemBorderRadiusAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public itemBorderRadius(value: number): this {
        if (this.checkPriority("itemBorderRadius")) {
            const value_casted = value as (number)
            this.getPeer()?.itemBorderRadiusAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupBackgroundBlurStyle(value: BlurStyle): this {
        if (this.checkPriority("popupBackgroundBlurStyle")) {
            const value_casted = value as (BlurStyle)
            this.getPeer()?.popupBackgroundBlurStyleAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public popupTitleBackground(value: ResourceColor): this {
        if (this.checkPriority("popupTitleBackground")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.popupTitleBackgroundAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public enableHapticFeedback(value: boolean): this {
        if (this.checkPriority("enableHapticFeedback")) {
            const value_casted = value as (boolean)
            this.getPeer()?.enableHapticFeedbackAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public alignStyle(value: IndexerAlign, offset?: Length): this {
        if (this.checkPriority("alignStyle")) {
            const value_casted = value as (IndexerAlign)
            const offset_casted = offset as (Length | undefined)
            this.getPeer()?.alignStyleAttribute(value_casted, offset_casted)
            return this
        }
        return this
    }
    /** @memo */
    public _onChangeEvent_selected(callback: ((index: number) => void)): void {
        if (this.checkPriority("_onChangeEvent_selected")) {
            const callback_casted = callback as (((index: number) => void))
            this.getPeer()?._onChangeEvent_selectedAttribute(callback_casted)
            return
        }
        return
    }
    public applyAttributesFinish(): void {
        // we calls this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}
