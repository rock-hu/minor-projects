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

import { Es2pandaAstNodeType } from "./../Es2pandaEnums"
import { Expression } from "./Expression"
import { FunctionSignature } from "./FunctionSignature"
import { TSTypeParameterDeclaration } from "./TSTypeParameterDeclaration"
import { TypeNode } from "./TypeNode"
export class TSMethodSignature extends AstNode {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 106)
        super(pointer)
    }
    static createTSMethodSignature(key: Expression | undefined, signature: FunctionSignature | undefined, computed: boolean, optional_arg: boolean): TSMethodSignature {
        return new TSMethodSignature(global.generatedEs2panda._CreateTSMethodSignature(global.context, passNode(key), passNode(signature), computed, optional_arg))
    }
    static updateTSMethodSignature(original: TSMethodSignature | undefined, key: Expression | undefined, signature: FunctionSignature | undefined, computed: boolean, optional_arg: boolean): TSMethodSignature {
        return new TSMethodSignature(global.generatedEs2panda._UpdateTSMethodSignature(global.context, passNode(original), passNode(key), passNode(signature), computed, optional_arg))
    }
    get key(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSMethodSignatureKey(global.context, this.peer))
    }
    get typeParams(): TSTypeParameterDeclaration | undefined {
        return unpackNode(global.generatedEs2panda._TSMethodSignatureTypeParams(global.context, this.peer))
    }
    get params(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._TSMethodSignatureParamsConst(global.context, this.peer))
    }
    get returnTypeAnnotation(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._TSMethodSignatureReturnTypeAnnotation(global.context, this.peer))
    }
    get computed(): boolean {
        return global.generatedEs2panda._TSMethodSignatureComputedConst(global.context, this.peer)
    }
    get optional(): boolean {
        return global.generatedEs2panda._TSMethodSignatureOptionalConst(global.context, this.peer)
    }
}
export function isTSMethodSignature(node: object | undefined): node is TSMethodSignature {
    return node instanceof TSMethodSignature
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TS_METHOD_SIGNATURE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TS_METHOD_SIGNATURE, (peer: KNativePointer) => new TSMethodSignature(peer))
}