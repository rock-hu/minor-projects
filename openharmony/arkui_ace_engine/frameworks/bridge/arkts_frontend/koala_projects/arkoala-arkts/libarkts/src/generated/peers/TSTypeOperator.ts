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

import { TypeNode } from "./TypeNode"
import { Es2pandaTSOperatorType } from "./../Es2pandaEnums"
export class TSTypeOperator extends TypeNode {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 116)
        super(pointer)
        console.warn("Warning: stub node TSTypeOperator")
    }
    static createTSTypeOperator(type: TypeNode | undefined, operatorType: Es2pandaTSOperatorType): TSTypeOperator {
        return new TSTypeOperator(global.generatedEs2panda._CreateTSTypeOperator(global.context, passNode(type), operatorType))
    }
    static updateTSTypeOperator(original: TSTypeOperator | undefined, type: TypeNode | undefined, operatorType: Es2pandaTSOperatorType): TSTypeOperator {
        return new TSTypeOperator(global.generatedEs2panda._UpdateTSTypeOperator(global.context, passNode(original), passNode(type), operatorType))
    }
    get type(): TypeNode | undefined {
        return unpackNode(global.generatedEs2panda._TSTypeOperatorTypeConst(global.context, this.peer))
    }
    get isReadonly(): boolean {
        return global.generatedEs2panda._TSTypeOperatorIsReadonlyConst(global.context, this.peer)
    }
    get isKeyof(): boolean {
        return global.generatedEs2panda._TSTypeOperatorIsKeyofConst(global.context, this.peer)
    }
    get isUnique(): boolean {
        return global.generatedEs2panda._TSTypeOperatorIsUniqueConst(global.context, this.peer)
    }
}
export function isTSTypeOperator(node: AstNode): node is TSTypeOperator {
    return node instanceof TSTypeOperator
}
if (!nodeByType.has(116)) {
    nodeByType.set(116, TSTypeOperator)
}