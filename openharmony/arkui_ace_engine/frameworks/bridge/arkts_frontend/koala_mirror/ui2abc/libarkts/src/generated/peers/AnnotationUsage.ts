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
import { Identifier } from "./Identifier"
import { Statement } from "./Statement"
export class AnnotationUsage extends Statement {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 2)
        super(pointer)
    }
    static create1AnnotationUsage(expr: Expression | undefined, properties: readonly AstNode[]): AnnotationUsage {
        return new AnnotationUsage(global.generatedEs2panda._CreateAnnotationUsage1(global.context, passNode(expr), passNodeArray(properties), properties.length))
    }
    static updateAnnotationUsage(original?: AnnotationUsage, expr?: Expression): AnnotationUsage {
        return new AnnotationUsage(global.generatedEs2panda._UpdateAnnotationUsage(global.context, passNode(original), passNode(expr)))
    }
    static update1AnnotationUsage(original: AnnotationUsage | undefined, expr: Expression | undefined, properties: readonly AstNode[]): AnnotationUsage {
        return new AnnotationUsage(global.generatedEs2panda._UpdateAnnotationUsage1(global.context, passNode(original), passNode(expr), passNodeArray(properties), properties.length))
    }
    get expr(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._AnnotationUsageExpr(global.context, this.peer))
    }
    get properties(): readonly AstNode[] {
        return unpackNodeArray(global.generatedEs2panda._AnnotationUsageProperties(global.context, this.peer))
    }
    /** @deprecated */
    addProperty(property?: AstNode): this {
        global.generatedEs2panda._AnnotationUsageAddProperty(global.context, this.peer, passNode(property))
        return this
    }
    /** @deprecated */
    setProperties(properties: readonly AstNode[]): this {
        global.generatedEs2panda._AnnotationUsageSetProperties(global.context, this.peer, passNodeArray(properties), properties.length)
        return this
    }
    get baseName(): Identifier | undefined {
        return unpackNode(global.generatedEs2panda._AnnotationUsageGetBaseNameConst(global.context, this.peer))
    }
}
export function isAnnotationUsage(node: object | undefined): node is AnnotationUsage {
    return node instanceof AnnotationUsage
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_ANNOTATION_USAGE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_ANNOTATION_USAGE, (peer: KNativePointer) => new AnnotationUsage(peer))
}