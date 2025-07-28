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
import { FunctionSignature } from "./FunctionSignature"
import { TSTypeParameterDeclaration } from "./TSTypeParameterDeclaration"
import { TypeNode } from "./TypeNode"
export class TSFunctionType extends TypeNode {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 125)
        super(pointer)
    }
    static createTSFunctionType(signature?: FunctionSignature): TSFunctionType {
        return new TSFunctionType(global.generatedEs2panda._CreateTSFunctionType(global.context, passNode(signature)))
    }
    static updateTSFunctionType(original?: TSFunctionType, signature?: FunctionSignature): TSFunctionType {
        return new TSFunctionType(global.generatedEs2panda._UpdateTSFunctionType(global.context, passNode(original), passNode(signature)))
    }
    get typeParams(): TSTypeParameterDeclaration | undefined {
        return unpackNode(global.generatedEs2panda._TSFunctionTypeTypeParams(global.context, this.peer))
    }
    get params(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._TSFunctionTypeParamsConst(global.context, this.peer))
    }
    get returnType(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._TSFunctionTypeReturnType(global.context, this.peer))
    }
    /** @deprecated */
    setNullable(nullable: boolean): this {
        global.generatedEs2panda._TSFunctionTypeSetNullable(global.context, this.peer, nullable)
        return this
    }
}
export function isTSFunctionType(node: object | undefined): node is TSFunctionType {
    return node instanceof TSFunctionType
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_TS_FUNCTION_TYPE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_TS_FUNCTION_TYPE, (peer: KNativePointer) => new TSFunctionType(peer))
}