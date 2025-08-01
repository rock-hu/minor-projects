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
export class TypeofExpression extends Expression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 145)
        super(pointer)
    }
    static createTypeofExpression(argument?: Expression): TypeofExpression {
        return new TypeofExpression(global.generatedEs2panda._CreateTypeofExpression(global.context, passNode(argument)))
    }
    static updateTypeofExpression(original?: TypeofExpression, argument?: Expression): TypeofExpression {
        return new TypeofExpression(global.generatedEs2panda._UpdateTypeofExpression(global.context, passNode(original), passNode(argument)))
    }
    get argument(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TypeofExpressionArgumentConst(global.context, this.peer))
    }
}
export function isTypeofExpression(node: object | undefined): node is TypeofExpression {
    return node instanceof TypeofExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TYPEOF_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TYPEOF_EXPRESSION, (peer: KNativePointer) => new TypeofExpression(peer))
}