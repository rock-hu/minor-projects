/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

import * as arkts from "@koalaui/libarkts"
import { factory } from "./MemoFactory"
import { AnalysisVisitor } from "./AnalysisVisitor"
import { FunctionTransformer } from "./FunctionTransformer"
import { dumpAstToFile, MemoFunctionKind, PositionalIdTracker } from "./utils"
import { InternalsTransformer } from "./InternalsTransformer"
import { ParameterTransformer } from "./ParameterTransformer"
import { ReturnTransformer } from "./ReturnTranformer"
import { SignatureTransformer } from "./SignatureTransformer"
import { DiagnosticVisitor } from "./DiagnosticVisitor"

export interface TransformerOptions {
    trace?: boolean,
    contextImport?: string,
    stableForTests?: boolean,
    keepTransformed?: string,
    manuallyDisableInsertingImport?: boolean,
    addLogging?: boolean,
}

export default function memoTransformer(
    userPluginOptions?: TransformerOptions
) {
    return (program: arkts.Program, options: arkts.CompilationOptions, context: arkts.PluginContext) => {
        const scriptFunctions = new Map<arkts.KNativePointer, MemoFunctionKind>()
        const ETSFunctionTypes = new Map<arkts.KNativePointer, MemoFunctionKind>()
        const analysisVisitor = new AnalysisVisitor(scriptFunctions, ETSFunctionTypes)
        const diagnosticVisitor = new DiagnosticVisitor(scriptFunctions)
        const positionalIdTracker = new PositionalIdTracker(`${arkts.getPackageName()}.${arkts.getFilePathFromPackageRoot()}`, userPluginOptions?.stableForTests)
        const signatureTransformer = new SignatureTransformer(scriptFunctions, ETSFunctionTypes)
        const internalsTransformer = new InternalsTransformer(positionalIdTracker)
        const parameterTransformer = new ParameterTransformer(positionalIdTracker)
        const returnTransformer = new ReturnTransformer()

        const node = program.astNode
        analysisVisitor.visitor(node)
        diagnosticVisitor.visitor(node)

        const functionTransformer = new FunctionTransformer(
            scriptFunctions,
            ETSFunctionTypes,
            positionalIdTracker,
            signatureTransformer,
            internalsTransformer,
            parameterTransformer,
            returnTransformer,
            userPluginOptions?.addLogging ?? false
        )
        let result = functionTransformer.visitor(node)
        if (userPluginOptions?.manuallyDisableInsertingImport != true) {
            if ((functionTransformer.modified || signatureTransformer.modified)) {
                result = arkts.updateETSModuleByStatements(
                    result,
                    [
                        factory.createContextTypesImportDeclaration(userPluginOptions?.stableForTests ?? false, userPluginOptions?.contextImport),
                        ...result.statements
                    ]
                )
            }
        }
        if (userPluginOptions?.keepTransformed) {
            dumpAstToFile(result, userPluginOptions.keepTransformed)
        }
        return result
    }
}
