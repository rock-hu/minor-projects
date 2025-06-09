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

import { AsyncCallback_image_PixelMap_Void } from "./SyntheticDeclarations"
import { PixelMap, PixelMapInternal } from "./ArkPixelMapMaterialized"
import { SnapshotOptions } from "./ArkArkuiExternalInterfaces"
import { TypeChecker, ArkUIGeneratedNativeModule } from "#components"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class GlobalScope_ohos_arkui_componentSnapshot {
    public static get(id: string, callback: ((result: PixelMap) => void), options?: SnapshotOptions): void {
        const id_casted = id as (string)
        const callback_casted = callback as (((result: PixelMap) => void))
        const options_casted = options as (SnapshotOptions | undefined)
        GlobalScope_ohos_arkui_componentSnapshot.get_serialize(id_casted, callback_casted, options_casted)
        return
    }
    private static get_serialize(id: string, callback: ((result: PixelMap) => void), options?: SnapshotOptions): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(callback)
        let options_type : int32 = RuntimeType.UNDEFINED
        options_type = runtimeType(options)
        thisSerializer.writeInt8(options_type as int32)
        if ((RuntimeType.UNDEFINED) != (options_type)) {
            const options_value  = options!
            thisSerializer.writeSnapshotOptions(options_value)
        }
        ArkUIGeneratedNativeModule._GlobalScope_ohos_arkui_componentSnapshot_get(id, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
