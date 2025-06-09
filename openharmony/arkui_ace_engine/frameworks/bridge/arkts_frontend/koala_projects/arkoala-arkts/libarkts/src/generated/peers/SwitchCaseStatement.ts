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

import { Statement } from "./Statement"
import { Expression } from "./Expression"
export class SwitchCaseStatement extends Statement {
     constructor(pointer: KNativePointer) {
        assertValidPeer(pointer, 84)
        super(pointer)
        console.warn("Warning: stub node SwitchCaseStatement")
    }
    static createSwitchCaseStatement(test: Expression | undefined, consequent: readonly Statement[]): SwitchCaseStatement {
        return new SwitchCaseStatement(global.generatedEs2panda._CreateSwitchCaseStatement(global.context, passNode(test), passNodeArray(consequent), consequent.length))
    }
    static updateSwitchCaseStatement(original: SwitchCaseStatement | undefined, test: Expression | undefined, consequent: readonly Statement[]): SwitchCaseStatement {
        return new SwitchCaseStatement(global.generatedEs2panda._UpdateSwitchCaseStatement(global.context, passNode(original), passNode(test), passNodeArray(consequent), consequent.length))
    }
    get test(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._SwitchCaseStatementTestConst(global.context, this.peer))
    }
    get consequent(): readonly Statement[] {
        return unpackNodeArray(global.generatedEs2panda._SwitchCaseStatementConsequentConst(global.context, this.peer))
    }
}
export function isSwitchCaseStatement(node: AstNode): node is SwitchCaseStatement {
    return node instanceof SwitchCaseStatement
}
if (!nodeByType.has(84)) {
    nodeByType.set(84, SwitchCaseStatement)
}