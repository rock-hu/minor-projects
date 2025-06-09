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

import { TypeNode } from "./TypeNode"
export class TSTupleType extends TypeNode {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 133)
        super(pointer)
        console.warn("Warning: stub node TSTupleType")
    }
    static createTSTupleType(elementTypes: readonly TypeNode[]): TSTupleType {
        return new TSTupleType(global.generatedEs2panda._CreateTSTupleType(global.context, passNodeArray(elementTypes), elementTypes.length))
    }
    static updateTSTupleType(original: TSTupleType | undefined, elementTypes: readonly TypeNode[]): TSTupleType {
        return new TSTupleType(global.generatedEs2panda._UpdateTSTupleType(global.context, passNode(original), passNodeArray(elementTypes), elementTypes.length))
    }
    get elementType(): readonly TypeNode[] {
        return unpackNodeArray(global.generatedEs2panda._TSTupleTypeElementTypeConst(global.context, this.peer))
    }
}
export function isTSTupleType(node: AstNode): node is TSTupleType {
    return node instanceof TSTupleType
}
if (!nodeByType.has(133)) {
    nodeByType.set(133, TSTupleType)
}