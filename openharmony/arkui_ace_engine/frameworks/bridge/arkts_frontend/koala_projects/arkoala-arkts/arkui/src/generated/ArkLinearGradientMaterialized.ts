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

import { ColorStop } from "./ArkDataPanelInterfaces"
import { TypeChecker, ArkUIGeneratedNativeModule } from "#components"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class LinearGradientInternal {
    public static fromPtr(ptr: KPointer): LinearGradient {
        const obj : LinearGradient = new LinearGradient(undefined)
        obj.peer = new Finalizable(ptr, LinearGradient.getFinalizer())
        return obj
    }
}
export class LinearGradient implements MaterializedBase {
    peer?: Finalizable | undefined = undefined
    public getPeer(): Finalizable | undefined {
        return this.peer
    }
    static ctor_lineargradient(colorStops: Array<ColorStop>): KPointer {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeInt32(colorStops.length as int32)
        for (let i = 0; i < colorStops.length; i++) {
            const colorStops_element : ColorStop = colorStops[i]
            thisSerializer.writeColorStop(colorStops_element)
        }
        const retval  = ArkUIGeneratedNativeModule._LinearGradient_ctor(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
    constructor(colorStops?: Array<ColorStop>) {
        if ((colorStops) !== (undefined))
        {
            const ctorPtr : KPointer = LinearGradient.ctor_lineargradient((colorStops)!)
            this.peer = new Finalizable(ctorPtr, LinearGradient.getFinalizer())
        }
    }
    static getFinalizer(): KPointer {
        return ArkUIGeneratedNativeModule._LinearGradient_getFinalizer()
    }
}
