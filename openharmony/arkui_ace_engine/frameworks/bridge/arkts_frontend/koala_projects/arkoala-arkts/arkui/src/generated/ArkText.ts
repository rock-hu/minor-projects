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
import { ArkTextPeer } from "./peers/ArkTextPeer"
import { CommonMethod, Rectangle, TouchTestInfo, TouchResult, PixelRoundPolicy, BackgroundEffectOptions, ForegroundEffectOptions, VisualEffect, Filter, BorderImageOption, OutlineStyle, AccessibilityCallback, AnimateParam, TransitionOptions, MotionBlurOptions, InvertOptions, TranslateOptions, ScaleOptions, RotateOptions, AlignRuleOption, LocalizedAlignRuleOptions, ClickEffect, CustomBuilder, DragItemInfo, UniformDataType, PreDragStatus, MotionPathOptions, ShadowOptions, ShadowStyle, StateStyles, PixelStretchEffectOptions, BackgroundBrightnessOptions, GestureRecognizerJudgeBeginCallback, ShouldBuiltInRecognizerParallelWithCallback, SizeChangeCallback, SafeAreaType, SafeAreaEdge, BlurStyle, BackgroundBlurStyleOptions, ForegroundBlurStyleOptions, TransitionFinishCallback, BlurOptions, LinearGradientBlurOptions, EffectType, sharedTransitionOptions, ChainStyle, DragPreviewOptions, DragInteractionOptions, ComponentContent, OverlayOptions, BlendMode, BlendApplyType, Blender, GeometryTransitionOptions, PopupOptions, CustomPopupOptions, MenuElement, MenuOptions, ContextMenuOptions, ModalTransition, ContentCoverOptions, SheetOptions, VisibleAreaChangeCallback } from "./ArkCommonInterfaces"
import { Length, SizeOptions, ConstraintSizeOptions, ChainWeightOptions, Padding, LocalizedPadding, Margin, LocalizedMargin, ResourceColor, Position, BorderOptions, EdgeStyles, EdgeWidths, LocalizedEdgeWidths, EdgeColors, LocalizedEdgeColors, BorderRadiuses, LocalizedBorderRadiuses, OutlineOptions, EdgeOutlineStyles, Dimension, EdgeOutlineWidths, OutlineRadiuses, Area, Edges, LocalizedEdges, LocalizedPosition, ResourceStr, AccessibilityOptions, Font } from "./ArkUnitsInterfaces"
import { DrawModifier } from "./ArkDrawModifierMaterialized"
import { HitTestMode, ImageSize, Alignment, BorderStyle, ColoringStrategy, HoverEffect, Color, Visibility, ItemAlign, Direction, GradientDirection, ObscuredReasons, RenderFit, ImageRepeat, Axis, ResponseType, FunctionKey, ModifierKey, FontStyle, FontWeight, TextAlign, TextCase, CopyOptions, TextHeightAdaptivePolicy, WordBreak, LineBreakStrategy, EllipsisMode, TextSelectableMode } from "./ArkEnumsInterfaces"
import { Callback_Array_TouchTestInfo_TouchResult, Callback_ClickEvent_Void, Callback_Boolean_HoverEvent_Void, Callback_MouseEvent_Void, Callback_TouchEvent_Void, Callback_KeyEvent_Void, Callback_KeyEvent_Boolean, Callback_Void, Callback_Area_Area_Void, Literal_Union_Number_Literal_Number_offset_span_lg_md_sm_xs, Literal_Number_offset_span, Callback_DragEvent_String_Union_CustomBuilder_DragItemInfo, Callback_DragEvent_String_Void, Callback_PreDragStatus_Void, Type_CommonMethod_linearGradient_value, Tuple_ResourceColor_Number, Type_CommonMethod_sweepGradient_value, Tuple_Length_Length, Type_CommonMethod_radialGradient_value, Callback_GestureInfo_BaseGestureEvent_GestureJudgeResult, Callback_TouchEvent_HitTestMode, Literal_Alignment_align, Callback_String_Void, Callback_Number_Number_Void } from "./SyntheticDeclarations"
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
import { TextOverflowOptions, TextAttribute, TextSpanType, TextResponseType, TextOptions } from "./ArkTextInterfaces"
import { DecorationStyleInterface } from "./ArkStyledStringInterfaces"
import { TextDataDetectorConfig, FontSettingOptions } from "./ArkTextCommonInterfaces"
import { EditMenuOptions } from "./ArkEditMenuOptionsMaterialized"
import { SelectionMenuOptions } from "./ArkRichEditorInterfaces"
import { TextInterface } from "./ArkTextInterfaces"
import { AttributeUpdater } from "../handwritten/modifiers/ArkAttributeUpdater"
import { ArkTextNode } from "../handwritten/modifiers/ArkTextNode"
import { ArkTextAttributeSet, TextModifier } from "../handwritten/modifiers/ArkTextModifier"
import { applyUIAttributes } from "../handwritten/modifiers/ArkCommonModifier"
/** @memo:stable */
export class ArkTextComponent extends ArkCommonMethodComponent {
    getPeer(): ArkTextPeer {
        return (this.peer as ArkTextPeer)
    }

    getModifierHost(): ArkTextNode {
        if (this._modifierHost === undefined || this._modifierHost === null) {
            this._modifierHost = new ArkTextNode()
            this._modifierHost!.setPeer(this.getPeer())
        }
        return this._modifierHost as ArkTextNode
    }

    /** @memo */
    public setTextOptions(content?: string | Resource, value?: TextOptions): this {
        if (this.checkPriority("setTextOptions")) {
            const content_casted = content as (string | Resource | undefined)
            const value_casted = value as (TextOptions | undefined)
            this.getPeer()?.setTextOptionsAttribute(content_casted, value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public font(fontValue: Font, options?: FontSettingOptions): this {
        if (this.checkPriority("font")) {
            const fontValue_type = runtimeType(fontValue)
            const options_type = runtimeType(options)
            if (RuntimeType.UNDEFINED == options_type) {
                const value_casted = fontValue as (Font)
                this.getPeer()?.font0Attribute(value_casted)
                return this
            }
            if ((RuntimeType.OBJECT == options_type) || (RuntimeType.UNDEFINED == options_type)) {
                const fontValue_casted = fontValue as (Font)
                const options_casted = options as (FontSettingOptions | undefined)
                this.getPeer()?.font1Attribute(fontValue_casted, options_casted)
                return this
            }
            throw new Error("Can not select appropriate overload")
        }
        return this
    }
    /** @memo */
    public fontColor(value: ResourceColor): this {
        if (this.checkPriority("fontColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.fontColorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fontSize(value: number | string | Resource): this {
        if (this.checkPriority("fontSize")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.fontSizeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public minFontSize(value: number | string | Resource): this {
        if (this.checkPriority("minFontSize")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.minFontSizeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public maxFontSize(value: number | string | Resource): this {
        if (this.checkPriority("maxFontSize")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.maxFontSizeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public minFontScale(value: number | Resource): this {
        if (this.checkPriority("minFontScale")) {
            const value_casted = value as (number | Resource)
            this.getPeer()?.minFontScaleAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public maxFontScale(value: number | Resource): this {
        if (this.checkPriority("maxFontScale")) {
            const value_casted = value as (number | Resource)
            this.getPeer()?.maxFontScaleAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fontStyle(value: FontStyle): this {
        if (this.checkPriority("fontStyle")) {
            const value_casted = value as (FontStyle)
            this.getPeer()?.fontStyleAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fontWeight(weight: number | FontWeight | string, options?: FontSettingOptions): this {
        if (this.checkPriority("fontWeight")) {
            const weight_type = runtimeType(weight)
            const options_type = runtimeType(options)
            if (((RuntimeType.NUMBER == weight_type) || (TypeChecker.isFontWeight(weight)) || (RuntimeType.STRING == weight_type)) && (RuntimeType.UNDEFINED == options_type)) {
                const value_casted = weight as (number | FontWeight | string)
                this.getPeer()?.fontWeight0Attribute(value_casted)
                return this
            }
            if (((RuntimeType.NUMBER == weight_type) || (TypeChecker.isFontWeight(weight)) || (RuntimeType.STRING == weight_type)) && ((RuntimeType.OBJECT == options_type) || (RuntimeType.UNDEFINED == options_type))) {
                const weight_casted = weight as (number | FontWeight | string)
                const options_casted = options as (FontSettingOptions | undefined)
                this.getPeer()?.fontWeight1Attribute(weight_casted, options_casted)
                return this
            }
            throw new Error("Can not select appropriate overload")
        }
        return this
    }
    /** @memo */
    public lineSpacing(value: LengthMetrics): this {
        if (this.checkPriority("lineSpacing")) {
            const value_casted = value as (LengthMetrics)
            this.getPeer()?.lineSpacingAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textAlign(value: TextAlign): this {
        if (this.checkPriority("textAlign")) {
            const value_casted = value as (TextAlign)
            this.getPeer()?.textAlignAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public lineHeight(value: number | string | Resource): this {
        if (this.checkPriority("lineHeight")) {
            const value_casted = value as (number | string | Resource)
            this.getPeer()?.lineHeightAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textOverflow(value: TextOverflowOptions): this {
        if (this.checkPriority("textOverflow")) {
            const value_casted = value as (TextOverflowOptions)
            this.getPeer()?.textOverflowAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fontFamily(value: string | Resource): this {
        if (this.checkPriority("fontFamily")) {
            const value_casted = value as (string | Resource)
            this.getPeer()?.fontFamilyAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public maxLines(value: number): this {
        if (this.checkPriority("maxLines")) {
            const value_casted = value as (number)
            this.getPeer()?.maxLinesAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public decoration(value: DecorationStyleInterface): this {
        if (this.checkPriority("decoration")) {
            const value_casted = value as (DecorationStyleInterface)
            this.getPeer()?.decorationAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public letterSpacing(value: number | string): this {
        if (this.checkPriority("letterSpacing")) {
            const value_casted = value as (number | string)
            this.getPeer()?.letterSpacingAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textCase(value: TextCase): this {
        if (this.checkPriority("textCase")) {
            const value_casted = value as (TextCase)
            this.getPeer()?.textCaseAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public baselineOffset(value: number | string): this {
        if (this.checkPriority("baselineOffset")) {
            const value_casted = value as (number | string)
            this.getPeer()?.baselineOffsetAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public copyOption(value: CopyOptions): this {
        if (this.checkPriority("copyOption")) {
            const value_casted = value as (CopyOptions)
            this.getPeer()?.copyOptionAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public draggable(value: boolean): this {
        if (this.checkPriority("draggable")) {
            const value_casted = value as (boolean)
            this.getPeer()?.draggableAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textShadow(value: ShadowOptions | Array<ShadowOptions>): this {
        if (this.checkPriority("textShadow")) {
            const value_casted = value as (ShadowOptions | Array<ShadowOptions>)
            this.getPeer()?.textShadowAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public heightAdaptivePolicy(value: TextHeightAdaptivePolicy): this {
        if (this.checkPriority("heightAdaptivePolicy")) {
            const value_casted = value as (TextHeightAdaptivePolicy)
            this.getPeer()?.heightAdaptivePolicyAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textIndent(value: Length): this {
        if (this.checkPriority("textIndent")) {
            const value_casted = value as (Length)
            this.getPeer()?.textIndentAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public wordBreak(value: WordBreak): this {
        if (this.checkPriority("wordBreak")) {
            const value_casted = value as (WordBreak)
            this.getPeer()?.wordBreakAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public lineBreakStrategy(value: LineBreakStrategy): this {
        if (this.checkPriority("lineBreakStrategy")) {
            const value_casted = value as (LineBreakStrategy)
            this.getPeer()?.lineBreakStrategyAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onCopy(value: ((breakpoints: string) => void)): this {
        if (this.checkPriority("onCopy")) {
            const value_casted = value as (((breakpoints: string) => void))
            this.getPeer()?.onCopyAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public caretColor(value: ResourceColor): this {
        if (this.checkPriority("caretColor")) {
            const value_casted = value as (ResourceColor)
            this.getPeer()?.caretColorAttribute(value_casted)
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
    public ellipsisMode(value: EllipsisMode): this {
        if (this.checkPriority("ellipsisMode")) {
            const value_casted = value as (EllipsisMode)
            this.getPeer()?.ellipsisModeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public enableDataDetector(value: boolean): this {
        if (this.checkPriority("enableDataDetector")) {
            const value_casted = value as (boolean)
            this.getPeer()?.enableDataDetectorAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public dataDetectorConfig(value: TextDataDetectorConfig): this {
        if (this.checkPriority("dataDetectorConfig")) {
            const value_casted = value as (TextDataDetectorConfig)
            this.getPeer()?.dataDetectorConfigAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public onTextSelectionChange(value: ((first: number,last: number) => void)): this {
        if (this.checkPriority("onTextSelectionChange")) {
            const value_casted = value as (((first: number,last: number) => void))
            this.getPeer()?.onTextSelectionChangeAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public fontFeature(value: string): this {
        if (this.checkPriority("fontFeature")) {
            const value_casted = value as (string)
            this.getPeer()?.fontFeatureAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public privacySensitive(value: boolean): this {
        if (this.checkPriority("privacySensitive")) {
            const value_casted = value as (boolean)
            this.getPeer()?.privacySensitiveAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public textSelectable(value: TextSelectableMode): this {
        if (this.checkPriority("textSelectable")) {
            const value_casted = value as (TextSelectableMode)
            this.getPeer()?.textSelectableAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public editMenuOptions(value: EditMenuOptions): this {
        if (this.checkPriority("editMenuOptions")) {
            const value_casted = value as (EditMenuOptions)
            this.getPeer()?.editMenuOptionsAttribute(value_casted)
            return this
        }
        return this
    }
    /** @memo */
    public halfLeading(value: boolean): this {
        if (this.checkPriority("halfLeading")) {
            const value_casted = value as (boolean)
            this.getPeer()?.halfLeadingAttribute(value_casted)
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
    public selection(selectionStart: number, selectionEnd: number): this {
        if (this.checkPriority("selection")) {
            const selectionStart_casted = selectionStart as (number)
            const selectionEnd_casted = selectionEnd as (number)
            this.getPeer()?.selectionAttribute(selectionStart_casted, selectionEnd_casted)
            return this
        }
        return this
    }
    /** @memo */
    public bindSelectionMenu(spanType: TextSpanType, content: CustomBuilder, responseType: TextResponseType, options?: SelectionMenuOptions): this {
        if (this.checkPriority("bindSelectionMenu")) {
            const spanType_casted = spanType as (TextSpanType)
            const content_casted = content as (CustomBuilder)
            const responseType_casted = responseType as (TextResponseType)
            const options_casted = options as (SelectionMenuOptions | undefined)
            this.getPeer()?.bindSelectionMenuAttribute(spanType_casted, content_casted, responseType_casted, options_casted)
            return this
        }
        return this
    }
    attributeModifier(modifier: AttributeModifier<TextAttribute>): this {
        let peerNode = this.getPeer()
        if (!peerNode._attributeSet) {
            peerNode._attributeSet = new ArkTextAttributeSet()
        }
        applyUIAttributes(modifier, peerNode);
        let isAttributeUpdater: boolean = (modifier instanceof AttributeUpdater);
        if (isAttributeUpdater) {
            let attributeUpdater = modifier as AttributeUpdater<TextAttribute, (content?: string | Resource, value?: TextOptions) => TextAttribute>
            attributeUpdater.initializeModifier(peerNode._attributeSet as object as TextAttribute);
            attributeUpdater.attribute = this.getModifierHost()
            attributeUpdater.updateConstructorParams = (content?: string | Resource, value?: TextOptions) : TextAttribute => {
                const content_casted = content as (string | Resource | undefined)
                const value_casted = value as (TextOptions | undefined)
                this.getPeer()?.setTextOptionsAttribute(content_casted, value_casted)
                return this.getModifierHost()! as TextAttribute
            };
            peerNode._attributeSet!.applyModifierPatch(peerNode);
        }
        let isAttributeset = (modifier instanceof ArkTextAttributeSet);
        if (isAttributeset) {
            let attributeSet: ArkTextAttributeSet = (modifier as object) as ArkTextAttributeSet;
            attributeSet.applyModifierPatch(peerNode);
        }
        return this
    }
    public applyAttributesFinish(): void {
        // we calls this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}

/** @memo */
export function ArkText(
  /** @memo */
  style: ((attributes: ArkTextComponent) => void) | undefined,
  /** @memo */
  content_: (() => void) | undefined,
  content?: string | Resource | undefined, value?: TextOptions | undefined
) {
    const receiver = remember(() => {
        return new ArkTextComponent()
    })
    NodeAttach<ArkTextPeer>((): ArkTextPeer => ArkTextPeer.create(receiver), (_: ArkTextPeer) => {
        receiver.setTextOptions(content,value)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
