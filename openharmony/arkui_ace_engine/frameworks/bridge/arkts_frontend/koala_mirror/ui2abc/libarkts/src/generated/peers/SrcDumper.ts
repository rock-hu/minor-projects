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

export class SrcDumper extends ArktsObject {
    constructor(pointer: KNativePointer) {
        super(pointer)
    }
    static create1SrcDumper(node: AstNode | undefined, isDeclgen: boolean, isIsolatedDeclgen: boolean): SrcDumper {
        return new SrcDumper(global.generatedEs2panda._CreateSrcDumper1(global.context, passNode(node), isDeclgen, isIsolatedDeclgen))
    }
    /** @deprecated */
    add(str: string): this {
        global.generatedEs2panda._SrcDumperAdd(global.context, this.peer, str)
        return this
    }
    /** @deprecated */
    add1(i: number): this {
        global.generatedEs2panda._SrcDumperAdd1(global.context, this.peer, i)
        return this
    }
    /** @deprecated */
    add2(l: number): this {
        global.generatedEs2panda._SrcDumperAdd2(global.context, this.peer, l)
        return this
    }
    /** @deprecated */
    add3(f: number): this {
        global.generatedEs2panda._SrcDumperAdd3(global.context, this.peer, f)
        return this
    }
    /** @deprecated */
    add4(d: number): this {
        global.generatedEs2panda._SrcDumperAdd4(global.context, this.peer, d)
        return this
    }
    get str(): string {
        return unpackString(global.generatedEs2panda._SrcDumperStrConst(global.context, this.peer))
    }
    /** @deprecated */
    incrIndent(): this {
        global.generatedEs2panda._SrcDumperIncrIndent(global.context, this.peer)
        return this
    }
    /** @deprecated */
    decrIndent(): this {
        global.generatedEs2panda._SrcDumperDecrIndent(global.context, this.peer)
        return this
    }
    /** @deprecated */
    endl(num: number): this {
        global.generatedEs2panda._SrcDumperEndl(global.context, this.peer, num)
        return this
    }
    get isDeclgen(): boolean {
        return global.generatedEs2panda._SrcDumperIsDeclgenConst(global.context, this.peer)
    }
    get isIsolatedDeclgen(): boolean {
        return global.generatedEs2panda._SrcDumperIsIsolatedDeclgenConst(global.context, this.peer)
    }
    /** @deprecated */
    dumpNode(key: string): this {
        global.generatedEs2panda._SrcDumperDumpNode(global.context, this.peer, key)
        return this
    }
    /** @deprecated */
    removeNode(key: string): this {
        global.generatedEs2panda._SrcDumperRemoveNode(global.context, this.peer, key)
        return this
    }
    get isIndirectDepPhase(): boolean {
        return global.generatedEs2panda._SrcDumperIsIndirectDepPhaseConst(global.context, this.peer)
    }
    /** @deprecated */
    run(): this {
        global.generatedEs2panda._SrcDumperRun(global.context, this.peer)
        return this
    }
}