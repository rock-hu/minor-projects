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
export class ETSClassLiteral extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 70)
        super(pointer)
        console.warn("Warning: stub node ETSClassLiteral")
    }
    static createETSClassLiteral(expr?: TypeNode): ETSClassLiteral {
        return new ETSClassLiteral(global.generatedEs2panda._CreateETSClassLiteral(global.context, passNode(expr)))
    }
    static updateETSClassLiteral(original?: ETSClassLiteral, expr?: TypeNode): ETSClassLiteral {
        return new ETSClassLiteral(global.generatedEs2panda._UpdateETSClassLiteral(global.context, passNode(original), passNode(expr)))
    }
    get expr(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._ETSClassLiteralExprConst(global.context, this.peer))
    }
}
export function isETSClassLiteral(node: AstNode): node is ETSClassLiteral {
    return node instanceof ETSClassLiteral
}
if (!nodeByType.has(70)) {
    nodeByType.set(70, ETSClassLiteral)
}