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

import { ArkUIGeneratedNativeModule, TypeChecker } from "#components"
import { ComponentInfo, Offset_componentutils, TranslateResult, ScaleResult, RotateResult, Matrix4Result } from "./arkui-componentutils"
import { Size, Edges } from "./../Graphics"
import { Dimension, BorderRadiuses, Length } from "./units"
import { WrappedBuilder, Callback_WrappedBuilder_Args_Void, CommonConfiguration } from "./arkui-wrapper-builder"
import { Context, BaseContext, Context_getGroupDir_Callback, ContextInternal, PointerStyle } from "./arkui-custom"
import { Resource } from "global/resource"
import { AnimateParam, ICurve, FinishCallbackType, ExpectedFrameRateRange } from "./common"
import { Curve, PlayMode } from "./enums"
import { Profiler } from "./inspector"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, KPointer, toPeerPtr, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class GlobalScope {
    public static getRectangleById(id: string): ComponentInfo {
        const id_casted = id as (string)
        return GlobalScope.getRectangleById_serialize(id_casted)
    }
    static getRectangleById_serialize(id: string): ComponentInfo {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_getRectangleById(id)
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : ComponentInfo = retvalDeserializer.readComponentInfo()
        return returnResult
    }
    public static edgeColors(all: number): Edges<number> {
        const all_casted = all as (number)
        return GlobalScope.edgeColors_serialize(all_casted)
    }
    static edgeColors_serialize(all: number): Edges<number> {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_edgeColors(all)
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : Edges<number> = retvalDeserializer.readEdgesNumber()
        return returnResult
    }
    public static edgeWidths(all: number): Edges<number> {
        const all_casted = all as (number)
        return GlobalScope.edgeWidths_serialize(all_casted)
    }
    static edgeWidths_serialize(all: number): Edges<number> {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_edgeWidths(all)
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : Edges<number> = retvalDeserializer.readEdgesNumber()
        return returnResult
    }
    public static borderRadiuses(all: number): BorderRadiuses {
        const all_casted = all as (number)
        return GlobalScope.borderRadiuses_serialize(all_casted)
    }
    static borderRadiuses_serialize(all: number): BorderRadiuses {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_borderRadiuses(all)
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : BorderRadiuses = retvalDeserializer.readBorderRadiuses()
        return returnResult
    }
    public static wrapBuilder(builder: ((args: CommonConfiguration) => void)): WrappedBuilder {
        const builder_casted = builder as (((args: CommonConfiguration) => void))
        return GlobalScope.wrapBuilder_serialize(builder_casted)
    }
    static wrapBuilder_serialize(builder: ((args: CommonConfiguration) => void)): WrappedBuilder {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(builder)
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_wrapBuilder(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : WrappedBuilder = retvalDeserializer.readWrappedBuilder()
        return returnResult
    }
    public static getContext(component: Object | undefined): Context {
        const component_casted = component as (Object | undefined)
        return GlobalScope.getContext_serialize(component_casted)
    }
    static getContext_serialize(component: Object | undefined): Context {
        const thisSerializer : Serializer = Serializer.hold()
        let component_type : int32 = RuntimeType.UNDEFINED
        component_type = runtimeType(component)
        thisSerializer.writeInt8(component_type as int32)
        if ((RuntimeType.UNDEFINED) != (component_type)) {
            const component_value  = component!
            thisSerializer.holdAndWriteObject(component_value)
        }
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_getContext(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        const obj : Context = ContextInternal.fromPtr(retval)
        return obj
    }
    public static postCardAction(component: Object, action: Object): void {
        const component_casted = component as (Object)
        const action_casted = action as (Object)
        GlobalScope.postCardAction_serialize(component_casted, action_casted)
        return
    }
    static postCardAction_serialize(component: Object, action: Object): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteObject(component)
        thisSerializer.holdAndWriteObject(action)
        ArkUIGeneratedNativeModule._GlobalScope_postCardAction(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    public static dollar_r(value: string, params: Array<object>): Resource {
        const value_casted = value as (string)
        const params_casted = params as (Array<object>)
        return GlobalScope.dollar_r_serialize(value_casted, params_casted)
    }
    static dollar_r_serialize(value: string, params: Array<object>): Resource {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeInt32(params.length as int32)
        for (let i = 0; i < params.length; i++) {
            const params_element : object = params[i]
            thisSerializer.holdAndWriteObject(params_element)
        }
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_dollar_r(value, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : Resource = retvalDeserializer.readResource()
        return returnResult
    }
    public static dollar_rawfile(value: string): Resource {
        const value_casted = value as (string)
        return GlobalScope.dollar_rawfile_serialize(value_casted)
    }
    static dollar_rawfile_serialize(value: string): Resource {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_dollar_rawfile(value)
        let retvalDeserializer : Deserializer = new Deserializer(retval, retval.length as int32)
        const returnResult : Resource = retvalDeserializer.readResource()
        return returnResult
    }
    public static animateTo(value: AnimateParam, event: (() => void)): void {
        const value_casted = value as (AnimateParam)
        const event_casted = event as ((() => void))
        GlobalScope.animateTo_serialize(value_casted, event_casted)
        return
    }
    static animateTo_serialize(value: AnimateParam, event: (() => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeAnimateParam(value)
        thisSerializer.holdAndWriteCallback(event)
        ArkUIGeneratedNativeModule._GlobalScope_animateTo(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    public static animateToImmediately(value: AnimateParam, event: (() => void)): void {
        const value_casted = value as (AnimateParam)
        const event_casted = event as ((() => void))
        GlobalScope.animateToImmediately_serialize(value_casted, event_casted)
        return
    }
    static animateToImmediately_serialize(value: AnimateParam, event: (() => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeAnimateParam(value)
        thisSerializer.holdAndWriteCallback(event)
        ArkUIGeneratedNativeModule._GlobalScope_animateToImmediately(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    public static vp2px(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.vp2px_serialize(value_casted)
    }
    static vp2px_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_vp2px(value)
        return retval
    }
    public static px2vp(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.px2vp_serialize(value_casted)
    }
    static px2vp_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_px2vp(value)
        return retval
    }
    public static fp2px(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.fp2px_serialize(value_casted)
    }
    static fp2px_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_fp2px(value)
        return retval
    }
    public static px2fp(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.px2fp_serialize(value_casted)
    }
    static px2fp_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_px2fp(value)
        return retval
    }
    public static lpx2px(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.lpx2px_serialize(value_casted)
    }
    static lpx2px_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_lpx2px(value)
        return retval
    }
    public static px2lpx(value: number): number {
        const value_casted = value as (number)
        return GlobalScope.px2lpx_serialize(value_casted)
    }
    static px2lpx_serialize(value: number): number {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_px2lpx(value)
        return retval
    }
    public static getInspectorNodes(): Object {
        return GlobalScope.getInspectorNodes_serialize()
    }
    static getInspectorNodes_serialize(): Object {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_getInspectorNodes()
        throw new Error("Object deserialization is not implemented.")
    }
    public static getInspectorNodeById(id: number): Object {
        const id_casted = id as (number)
        return GlobalScope.getInspectorNodeById_serialize(id_casted)
    }
    static getInspectorNodeById_serialize(id: number): Object {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_getInspectorNodeById(id)
        throw new Error("Object deserialization is not implemented.")
    }
    public static setAppBgColor(value: string): void {
        const value_casted = value as (string)
        GlobalScope.setAppBgColor_serialize(value_casted)
        return
    }
    static setAppBgColor_serialize(value: string): void {
        ArkUIGeneratedNativeModule._GlobalScope_setAppBgColor(value)
    }
    public static Profiler_registerVsyncCallback(callback_: ((info: string) => void)): void {
        const callback__casted = callback_ as (((info: string) => void))
        GlobalScope.Profiler_registerVsyncCallback_serialize(callback__casted)
        return
    }
    static Profiler_registerVsyncCallback_serialize(callback_: ((info: string) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(callback_)
        ArkUIGeneratedNativeModule._GlobalScope_Profiler_registerVsyncCallback(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    public static Profiler_unregisterVsyncCallback(): void {
        GlobalScope.Profiler_unregisterVsyncCallback_serialize()
        return
    }
    static Profiler_unregisterVsyncCallback_serialize(): void {
        ArkUIGeneratedNativeModule._GlobalScope_Profiler_unregisterVsyncCallback()
    }
    public static cursorControl_setCursor(value: PointerStyle): void {
        const value_casted = value as (PointerStyle)
        GlobalScope.cursorControl_setCursor_serialize(value_casted)
        return
    }
    static cursorControl_setCursor_serialize(value: PointerStyle): void {
        ArkUIGeneratedNativeModule._GlobalScope_cursorControl_setCursor(TypeChecker.PointerStyle_ToNumeric(value))
    }
    public static cursorControl_restoreDefault(): void {
        GlobalScope.cursorControl_restoreDefault_serialize()
        return
    }
    static cursorControl_restoreDefault_serialize(): void {
        ArkUIGeneratedNativeModule._GlobalScope_cursorControl_restoreDefault()
    }
    public static focusControl_requestFocus(value: string): boolean {
        const value_casted = value as (string)
        return GlobalScope.focusControl_requestFocus_serialize(value_casted)
    }
    static focusControl_requestFocus_serialize(value: string): boolean {
        const retval  = ArkUIGeneratedNativeModule._GlobalScope_focusControl_requestFocus(value)
        return retval
    }
}
