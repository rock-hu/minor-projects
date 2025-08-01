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

import { AnnotationUsage } from "./AnnotationUsage"
import { Expression } from "./Expression"
export class TypeNode extends Expression {
    constructor(pointer: KNativePointer) {
        super(pointer)
    }
    get annotations(): readonly AnnotationUsage[] {
        return unpackNodeArray(global.generatedEs2panda._TypeNodeAnnotations(global.context, this.peer))
    }
    /** @deprecated */
    setAnnotations(annotations: readonly AnnotationUsage[]): this {
        global.generatedEs2panda._TypeNodeSetAnnotations(global.context, this.peer, passNodeArray(annotations), annotations.length)
        return this
    }
}
export function isTypeNode(node: object | undefined): node is TypeNode {
    return node instanceof TypeNode
}