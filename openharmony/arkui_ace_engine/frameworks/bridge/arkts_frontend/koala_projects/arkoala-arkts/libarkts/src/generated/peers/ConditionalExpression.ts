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
export class ConditionalExpression extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 19)
        super(pointer)
        console.warn("Warning: stub node ConditionalExpression")
    }
    static createConditionalExpression(test?: Expression, consequent?: Expression, alternate?: Expression): ConditionalExpression {
        return new ConditionalExpression(global.generatedEs2panda._CreateConditionalExpression(global.context, passNode(test), passNode(consequent), passNode(alternate)))
    }
    static updateConditionalExpression(original?: ConditionalExpression, test?: Expression, consequent?: Expression, alternate?: Expression): ConditionalExpression {
        return new ConditionalExpression(global.generatedEs2panda._UpdateConditionalExpression(global.context, passNode(original), passNode(test), passNode(consequent), passNode(alternate)))
    }
    get test(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ConditionalExpressionTestConst(global.context, this.peer))
    }
    /** @deprecated */
    setTest(expr: Expression): this {
        global.generatedEs2panda._ConditionalExpressionSetTest(global.context, this.peer, passNode(expr))
        return this
    }
    get consequent(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ConditionalExpressionConsequentConst(global.context, this.peer))
    }
    /** @deprecated */
    setConsequent(expr: Expression): this {
        global.generatedEs2panda._ConditionalExpressionSetConsequent(global.context, this.peer, passNode(expr))
        return this
    }
    get alternate(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ConditionalExpressionAlternateConst(global.context, this.peer))
    }
    /** @deprecated */
    setAlternate(expr: Expression): this {
        global.generatedEs2panda._ConditionalExpressionSetAlternate(global.context, this.peer, passNode(expr))
        return this
    }
}
export function isConditionalExpression(node: AstNode): node is ConditionalExpression {
    return node instanceof ConditionalExpression
}
if (!nodeByType.has(19)) {
    nodeByType.set(19, ConditionalExpression)
}