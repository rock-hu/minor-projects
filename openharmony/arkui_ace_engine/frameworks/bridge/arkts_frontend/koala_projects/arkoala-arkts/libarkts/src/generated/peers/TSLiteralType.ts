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
import { Expression } from "./Expression"
export class TSLiteralType extends TypeNode {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 108)
        super(pointer)
        console.warn("Warning: stub node TSLiteralType")
    }
    static createTSLiteralType(literal?: Expression): TSLiteralType {
        return new TSLiteralType(global.generatedEs2panda._CreateTSLiteralType(global.context, passNode(literal)))
    }
    static updateTSLiteralType(original?: TSLiteralType, literal?: Expression): TSLiteralType {
        return new TSLiteralType(global.generatedEs2panda._UpdateTSLiteralType(global.context, passNode(original), passNode(literal)))
    }
    get literal(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSLiteralTypeLiteralConst(global.context, this.peer))
    }
}
export function isTSLiteralType(node: AstNode): node is TSLiteralType {
    return node instanceof TSLiteralType
}
if (!nodeByType.has(108)) {
    nodeByType.set(108, TSLiteralType)
}