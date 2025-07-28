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

import { AnnotatedExpression } from "./AnnotatedExpression"
import { Decorator } from "./Decorator"
import { Es2pandaAstNodeType } from "./../Es2pandaEnums"
import { Expression } from "./Expression"
import { TypeNode } from "./TypeNode"
import { ValidationInfo } from "./ValidationInfo"
export class ArrayExpression extends AnnotatedExpression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 157)
        super(pointer)
    }
    static create1ArrayExpression(nodeType: Es2pandaAstNodeType, elements: readonly Expression[], trailingComma: boolean): ArrayExpression {
        return new ArrayExpression(global.generatedEs2panda._CreateArrayExpression1(global.context, nodeType, passNodeArray(elements), elements.length, trailingComma))
    }
    static updateArrayExpression(original: ArrayExpression | undefined, elements: readonly Expression[]): ArrayExpression {
        return new ArrayExpression(global.generatedEs2panda._UpdateArrayExpression(global.context, passNode(original), passNodeArray(elements), elements.length))
    }
    static update1ArrayExpression(original: ArrayExpression | undefined, nodeType: Es2pandaAstNodeType, elements: readonly Expression[], trailingComma: boolean): ArrayExpression {
        return new ArrayExpression(global.generatedEs2panda._UpdateArrayExpression1(global.context, passNode(original), nodeType, passNodeArray(elements), elements.length, trailingComma))
    }
    get elements(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._ArrayExpressionElements(global.context, this.peer))
    }
    /** @deprecated */
    setElements(elements: readonly Expression[]): this {
        global.generatedEs2panda._ArrayExpressionSetElements(global.context, this.peer, passNodeArray(elements), elements.length)
        return this
    }
    get isDeclaration(): boolean {
        return global.generatedEs2panda._ArrayExpressionIsDeclarationConst(global.context, this.peer)
    }
    get isOptional(): boolean {
        return global.generatedEs2panda._ArrayExpressionIsOptionalConst(global.context, this.peer)
    }
    /** @deprecated */
    setDeclaration(): this {
        global.generatedEs2panda._ArrayExpressionSetDeclaration(global.context, this.peer)
        return this
    }
    /** @deprecated */
    setOptional(optional_arg: boolean): this {
        global.generatedEs2panda._ArrayExpressionSetOptional(global.context, this.peer, optional_arg)
        return this
    }
    get decorators(): readonly Decorator[] {
        return unpackNodeArray(global.generatedEs2panda._ArrayExpressionDecoratorsConst(global.context, this.peer))
    }
    /** @deprecated */
    clearPreferredType(): this {
        global.generatedEs2panda._ArrayExpressionClearPreferredType(global.context, this.peer)
        return this
    }
    get convertibleToArrayPattern(): boolean {
        return global.generatedEs2panda._ArrayExpressionConvertibleToArrayPattern(global.context, this.peer)
    }
    get validateExpression(): ValidationInfo | undefined {
        return new ValidationInfo(global.generatedEs2panda._ArrayExpressionValidateExpression(global.context, this.peer))
    }
    get typeAnnotation(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._ArrayExpressionTypeAnnotationConst(global.context, this.peer))
    }
    /** @deprecated */
    setTsTypeAnnotation(typeAnnotation?: TypeNode): this {
        global.generatedEs2panda._ArrayExpressionSetTsTypeAnnotation(global.context, this.peer, passNode(typeAnnotation))
        return this
    }
}
export function isArrayExpression(node: object | undefined): node is ArrayExpression {
    return node instanceof ArrayExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_ARRAY_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_ARRAY_EXPRESSION, (peer: KNativePointer) => new ArrayExpression(peer))
}