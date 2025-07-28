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
import { Es2pandaScriptFunctionFlags } from "./../Es2pandaEnums"
import { Expression } from "./Expression"
import { FunctionSignature } from "./FunctionSignature"
import { TSInterfaceDeclaration } from "./TSInterfaceDeclaration"
import { TSTypeParameterDeclaration } from "./TSTypeParameterDeclaration"
import { TypeNode } from "./TypeNode"
export class ETSFunctionType extends TypeNode {
    constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 67)
        super(pointer)
    }
    static createETSFunctionType(signature: FunctionSignature | undefined, funcFlags: Es2pandaScriptFunctionFlags): ETSFunctionType {
        return new ETSFunctionType(global.generatedEs2panda._CreateETSFunctionType(global.context, passNode(signature), funcFlags))
    }
    static updateETSFunctionType(original: ETSFunctionType | undefined, signature: FunctionSignature | undefined, funcFlags: Es2pandaScriptFunctionFlags): ETSFunctionType {
        return new ETSFunctionType(global.generatedEs2panda._UpdateETSFunctionType(global.context, passNode(original), passNode(signature), funcFlags))
    }
    get typeParams(): TSTypeParameterDeclaration | undefined {
        return unpackNode(global.generatedEs2panda._ETSFunctionTypeTypeParams(global.context, this.peer))
    }
    get params(): readonly Expression[] {
        return unpackNodeArray(global.generatedEs2panda._ETSFunctionTypeParamsConst(global.context, this.peer))
    }
    get returnType(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._ETSFunctionTypeReturnType(global.context, this.peer))
    }
    get functionalInterface(): TSInterfaceDeclaration | undefined {
        return unpackNode(global.generatedEs2panda._ETSFunctionTypeFunctionalInterface(global.context, this.peer))
    }
    /** @deprecated */
    setFunctionalInterface(functionalInterface?: TSInterfaceDeclaration): this {
        global.generatedEs2panda._ETSFunctionTypeSetFunctionalInterface(global.context, this.peer, passNode(functionalInterface))
        return this
    }
    get flags(): Es2pandaScriptFunctionFlags {
        return global.generatedEs2panda._ETSFunctionTypeFlags(global.context, this.peer)
    }
    get isThrowing(): boolean {
        return global.generatedEs2panda._ETSFunctionTypeIsThrowingConst(global.context, this.peer)
    }
    get isRethrowing(): boolean {
        return global.generatedEs2panda._ETSFunctionTypeIsRethrowingConst(global.context, this.peer)
    }
    get isExtensionFunction(): boolean {
        return global.generatedEs2panda._ETSFunctionTypeIsExtensionFunctionConst(global.context, this.peer)
    }
}
export function isETSFunctionType(node: object | undefined): node is ETSFunctionType {
    return node instanceof ETSFunctionType
}
if (!nodeByType.has(Es2pandaAstNodeType.AST_NODE_TYPE_ETS_FUNCTION_TYPE)) {
    nodeByType.set(Es2pandaAstNodeType.AST_NODE_TYPE_ETS_FUNCTION_TYPE, (peer: KNativePointer) => new ETSFunctionType(peer))
}