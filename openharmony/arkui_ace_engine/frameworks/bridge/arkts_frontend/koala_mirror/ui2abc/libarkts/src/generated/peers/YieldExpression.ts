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
export class YieldExpression extends Expression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 153)
        super(pointer)
    }
    static createYieldExpression(argument: Expression | undefined, isDelegate: boolean): YieldExpression {
        return new YieldExpression(global.generatedEs2panda._CreateYieldExpression(global.context, passNode(argument), isDelegate))
    }
    static updateYieldExpression(original: YieldExpression | undefined, argument: Expression | undefined, isDelegate: boolean): YieldExpression {
        return new YieldExpression(global.generatedEs2panda._UpdateYieldExpression(global.context, passNode(original), passNode(argument), isDelegate))
    }
    get hasDelegate(): boolean {
        return global.generatedEs2panda._YieldExpressionHasDelegateConst(global.context, this.peer)
    }
    get argument(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._YieldExpressionArgumentConst(global.context, this.peer))
    }
}
export function isYieldExpression(node: object | undefined): node is YieldExpression {
    return node instanceof YieldExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_YIELD_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_YIELD_EXPRESSION, (peer: KNativePointer) => new YieldExpression(peer))
}