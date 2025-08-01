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

import { CodeGen } from "./CodeGen"
import { Es2pandaAstNodeType } from "./../Es2pandaEnums"
import { Es2pandaTokenType } from "./../Es2pandaEnums"
import { Expression } from "./Expression"
import { VReg } from "./VReg"
export class BinaryExpression extends Expression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 6)
        super(pointer)
    }
    static createBinaryExpression(left: Expression | undefined, right: Expression | undefined, operatorType: Es2pandaTokenType): BinaryExpression {
        return new BinaryExpression(global.generatedEs2panda._CreateBinaryExpression(global.context, passNode(left), passNode(right), operatorType))
    }
    static updateBinaryExpression(original: BinaryExpression | undefined, left: Expression | undefined, right: Expression | undefined, operatorType: Es2pandaTokenType): BinaryExpression {
        return new BinaryExpression(global.generatedEs2panda._UpdateBinaryExpression(global.context, passNode(original), passNode(left), passNode(right), operatorType))
    }
    get left(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._BinaryExpressionLeft(global.context, this.peer))
    }
    get right(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._BinaryExpressionRight(global.context, this.peer))
    }
    get result(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._BinaryExpressionResult(global.context, this.peer))
    }
    get operatorType(): Es2pandaTokenType {
        return global.generatedEs2panda._BinaryExpressionOperatorTypeConst(global.context, this.peer)
    }
    get isLogical(): boolean {
        return global.generatedEs2panda._BinaryExpressionIsLogicalConst(global.context, this.peer)
    }
    get isLogicalExtended(): boolean {
        return global.generatedEs2panda._BinaryExpressionIsLogicalExtendedConst(global.context, this.peer)
    }
    get isBitwise(): boolean {
        return global.generatedEs2panda._BinaryExpressionIsBitwiseConst(global.context, this.peer)
    }
    get isArithmetic(): boolean {
        return global.generatedEs2panda._BinaryExpressionIsArithmeticConst(global.context, this.peer)
    }
    /** @deprecated */
    setLeft(expr?: Expression): this {
        global.generatedEs2panda._BinaryExpressionSetLeft(global.context, this.peer, passNode(expr))
        return this
    }
    /** @deprecated */
    setRight(expr?: Expression): this {
        global.generatedEs2panda._BinaryExpressionSetRight(global.context, this.peer, passNode(expr))
        return this
    }
    /** @deprecated */
    setResult(expr?: Expression): this {
        global.generatedEs2panda._BinaryExpressionSetResult(global.context, this.peer, passNode(expr))
        return this
    }
    /** @deprecated */
    setOperator(operatorType: Es2pandaTokenType): this {
        global.generatedEs2panda._BinaryExpressionSetOperator(global.context, this.peer, operatorType)
        return this
    }
    /** @deprecated */
    compileOperands(etsg?: CodeGen, lhs?: VReg): this {
        global.generatedEs2panda._BinaryExpressionCompileOperandsConst(global.context, this.peer, passNode(etsg), passNode(lhs))
        return this
    }
}
export function isBinaryExpression(node: object | undefined): node is BinaryExpression {
    return node instanceof BinaryExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_BINARY_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_BINARY_EXPRESSION, (peer: KNativePointer) => new BinaryExpression(peer))
}