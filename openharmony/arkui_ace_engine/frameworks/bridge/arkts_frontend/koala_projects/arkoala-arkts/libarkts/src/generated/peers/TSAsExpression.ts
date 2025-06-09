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

import { AnnotatedExpression } from "./AnnotatedExpression"
import { Expression } from "./Expression"
import { TypeNode } from "./TypeNode"
export class TSAsExpression extends AnnotatedExpression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 137)
        super(pointer)
        console.warn("Warning: stub node TSAsExpression")
    }
    static createTSAsExpression(expression: Expression | undefined, typeAnnotation: TypeNode | undefined, isConst: boolean): TSAsExpression {
        return new TSAsExpression(global.generatedEs2panda._CreateTSAsExpression(global.context, passNode(expression), passNode(typeAnnotation), isConst))
    }
    static updateTSAsExpression(original: TSAsExpression | undefined, expression: Expression | undefined, typeAnnotation: TypeNode | undefined, isConst: boolean): TSAsExpression {
        return new TSAsExpression(global.generatedEs2panda._UpdateTSAsExpression(global.context, passNode(original), passNode(expression), passNode(typeAnnotation), isConst))
    }
    get expr(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSAsExpressionExprConst(global.context, this.peer))
    }
    /** @deprecated */
    setExpr(expr: Expression): this {
        global.generatedEs2panda._TSAsExpressionSetExpr(global.context, this.peer, passNode(expr))
        return this
    }
    get isConst(): boolean {
        return global.generatedEs2panda._TSAsExpressionIsConstConst(global.context, this.peer)
    }
    /** @deprecated */
    setUncheckedCast(isUncheckedCast: boolean): this {
        global.generatedEs2panda._TSAsExpressionSetUncheckedCast(global.context, this.peer, isUncheckedCast)
        return this
    }
    get typeAnnotation(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._TSAsExpressionTypeAnnotationConst(global.context, this.peer))
    }
    /** @deprecated */
    setTsTypeAnnotation(typeAnnotation: TypeNode): this {
        global.generatedEs2panda._TSAsExpressionSetTsTypeAnnotation(global.context, this.peer, passNode(typeAnnotation))
        return this
    }
}
export function isTSAsExpression(node: AstNode): node is TSAsExpression {
    return node instanceof TSAsExpression
}
if (!nodeByType.has(137)) {
    nodeByType.set(137, TSAsExpression)
}