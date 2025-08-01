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
import { Statement } from "./Statement"
export class BlockExpression extends Expression {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 155)
        super(pointer)
    }
    static createBlockExpression(statements: readonly Statement[]): BlockExpression {
        return new BlockExpression(global.generatedEs2panda._CreateBlockExpression(global.context, passNodeArray(statements), statements.length))
    }
    static updateBlockExpression(original: BlockExpression | undefined, statements: readonly Statement[]): BlockExpression {
        return new BlockExpression(global.generatedEs2panda._UpdateBlockExpression(global.context, passNode(original), passNodeArray(statements), statements.length))
    }
    get statements(): readonly Statement[] {
        return unpackNodeArray(global.generatedEs2panda._BlockExpressionStatements(global.context, this.peer))
    }
    /** @deprecated */
    addStatements(statements: readonly Statement[]): this {
        global.generatedEs2panda._BlockExpressionAddStatements(global.context, this.peer, passNodeArray(statements), statements.length)
        return this
    }
    /** @deprecated */
    addStatement(statement?: Statement): this {
        global.generatedEs2panda._BlockExpressionAddStatement(global.context, this.peer, passNode(statement))
        return this
    }
}
export function isBlockExpression(node: object | undefined): node is BlockExpression {
    return node instanceof BlockExpression
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_BLOCK_EXPRESSION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_BLOCK_EXPRESSION, (peer: KNativePointer) => new BlockExpression(peer))
}