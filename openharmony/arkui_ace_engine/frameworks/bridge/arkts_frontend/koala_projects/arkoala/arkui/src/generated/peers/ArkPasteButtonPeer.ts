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
import { ArkSecurityComponentMethodPeer, ArkSecurityComponentMethodAttributes } from "./ArkSecurityComponentPeer"
import { SecurityComponentMethod, SecurityComponentLayoutDirection } from "./../ArkSecurityComponentInterfaces"
import { Dimension, Position, Edges, LocalizedEdges, ResourceColor, Padding, Length, SizeOptions, ConstraintSizeOptions, PX, VP, FP, LPX, Percentage, Area } from "./../ArkUnitsInterfaces"
import { FontStyle, FontWeight, BorderStyle, Color } from "./../ArkEnumsInterfaces"
import { Resource } from "./../ArkResourceInterfaces"
import { LengthMetrics } from "./../ArkLengthMetricsMaterialized"
import { ClickEvent } from "./../ArkClickEventMaterialized"
import { PasteButtonOnClickResult, PasteButtonAttribute, PasteButtonOptions, PasteIconStyle, PasteDescription } from "./../ArkPasteButtonInterfaces"
import { LengthUnit } from "./../ArkArkuiExternalInterfaces"
import { BaseEvent } from "./../ArkBaseEventMaterialized"
import { EventTarget, SourceType, SourceTool } from "./../ArkCommonInterfaces"
import { ButtonType } from "./../ArkButtonInterfaces"
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
export class ArkPasteButtonPeer extends ArkSecurityComponentMethodPeer {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkPasteButtonPeer {
        const peerId = PeerNode.nextId()
        const _peerPtr = ArkUIGeneratedNativeModule._PasteButton_construct(peerId, flags)
        const _peer = new ArkPasteButtonPeer(_peerPtr, peerId, "PasteButton", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setPasteButtonOptions0Attribute(): void {
        ArkUIGeneratedNativeModule._PasteButtonInterface_setPasteButtonOptions0(this.peer.ptr)
    }
    setPasteButtonOptions1Attribute(options: PasteButtonOptions): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.writePasteButtonOptions(options)
        ArkUIGeneratedNativeModule._PasteButtonInterface_setPasteButtonOptions1(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
    onClickAttribute(value: ((event: ClickEvent, result: PasteButtonOnClickResult) => void)): void {
        const thisSerializer: Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._PasteButtonAttribute_onClick(this.peer.ptr, thisSerializer.asArray(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkPasteButtonAttributes extends ArkSecurityComponentMethodAttributes {
    onClick?: ((event: ClickEvent, result: PasteButtonOnClickResult) => void)
}
