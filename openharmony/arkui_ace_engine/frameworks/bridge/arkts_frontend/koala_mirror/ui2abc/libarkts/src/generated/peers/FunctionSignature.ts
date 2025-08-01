/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import {
    global,
    passNode,
    passNodeArray,
    unpackNonNullableNode,
    unpackNode,
    unpackNodeArray,
    assertValidPeer,
    AstNode,
    KNativePointer,
    nodeByType,
    ArktsObject,
    unpackString
} from "../../reexport-for-generated"

import { Expression } from "./Expression"
import { TSTypeParameterDeclaration } from "./TSTypeParameterDeclaration"
import { TypeNode } from "./TypeNode"
export class FunctionSignature extends ArktsObject {
    constructor(pointer: KNativePointer) {
        super(pointer)
    }
    static createFunctionSignature(typeParams: TSTypeParameterDeclaration | undefined, params: readonly Expression[], returnTypeAnnotation: TypeNode | undefined, hasReceiver: boolean): FunctionSignature {
        return new FunctionSignature(global.generatedEs2panda._CreateFunctionSignature(global.context, passNode(typeParams), passNodeArray(params), params.length, passNode(returnTypeAnnotation), hasReceiver))
    }
    get params(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._FunctionSignatureParams(global.context, this.peer))
    }
    get typeParams(): TSTypeParameterDeclaration | undefined {
        return unpackNode(global.generatedEs2panda._FunctionSignatureTypeParams(global.context, this.peer))
    }
    get returnType(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._FunctionSignatureReturnType(global.context, this.peer))
    }
    /** @deprecated */
    setReturnType(type?: TypeNode): this {
        global.generatedEs2panda._FunctionSignatureSetReturnType(global.context, this.peer, passNode(type))
        return this
    }
    get clone(): FunctionSignature | undefined {
        return new FunctionSignature(global.generatedEs2panda._FunctionSignatureClone(global.context, this.peer))
    }
    get hasReceiver(): boolean {
        return global.generatedEs2panda._FunctionSignatureHasReceiverConst(global.context, this.peer)
    }
}