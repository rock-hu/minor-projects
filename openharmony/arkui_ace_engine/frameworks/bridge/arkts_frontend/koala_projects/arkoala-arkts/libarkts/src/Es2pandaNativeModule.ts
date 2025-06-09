/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
    KNativePointer as KPtr,
    KInt,
    KBoolean,
    KNativePointer,
    registerNativeModuleLibraryName,
    loadNativeModuleLibrary,
    KDouble,
    KStringPtr
} from "@koalaui/interop"
import { Es2pandaNativeModule as GeneratedEs2pandaNativeModule } from "./generated/Es2pandaNativeModule"
import * as path from "path"

// TODO: this type should be in interop
export type KPtrArray = BigUint64Array

export class Es2pandaNativeModule {
    _AnnotationAllowedAnnotations(context: KPtr, node: KPtr, returnLen: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _AstNodeRebind(context: KPtr, node: KPtr): void {
        throw new Error("Not implemented")
    }
    _AstNodeRecheck(context: KPtr, node: KPtr): void {
        throw new Error("Not implemented")
    }
    _ContextState(context: KPtr): KInt {
        throw new Error("Not implemented")
    }
    _ContextErrorMessage(context: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _AstNodeChildren(context: KPtr, node: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _AstNodeDumpModifiers(context: KPtr, node: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _CreateConfig(argc: number, argv: string[]): KPtr {
        throw new Error("Not implemented")
    }
    _DestroyConfig(peer: KNativePointer): void {
        throw new Error("Not implemented")
    }
    _CreateContextFromString(config: KPtr, source: String, filename: String): KPtr {
        throw new Error("Not implemented")
    }
    _DestroyContext(context: KPtr): void {
        throw new Error("Not implemented")
    }
    _ProceedToState(context: KPtr, state: number): void {
        throw new Error("Not implemented")
    }
    _ContextProgram(context: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _ProgramAst(context: KPtr, program: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _CheckerStartChecker(context: KPtr): KBoolean {
        throw new Error("Not implemented")
    }
    _AstNodeVariableConst(context: KPtr, ast: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _CreateNumberLiteral(context: KPtr, value: KDouble): KPtr {
        throw new Error("Not implemented")
    }
    _AstNodeUpdateChildren(context: KPtr, node: KPtr): void {
        throw new Error("Not implemented")
    }
    _AstNodeUpdateAll(context: KPtr, node: KPtr): void {
        throw new Error("Not implemented")
    }
    _VariableDeclaration(context: KPtr, variable: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _DeclNode(context: KPtr, decl: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _DeclarationFromIdentifier(context: KPtr, identifier: KPtr): KPtr {
        throw new Error("Not implemented")
    }
    _ProgramExternalSources(context: KNativePointer, instance: KNativePointer): KNativePointer {
        throw new Error("Not implemented");
    }
    _ExternalSourceName(instance: KNativePointer): KNativePointer {
        throw new Error("Not implemented");
    }
    _ExternalSourcePrograms(instance: KNativePointer): KNativePointer {
        throw new Error("Not implemented");
    }
}

export function initEs2panda(): Es2pandaNativeModule {
    registerNativeModuleLibraryName("NativeModule", path.resolve(__dirname, "../native/build/es2panda.node"))
    const instance = new Es2pandaNativeModule()
    loadNativeModuleLibrary("NativeModule", instance)
    return instance
}

export function initGeneratedEs2panda(): GeneratedEs2pandaNativeModule {
    registerNativeModuleLibraryName("NativeModule", path.resolve(__dirname, "../native/build/es2panda.node"))
    const instance = new GeneratedEs2pandaNativeModule()
    // registerNativeModule("InteropNativeModule", NativeModule)
    loadNativeModuleLibrary("NativeModule", instance)
    return instance
}