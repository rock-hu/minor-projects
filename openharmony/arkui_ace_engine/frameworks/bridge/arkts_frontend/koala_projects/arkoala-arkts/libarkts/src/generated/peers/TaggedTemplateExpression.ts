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
import { TemplateLiteral } from "./TemplateLiteral"
import { TSTypeParameterInstantiation } from "./TSTypeParameterInstantiation"
export class TaggedTemplateExpression extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 140)
        super(pointer)
        console.warn("Warning: stub node TaggedTemplateExpression")
    }
    static createTaggedTemplateExpression(tag?: Expression, quasi?: TemplateLiteral, typeParams?: TSTypeParameterInstantiation): TaggedTemplateExpression {
        return new TaggedTemplateExpression(global.generatedEs2panda._CreateTaggedTemplateExpression(global.context, passNode(tag), passNode(quasi), passNode(typeParams)))
    }
    static updateTaggedTemplateExpression(original?: TaggedTemplateExpression, tag?: Expression, quasi?: TemplateLiteral, typeParams?: TSTypeParameterInstantiation): TaggedTemplateExpression {
        return new TaggedTemplateExpression(global.generatedEs2panda._UpdateTaggedTemplateExpression(global.context, passNode(original), passNode(tag), passNode(quasi), passNode(typeParams)))
    }
    get tag(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TaggedTemplateExpressionTagConst(global.context, this.peer))
    }
    get quasi(): TemplateLiteral | undefined {
        return unpackNode(global.generatedEs2panda._TaggedTemplateExpressionQuasiConst(global.context, this.peer))
    }
    get typeParams(): TSTypeParameterInstantiation | undefined {
        return unpackNode(global.generatedEs2panda._TaggedTemplateExpressionTypeParamsConst(global.context, this.peer))
    }
}
export function isTaggedTemplateExpression(node: AstNode): node is TaggedTemplateExpression {
    return node instanceof TaggedTemplateExpression
}
if (!nodeByType.has(140)) {
    nodeByType.set(140, TaggedTemplateExpression)
}