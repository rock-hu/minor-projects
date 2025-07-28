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
import { Es2pandaAstNodeType } from "./../Es2pandaEnums"
import { Expression } from "./Expression"
import { TypedStatement } from "./TypedStatement"
export class CatchClause extends TypedStatement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 11)
        super(pointer)
    }
    static createCatchClause(param?: Expression, body?: BlockStatement): CatchClause {
        return new CatchClause(global.generatedEs2panda._CreateCatchClause(global.context, passNode(param), passNode(body)))
    }
    static updateCatchClause(original?: CatchClause, param?: Expression, body?: BlockStatement): CatchClause {
        return new CatchClause(global.generatedEs2panda._UpdateCatchClause(global.context, passNode(original), passNode(param), passNode(body)))
    }
    static update1CatchClause(original?: CatchClause, other?: CatchClause): CatchClause {
        return new CatchClause(global.generatedEs2panda._UpdateCatchClause1(global.context, passNode(original), passNode(other)))
    }
    get param(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._CatchClauseParam(global.context, this.peer))
    }
    get body(): BlockStatement | undefined {
        return unpackNode(global.generatedEs2panda._CatchClauseBody(global.context, this.peer))
    }
    get isDefaultCatchClause(): boolean {
        return global.generatedEs2panda._CatchClauseIsDefaultCatchClauseConst(global.context, this.peer)
    }
}
export function isCatchClause(node: object | undefined): node is CatchClause {
    return node instanceof CatchClause
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_CATCH_CLAUSE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_CATCH_CLAUSE, (peer: KNativePointer) => new CatchClause(peer))
}