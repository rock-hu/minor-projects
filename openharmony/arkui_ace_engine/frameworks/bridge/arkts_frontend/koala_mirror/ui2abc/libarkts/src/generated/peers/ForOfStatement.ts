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
import { LoopStatement } from "./LoopStatement"
import { Statement } from "./Statement"
export class ForOfStatement extends LoopStatement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 32)
        super(pointer)
    }
    static createForOfStatement(left: AstNode | undefined, right: Expression | undefined, body: Statement | undefined, isAwait: boolean): ForOfStatement {
        return new ForOfStatement(global.generatedEs2panda._CreateForOfStatement(global.context, passNode(left), passNode(right), passNode(body), isAwait))
    }
    static updateForOfStatement(original: ForOfStatement | undefined, left: AstNode | undefined, right: Expression | undefined, body: Statement | undefined, isAwait: boolean): ForOfStatement {
        return new ForOfStatement(global.generatedEs2panda._UpdateForOfStatement(global.context, passNode(original), passNode(left), passNode(right), passNode(body), isAwait))
    }
    get left(): AstNode | undefined {
        return unpackNode(global.generatedEs2panda._ForOfStatementLeft(global.context, this.peer))
    }
    get right(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ForOfStatementRight(global.context, this.peer))
    }
    get body(): Statement | undefined {
        return unpackNode(global.generatedEs2panda._ForOfStatementBody(global.context, this.peer))
    }
    get isAwait(): boolean {
        return global.generatedEs2panda._ForOfStatementIsAwaitConst(global.context, this.peer)
    }
}
export function isForOfStatement(node: object | undefined): node is ForOfStatement {
    return node instanceof ForOfStatement
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_FOR_OF_STATEMENT)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_FOR_OF_STATEMENT, (peer: KNativePointer) => new ForOfStatement(peer))
}