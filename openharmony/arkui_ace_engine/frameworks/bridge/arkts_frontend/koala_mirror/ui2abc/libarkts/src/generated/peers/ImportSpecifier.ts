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
export class ImportSpecifier extends Statement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 43)
        super(pointer)
    }
    static createImportSpecifier(imported?: Identifier, local?: Identifier): ImportSpecifier {
        return new ImportSpecifier(global.generatedEs2panda._CreateImportSpecifier(global.context, passNode(imported), passNode(local)))
    }
    static updateImportSpecifier(original?: ImportSpecifier, imported?: Identifier, local?: Identifier): ImportSpecifier {
        return new ImportSpecifier(global.generatedEs2panda._UpdateImportSpecifier(global.context, passNode(original), passNode(imported), passNode(local)))
    }
    get imported(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._ImportSpecifierImported(global.context, this.peer))
    }
    get local(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._ImportSpecifierLocal(global.context, this.peer))
    }
    get isRemovable(): boolean {
        return global.generatedEs2panda._ImportSpecifierIsRemovableConst(global.context, this.peer)
    }
    /** @deprecated */
    setRemovable(isRemovable: boolean): this {
        global.generatedEs2panda._ImportSpecifierSetRemovable(global.context, this.peer, isRemovable)
        return this
    }
}
export function isImportSpecifier(node: object | undefined): node is ImportSpecifier {
    return node instanceof ImportSpecifier
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_IMPORT_SPECIFIER)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_IMPORT_SPECIFIER, (peer: KNativePointer) => new ImportSpecifier(peer))
}