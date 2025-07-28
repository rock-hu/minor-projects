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

import { ImageAttachment, ImageAttachmentInternal } from "./ArkImageAttachmentMaterialized"
import { CustomSpan, CustomSpanInternal } from "./ArkCustomSpanMaterialized"
import { StyleOptions, StyledStringKey, SpanStyle, ImageAttachmentLayoutStyle, ImageAttachmentInterface, CustomSpanMeasureInfo, CustomSpanMetrics, CustomSpanDrawInfo, StyledStringValue } from "./ArkStyledStringInterfaces"
import { PixelMap, PixelMapInternal } from "./ArkPixelMapMaterialized"
import { SizeOptions, Length, Margin, Padding, BorderRadiuses } from "./ArkUnitsInterfaces"
import { ImageSpanAlignment, ImageFit } from "./ArkEnumsInterfaces"
import { DrawContext } from "./ArkCommonInterfaces"
import { LengthMetrics, LengthMetricsInternal } from "./ArkLengthMetricsMaterialized"
import { LengthUnit } from "./ArkArkuiExternalInterfaces"
import { Resource } from "./ArkResourceInterfaces"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, isInstanceOf } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { isResource, isPadding } from "./../utils"
import { Deserializer, createDeserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
import { ArkUIGeneratedNativeModule } from "./ArkUIGeneratedNativeModule"
export class StyledStringInternal {
    public static fromPtr(ptr: KPointer): StyledString {
        const obj: StyledString = new StyledString(undefined, undefined)
        obj.peer = new Finalizable(ptr, StyledString.getFinalizer())
        return obj
    }
}
export class StyledString implements MaterializedBase {
    peer?: Finalizable | undefined = undefined
    public getPeer(): Finalizable | undefined {
        return this.peer
    }
    get length(): number {
        return this.getLength()
    }
    static ctor_styledstring(value: string | ImageAttachment | CustomSpan, styles?: Array<StyleOptions>): KPointer {
        const thisSerializer: Serializer = Serializer.hold()
        let value_type: int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        if (RuntimeType.STRING == value_type) {
            thisSerializer.writeInt8(0)
            const value_0 = unsafeCast<string>(value)
            thisSerializer.writeString(value_0)
        }
        else if (((RuntimeType.OBJECT) == (value_type)) && (value instanceof ImageAttachment)) {
            thisSerializer.writeInt8(1)
            const value_1 = unsafeCast<ImageAttachment>(value)
            thisSerializer.writeImageAttachment(value_1)
        }
        else if (((RuntimeType.OBJECT) == (value_type)) && (value instanceof CustomSpan)) {
            thisSerializer.writeInt8(2)
            const value_2 = unsafeCast<CustomSpan>(value)
            thisSerializer.writeCustomSpan(value_2)
        }
        let styles_type: int32 = RuntimeType.UNDEFINED
        styles_type = runtimeType(styles)
        thisSerializer.writeInt8(styles_type)
        if ((RuntimeType.UNDEFINED) != (styles_type)) {
            const styles_value = styles!
            thisSerializer.writeInt32(styles_value.length)
            for (let i = 0; i < styles_value.length; i++) {
                const styles_value_element: StyleOptions = styles_value[i]
                thisSerializer.writeStyleOptions(styles_value_element)
            }
        }
        const retval = ArkUIGeneratedNativeModule._StyledString_ctor(thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
     constructor(value?: string | ImageAttachment | CustomSpan, styles?: Array<StyleOptions>) {
        if (((value) !== (undefined)) || ((styles) !== (undefined)))
        {
            const ctorPtr: KPointer = StyledString.ctor_styledstring((value)!, (styles)!)
            this.peer = new Finalizable(ctorPtr, StyledString.getFinalizer())
        }
    }
    static getFinalizer(): KPointer {
        return ArkUIGeneratedNativeModule._StyledString_getFinalizer()
    }
    public getString(): string {
        return this.getString_serialize()
    }
    public getStyles(start: number, length: number, styledKey?: StyledStringKey): Array<SpanStyle> {
        const start_casted = start as (number)
        const length_casted = length as (number)
        const styledKey_casted = styledKey as (StyledStringKey | undefined)
        return this.getStyles_serialize(start_casted, length_casted, styledKey_casted)
    }
    public equals(other: StyledString): boolean {
        const other_casted = other as (StyledString)
        return this.equals_serialize(other_casted)
    }
    public subStyledString(start: number, length?: number): StyledString {
        const start_casted = start as (number)
        const length_casted = length as (number | undefined)
        return this.subStyledString_serialize(start_casted, length_casted)
    }
    public static fromHtml(html: string): Promise<StyledString> {
        const html_casted = html as (string)
        return StyledString.fromHtml_serialize(html_casted)
    }
    public static toHtml(styledString: StyledString): string {
        const styledString_casted = styledString as (StyledString)
        return StyledString.toHtml_serialize(styledString_casted)
    }
    public static marshalling(styledString: StyledString): ArrayBuffer {
        const styledString_casted = styledString as (StyledString)
        return StyledString.marshalling_serialize(styledString_casted)
    }
    public static unmarshalling(buffer: ArrayBuffer): Promise<StyledString> {
        const buffer_casted = buffer as (ArrayBuffer)
        return StyledString.unmarshalling_serialize(buffer_casted)
    }
    private getLength(): number {
        return this.getLength_serialize()
    }
    private getString_serialize(): string {
        const retval = ArkUIGeneratedNativeModule._StyledString_getString(this.peer!.ptr)
        return retval
    }
    private getStyles_serialize(start: number, length: number, styledKey?: StyledStringKey): Array<SpanStyle> {
        const thisSerializer: Serializer = Serializer.hold()
        let styledKey_type: int32 = RuntimeType.UNDEFINED
        styledKey_type = runtimeType(styledKey)
        thisSerializer.writeInt8(styledKey_type)
        if ((RuntimeType.UNDEFINED) != (styledKey_type)) {
            const styledKey_value = styledKey!
            thisSerializer.writeInt32(styledKey_value)
        }
        const retval = ArkUIGeneratedNativeModule._StyledString_getStyles(this.peer!.ptr, start, length, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
        let retvalDeserializer: Deserializer = new Deserializer(retval.buffer, retval.byteLength)
        const buffer_length: int32 = retvalDeserializer.readInt32()
        let buffer: Array<SpanStyle> = new Array<SpanStyle>(buffer_length)
        for (let buffer_i = 0; buffer_i < buffer_length; buffer_i++) {
            buffer[buffer_i] = retvalDeserializer.readSpanStyle()
        }
        const returnResult: Array<SpanStyle> = buffer
        return returnResult
    }
    private equals_serialize(other: StyledString): boolean {
        const retval = ArkUIGeneratedNativeModule._StyledString_equals(this.peer!.ptr, toPeerPtr(other))
        return retval
    }
    private subStyledString_serialize(start: number, length?: number): StyledString {
        const thisSerializer: Serializer = Serializer.hold()
        let length_type: int32 = RuntimeType.UNDEFINED
        length_type = runtimeType(length)
        thisSerializer.writeInt8(length_type)
        if ((RuntimeType.UNDEFINED) != (length_type)) {
            const length_value = length!
            thisSerializer.writeNumber(length_value)
        }
        const retval = ArkUIGeneratedNativeModule._StyledString_subStyledString(this.peer!.ptr, start, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
        const obj: StyledString = StyledStringInternal.fromPtr(retval)
        return obj
    }
    private static fromHtml_serialize(html: string): Promise<StyledString> {
        const thisSerializer: Serializer = Serializer.hold()
        const retval = thisSerializer.holdAndWriteCallbackForPromise<StyledString>()[0]
        ArkUIGeneratedNativeModule._StyledString_fromHtml(html, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
    private static toHtml_serialize(styledString: StyledString): string {
        const retval = ArkUIGeneratedNativeModule._StyledString_toHtml(toPeerPtr(styledString))
        return retval
    }
    private static marshalling_serialize(styledString: StyledString): ArrayBuffer {
        const retval = ArkUIGeneratedNativeModule._StyledString_marshalling(toPeerPtr(styledString))
        return new Deserializer(retval.buffer, retval.byteLength).readBuffer()
    }
    private static unmarshalling_serialize(buffer: ArrayBuffer): Promise<StyledString> {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.writeBuffer(buffer)
        const retval = thisSerializer.holdAndWriteCallbackForPromise<StyledString>()[0]
        ArkUIGeneratedNativeModule._StyledString_unmarshalling(thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
    private getLength_serialize(): number {
        const retval = ArkUIGeneratedNativeModule._StyledString_getLength(this.peer!.ptr)
        return retval
    }
}
