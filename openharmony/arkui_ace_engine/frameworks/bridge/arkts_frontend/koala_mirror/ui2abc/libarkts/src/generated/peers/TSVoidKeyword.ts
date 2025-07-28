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
import { TypeNode } from "./TypeNode"
export class TSVoidKeyword extends TypeNode {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 94)
        super(pointer)
    }
    static createTSVoidKeyword(): TSVoidKeyword {
        return new TSVoidKeyword(global.generatedEs2panda._CreateTSVoidKeyword(global.context))
    }
    static updateTSVoidKeyword(original?: TSVoidKeyword): TSVoidKeyword {
        return new TSVoidKeyword(global.generatedEs2panda._UpdateTSVoidKeyword(global.context, passNode(original)))
    }
}
export function isTSVoidKeyword(node: object | undefined): node is TSVoidKeyword {
    return node instanceof TSVoidKeyword
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TS_VOID_KEYWORD)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TS_VOID_KEYWORD, (peer: KNativePointer) => new TSVoidKeyword(peer))
}