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
import { Es2pandaAccessibilityOption } from "./../Es2pandaEnums"
export class TSParameterProperty extends Expression {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 121)
        super(pointer)
        console.warn("Warning: stub node TSParameterProperty")
    }
    static createTSParameterProperty(accessibility: Es2pandaAccessibilityOption, parameter: Expression | undefined, readonly_arg: boolean, isStatic: boolean, isExport: boolean): TSParameterProperty {
        return new TSParameterProperty(global.generatedEs2panda._CreateTSParameterProperty(global.context, accessibility, passNode(parameter), readonly_arg, isStatic, isExport))
    }
    static updateTSParameterProperty(original: TSParameterProperty | undefined, accessibility: Es2pandaAccessibilityOption, parameter: Expression | undefined, readonly_arg: boolean, isStatic: boolean, isExport: boolean): TSParameterProperty {
        return new TSParameterProperty(global.generatedEs2panda._UpdateTSParameterProperty(global.context, passNode(original), accessibility, passNode(parameter), readonly_arg, isStatic, isExport))
    }
    get accessibility(): Es2pandaAccessibilityOption {
        return global.generatedEs2panda._TSParameterPropertyAccessibilityConst(global.context, this.peer)
    }
    get readonly(): boolean {
        return global.generatedEs2panda._TSParameterPropertyReadonlyConst(global.context, this.peer)
    }
    get isStatic(): boolean {
        return global.generatedEs2panda._TSParameterPropertyIsStaticConst(global.context, this.peer)
    }
    get isExport(): boolean {
        return global.generatedEs2panda._TSParameterPropertyIsExportConst(global.context, this.peer)
    }
    get parameter(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._TSParameterPropertyParameterConst(global.context, this.peer))
    }
}
export function isTSParameterProperty(node: AstNode): node is TSParameterProperty {
    return node instanceof TSParameterProperty
}
if (!nodeByType.has(121)) {
    nodeByType.set(121, TSParameterProperty)
}