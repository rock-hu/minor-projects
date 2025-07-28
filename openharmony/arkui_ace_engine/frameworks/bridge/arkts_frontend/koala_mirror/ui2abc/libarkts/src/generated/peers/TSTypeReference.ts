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
import { Identifier } from "./Identifier"
import { TSTypeParameterInstantiation } from "./TSTypeParameterInstantiation"
import { TypeNode } from "./TypeNode"
export class TSTypeReference extends TypeNode {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 128)
        super(pointer)
    }
    static createTSTypeReference(typeName?: Expression, typeParams?: TSTypeParameterInstantiation): TSTypeReference {
        return new TSTypeReference(global.generatedEs2panda._CreateTSTypeReference(global.context, passNode(typeName), passNode(typeParams)))
    }
    static updateTSTypeReference(original?: TSTypeReference, typeName?: Expression, typeParams?: TSTypeParameterInstantiation): TSTypeReference {
        return new TSTypeReference(global.generatedEs2panda._UpdateTSTypeReference(global.context, passNode(original), passNode(typeName), passNode(typeParams)))
    }
    get typeParams(): TSTypeParameterInstantiation | undefined {
        return unpackNode(global.generatedEs2panda._TSTypeReferenceTypeParamsConst(global.context, this.peer))
    }
    get typeName(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSTypeReferenceTypeNameConst(global.context, this.peer))
    }
    get baseName(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._TSTypeReferenceBaseNameConst(global.context, this.peer))
    }
}
export function isTSTypeReference(node: object | undefined): node is TSTypeReference {
    return node instanceof TSTypeReference
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TS_TYPE_REFERENCE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TS_TYPE_REFERENCE, (peer: KNativePointer) => new TSTypeReference(peer))
}