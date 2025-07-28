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
export class SuperExpression extends Expression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 83)
        super(pointer)
    }
    static createSuperExpression(): SuperExpression {
        return new SuperExpression(global.generatedEs2panda._CreateSuperExpression(global.context))
    }
    static updateSuperExpression(original?: SuperExpression): SuperExpression {
        return new SuperExpression(global.generatedEs2panda._UpdateSuperExpression(global.context, passNode(original)))
    }
}
export function isSuperExpression(node: object | undefined): node is SuperExpression {
    return node instanceof SuperExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_SUPER_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_SUPER_EXPRESSION, (peer: KNativePointer) => new SuperExpression(peer))
}