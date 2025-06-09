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
import { Es2pandaTokenType } from "./../Es2pandaEnums"
export class UnaryExpression extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 147)
        super(pointer)
        console.warn("Warning: stub node UnaryExpression")
    }
    static createUnaryExpression(argument: Expression | undefined, unaryOperator: Es2pandaTokenType): UnaryExpression {
        return new UnaryExpression(global.generatedEs2panda._CreateUnaryExpression(global.context, passNode(argument), unaryOperator))
    }
    static updateUnaryExpression(original: UnaryExpression | undefined, argument: Expression | undefined, unaryOperator: Es2pandaTokenType): UnaryExpression {
        return new UnaryExpression(global.generatedEs2panda._UpdateUnaryExpression(global.context, passNode(original), passNode(argument), unaryOperator))
    }
    get operatorType(): Es2pandaTokenType {
        return global.generatedEs2panda._UnaryExpressionOperatorTypeConst(global.context, this.peer)
    }
    get argument(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._UnaryExpressionArgumentConst(global.context, this.peer))
    }
}
export function isUnaryExpression(node: AstNode): node is UnaryExpression {
    return node instanceof UnaryExpression
}
if (!nodeByType.has(147)) {
    nodeByType.set(147, UnaryExpression)
}