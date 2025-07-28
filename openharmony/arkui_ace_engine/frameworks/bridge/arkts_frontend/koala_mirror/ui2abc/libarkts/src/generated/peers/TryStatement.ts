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

import { BlockStatement } from "./BlockStatement"
import { CatchClause } from "./CatchClause"
import { Es2pandaAstNodeType } from "./../Es2pandaEnums"
import { LabelPair } from "./LabelPair"
import { Statement } from "./Statement"
export class TryStatement extends Statement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 147)
        super(pointer)
    }
    static createTryStatement(block: BlockStatement | undefined, catchClauses: readonly CatchClause[], finalizer: BlockStatement | undefined, finalizerInsertionsLabelPair: readonly LabelPair[], finalizerInsertionsStatement: readonly Statement[]): TryStatement {
        return new TryStatement(global.generatedEs2panda._CreateTryStatement(global.context, passNode(block), passNodeArray(catchClauses), catchClauses.length, passNode(finalizer), passNodeArray(finalizerInsertionsLabelPair), finalizerInsertionsLabelPair.length, passNodeArray(finalizerInsertionsStatement), finalizerInsertionsStatement.length))
    }
    static updateTryStatement(original: TryStatement | undefined, block: BlockStatement | undefined, catchClauses: readonly CatchClause[], finalizer: BlockStatement | undefined, finalizerInsertionsLabelPair: readonly LabelPair[], finalizerInsertionsStatement: readonly Statement[]): TryStatement {
        return new TryStatement(global.generatedEs2panda._UpdateTryStatement(global.context, passNode(original), passNode(block), passNodeArray(catchClauses), catchClauses.length, passNode(finalizer), passNodeArray(finalizerInsertionsLabelPair), finalizerInsertionsLabelPair.length, passNodeArray(finalizerInsertionsStatement), finalizerInsertionsStatement.length))
    }
    static update1TryStatement(original?: TryStatement, other?: TryStatement): TryStatement {
        return new TryStatement(global.generatedEs2panda._UpdateTryStatement1(global.context, passNode(original), passNode(other)))
    }
    get finallyBlock(): BlockStatement | undefined {
        return unpackNode(global.generatedEs2panda._TryStatementFinallyBlockConst(global.context, this.peer))
    }
    get block(): BlockStatement | undefined {
        return unpackNode(global.generatedEs2panda._TryStatementBlockConst(global.context, this.peer))
    }
    get hasFinalizer(): boolean {
        return global.generatedEs2panda._TryStatementHasFinalizerConst(global.context, this.peer)
    }
    get hasDefaultCatchClause(): boolean {
        return global.generatedEs2panda._TryStatementHasDefaultCatchClauseConst(global.context, this.peer)
    }
    get catchClauses(): readonly CatchClause[] {
        return unpackNodeArray(global.generatedEs2panda._TryStatementCatchClausesConst(global.context, this.peer))
    }
    get finallyCanCompleteNormally(): boolean {
        return global.generatedEs2panda._TryStatementFinallyCanCompleteNormallyConst(global.context, this.peer)
    }
    /** @deprecated */
    setFinallyCanCompleteNormally(finallyCanCompleteNormally: boolean): this {
        global.generatedEs2panda._TryStatementSetFinallyCanCompleteNormally(global.context, this.peer, finallyCanCompleteNormally)
        return this
    }
}
export function isTryStatement(node: object | undefined): node is TryStatement {
    return node instanceof TryStatement
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TRY_STATEMENT)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TRY_STATEMENT, (peer: KNativePointer) => new TryStatement(peer))
}