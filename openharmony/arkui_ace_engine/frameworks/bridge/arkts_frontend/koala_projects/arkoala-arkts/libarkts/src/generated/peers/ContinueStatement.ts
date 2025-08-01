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

import { Statement } from "./Statement"
import { Identifier } from "./Identifier"
export class ContinueStatement extends Statement {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 20)
        super(pointer)
        console.warn("Warning: stub node ContinueStatement")
    }
    static createContinueStatement(): ContinueStatement {
        return new ContinueStatement(global.generatedEs2panda._CreateContinueStatement(global.context))
    }
    static updateContinueStatement(original?: ContinueStatement): ContinueStatement {
        return new ContinueStatement(global.generatedEs2panda._UpdateContinueStatement(global.context, passNode(original)))
    }
    static create1ContinueStatement(ident?: Identifier): ContinueStatement {
        return new ContinueStatement(global.generatedEs2panda._CreateContinueStatement1(global.context, passNode(ident)))
    }
    static update1ContinueStatement(original?: ContinueStatement, ident?: Identifier): ContinueStatement {
        return new ContinueStatement(global.generatedEs2panda._UpdateContinueStatement1(global.context, passNode(original), passNode(ident)))
    }
    get ident(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._ContinueStatementIdentConst(global.context, this.peer))
    }
    get target(): AstNode | undefined {
        return unpackNode(global.generatedEs2panda._ContinueStatementTargetConst(global.context, this.peer))
    }
    /** @deprecated */
    setTarget(target: AstNode): this {
        global.generatedEs2panda._ContinueStatementSetTarget(global.context, this.peer, passNode(target))
        return this
    }
}
export function isContinueStatement(node: AstNode): node is ContinueStatement {
    return node instanceof ContinueStatement
}
if (!nodeByType.has(20)) {
    nodeByType.set(20, ContinueStatement)
}