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
import { KInt, KPointer, KBoolean, NativeBuffer, KStringPtr, wrapCallback } from "@koalaui/interop"
import { NodeAttach, remember } from "@koalaui/runtime"
import { ItemDragInfo, CustomBuilder } from "./common"
export type AnimationRange_Number = [
    number,
    number
]
export type GridAttribute_onItemDragStart_event_type = (event: ItemDragInfo, itemIndex: number) => CustomBuilder;
export type ListAttribute_onItemDragStart_event_type = (event: ItemDragInfo, itemIndex: number) => CustomBuilder;
export type TextTimerAttribute_onTimer_event_type = (utc: int64, elapsedTime: int64) => void;
