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
export class TSInterfaceBody extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 131)
        super(pointer)
        console.warn("Warning: stub node TSInterfaceBody")
    }
    static createTSInterfaceBody(body: readonly AstNode[]): TSInterfaceBody {
        return new TSInterfaceBody(global.generatedEs2panda._CreateTSInterfaceBody(global.context, passNodeArray(body), body.length))
    }
    static updateTSInterfaceBody(original: TSInterfaceBody | undefined, body: readonly AstNode[]): TSInterfaceBody {
        return new TSInterfaceBody(global.generatedEs2panda._UpdateTSInterfaceBody(global.context, passNode(original), passNodeArray(body), body.length))
    }
    get bodyPtr(): readonly AstNode[] {
        return unpackNodeArray(global.generatedEs2panda._TSInterfaceBodyBodyPtr(global.context, this.peer))
    }
    get body(): readonly AstNode[] {
        return unpackNodeArray(global.generatedEs2panda._TSInterfaceBodyBodyConst(global.context, this.peer))
    }
}
export function isTSInterfaceBody(node: AstNode): node is TSInterfaceBody {
    return node instanceof TSInterfaceBody
}
if (!nodeByType.has(131)) {
    nodeByType.set(131, TSInterfaceBody)
}