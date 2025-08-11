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
import { nullptr, KPointer, KInt, KBoolean, KStringPtr, runtimeType, RuntimeType, MaterializedBase, toPeerPtr, wrapCallback, NativeBuffer } from "@koalaui/interop"
import { Serializer } from "./peers/Serializer"
import { ComponentBase } from "./../ComponentBase"
import { PeerNode } from "./../PeerNode"
import { ArkUIGeneratedNativeModule, TypeChecker } from "#components"
import { ArkCommonMethodPeer, CommonMethod, ArkCommonMethodComponent, ArkCommonMethodStyle } from "./common"
import { ResourceColor } from "./units"
import { LineCapStyle, Color } from "./enums"
import { Resource } from "global.resource"
import { CallbackKind } from "./peers/CallbackKind"
import { CallbackTransformer } from "./peers/CallbackTransformer"
import { NodeAttach, remember } from "@koalaui/runtime"

export class ArkDividerPeer extends ArkCommonMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component: ComponentBase | undefined, flags: int32 = 0): ArkDividerPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._Divider_construct(peerId, flags)
        const _peer  = new ArkDividerPeer(_peerPtr, peerId, "Divider", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setDividerOptionsAttribute(): void {
        ArkUIGeneratedNativeModule._DividerInterface_setDividerOptions(this.peer.ptr)
    }
    verticalAttribute(value: boolean | undefined): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        thisSerializer.writeInt8(value_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_type)) {
            const value_value  = value!
            thisSerializer.writeBoolean(value_value)
        }
        ArkUIGeneratedNativeModule._DividerAttribute_vertical(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    colorAttribute(value: ResourceColor | undefined): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        thisSerializer.writeInt8(value_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_type)) {
            const value_value  = value!
            let value_value_type : int32 = RuntimeType.UNDEFINED
            value_value_type = runtimeType(value_value)
            if (TypeChecker.isColor(value_value)) {
                thisSerializer.writeInt8(0 as int32)
                const value_value_0  = value_value as Color
                thisSerializer.writeInt32(TypeChecker.Color_ToNumeric(value_value_0))
            }
            else if (RuntimeType.NUMBER == value_value_type) {
                thisSerializer.writeInt8(1 as int32)
                const value_value_1  = value_value as number
                thisSerializer.writeNumber(value_value_1)
            }
            else if (RuntimeType.STRING == value_value_type) {
                thisSerializer.writeInt8(2 as int32)
                const value_value_2  = value_value as string
                thisSerializer.writeString(value_value_2)
            }
            else if (RuntimeType.OBJECT == value_value_type) {
                thisSerializer.writeInt8(3 as int32)
                const value_value_3  = value_value as Resource
                thisSerializer.writeResource(value_value_3)
            }
        }
        ArkUIGeneratedNativeModule._DividerAttribute_color(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    strokeWidthAttribute(value: number | string | undefined): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        thisSerializer.writeInt8(value_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_type)) {
            const value_value  = value!
            let value_value_type : int32 = RuntimeType.UNDEFINED
            value_value_type = runtimeType(value_value)
            if (RuntimeType.NUMBER == value_value_type) {
                thisSerializer.writeInt8(0 as int32)
                const value_value_0  = value_value as number
                thisSerializer.writeNumber(value_value_0)
            }
            else if (RuntimeType.STRING == value_value_type) {
                thisSerializer.writeInt8(1 as int32)
                const value_value_1  = value_value as string
                thisSerializer.writeString(value_value_1)
            }
        }
        ArkUIGeneratedNativeModule._DividerAttribute_strokeWidth(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    lineCapAttribute(value: LineCapStyle | undefined): void {
        const thisSerializer : Serializer = Serializer.hold()
        let value_type : int32 = RuntimeType.UNDEFINED
        value_type = runtimeType(value)
        thisSerializer.writeInt8(value_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_type)) {
            const value_value  = (value as LineCapStyle)
            thisSerializer.writeInt32(TypeChecker.LineCapStyle_ToNumeric(value_value))
        }
        ArkUIGeneratedNativeModule._DividerAttribute_lineCap(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
export type DividerInterface = () => DividerAttribute;
export interface DividerAttribute extends CommonMethod {
    vertical(value: boolean | undefined): this
    color(value: ResourceColor | undefined): this
    strokeWidth(value: number | string | undefined): this
    lineCap(value: LineCapStyle | undefined): this
}
export class ArkDividerStyle extends ArkCommonMethodStyle implements DividerAttribute {
    vertical_value?: boolean | undefined
    color_value?: ResourceColor | undefined
    strokeWidth_value?: number | string | undefined
    lineCap_value?: LineCapStyle | undefined
    public vertical(value: boolean | undefined): this {
        return this
    }
    public color(value: ResourceColor | undefined): this {
        return this
    }
    public strokeWidth(value: number | string | undefined): this {
        return this
    }
    public lineCap(value: LineCapStyle | undefined): this {
        return this
        }
}
export class ArkDividerComponent extends ArkCommonMethodComponent implements DividerAttribute {
    getPeer(): ArkDividerPeer {
        return (this.peer as ArkDividerPeer)
    }
    public setDividerOptions(): this {
        if (this.checkPriority("setDividerOptions")) {
            this.getPeer()?.setDividerOptionsAttribute()
            return this
        }
        return this
    }
    public vertical(value: boolean | undefined): this {
        if (this.checkPriority("vertical")) {
            const value_casted = value as (boolean | undefined)
            this.getPeer()?.verticalAttribute(value_casted)
            return this
        }
        return this
    }
    public color(value: ResourceColor | undefined): this {
        if (this.checkPriority("color")) {
            const value_casted = value as (ResourceColor | undefined)
            this.getPeer()?.colorAttribute(value_casted)
            return this
        }
        return this
    }
    public strokeWidth(value: number | string | undefined): this {
        if (this.checkPriority("strokeWidth")) {
            const value_casted = value as (number | string | undefined)
            this.getPeer()?.strokeWidthAttribute(value_casted)
            return this
        }
        return this
    }
    public lineCap(value: LineCapStyle | undefined): this {
        if (this.checkPriority("lineCap")) {
            const value_casted = value as (LineCapStyle | undefined)
            this.getPeer()?.lineCapAttribute(value_casted)
            return this
        }
        return this
    }
    
    public applyAttributesFinish(): void {
        // we call this function outside of class, so need to make it public
        super.applyAttributesFinish()
    }
}
/** @memo */
export function Divider(
    /** @memo */
    style: ((attributes: DividerAttribute) => void) | undefined,
    
    /** @memo */
    content_?: (() => void) | undefined,
): void {
    const receiver = remember(() => {
        return new ArkDividerComponent()
    })
    NodeAttach<ArkDividerPeer>((): ArkDividerPeer => ArkDividerPeer.create(receiver), (_: ArkDividerPeer) => {
        receiver.setDividerOptions()
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
