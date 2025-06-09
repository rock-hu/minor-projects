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
    Es2pandaAstNodeType,
    KNativePointer,
    nodeByType,
    ArktsObject,
    unpackString
} from "../../reexport-for-generated"

import { Expression } from "./Expression"
import { TypeNode } from "./TypeNode"
export class TSTypeParameterInstantiation extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 119)
        super(pointer)
        console.warn("Warning: stub node TSTypeParameterInstantiation")
    }
    static createTSTypeParameterInstantiation(params: readonly TypeNode[]): TSTypeParameterInstantiation {
        return new TSTypeParameterInstantiation(global.generatedEs2panda._CreateTSTypeParameterInstantiation(global.context, passNodeArray(params), params.length))
    }
    static updateTSTypeParameterInstantiation(original: TSTypeParameterInstantiation | undefined, params: readonly TypeNode[]): TSTypeParameterInstantiation {
        return new TSTypeParameterInstantiation(global.generatedEs2panda._UpdateTSTypeParameterInstantiation(global.context, passNode(original), passNodeArray(params), params.length))
    }
    get params(): readonly TypeNode[] {
        return unpackNodeArray(global.generatedEs2panda._TSTypeParameterInstantiationParamsConst(global.context, this.peer))
    }
}
export function isTSTypeParameterInstantiation(node: AstNode): node is TSTypeParameterInstantiation {
    return node instanceof TSTypeParameterInstantiation
}
if (!nodeByType.has(119)) {
    nodeByType.set(119, TSTypeParameterInstantiation)
}