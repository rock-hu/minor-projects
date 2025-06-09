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
import { TemplateElement } from "./TemplateElement"
export class TemplateLiteral extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 142)
        super(pointer)
        console.warn("Warning: stub node TemplateLiteral")
    }
    static createTemplateLiteral(quasis: readonly TemplateElement[], expressions: readonly Expression[], multilineString: string): TemplateLiteral {
        return new TemplateLiteral(global.generatedEs2panda._CreateTemplateLiteral(global.context, passNodeArray(quasis), quasis.length, passNodeArray(expressions), expressions.length, multilineString))
    }
    static updateTemplateLiteral(original: TemplateLiteral | undefined, quasis: readonly TemplateElement[], expressions: readonly Expression[], multilineString: string): TemplateLiteral {
        return new TemplateLiteral(global.generatedEs2panda._UpdateTemplateLiteral(global.context, passNode(original), passNodeArray(quasis), quasis.length, passNodeArray(expressions), expressions.length, multilineString))
    }
    get quasis(): readonly TemplateElement[] {
        return unpackNodeArray(global.generatedEs2panda._TemplateLiteralQuasisConst(global.context, this.peer))
    }
    get expressions(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._TemplateLiteralExpressionsConst(global.context, this.peer))
    }
}
export function isTemplateLiteral(node: AstNode): node is TemplateLiteral {
    return node instanceof TemplateLiteral
}
if (!nodeByType.has(142)) {
    nodeByType.set(142, TemplateLiteral)
}