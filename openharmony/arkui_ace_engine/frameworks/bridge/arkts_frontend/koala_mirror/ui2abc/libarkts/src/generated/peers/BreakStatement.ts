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
import { Identifier } from "./Identifier"
import { Statement } from "./Statement"
export class BreakStatement extends Statement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 9)
        super(pointer)
    }
    static create1BreakStatement(ident?: Identifier): BreakStatement {
        return new BreakStatement(global.generatedEs2panda._CreateBreakStatement1(global.context, passNode(ident)))
    }
    static updateBreakStatement(original?: BreakStatement): BreakStatement {
        return new BreakStatement(global.generatedEs2panda._UpdateBreakStatement(global.context, passNode(original)))
    }
    static update1BreakStatement(original?: BreakStatement, ident?: Identifier): BreakStatement {
        return new BreakStatement(global.generatedEs2panda._UpdateBreakStatement1(global.context, passNode(original), passNode(ident)))
    }
    get ident(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._BreakStatementIdent(global.context, this.peer))
    }
    get target(): AstNode | undefined {
        return unpackNode(global.generatedEs2panda._BreakStatementTargetConst(global.context, this.peer))
    }
    get hasTarget(): boolean {
        return global.generatedEs2panda._BreakStatementHasTargetConst(global.context, this.peer)
    }
    /** @deprecated */
    setTarget(target?: AstNode): this {
        global.generatedEs2panda._BreakStatementSetTarget(global.context, this.peer, passNode(target))
        return this
    }
}
export function isBreakStatement(node: object | undefined): node is BreakStatement {
    return node instanceof BreakStatement
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_BREAK_STATEMENT)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_BREAK_STATEMENT, (peer: KNativePointer) => new BreakStatement(peer))
}