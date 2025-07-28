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

import { CustomDialogControllerOptions } from "./ArkCustomDialogControllerInterfaces"
import { TypeChecker, ArkUIGeneratedNativeModule } from "#components"
import { Finalizable, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, toPeerPtr, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { CallbackTransformer } from "./peers/CallbackTransformer"
export class CustomDialogControllerInternal {
    public static fromPtr(ptr: KPointer): CustomDialogController {
        const obj : CustomDialogController = new CustomDialogController(undefined)
        obj.peer = new Finalizable(ptr, CustomDialogController.getFinalizer())
        return obj
    }
}
export class CustomDialogController implements MaterializedBase {
    peer?: Finalizable | undefined = undefined
    public getPeer(): Finalizable | undefined {
        return this.peer
    }
    static ctor_customdialogcontroller(value: CustomDialogControllerOptions): KPointer {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeCustomDialogControllerOptions(value)
        const retval  = ArkUIGeneratedNativeModule._CustomDialogController_ctor(thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
        return retval
    }
    constructor(value?: CustomDialogControllerOptions) {
        if ((value) !== (undefined))
        {
            const ctorPtr : KPointer = CustomDialogController.ctor_customdialogcontroller((value)!)
            this.peer = new Finalizable(ctorPtr, CustomDialogController.getFinalizer())
        }
        // /*TODO: API required*/ if (value) value.builder.buildOptions.controller = this
    }
    static getFinalizer(): KPointer {
        return ArkUIGeneratedNativeModule._CustomDialogController_getFinalizer()
    }
    public open(): undefined {
        return this.open_serialize()
    }
    public close(): undefined {
        return this.close_serialize()
    }
    private open_serialize(): undefined {
        const retval  = ArkUIGeneratedNativeModule._CustomDialogController_open(this.peer!.ptr)
        return retval
    }
    private close_serialize(): undefined {
        const retval  = ArkUIGeneratedNativeModule._CustomDialogController_close(this.peer!.ptr)
        return retval
    }
}
