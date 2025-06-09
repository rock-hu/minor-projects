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

import { TypedStatement } from "./TypedStatement"
import { Identifier } from "./Identifier"
import { Expression } from "./Expression"
import { Decorator } from "./Decorator"
import { Es2pandaPrivateFieldKind } from "./../Es2pandaEnums"
export class ClassElement extends TypedStatement {
     constructor(pointer: KNativePointer) {
        super(pointer)
        console.warn("Warning: stub node ClassElement")
    }
    get key(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ClassElementKeyConst(global.context, this.peer))
    }
    /** @deprecated */
    setValue(value: Expression): this {
        global.generatedEs2panda._ClassElementSetValue(global.context, this.peer, passNode(value))
        return this
    }
    get value(): Expression | undefined {
        return unpackNode(global.generatedEs2panda._ClassElementValueConst(global.context, this.peer))
    }
    get decorators(): readonly Decorator[] {
        return unpackNodeArray(global.generatedEs2panda._ClassElementDecoratorsConst(global.context, this.peer))
    }
    get isComputed(): boolean {
        return global.generatedEs2panda._ClassElementIsComputedConst(global.context, this.peer)
    }
    /** @deprecated */
    addDecorator(decorator: Decorator): this {
        global.generatedEs2panda._ClassElementAddDecorator(global.context, this.peer, passNode(decorator))
        return this
    }
}
export function isClassElement(node: AstNode): node is ClassElement {
    return node instanceof ClassElement
}