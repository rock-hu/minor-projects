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
import { StringLiteral } from "./StringLiteral"
export class ExportAllDeclaration extends Statement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 26)
        super(pointer)
    }
    static createExportAllDeclaration(source?: StringLiteral, exported?: Identifier): ExportAllDeclaration {
        return new ExportAllDeclaration(global.generatedEs2panda._CreateExportAllDeclaration(global.context, passNode(source), passNode(exported)))
    }
    static updateExportAllDeclaration(original?: ExportAllDeclaration, source?: StringLiteral, exported?: Identifier): ExportAllDeclaration {
        return new ExportAllDeclaration(global.generatedEs2panda._UpdateExportAllDeclaration(global.context, passNode(original), passNode(source), passNode(exported)))
    }
    get source(): StringLiteral | undefined {
        return unpackNode(global.generatedEs2panda._ExportAllDeclarationSourceConst(global.context, this.peer))
    }
    get exported(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._ExportAllDeclarationExportedConst(global.context, this.peer))
    }
}
export function isExportAllDeclaration(node: object | undefined): node is ExportAllDeclaration {
    return node instanceof ExportAllDeclaration
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_EXPORT_ALL_DECLARATION)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_EXPORT_ALL_DECLARATION, (peer: KNativePointer) => new ExportAllDeclaration(peer))
}