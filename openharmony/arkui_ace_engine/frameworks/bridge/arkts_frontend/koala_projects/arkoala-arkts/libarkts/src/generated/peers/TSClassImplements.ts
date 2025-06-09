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
import { TSTypeParameterInstantiation } from "./TSTypeParameterInstantiation"
export class TSClassImplements extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 138)
        super(pointer)
        console.warn("Warning: stub node TSClassImplements")
    }
    static createTSClassImplements(expression?: Expression, typeParameters?: TSTypeParameterInstantiation): TSClassImplements {
        return new TSClassImplements(global.generatedEs2panda._CreateTSClassImplements(global.context, passNode(expression), passNode(typeParameters)))
    }
    static updateTSClassImplements(original?: TSClassImplements, expression?: Expression, typeParameters?: TSTypeParameterInstantiation): TSClassImplements {
        return new TSClassImplements(global.generatedEs2panda._UpdateTSClassImplements(global.context, passNode(original), passNode(expression), passNode(typeParameters)))
    }
    static create1TSClassImplements(expression?: Expression): TSClassImplements {
        return new TSClassImplements(global.generatedEs2panda._CreateTSClassImplements1(global.context, passNode(expression)))
    }
    static update1TSClassImplements(original?: TSClassImplements, expression?: Expression): TSClassImplements {
        return new TSClassImplements(global.generatedEs2panda._UpdateTSClassImplements1(global.context, passNode(original), passNode(expression)))
    }
    get expr(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSClassImplementsExprConst(global.context, this.peer))
    }
    get typeParameters(): TSTypeParameterInstantiation | undefined {
        return unpackNode(global.generatedEs2panda._TSClassImplementsTypeParametersConst(global.context, this.peer))
    }
}
export function isTSClassImplements(node: AstNode): node is TSClassImplements {
    return node instanceof TSClassImplements
}
if (!nodeByType.has(138)) {
    nodeByType.set(138, TSClassImplements)
}