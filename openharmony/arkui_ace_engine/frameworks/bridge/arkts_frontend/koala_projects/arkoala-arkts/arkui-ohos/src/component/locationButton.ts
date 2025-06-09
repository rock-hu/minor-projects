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
import { KInt, KPointer, KBoolean, KStringPtr, wrapCallback, NativeBuffer } from "@koalaui/interop"
import { NodeAttach, remember } from "@koalaui/runtime"
import { ButtonType } from "./button"
import { ClickEvent } from "./common"
import { SecurityComponentMethod, SecurityComponentLayoutDirection } from "./securityComponent"
import { Dimension, Position, Edges, LocalizedEdges, ResourceColor, Padding, Length, SizeOptions, ConstraintSizeOptions } from "./units"
import { FontStyle, FontWeight, BorderStyle } from "./enums"
import { Resource } from "global/resource";
import { ArkLocationButtonComponent } from "./../generated/ArkLocationButton"
import { ArkLocationButtonPeer } from "./../generated/peers/ArkLocationButtonPeer"
export enum LocationIconStyle {
    FULL_FILLED = 0,
    LINES = 1
}
export enum LocationDescription {
    CURRENT_LOCATION = 0,
    ADD_LOCATION = 1,
    SELECT_LOCATION = 2,
    SHARE_LOCATION = 3,
    SEND_LOCATION = 4,
    LOCATING = 5,
    LOCATION = 6,
    SEND_CURRENT_LOCATION = 7,
    RELOCATION = 8,
    PUNCH_IN = 9,
    CURRENT_POSITION = 10
}
export interface LocationButtonOptions {
    icon?: LocationIconStyle;
    text?: LocationDescription;
    buttonType?: ButtonType;
}
export enum LocationButtonOnClickResult {
    SUCCESS = 0,
    TEMPORARY_AUTHORIZATION_FAILED = 1
}
export type Callback_ClickEvent_LocationButtonOnClickResult_Void = (event: ClickEvent, result: LocationButtonOnClickResult) => void;
/** @memo:stable */
export interface LocationButtonAttribute extends SecurityComponentMethod {
    /** @memo */
    setLocationButtonOptions(options?: LocationButtonOptions): this
    /** @memo */
    onClick(value: ((event: ClickEvent,result: LocationButtonOnClickResult) => void)): this
}
/** @memo */
export function LocationButton(
  /** @memo */
  style: ((attributes: LocationButtonAttribute) => void) | undefined,
  options?: LocationButtonOptions | undefined, 
  /** @memo */
  content_?: () => void,
) {
    const receiver = remember(() => {
        return new ArkLocationButtonComponent()
    })
    NodeAttach<ArkLocationButtonPeer>((): ArkLocationButtonPeer => ArkLocationButtonPeer.create(receiver), (_: ArkLocationButtonPeer) => {
        receiver.setLocationButtonOptions(options)
        style?.(receiver)
        content_?.()
        receiver.applyAttributesFinish()
    })
}
