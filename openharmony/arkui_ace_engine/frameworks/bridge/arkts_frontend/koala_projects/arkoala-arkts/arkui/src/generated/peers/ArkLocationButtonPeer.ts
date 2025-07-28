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
import { ArkSecurityComponentMethodPeer, ArkSecurityComponentMethodAttributes } from "./ArkSecurityComponentPeer"
import { SecurityComponentMethod, SecurityComponentLayoutDirection } from "./../ArkSecurityComponentInterfaces"
import { Dimension, Position, Edges, LocalizedEdges, ResourceColor, Padding, Length, SizeOptions, ConstraintSizeOptions, PX, VP, FP, LPX, Percentage } from "./../ArkUnitsInterfaces"
import { FontStyle, FontWeight, BorderStyle, Color } from "./../ArkEnumsInterfaces"
import { Resource } from "./../ArkResourceInterfaces"
import { Callback_ClickEvent_LocationButtonOnClickResult_Void } from "./../SyntheticDeclarations"
import { ClickEvent } from "./../ArkClickEventMaterialized"
import { LocationButtonOnClickResult, LocationButtonAttribute, LocationButtonOptions } from "./../ArkLocationButtonInterfaces"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { DotIndicator } from "./../ArkDotIndicatorBuilder"
import { DigitIndicator } from "./../ArkDigitIndicatorBuilder"
import { SubTabBarStyle } from "./../ArkSubTabBarStyleBuilder"
import { BottomTabBarStyle } from "./../ArkBottomTabBarStyleBuilder"
export class ArkLocationButtonPeer extends ArkSecurityComponentMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkLocationButtonPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._LocationButton_construct(peerId, flags)
        const _peer  = new ArkLocationButtonPeer(_peerPtr, peerId, "LocationButton", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setLocationButtonOptions0Attribute(): void {
        ArkUIGeneratedNativeModule._LocationButtonInterface_setLocationButtonOptions0(this.peer.ptr)
    }
    setLocationButtonOptions1Attribute(options: LocationButtonOptions): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeLocationButtonOptions(options)
        ArkUIGeneratedNativeModule._LocationButtonInterface_setLocationButtonOptions1(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onClickAttribute(value: ((event: ClickEvent,result: LocationButtonOnClickResult) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._LocationButtonAttribute_onClick(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkLocationButtonAttributes extends ArkSecurityComponentMethodAttributes {
    onClick?: ((event: ClickEvent,result: LocationButtonOnClickResult) => void)
}
